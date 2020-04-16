#include "imgui.h"
#include "ImGuiDebugOverlay.h"
#include "Application.h"
#include "IInputManager.h"
#include "InputEvents.h"

#include "DrawableNode.h"
#include "MeshSprite.h"
#include "ParticleSystem.h"
#include "TextNode.h"

#ifdef WITH_AUDIO
	#include "IAudioPlayer.h"
#endif

#include "RenderStatistics.h"
#ifdef WITH_LUA
	#include "LuaStatistics.h"
#endif

#ifdef WITH_RENDERDOC
	#include "RenderDocCapture.h"
#endif

#ifdef WITH_ALLOCATORS
	#include "allocators_config.h"
#endif

#include "version.h"

namespace ncine {

namespace {

	int inputTextCallback(ImGuiInputTextCallbackData *data)
	{
		nctl::String *string = reinterpret_cast<nctl::String *>(data->UserData);
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
		{
			// Resize string callback
			ASSERT(data->Buf == string->data());
			string->setLength(data->BufTextLen);
			data->Buf = string->data();
		}
		return 0;
	}

	const char *nodeTypeToString(Object::ObjectType type)
	{
		switch (type)
		{
			case Object::ObjectType::SCENENODE: return "SceneNode";
			case Object::ObjectType::SPRITE: return "Sprite";
			case Object::ObjectType::MESH_SPRITE: return "MeshSprite";
			case Object::ObjectType::ANIMATED_SPRITE: return "AnimatedSprite";
			case Object::ObjectType::PARTICLE_SYSTEM: return "ParticleSystem";
			case Object::ObjectType::TEXTNODE: return "TextNode";
			default: return "N/A";
		}
	}

}

///////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
///////////////////////////////////////////////////////////

ImGuiDebugOverlay::ImGuiDebugOverlay(float profileTextUpdateTime)
    : IDebugOverlay(profileTextUpdateTime), disableAppInputEvents_(false), appInputHandler_(nullptr),
      lockOverlayPositions_(true), showTopLeftOverlay_(true), showTopRightOverlay_(true),
      showBottomLeftOverlay_(true), showBottomRightOverlay_(true), numValues_(128),
      maxFrameTime_(0.0f), maxUpdateVisitDraw_(0.0f), index_(0), widgetName_(256),
      plotAdditionalFrameValues_(false), plotOverlayValues_(false), comboVideoModes_(4096)
#ifdef WITH_RENDERDOC
      ,
      renderDocPathTemplate_(MaxRenderDocPathLength),
      renderDocFileComments_(MaxRenderDocCommentsLength),
      renderDocCapturePath_(MaxRenderDocPathLength),
      renderDocLastNumCaptures_(0)
#endif
{
	initPlotValues();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ImGuiDebugOverlay::update()
{
	// Application events cannot stay disabled after the interface is hidden
	if (settings_.showInterface == false)
		disableAppInputEvents_ = false;

	if (disableAppInputEvents_ && appInputHandler_ == nullptr)
	{
		appInputHandler_ = IInputManager::handler();
		IInputManager::setHandler(nullptr);
	}
	else if (disableAppInputEvents_ == false && appInputHandler_)
	{
		IInputManager::setHandler(appInputHandler_);
		appInputHandler_ = nullptr;
	}

	guiWindow();

	if (settings_.showInfoText)
	{
		const AppConfiguration &appCfg = theApplication().appConfiguration();
		if (appCfg.withScenegraph)
		{
			guiTopLeft();
			guiBottomRight();
		}
		guiTopRight();
		guiBottomLeft();
	}

	if (settings_.showProfilerGraphs)
		guiPlots();
}

void ImGuiDebugOverlay::updateFrameTimings()
{
	if (lastUpdateTime_.secondsSince() > updateTime_)
	{
		const AppConfiguration &appCfg = theApplication().appConfiguration();
		const float *timings = theApplication().timings();

		plotValues_[ValuesType::FRAME_TIME][index_] = theApplication().interval();
		plotValues_[ValuesType::FRAME_START][index_] = timings[Application::Timings::FRAME_START];
		plotValues_[ValuesType::IMGUI][index_] = timings[Application::Timings::IMGUI];
#ifdef WITH_NUKLEAR
		plotValues_[ValuesType::NUKLEAR][index_] = timings[Application::Timings::NUKLEAR];
#endif
		plotValues_[ValuesType::FRAME_END][index_] = timings[Application::Timings::FRAME_END];

		if (appCfg.withScenegraph)
		{
			plotValues_[ValuesType::UPDATE_VISIT_DRAW][index_] = timings[Application::Timings::UPDATE] +
			                                                     timings[Application::Timings::VISIT] +
			                                                     timings[Application::Timings::DRAW];
			plotValues_[ValuesType::UPDATE][index_] = timings[Application::Timings::UPDATE];
			plotValues_[ValuesType::VISIT][index_] = timings[Application::Timings::VISIT];
			plotValues_[ValuesType::DRAW][index_] = timings[Application::Timings::DRAW];
		}

		for (unsigned int i = 0; i < index_; i++)
		{
			if (plotValues_[ValuesType::FRAME_TIME][i] > maxFrameTime_)
				maxFrameTime_ = plotValues_[ValuesType::FRAME_TIME][i];
			if (appCfg.withScenegraph && plotValues_[ValuesType::UPDATE_VISIT_DRAW][i] > maxUpdateVisitDraw_)
				maxUpdateVisitDraw_ = plotValues_[ValuesType::UPDATE_VISIT_DRAW][i];
		}

		if (appCfg.withScenegraph)
			updateOverlayTimings();

		index_ = (index_ + 1) % numValues_;
		lastUpdateTime_ = TimeStamp::now();
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

namespace {

#ifdef WITH_AUDIO
	const char *audioPlayerStateToString(IAudioPlayer::PlayerState state)
	{
		switch (state)
		{
			case IAudioPlayer::PlayerState::INITIAL: return "Initial";
			case IAudioPlayer::PlayerState::PLAYING: return "Playing";
			case IAudioPlayer::PlayerState::PAUSED: return "Paused";
			case IAudioPlayer::PlayerState::STOPPED: return "Stopped";
		}

		return "Unknown";
	}
#endif

	const char *mouseCursorModeToString(IInputManager::MouseCursorMode mode)
	{
		switch (mode)
		{
			case IInputManager::MouseCursorMode::NORMAL: return "Normal";
			case IInputManager::MouseCursorMode::HIDDEN: return "Hidden";
			case IInputManager::MouseCursorMode::DISABLED: return "Disabled";
		}

		return "Unknown";
	}

	const char *mappedButtonNameToString(ButtonName name)
	{
		switch (name)
		{
			case ButtonName::UNKNOWN: return "Unknown";
			case ButtonName::A: return "A";
			case ButtonName::B: return "B";
			case ButtonName::X: return "X";
			case ButtonName::Y: return "Y";
			case ButtonName::BACK: return "Back";
			case ButtonName::GUIDE: return "Guide";
			case ButtonName::START: return "Start";
			case ButtonName::LSTICK: return "LStick";
			case ButtonName::RSTICK: return "RStick";
			case ButtonName::LBUMPER: return "LBumper";
			case ButtonName::RBUMPER: return "RBumper";
			case ButtonName::DPAD_UP: return "DPad_Up";
			case ButtonName::DPAD_DOWN: return "DPad_Down";
			case ButtonName::DPAD_LEFT: return "DPad_Left";
			case ButtonName::DPAD_RIGHT: return "DPad_Right";
		}

		return "Unknown";
	}

	const char *mappedAxisNameToString(AxisName name)
	{
		switch (name)
		{
			case AxisName::UNKNOWN: return "Unknown";
			case AxisName::LX: return "LX";
			case AxisName::LY: return "LY";
			case AxisName::RX: return "RX";
			case AxisName::RY: return "RY";
			case AxisName::LTRIGGER: return "LTrigger";
			case AxisName::RTRIGGER: return "RTrigger";
		}

		return "Unknown";
	}

}

void ImGuiDebugOverlay::guiWindow()
{
	if (settings_.showInterface == false)
		return;

	const ImVec2 windowPos = ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y - 0.5f);
	const ImVec2 windowPosPivot = ImVec2(0.5f, 0.5f);
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_FirstUseEver, windowPosPivot);
	if (ImGui::Begin("Debug Overlay", &settings_.showInterface))
	{
		const AppConfiguration &appCfg = theApplication().appConfiguration();

		ImGui::Checkbox("Disable app input events", &disableAppInputEvents_);
		ImGui::SameLine();
		bool disableAutoSuspension = !theApplication().autoSuspension();
		ImGui::Checkbox("Disable auto-suspension", &disableAutoSuspension);
		theApplication().setAutoSuspension(!disableAutoSuspension);
		ImGui::SameLine();
		if (ImGui::Button("Quit"))
			theApplication().quit();

		guiConfigureGui();
		guiPreprocessorDefines();
		guiVersionStrings();
		guiInitTimes();
		guiLog();
		guiGraphicsCapabilities();
		guiApplicationConfiguration();
		if (appCfg.withScenegraph)
			guiRenderingSettings();
		guiWindowSettings();
		guiAudioPlayers();
		guiInputState();
		guiRenderDoc();
		guiAllocators();
		guiNodeInspector();
	}
	ImGui::End();
}

void ImGuiDebugOverlay::guiConfigureGui()
{
	static int numValues = 0;

	if (ImGui::CollapsingHeader("Configure GUI"))
	{
		const AppConfiguration &appCfg = theApplication().appConfiguration();

		ImGui::Checkbox("Show interface", &settings_.showInterface);
		if (ImGui::TreeNodeEx("Overlays", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Checkbox("Show overlays", &settings_.showInfoText);
			ImGui::Checkbox("Lock overlay positions", &lockOverlayPositions_);
			if (appCfg.withScenegraph)
			{
				ImGui::Checkbox("Show Top-Left", &showTopLeftOverlay_);
				ImGui::SameLine();
			}
			ImGui::Checkbox("Show Top-Right", &showTopRightOverlay_);
			ImGui::Checkbox("Show Bottom-Left", &showBottomLeftOverlay_);
#ifdef WITH_LUA
			if (appCfg.withScenegraph)
			{
				ImGui::SameLine();
				ImGui::Checkbox("Show Bottom-Right", &showBottomRightOverlay_);
			}
#endif
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Profiling Graphs", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Checkbox("Show profiling graphs", &settings_.showProfilerGraphs);
			ImGui::Checkbox("Plot additional frame values", &plotAdditionalFrameValues_);
			if (appCfg.withScenegraph)
				ImGui::Checkbox("Plot overlay values", &plotOverlayValues_);
			ImGui::SliderFloat("Graphs update time", &updateTime_, 0.0f, 1.0f, "%.2fs");
			numValues = (numValues == 0) ? static_cast<int>(numValues_) : numValues;
			ImGui::SliderInt("Number of values", &numValues, 16, 512);
			ImGui::SameLine();
			if (ImGui::Button("Apply") && numValues_ != static_cast<unsigned int>(numValues))
			{
				numValues_ = static_cast<unsigned int>(numValues);
				initPlotValues();
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("GUI Style"))
		{
			static int styleIndex = 0;
			ImGui::Combo("Theme", &styleIndex, "Dark\0Light\0Classic\0");

			if (styleIndex < 0)
				styleIndex = 0;
			else if (styleIndex > 2)
				styleIndex = 2;

			switch (styleIndex)
			{
				case 0: ImGui::StyleColorsDark(); break;
				case 1: ImGui::StyleColorsLight(); break;
				case 2: ImGui::StyleColorsClassic(); break;
			}

			const float MinFrameRounding = 0.0f;
			const float MaxFrameRounding = 12.0f;
			ImGuiStyle &style = ImGui::GetStyle();
			ImGui::SliderFloat("Frame Rounding", &style.FrameRounding, MinFrameRounding, MaxFrameRounding, "%.0f");

			if (style.FrameRounding < MinFrameRounding)
				style.FrameRounding = MinFrameRounding;
			else if (style.FrameRounding > MaxFrameRounding)
				style.FrameRounding = MaxFrameRounding;
			// Make `GrabRounding` always the same value as `FrameRounding`
			style.GrabRounding = style.FrameRounding;

			static bool windowBorder = true;
			ImGui::Checkbox("Window Border", &windowBorder);
			style.WindowBorderSize = windowBorder ? 1.0f : 0.0f;
			ImGui::SameLine();
			static bool frameBorder = true;
			ImGui::Checkbox("Frame Border", &frameBorder);
			style.FrameBorderSize = frameBorder ? 1.0f : 0.0f;
			ImGui::SameLine();
			static bool popupBorder = true;
			ImGui::Checkbox("Popup Border", &popupBorder);
			style.PopupBorderSize = popupBorder ? 1.0f : 0.0f;

			const float MinScaling = 0.5f;
			const float MaxScaling = 2.0f;
			static float scaling = 1.0f;
			ImGui::SetNextItemWidth(100);
			ImGui::DragFloat("Scaling", &scaling, 0.005f, MinScaling, MaxScaling, "%.1f");
			ImGui::SameLine();
			if (ImGui::Button("Reset"))
				scaling = 1.0f;

			if (scaling < MinScaling)
				scaling = MinScaling;
			if (scaling > MaxScaling)
				scaling = MaxScaling;
			ImGui::GetIO().FontGlobalScale = scaling;

			ImGui::TreePop();
		}
	}
	else
		numValues = 0;
}

void ImGuiDebugOverlay::guiPreprocessorDefines()
{
	if (ImGui::CollapsingHeader("Preprocessor Defines"))
	{
		if (ImGui::TreeNodeEx("System", ImGuiTreeNodeFlags_DefaultOpen))
		{
#ifdef __linux__
			ImGui::Text("__linux__");
#endif
#ifdef _WIN32
			ImGui::Text("_WIN32");
#endif
#ifdef _WIN64
			ImGui::Text("_WIN64");
#endif
#ifdef __ANDROID__
			ImGui::Text("__ANDROID__");
#endif
#ifdef __ANDROID_API__
			ImGui::Text("__ANDROID_API__ = %d", static_cast<int>(__ANDROID_API__));
#endif
#ifdef __APPLE__
			ImGui::Text("__APPLE__");
#endif
#ifdef _MSC_VER
			ImGui::Text("_MSC_VER = %d", _MSC_VER);
#endif
#ifdef __MINGW32__
			ImGui::Text("__MINGW32__");
#endif
#ifdef __MINGW64__
			ImGui::Text("__MINGW64__");
#endif
#ifdef __EMSCRIPTEN__
			ImGui::Text("__EMSCRIPTEN__");
#endif
#ifdef __GNUC__
			ImGui::Text("__GNUC__ = %d", static_cast<int>(__GNUC__));
#endif
#ifdef __clang_version__
			ImGui::Text("__clang_version__ = %s", __clang_version__);
#endif
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("nCine", ImGuiTreeNodeFlags_DefaultOpen))
		{
#ifdef WITH_THREADS
			ImGui::Text("WITH_THREADS");
#endif
#ifdef WITH_ANGLE
			ImGui::Text("WITH_ANGLE");
#endif
#ifdef WITH_GLEW
			ImGui::Text("WITH_GLEW");
#endif
#ifdef WITH_GLFW
			ImGui::Text("WITH_GLFW");
#endif
#ifdef WITH_SDL
			ImGui::Text("WITH_SDL");
#endif
#ifdef WITH_QT5
			ImGui::Text("WITH_QT5");
#endif
#ifdef WITH_AUDIO
			ImGui::Text("WITH_AUDIO");
#endif
#ifdef WITH_VORBIS
			ImGui::Text("WITH_VORBIS");
#endif
#ifdef WITH_PNG
			ImGui::Text("WITH_PNG");
#endif
#ifdef WITH_WEBP
			ImGui::Text("WITH_WEBP");
#endif
#ifdef WITH_LUA
			ImGui::Text("WITH_LUA");
#endif
#ifdef WITH_ALLOCATORS
			ImGui::Text("WITH_ALLOCATORS");
#endif
#ifdef WITH_IMGUI
			ImGui::Text("WITH_IMGUI");
#endif
#ifdef WITH_NUKLEAR
			ImGui::Text("WITH_NUKLEAR");
#endif
#ifdef WITH_TRACY
			ImGui::Text("WITH_TRACY");
#endif
#ifdef WITH_RENDERDOC
			ImGui::Text("WITH_RENDERDOC");
#endif
			ImGui::TreePop();
		}
	}
}

void ImGuiDebugOverlay::guiVersionStrings()
{
	if (ImGui::CollapsingHeader("Version Strings"))
	{
#ifdef WITH_GIT_VERSION
		ImGui::Text("Version: %s", VersionStrings::Version);
		ImGui::Text("Git revision count: %s", VersionStrings::GitRevCount);
		ImGui::Text("Git short hash: %s", VersionStrings::GitShortHash);
		ImGui::Text("Git last commit date: %s", VersionStrings::GitLastCommitDate);
		ImGui::Text("Git branch: %s", VersionStrings::GitBranch);
#endif
		ImGui::Text("Compilation date: %s", VersionStrings::CompilationDate);
		ImGui::Text("Compilation time: %s", VersionStrings::CompilationTime);
	}
}

void ImGuiDebugOverlay::guiInitTimes()
{
	if (ImGui::CollapsingHeader("Init Times"))
	{
		const float *timings = theApplication().timings();

		float initTimes[3];
		initTimes[0] = timings[Application::Timings::PRE_INIT];
		initTimes[1] = initTimes[0] + timings[Application::Timings::INIT_COMMON];
		initTimes[2] = initTimes[1] + timings[Application::Timings::APP_INIT];
		ImGui::PlotHistogram("Init Times", initTimes, 3, 0, nullptr, 0.0f, initTimes[2], ImVec2(0.0f, 100.0f));

		ImGui::Text("Pre-Init Time: %.2fs", timings[Application::Timings::PRE_INIT]);
		ImGui::Text("Init Time: %.2fs", timings[Application::Timings::INIT_COMMON]);
		ImGui::Text("Application Init Time: %.2fs", timings[Application::Timings::APP_INIT]);
	}
}

void ImGuiDebugOverlay::guiLog()
{
	if (ImGui::CollapsingHeader("Log"))
	{
		ILogger &logger = theServiceLocator().logger();

		ImGui::BeginChild("scrolling", ImVec2(0.0f, -1.2f * ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
		ImGui::TextUnformatted(logger.logString());
		ImGui::EndChild();
		ImGui::Separator();
		if (ImGui::Button("Clear"))
			logger.clearLogString();
		ImGui::SameLine();
		ImGui::Text("Length: %u / %u", logger.logStringLength(), logger.logStringCapacity());
	}
}

void ImGuiDebugOverlay::guiGraphicsCapabilities()
{
	if (ImGui::CollapsingHeader("Graphics Capabilities"))
	{
		const IGfxCapabilities &gfxCaps = theServiceLocator().gfxCapabilities();

		const IGfxCapabilities::GlInfoStrings &glInfoStrings = gfxCaps.glInfoStrings();
		ImGui::Text("OpenGL Vendor: %s", glInfoStrings.vendor);
		ImGui::Text("OpenGL Renderer: %s", glInfoStrings.renderer);
		ImGui::Text("OpenGL Version: %s", glInfoStrings.glVersion);
		ImGui::Text("GLSL Version: %s", glInfoStrings.glslVersion);

		ImGui::Text("OpenGL parsed version: %d.%d.%d",
		            gfxCaps.glVersion(IGfxCapabilities::GLVersion::MAJOR),
		            gfxCaps.glVersion(IGfxCapabilities::GLVersion::MINOR),
		            gfxCaps.glVersion(IGfxCapabilities::GLVersion::RELEASE));

		ImGui::Separator();
		ImGui::Text("GL_MAX_TEXTURE_SIZE: %d", gfxCaps.value(IGfxCapabilities::GLIntValues::MAX_TEXTURE_SIZE));
		ImGui::Text("GL_MAX_TEXTURE_IMAGE_UNITS: %d", gfxCaps.value(IGfxCapabilities::GLIntValues::MAX_TEXTURE_IMAGE_UNITS));
		ImGui::Text("GL_MAX_UNIFORM_BLOCK_SIZE: %d", gfxCaps.value(IGfxCapabilities::GLIntValues::MAX_UNIFORM_BLOCK_SIZE));
		ImGui::Text("GL_MAX_UNIFORM_BUFFER_BINDINGS: %d", gfxCaps.value(IGfxCapabilities::GLIntValues::MAX_UNIFORM_BUFFER_BINDINGS));
		ImGui::Text("GL_MAX_VERTEX_UNIFORM_BLOCKS: %d", gfxCaps.value(IGfxCapabilities::GLIntValues::MAX_VERTEX_UNIFORM_BLOCKS));
		ImGui::Text("GL_MAX_FRAGMENT_UNIFORM_BLOCKS: %d", gfxCaps.value(IGfxCapabilities::GLIntValues::MAX_FRAGMENT_UNIFORM_BLOCKS));
		ImGui::Text("GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT: %d", gfxCaps.value(IGfxCapabilities::GLIntValues::UNIFORM_BUFFER_OFFSET_ALIGNMENT));

		ImGui::Separator();
		ImGui::Text("GL_KHR_debug: %d", gfxCaps.hasExtension(IGfxCapabilities::GLExtensions::KHR_DEBUG));
		ImGui::Text("GL_ARB_texture_storage: %d", gfxCaps.hasExtension(IGfxCapabilities::GLExtensions::ARB_TEXTURE_STORAGE));
		ImGui::Text("GL_EXT_texture_compression_s3tc: %d", gfxCaps.hasExtension(IGfxCapabilities::GLExtensions::EXT_TEXTURE_COMPRESSION_S3TC));
		ImGui::Text("GL_OES_compressed_ETC1_RGB8_texture: %d", gfxCaps.hasExtension(IGfxCapabilities::GLExtensions::OES_COMPRESSED_ETC1_RGB8_TEXTURE));
		ImGui::Text("GL_AMD_compressed_ATC_texture: %d", gfxCaps.hasExtension(IGfxCapabilities::GLExtensions::AMD_COMPRESSED_ATC_TEXTURE));
		ImGui::Text("GL_IMG_texture_compression_pvrtc: %d", gfxCaps.hasExtension(IGfxCapabilities::GLExtensions::IMG_TEXTURE_COMPRESSION_PVRTC));
		ImGui::Text("GL_KHR_texture_compression_astc_ldr: %d", gfxCaps.hasExtension(IGfxCapabilities::GLExtensions::KHR_TEXTURE_COMPRESSION_ASTC_LDR));
	}
}

void ImGuiDebugOverlay::guiApplicationConfiguration()
{
	if (ImGui::CollapsingHeader("Application Configuration"))
	{
		const AppConfiguration &appCfg = theApplication().appConfiguration();
		ImGui::Text("OpenGL Core: %s", appCfg.glCoreProfile() ? "true" : "false");
		ImGui::Text("OpenGL Forward: %s", appCfg.glForwardCompatible() ? "true" : "false");
		ImGui::Text("OpenGL Major: %d", appCfg.glMajorVersion());
		ImGui::Text("OpenGL Minor: %d", appCfg.glMinorVersion());

		ImGui::Separator();
		ImGui::Text("Data path: %s", appCfg.dataPath().data());
		ImGui::Text("Log file: %s", appCfg.logFile.data());
		ImGui::Text("Console log level: %d", static_cast<int>(appCfg.consoleLogLevel));
		ImGui::Text("File log level: %d", static_cast<int>(appCfg.fileLogLevel));
		ImGui::Text("Frametimer Log interval: %f", appCfg.frameTimerLogInterval);
		ImGui::Text("Profile text update time: %f", appCfg.profileTextUpdateTime());
		ImGui::Text("Resolution: %u x %u", appCfg.resolution.x, appCfg.resolution.y);
		ImGui::Text("Full Screen: %s", appCfg.inFullscreen ? "true" : "false");
		ImGui::Text("Resizable: %s", appCfg.isResizable ? "true" : "false");
		ImGui::Text("Frame Limit: %u", appCfg.frameLimit);

		ImGui::Separator();
		ImGui::Text("Window title: %s", appCfg.windowTitle.data());
		ImGui::Text("Window icon: %s", appCfg.windowIconFilename.data());

		ImGui::Separator();
		ImGui::Text("Buffer mapping: %s", appCfg.useBufferMapping ? "true" : "false");
		ImGui::Text("Defer shader queries: %s", appCfg.deferShaderQueries ? "true" : "false");
		ImGui::Text("VBO size: %lu", appCfg.vboSize);
		ImGui::Text("IBO size: %lu", appCfg.iboSize);
		ImGui::Text("Vao pool size: %u", appCfg.vaoPoolSize);

		ImGui::Separator();
		ImGui::Text("Debug Overlay: %s", appCfg.withDebugOverlay ? "true" : "false");
		ImGui::Text("Audio: %s", appCfg.withAudio ? "true" : "false");
		ImGui::Text("Threads: %s", appCfg.withThreads ? "true" : "false");
		ImGui::Text("Scenegraph: %s", appCfg.withScenegraph ? "true" : "false");
		ImGui::Text("VSync: %s", appCfg.withVSync ? "true" : "false");
		ImGui::Text("OpenGL Debug Context: %s", appCfg.withGlDebugContext ? "true" : "false");
	}
}

void ImGuiDebugOverlay::guiRenderingSettings()
{
	if (ImGui::CollapsingHeader("Rendering Settings"))
	{
		Application::RenderingSettings &settings = theApplication().renderingSettings();
		int minBatchSize = settings.minBatchSize;
		int maxBatchSize = settings.maxBatchSize;

		ImGui::Checkbox("Batching", &settings.batchingEnabled);
		ImGui::SameLine();
		ImGui::Checkbox("Batching with indices", &settings.batchingWithIndices);
		ImGui::SameLine();
		ImGui::Checkbox("Culling", &settings.cullingEnabled);
		ImGui::DragIntRange2("Batch size", &minBatchSize, &maxBatchSize, 1.0f, 0, 512);

		settings.minBatchSize = minBatchSize;
		settings.maxBatchSize = maxBatchSize;
	}
}

void ImGuiDebugOverlay::guiWindowSettings()
{
#ifndef __ANDROID__
	if (ImGui::CollapsingHeader("Window Settings"))
	{
		static int resolution[2] = { theApplication().widthInt(), theApplication().heightInt() };
		static bool fullScreen = theApplication().gfxDevice().isFullScreen();

		static int selectedVideoMode = -1;
		const IGfxDevice::VideoMode currentVideoMode = theApplication().gfxDevice().currentVideoMode();
		if (fullScreen == false)
		{
			ImGui::InputInt2("Resolution", resolution);
			selectedVideoMode = -1;
		}
		else
		{
			unsigned int currentVideoModeIndex = 0;
			const unsigned int numVideoModes = theApplication().gfxDevice().numVideoModes();
			comboVideoModes_.clear();
			for (unsigned int i = 0; i < numVideoModes; i++)
			{
				const IGfxDevice::VideoMode &mode = theApplication().gfxDevice().videoMode(i);
				comboVideoModes_.formatAppend("%ux%u, %uHz", mode.width, mode.height, mode.refreshRate);
				comboVideoModes_.setLength(comboVideoModes_.length() + 1);

				if (mode == currentVideoMode)
					currentVideoModeIndex = i;
			}
			comboVideoModes_.setLength(comboVideoModes_.length() + 1);
			// Append a second '\0' to signal the end of the combo item list
			comboVideoModes_[comboVideoModes_.length() - 1] = '\0';

			if (selectedVideoMode < 0)
				selectedVideoMode = currentVideoModeIndex;

			ImGui::Combo("Video Mode", &selectedVideoMode, comboVideoModes_.data());
			resolution[0] = theApplication().gfxDevice().videoMode(selectedVideoMode).width;
			resolution[1] = theApplication().gfxDevice().videoMode(selectedVideoMode).height;
		}
		ImGui::Checkbox("Full Screen", &fullScreen);

		ImGui::SameLine();
		if (ImGui::Button("Apply"))
		{
			theApplication().gfxDevice().setFullScreen(fullScreen);
			if (fullScreen == false)
				theApplication().gfxDevice().setResolution(resolution[0], resolution[1]);
			else
				theApplication().gfxDevice().setVideoMode(selectedVideoMode);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
		{
			resolution[0] = theApplication().appConfiguration().resolution.x;
			resolution[1] = theApplication().appConfiguration().resolution.y;
			fullScreen = theApplication().appConfiguration().inFullscreen;
			theApplication().gfxDevice().setResolution(resolution[0], resolution[1]);
			theApplication().gfxDevice().setFullScreen(fullScreen);
		}
		ImGui::SameLine();
		if (ImGui::Button("Current"))
		{
			resolution[0] = theApplication().widthInt();
			resolution[1] = theApplication().heightInt();
			fullScreen = theApplication().gfxDevice().isFullScreen();
			selectedVideoMode = -1;
		}

		ImGui::Text("Resizable: %s", theApplication().gfxDevice().isResizable() ? "true" : "false");
	}
#endif
}

void ImGuiDebugOverlay::guiAudioPlayers()
{
#ifdef WITH_AUDIO
	if (ImGui::CollapsingHeader("Audio Players"))
	{
		ImGui::Text("Device Name: %s", theServiceLocator().audioDevice().name());
		ImGui::Text("Listener Gain: %f", theServiceLocator().audioDevice().gain());

		unsigned int numPlayers = theServiceLocator().audioDevice().numPlayers();
		ImGui::Text("Active Players: %d", numPlayers);

		if (numPlayers > 0)
		{
			if (ImGui::Button("Stop"))
				theServiceLocator().audioDevice().stopPlayers();
			ImGui::SameLine();
			if (ImGui::Button("Pause"))
				theServiceLocator().audioDevice().pausePlayers();
		}

		// Stopping or pausing players change the number of active ones
		numPlayers = theServiceLocator().audioDevice().numPlayers();
		for (unsigned int i = 0; i < numPlayers; i++)
		{
			const IAudioPlayer *player = theServiceLocator().audioDevice().player(i);
			widgetName_.format("Player %d", i);
			if (ImGui::TreeNode(widgetName_.data()))
			{
				ImGui::Text("Source Id: %u", player->sourceId());
				ImGui::Text("Buffer Id: %u", player->bufferId());
				ImGui::Text("Channels: %d", player->numChannels());
				ImGui::Text("Frequency: %dHz", player->frequency());
				ImGui::Text("Buffer Size: %lu bytes", player->bufferSize());
				ImGui::NewLine();

				ImGui::Text("State: %s", audioPlayerStateToString(player->state()));
				ImGui::Text("Looping: %s", player->isLooping() ? "true" : "false");
				ImGui::Text("Gain: %f", player->gain());
				ImGui::Text("Pitch: %f", player->pitch());
				const Vector3f &pos = player->position();
				ImGui::Text("Position: <%f, %f, %f>", pos.x, pos.y, pos.z);

				ImGui::TreePop();
			}
		}
	}
#endif
}

void ImGuiDebugOverlay::guiInputState()
{
	if (ImGui::CollapsingHeader("Input State"))
	{
		const IInputManager &input = theApplication().inputManager();

		if (ImGui::TreeNode("Keyboard"))
		{
			nctl::String pressedKeys;
			const KeyboardState &keyState = input.keyboardState();
			for (unsigned int i = 0; i < static_cast<int>(KeySym::COUNT); i++)
			{
				if (keyState.isKeyDown(static_cast<KeySym>(i)))
					pressedKeys.formatAppend("%d ", i);
			}
			ImGui::Text("Keys pressed: %s", pressedKeys.data());
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Mouse"))
		{
			ImGui::Text("Cursor Mode: %s", mouseCursorModeToString(input.mouseCursorMode()));

			const MouseState &mouseState = input.mouseState();
			ImGui::Text("Position: %d, %d", mouseState.x, mouseState.y);

			nctl::String pressedMouseButtons(32);
			if (mouseState.isLeftButtonDown())
				pressedMouseButtons.append("left ");
			if (mouseState.isRightButtonDown())
				pressedMouseButtons.append("right ");
			if (mouseState.isMiddleButtonDown())
				pressedMouseButtons.append("middle ");
			if (mouseState.isFourthButtonDown())
				pressedMouseButtons.append("fourth ");
			if (mouseState.isFifthButtonDown())
				pressedMouseButtons.append("fifth");
			ImGui::Text("Pressed buttons: %s", pressedMouseButtons.data());
			ImGui::TreePop();
		}

		unsigned int numConnectedJoysticks = 0;
		for (int joyId = 0; joyId < IInputManager::MaxNumJoysticks; joyId++)
		{
			if (input.isJoyPresent(joyId))
				numConnectedJoysticks++;
		}
		if (numConnectedJoysticks > 0)
		{
			widgetName_.format("%d Joystick(s)", numConnectedJoysticks);
			if (ImGui::TreeNode(widgetName_.data()))
			{
				ImGui::Text("Joystick mappings: %u", input.numJoyMappings());

				for (int joyId = 0; joyId < IInputManager::MaxNumJoysticks; joyId++)
				{
					if (input.isJoyPresent(joyId) == false)
						continue;

					widgetName_.format("Joystick %d", joyId);
					if (ImGui::TreeNode(widgetName_.data()))
					{
						ImGui::Text("Name: %s", input.joyName(joyId));
						ImGui::Text("GUID: %s", input.joyGuid(joyId));
						ImGui::Text("Num Buttons: %d", input.joyNumButtons(joyId));
						ImGui::Text("Num Hats: %d", input.joyNumHats(joyId));
						ImGui::Text("Num Axes: %d", input.joyNumAxes(joyId));
						ImGui::Separator();

						const JoystickState &joyState = input.joystickState(joyId);
						nctl::String pressedButtons;
						for (int buttonId = 0; buttonId < input.joyNumButtons(joyId); buttonId++)
						{
							if (joyState.isButtonPressed(buttonId))
								pressedButtons.formatAppend("%d ", buttonId);
						}
						ImGui::Text("Pressed buttons: %s", pressedButtons.data());

						for (int hatId = 0; hatId < input.joyNumHats(joyId); hatId++)
						{
							unsigned char hatState = joyState.hatState(hatId);
							ImGui::Text("Hat %d: %u", hatId, hatState);
						}

						for (int axisId = 0; axisId < input.joyNumAxes(joyId); axisId++)
						{
							const float axisValue = joyState.axisNormValue(axisId);
							ImGui::Text("Axis %d:", axisId);
							ImGui::SameLine();
							ImGui::ProgressBar((axisValue + 1.0f) / 2.0f);
							ImGui::SameLine();
							ImGui::Text("%.2f", axisValue);
						}

						if (input.isJoyMapped(joyId))
						{
							ImGui::Separator();
							const JoyMappedState &joyMappedState = input.joyMappedState(joyId);
							nctl::String pressedMappedButtons(64);
							for (unsigned int buttonId = 0; buttonId < JoyMappedState::NumButtons; buttonId++)
							{
								const ButtonName buttonName = static_cast<ButtonName>(buttonId);
								if (joyMappedState.isButtonPressed(buttonName))
									pressedMappedButtons.formatAppend("%s ", mappedButtonNameToString(buttonName));
							}
							ImGui::Text("Pressed buttons: %s", pressedMappedButtons.data());

							for (unsigned int axisId = 0; axisId < JoyMappedState::NumAxes; axisId++)
							{
								const AxisName axisName = static_cast<AxisName>(axisId);
								const float axisValue = joyMappedState.axisValue(axisName);
								ImGui::Text("Axis %s:", mappedAxisNameToString(axisName));
								ImGui::SameLine();
								ImGui::ProgressBar((axisValue + 1.0f) / 2.0f);
								ImGui::SameLine();
								ImGui::Text("%.2f", axisValue);
							}
						}
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
		}
		else
			ImGui::Text("No joysticks connected");
	}
}

void ImGuiDebugOverlay::guiRenderDoc()
{
#ifdef WITH_RENDERDOC
	if (RenderDocCapture::isAvailable() == false)
		return;

	if (RenderDocCapture::numCaptures() > renderDocLastNumCaptures_)
	{
		unsigned int pathLength = 0;
		uint64_t timestamp = 0;
		RenderDocCapture::captureInfo(RenderDocCapture::numCaptures() - 1, renderDocCapturePath_.data(), &pathLength, &timestamp);
		renderDocCapturePath_.setLength(pathLength);
		RenderDocCapture::setCaptureFileComments(renderDocCapturePath_.data(), renderDocFileComments_.data());
		renderDocLastNumCaptures_ = RenderDocCapture::numCaptures();
		LOGI_X("RenderDoc capture %d: %s (%lu)", RenderDocCapture::numCaptures() - 1, renderDocCapturePath_.data(), timestamp);
	}

	if (ImGui::CollapsingHeader("RenderDoc"))
	{
		int major, minor, patch;
		RenderDocCapture::apiVersion(&major, &minor, &patch);
		ImGui::Text("RenderDoc API: %d.%d.%d", major, minor, patch);
		ImGui::Text("Target control connected: %s", RenderDocCapture::isTargetControlConnected() ? "true" : "false");
		ImGui::Text("Number of captures: %u", RenderDocCapture::numCaptures());
		if (renderDocCapturePath_.isEmpty())
			ImGui::Text("Last capture path: (no capture has been made yet)");
		else
			ImGui::Text("Last capture path: %s", renderDocCapturePath_.data());
		ImGui::Separator();

		renderDocPathTemplate_ = RenderDocCapture::captureFilePathTemplate();
		if (ImGui::InputText("File path template", renderDocPathTemplate_.data(), MaxRenderDocPathLength,
		                     ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackResize,
		                     inputTextCallback, &renderDocPathTemplate_))
		{
			RenderDocCapture::setCaptureFilePathTemplate(renderDocPathTemplate_.data());
		}

		ImGui::InputTextMultiline("File comments", renderDocFileComments_.data(), MaxRenderDocCommentsLength,
		                          ImVec2(0, 0), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackResize,
		                          inputTextCallback, &renderDocFileComments_);

		static bool overlayEnabled = RenderDocCapture::isOverlayEnabled();
		ImGui::Checkbox("Enable overlay", &overlayEnabled);
		RenderDocCapture::enableOverlay(overlayEnabled);

		if (RenderDocCapture::isFrameCapturing())
			ImGui::Text("Capturing a frame...");
		else
		{
			static int numFrames = 1;
			ImGui::SetNextItemWidth(80.0f);
			ImGui::InputInt("Frames", &numFrames);
			if (numFrames < 1)
				numFrames = 1;
			ImGui::SameLine();
			if (ImGui::Button("Capture"))
				RenderDocCapture::triggerMultiFrameCapture(numFrames);
		}

		if (RenderDocCapture::isTargetControlConnected())
			ImGui::Text("Replay UI is connected");
		else
		{
			if (ImGui::Button("Launch Replay UI"))
				RenderDocCapture::launchReplayUI(1, nullptr);
		}

		static bool crashHandlerLoaded = true;
		if (crashHandlerLoaded)
		{
			if (ImGui::Button("Unload crash handler"))
			{
				RenderDocCapture::unloadCrashHandler();
				crashHandlerLoaded = false;
			}
		}
		else
			ImGui::Text("Crash handler not loaded");
	}
#endif
}

#ifdef RECORD_ALLOCATIONS
void guiAllocator(nctl::IAllocator &alloc)
{
	ImGui::Text("Allocations - Recorded: %lu, Active: %lu, Used Memory: %lu",
	            alloc.numEntries(), alloc.numAllocations(), alloc.usedMemory());
	ImGui::NewLine();

	ImGui::Columns(6, "allocationEntries", true);
	ImGui::Text("Entry");
	ImGui::NextColumn();
	ImGui::Text("Time");
	ImGui::NextColumn();
	ImGui::Text("Pointer");
	ImGui::NextColumn();
	ImGui::Text("Bytes");
	ImGui::NextColumn();
	ImGui::Text("Alignment");
	ImGui::NextColumn();
	ImGui::Text("State");
	ImGui::NextColumn();
	ImGui::Separator();
	for (unsigned int i = 0; i < alloc.numEntries(); i++)
	{
		const nctl::IAllocator::Entry &e = alloc.entry(i);
		const unsigned int deallocationIndex = alloc.findDeallocation(i);

		ImGui::Text("#%u", i);
		ImGui::NextColumn();
		ImGui::Text("%fs", e.timestamp.seconds());
		ImGui::NextColumn();
		ImGui::Text("0x%lx", uintptr_t(e.ptr));
		ImGui::NextColumn();
		ImGui::Text("%lu", e.bytes);
		ImGui::NextColumn();
		ImGui::Text("%u", e.alignment);
		ImGui::NextColumn();
		if (deallocationIndex > 0)
		{
			const TimeStamp diffStamp = alloc.entry(deallocationIndex).timestamp - e.timestamp;
			ImGui::Text("Freed by #%u after %fs", deallocationIndex, diffStamp.seconds());
		}
		else
			ImGui::Text("Active");
		ImGui::NextColumn();
	}
	ImGui::Columns(1);
}
#endif

void ImGuiDebugOverlay::guiAllocators()
{
#ifdef WITH_ALLOCATORS
	if (ImGui::CollapsingHeader("Memory Allocators"))
	{
		widgetName_.format("Default Allocator (%d allocations, %lu bytes)",
		                   nctl::theDefaultAllocator().numAllocations(), nctl::theDefaultAllocator().usedMemory());
	#ifndef RECORD_ALLOCATIONS
		ImGui::BulletText("%s", widgetName_.data());
	#else
		widgetName_.append("###DefaultAllocator");
		if (ImGui::TreeNode(widgetName_.data()))
		{
			guiAllocator(nctl::theDefaultAllocator());
			ImGui::TreePop();
		}
	#endif

		if (&nctl::theStringAllocator() != &nctl::theDefaultAllocator())
		{
			widgetName_.format("String Allocator (%d allocations, %lu bytes)",
			                   nctl::theStringAllocator().numAllocations(), nctl::theStringAllocator().usedMemory());
	#ifndef RECORD_ALLOCATIONS
			ImGui::BulletText("%s", widgetName_.data());
	#else
			widgetName_.append("###StringAllocator");
			if (ImGui::TreeNode(widgetName_.data()))
			{
				guiAllocator(nctl::theStringAllocator());
				ImGui::TreePop();
			}
	#endif
		}
		else
			ImGui::Text("The string allocator is the default one");

		if (&nctl::theImGuiAllocator() != &nctl::theDefaultAllocator())
		{
			widgetName_.format("ImGui Allocator (%d allocations, %lu bytes)",
			                   nctl::theImGuiAllocator().numAllocations(), nctl::theImGuiAllocator().usedMemory());
	#ifndef RECORD_ALLOCATIONS
			ImGui::BulletText("%s", widgetName_.data());
	#else
			widgetName_.append("###ImGuiAllocator");
			if (ImGui::TreeNode(widgetName_.data()))
			{
				guiAllocator(nctl::theImGuiAllocator());
				ImGui::TreePop();
			}
	#endif
		}
		else
			ImGui::Text("The ImGui allocator is the default one");

	#ifdef WITH_LUA
		if (&nctl::theLuaAllocator() != &nctl::theDefaultAllocator())
		{
			widgetName_.format("Lua Allocator (%d allocations, %lu bytes)",
			                   nctl::theLuaAllocator().numAllocations(), nctl::theLuaAllocator().usedMemory());
		#ifndef RECORD_ALLOCATIONS
			ImGui::BulletText("%s", widgetName_.data());
		#else
			widgetName_.append("###LuaAllocator");
			if (ImGui::TreeNode(widgetName_.data()))
			{
				guiAllocator(nctl::theLuaAllocator());
				ImGui::TreePop();
			}
		#endif
		}
		else
			ImGui::Text("The Lua allocator is the default one");
	#endif
	}

#endif
}

void ImGuiDebugOverlay::guiRescursiveChildrenNodes(SceneNode *node, unsigned int childId)
{
	DrawableNode *drawable = nullptr;
	if (node->type() != Object::ObjectType::SCENENODE &&
	    node->type() != Object::ObjectType::PARTICLE_SYSTEM)
	{
		drawable = reinterpret_cast<DrawableNode *>(node);
	}

	BaseSprite *baseSprite = nullptr;
	if (node->type() == Object::ObjectType::SPRITE ||
	    node->type() == Object::ObjectType::MESH_SPRITE ||
	    node->type() == Object::ObjectType::ANIMATED_SPRITE)
	{
		baseSprite = reinterpret_cast<BaseSprite *>(node);
	}

	MeshSprite *meshSprite = nullptr;
	if (node->type() == Object::ObjectType::MESH_SPRITE)
		meshSprite = reinterpret_cast<MeshSprite *>(node);

	ParticleSystem *particleSys = nullptr;
	if (node->type() == Object::ObjectType::PARTICLE_SYSTEM)
		particleSys = reinterpret_cast<ParticleSystem *>(node);

	TextNode *textnode = nullptr;
	if (node->type() == Object::ObjectType::TEXTNODE)
		textnode = reinterpret_cast<TextNode *>(node);

	widgetName_.format("#%u ", childId);
	if (node->name().isEmpty() == false)
		widgetName_.formatAppend("\"%s\" ", node->name().data());
	widgetName_.formatAppend("%s", nodeTypeToString(node->type()));
	const unsigned int numChildren = node->children().size();
	if (numChildren > 0)
		widgetName_.formatAppend(" (%u children)", node->children().size());
	widgetName_.formatAppend(" - position: %.1f x %.1f", node->position().x, node->position().y);
	if (drawable)
		widgetName_.formatAppend(" - size: %.1f x %.1f", drawable->width(), drawable->height());
	widgetName_.formatAppend("###0x%x", node);

	if (ImGui::TreeNode(widgetName_.data()))
	{
		ImGui::PushID(reinterpret_cast<void *>(node));
		Colorf nodeColor(node->color());
		ImGui::SliderFloat2("Position", node->position().data(), 0.0f, theApplication().width());
		if (drawable)
		{
			Vector2f nodeAnchorPoint = drawable->anchorPoint();
			ImGui::SliderFloat2("Anchor", nodeAnchorPoint.data(), 0.0f, 1.0f);
			ImGui::SameLine();
			if (ImGui::Button("Reset##Anchor"))
				nodeAnchorPoint = DrawableNode::AnchorCenter;
			drawable->setAnchorPoint(nodeAnchorPoint);
		}
		Vector2f nodeScale = node->scale();
		ImGui::SliderFloat2("Scale", nodeScale.data(), 0.01f, 3.0f);
		ImGui::SameLine();
		if (ImGui::Button("Reset##Scale"))
			nodeScale.set(1.0f, 1.0f);
		node->setScale(nodeScale);
		float nodeRotation = node->rotation();
		ImGui::SliderFloat("Rotation", &nodeRotation, 0.0f, 360.0f);
		ImGui::SameLine();
		if (ImGui::Button("Reset##Rotation"))
			nodeRotation = 0.0f;
		node->setRotation(nodeRotation);
		ImGui::ColorEdit4("Color", nodeColor.data());
		ImGui::SameLine();
		if (ImGui::Button("Reset##Color"))
			nodeColor.set(1.0f, 1.0f, 1.0f, 1.0f);
		node->setColor(nodeColor);

		if (drawable)
		{
			int layer = drawable->layer();
			ImGui::PushItemWidth(100.0f);
			ImGui::InputInt("Layer", &layer);
			ImGui::PopItemWidth();
			if (layer < DrawableNode::LayerBase::LOWEST)
				layer = DrawableNode::LayerBase::LOWEST;
			else if (layer > DrawableNode::LayerBase::HIGHEST)
				layer = DrawableNode::LayerBase::HIGHEST;
			drawable->setLayer(static_cast<unsigned short>(layer));

			ImGui::SameLine();
			bool isBlendingEnabled = drawable->isBlendingEnabled();
			ImGui::Checkbox("Blending", &isBlendingEnabled);
			drawable->setBlendingEnabled(isBlendingEnabled);
		}

		if (baseSprite)
		{
			ImGui::Text("Texture: \"%s\" (%d x %d)", baseSprite->texture()->name().data(),
			            baseSprite->texture()->width(), baseSprite->texture()->height());

			bool isFlippedX = baseSprite->isFlippedX();
			ImGui::Checkbox("Flipped X", &isFlippedX);
			baseSprite->setFlippedX(isFlippedX);
			ImGui::SameLine();
			bool isFlippedY = baseSprite->isFlippedY();
			ImGui::Checkbox("Flipped Y", &isFlippedY);
			baseSprite->setFlippedY(isFlippedY);

			const Texture *tex = baseSprite->texture();
			Recti texRect = baseSprite->texRect();
			int minX = texRect.x;
			int maxX = minX + texRect.w;
			ImGui::DragIntRange2("Rect X", &minX, &maxX, 1.0f, 0, tex->width());

			int minY = texRect.y;
			int maxY = minY + texRect.h;
			ImGui::DragIntRange2("Rect Y", &minY, &maxY, 1.0f, 0, tex->height());

			texRect.x = minX;
			texRect.w = maxX - minX;
			texRect.y = minY;
			texRect.h = maxY - minY;
			const Recti oldRect = baseSprite->texRect();
			if (oldRect.x != texRect.x || oldRect.y != texRect.y ||
			    oldRect.w != texRect.w || oldRect.h != texRect.h)
			{
				baseSprite->setTexRect(texRect);
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset##TexRect"))
				texRect = Recti(0, 0, tex->width(), tex->height());
		}

		if (meshSprite)
			ImGui::Text("Vertices: %u, Indices: %u", meshSprite->numVertices(), meshSprite->numIndices());
		else if (particleSys)
		{
			const float aliveFraction = particleSys->numAliveParticles() / static_cast<float>(particleSys->numParticles());
			widgetName_.format("%u / %u", particleSys->numAliveParticles(), particleSys->numParticles());
			ImGui::ProgressBar(aliveFraction, ImVec2(0.0f, 0.0f), widgetName_.data());
			ImGui::SameLine();
			if (ImGui::Button("Kill All##Particles"))
				particleSys->killParticles();
		}
		if (textnode)
		{
			nctl::String textnodeString(textnode->string().capacity());
			textnodeString.assign(textnode->string());
			if (ImGui::InputTextMultiline("String", textnodeString.data(), textnodeString.capacity(),
			                              ImVec2(0.0f, 3.0f * ImGui::GetTextLineHeightWithSpacing()),
			                              ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackResize,
			                              inputTextCallback, &textnodeString))
			{
				textnode->setString(textnodeString);
			}
		}

		bool updateEnabled = node->updateEnabled();
		ImGui::Checkbox("Update", &updateEnabled);
		node->setUpdateEnabled(updateEnabled);
		ImGui::SameLine();
		bool drawEnabled = node->drawEnabled();
		ImGui::Checkbox("Draw", &drawEnabled);
		node->setDrawEnabled(drawEnabled);
		ImGui::SameLine();
		bool deleteChildrenOnDestruction = node->deleteChildrenOnDestruction();
		ImGui::Checkbox("Delete Children on Destruction", &deleteChildrenOnDestruction);
		node->setDeleteChildrenOnDestruction(deleteChildrenOnDestruction);

		if (ImGui::TreeNode("Absolute Measures"))
		{
			if (drawable)
				ImGui::Text("Absolute Size: %.1f x %.1f", drawable->absWidth(), drawable->absHeight());
			ImGui::Text("Absolute Position: %.1f, %.1f", node->absPosition().x, node->absPosition().y);
			ImGui::Text("Absolute Anchor Points: %.1f, %.1f", node->absAnchorPoint().x, node->absAnchorPoint().y);
			ImGui::Text("Absolute Scale Factors: %.1f, %.1f", node->absScale().x, node->absScale().y);
			ImGui::Text("Absolute Rotation: %.1f", node->absRotation());

			ImGui::TreePop();
		}

		if (numChildren > 0)
		{
			if (ImGui::TreeNode("Child Nodes"))
			{
				const nctl::Array<SceneNode *> &children = node->children();
				for (unsigned int i = 0; i < children.size(); i++)
					guiRescursiveChildrenNodes(children[i], i);
				ImGui::TreePop();
			}
		}

		ImGui::PopID();
		ImGui::TreePop();
	}
}

void ImGuiDebugOverlay::guiNodeInspector()
{
	if (theApplication().appConfiguration().withScenegraph)
	{
		if (ImGui::CollapsingHeader("Node Inspector"))
			guiRescursiveChildrenNodes(&theApplication().rootNode(), 0);
	}
}

void ImGuiDebugOverlay::guiTopLeft()
{
	const RenderStatistics::VaoPool &vaoPool = RenderStatistics::vaoPool();
	const RenderStatistics::Textures &textures = RenderStatistics::textures();
	const RenderStatistics::CustomBuffers &customVbos = RenderStatistics::customVBOs();
	const RenderStatistics::CustomBuffers &customIbos = RenderStatistics::customIBOs();
	const RenderStatistics::Buffers &vboBuffers = RenderStatistics::buffers(RenderBuffersManager::BufferTypes::ARRAY);
	const RenderStatistics::Buffers &iboBuffers = RenderStatistics::buffers(RenderBuffersManager::BufferTypes::ELEMENT_ARRAY);
	const RenderStatistics::Buffers &uboBuffers = RenderStatistics::buffers(RenderBuffersManager::BufferTypes::UNIFORM);

	const ImVec2 windowPos = ImVec2(Margin, Margin);
	const ImVec2 windowPosPivot = ImVec2(0.0f, 0.0f);
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_FirstUseEver, windowPosPivot);
	ImGui::SetNextWindowBgAlpha(Transparency);
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	if (lockOverlayPositions_)
		windowFlags |= ImGuiWindowFlags_NoMove;
	if (showTopLeftOverlay_ && ImGui::Begin("###Top-Left", nullptr, windowFlags))
	{
		ImGui::Text("Culled nodes: %u", RenderStatistics::culled());
		if (plotOverlayValues_)
		{
			ImGui::SameLine();
			ImGui::PlotLines("", plotValues_[ValuesType::CULLED_NODES].get(), numValues_, 0, nullptr, 0.0f, FLT_MAX);
		}

		ImGui::Text("%u/%u VAOs (%u reuses, %u bindings)", vaoPool.size, vaoPool.capacity, vaoPool.reuses, vaoPool.bindings);
		ImGui::Text("%.2f Kb in %u Texture(s)", textures.dataSize / 1024.0f, textures.count);
		ImGui::Text("%.2f Kb in %u custom VBO(s)", customVbos.dataSize / 1024.0f, customVbos.count);
		ImGui::Text("%.2f Kb in %u custom IBO(s)", customIbos.dataSize / 1024.0f, customIbos.count);
		ImGui::Text("%.2f/%lu Kb in %u VBO(s)", vboBuffers.usedSpace / 1024.0f, vboBuffers.size / 1024, vboBuffers.count);
		if (plotOverlayValues_)
		{
			ImGui::SameLine();
			ImGui::PlotLines("", plotValues_[ValuesType::VBO_USED].get(), numValues_, 0, nullptr, 0.0f, vboBuffers.size / 1024.0f);
		}

		ImGui::Text("%.2f/%lu Kb in %u IBO(s)", iboBuffers.usedSpace / 1024.0f, iboBuffers.size / 1024, iboBuffers.count);
		if (plotOverlayValues_)
		{
			ImGui::SameLine();
			ImGui::PlotLines("", plotValues_[ValuesType::IBO_USED].get(), numValues_, 0, nullptr, 0.0f, iboBuffers.size / 1024.0f);
		}

		ImGui::Text("%.2f/%lu Kb in %u UBO(s)", uboBuffers.usedSpace / 1024.0f, uboBuffers.size / 1024, uboBuffers.count);
		if (plotOverlayValues_)
		{
			ImGui::SameLine();
			ImGui::PlotLines("", plotValues_[ValuesType::UBO_USED].get(), numValues_, 0, nullptr, 0.0f, uboBuffers.size / 1024.0f);
		}
		ImGui::End();
	}
}

void ImGuiDebugOverlay::guiTopRight()
{
	const ImVec2 windowPos = ImVec2(ImGui::GetIO().DisplaySize.x - Margin, Margin);
	const ImVec2 windowPosPivot = ImVec2(1.0f, 0.0f);
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_FirstUseEver, windowPosPivot);
	ImGui::SetNextWindowBgAlpha(Transparency);
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	if (lockOverlayPositions_)
		windowFlags |= ImGuiWindowFlags_NoMove;
	if (showTopRightOverlay_ && ImGui::Begin("###Top-Right", nullptr, windowFlags))
	{
		ImGui::Text("FPS: %.0f (%.2fms)", 1.0f / theApplication().interval(), theApplication().interval() * 1000.0f);
		ImGui::Text("Num Frames: %lu", theApplication().numFrames());

		const AppConfiguration &appCfg = theApplication().appConfiguration();
		if (appCfg.withScenegraph)
		{
			const RenderStatistics::Commands &spriteCommands = RenderStatistics::commands(RenderCommand::CommandTypes::SPRITE);
			const RenderStatistics::Commands &meshspriteCommands = RenderStatistics::commands(RenderCommand::CommandTypes::MESH_SPRITE);
			const RenderStatistics::Commands &particleCommands = RenderStatistics::commands(RenderCommand::CommandTypes::PARTICLE);
			const RenderStatistics::Commands &textCommands = RenderStatistics::commands(RenderCommand::CommandTypes::TEXT);
			const RenderStatistics::Commands &imguiCommands = RenderStatistics::commands(RenderCommand::CommandTypes::IMGUI);
#ifdef WITH_NUKLEAR
			const RenderStatistics::Commands &nuklearCommands = RenderStatistics::commands(RenderCommand::CommandTypes::NUKLEAR);
#endif
			const RenderStatistics::Commands &allCommands = RenderStatistics::allCommands();

			ImGui::Text("Sprites: %uV, %uDC (%u Tr), %uI/%uB", spriteCommands.vertices, spriteCommands.commands, spriteCommands.transparents, spriteCommands.instances, spriteCommands.batchSize);
			if (plotOverlayValues_)
			{
				ImGui::SameLine();
				ImGui::PlotLines("", plotValues_[ValuesType::SPRITE_VERTICES].get(), numValues_, 0, nullptr, 0.0f, FLT_MAX);
			}

			ImGui::Text("Mesh Sprites: %uV, %uDC (%u Tr), %uI/%uB", meshspriteCommands.vertices, meshspriteCommands.commands, meshspriteCommands.transparents, meshspriteCommands.instances, meshspriteCommands.batchSize);
			if (plotOverlayValues_)
			{
				ImGui::SameLine();
				ImGui::PlotLines("", plotValues_[ValuesType::MESHSPRITE_VERTICES].get(), numValues_, 0, nullptr, 0.0f, FLT_MAX);
			}

			ImGui::Text("Particles: %uV, %uDC (%u Tr), %uI/%uB\n", particleCommands.vertices, particleCommands.commands, particleCommands.transparents, particleCommands.instances, particleCommands.batchSize);
			if (plotOverlayValues_)
			{
				ImGui::SameLine();
				ImGui::PlotLines("", plotValues_[ValuesType::PARTICLE_VERTICES].get(), numValues_, 0, nullptr, 0.0f, FLT_MAX);
			}

			ImGui::Text("Text: %uV, %uDC (%u Tr), %uI/%uB", textCommands.vertices, textCommands.commands, textCommands.transparents, textCommands.instances, textCommands.batchSize);
			if (plotOverlayValues_)
			{
				ImGui::SameLine();
				ImGui::PlotLines("", plotValues_[ValuesType::TEXT_VERTICES].get(), numValues_, 0, nullptr, 0.0f, FLT_MAX);
			}

			ImGui::Text("ImGui: %uV, %uDC (%u Tr), %uI/%u", imguiCommands.vertices, imguiCommands.commands, imguiCommands.transparents, imguiCommands.instances, imguiCommands.batchSize);
			if (plotOverlayValues_)
			{
				ImGui::SameLine();
				ImGui::PlotLines("", plotValues_[ValuesType::IMGUI_VERTICES].get(), numValues_, 0, nullptr, 0.0f, FLT_MAX);
			}

#ifdef WITH_NUKLEAR
			ImGui::Text("Nuklear: %uV, %uDC (%u Tr), %uI/%u", nuklearCommands.vertices, nuklearCommands.commands, nuklearCommands.transparents, nuklearCommands.instances, nuklearCommands.batchSize);
			if (plotOverlayValues_)
			{
				ImGui::SameLine();
				ImGui::PlotLines("", plotValues_[ValuesType::NUKLEAR_VERTICES].get(), numValues_, 0, nullptr, 0.0f, FLT_MAX);
			}
#endif

			ImGui::Text("Total: %uV, %uDC (%u Tr), %uI/%uB", allCommands.vertices, allCommands.commands, allCommands.transparents, allCommands.instances, allCommands.batchSize);
			if (plotOverlayValues_)
			{
				ImGui::SameLine();
				ImGui::PlotLines("", plotValues_[ValuesType::TOTAL_VERTICES].get(), numValues_, 0, nullptr, 0.0f, FLT_MAX);
			}
		}
		ImGui::End();
	}
}

void ImGuiDebugOverlay::guiBottomLeft()
{
	const ImVec2 windowPos = ImVec2(Margin, ImGui::GetIO().DisplaySize.y - Margin);
	const ImVec2 windowPosPivot = ImVec2(0.0f, 1.0f);
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_FirstUseEver, windowPosPivot);
	ImGui::SetNextWindowBgAlpha(Transparency);
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	if (lockOverlayPositions_)
		windowFlags |= ImGuiWindowFlags_NoMove;
	if (showBottomLeftOverlay_ && ImGui::Begin("###Bottom-Left", nullptr, windowFlags))
	{
#ifdef WITH_GIT_VERSION
		ImGui::Text("%s (%s)", VersionStrings::Version, VersionStrings::GitBranch);
#else
		ImGui::Text("%s at %s", VersionStrings::CompilationDate, VersionStrings::CompilationTime);
#endif
		ImGui::End();
	}
}

void ImGuiDebugOverlay::guiBottomRight()
{
#ifdef WITH_LUA
	// Do not show statistics if there are no registered state managers
	if (LuaStatistics::numRegistered() == 0)
		return;

	const ImVec2 windowPos = ImVec2(ImGui::GetIO().DisplaySize.x - Margin, ImGui::GetIO().DisplaySize.y - Margin);
	const ImVec2 windowPosPivot = ImVec2(1.0f, 1.0f);
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_FirstUseEver, windowPosPivot);
	ImGui::SetNextWindowBgAlpha(Transparency);
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	if (lockOverlayPositions_)
		windowFlags |= ImGuiWindowFlags_NoMove;
	if (showBottomRightOverlay_ && ImGui::Begin("###Bottom-Right", nullptr, windowFlags))
	{
		ImGui::Text("%u Lua state(s) with %u tracked userdata", LuaStatistics::numRegistered(), LuaStatistics::numTrackedUserDatas());
		ImGui::Text("Used memory: %zu Kb", LuaStatistics::usedMemory() / 1024);
		if (plotOverlayValues_)
		{
			ImGui::SameLine();
			ImGui::PlotLines("", plotValues_[ValuesType::LUA_USED].get(), numValues_, 0, nullptr, 0.0f, FLT_MAX);
		}

		ImGui::Text("Operations: %d ops/s", LuaStatistics::operations());
		if (plotOverlayValues_)
		{
			ImGui::SameLine();
			ImGui::PlotLines("", plotValues_[ValuesType::LUA_OPERATIONS].get(), numValues_, 0, nullptr, 0.0f, FLT_MAX);
		}

		ImGui::Text("Textures: %u, Sprites: %u, Mesh sprites: %u",
		            LuaStatistics::numTypedUserDatas(LuaTypes::UserDataType::TEXTURE),
		            LuaStatistics::numTypedUserDatas(LuaTypes::UserDataType::SPRITE),
		            LuaStatistics::numTypedUserDatas(LuaTypes::UserDataType::MESH_SPRITE));
		ImGui::Text("Animated sprites: %u, Fonts: %u, Textnodes: %u",
		            LuaStatistics::numTypedUserDatas(LuaTypes::UserDataType::ANIMATED_SPRITE),
		            LuaStatistics::numTypedUserDatas(LuaTypes::UserDataType::FONT),
		            LuaStatistics::numTypedUserDatas(LuaTypes::UserDataType::TEXTNODE));
		ImGui::Text("Audio buffers: %u, Buffer players: %u\n",
		            LuaStatistics::numTypedUserDatas(LuaTypes::UserDataType::AUDIOBUFFER),
		            LuaStatistics::numTypedUserDatas(LuaTypes::UserDataType::AUDIOBUFFER_PLAYER));
		ImGui::Text("Stream players: %u, Particle systems: %u",
		            LuaStatistics::numTypedUserDatas(LuaTypes::UserDataType::AUDIOSTREAM_PLAYER),
		            LuaStatistics::numTypedUserDatas(LuaTypes::UserDataType::PARTICLE_SYSTEM));
		ImGui::End();
	}
#endif
}

void ImGuiDebugOverlay::guiPlots()
{
	const float appWidth = theApplication().width();

	const ImVec2 windowPos = ImVec2(appWidth * 0.5f, ImGui::GetIO().DisplaySize.y - Margin);
	const ImVec2 windowPosPivot = ImVec2(0.5f, 1.0f);
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_FirstUseEver, windowPosPivot);
	ImGui::SetNextWindowBgAlpha(Transparency);
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	if (lockOverlayPositions_)
		windowFlags |= ImGuiWindowFlags_NoMove;
	if (ImGui::Begin("###Plots", nullptr, windowFlags))
	{
		ImGui::PlotLines("Frame time", plotValues_[ValuesType::FRAME_TIME].get(), numValues_, 0, nullptr, 0.0f, maxFrameTime_, ImVec2(appWidth * 0.33f, 0.0f));

		const AppConfiguration &appCfg = theApplication().appConfiguration();
		if (appCfg.withScenegraph)
		{
			ImGui::Separator();
			ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
			ImGui::PlotLines("Update", plotValues_[ValuesType::UPDATE].get(), numValues_, 0, nullptr, 0.0f, maxUpdateVisitDraw_, ImVec2(appWidth * 0.33f, 0.0f));
			ImGui::PopStyleColor();
			ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
			ImGui::PlotLines("Visit", plotValues_[ValuesType::VISIT].get(), numValues_, 0, nullptr, 0.0f, maxUpdateVisitDraw_, ImVec2(appWidth * 0.33f, 0.0f));
			ImGui::PopStyleColor();
			ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
			ImGui::PlotLines("Draw", plotValues_[ValuesType::DRAW].get(), numValues_, 0, nullptr, 0.0f, maxUpdateVisitDraw_, ImVec2(appWidth * 0.33f, 0.0f));
			ImGui::PopStyleColor();
			ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
			ImGui::PlotLines("Aggregated", plotValues_[ValuesType::UPDATE_VISIT_DRAW].get(), numValues_, 0, nullptr, 0.0f, maxUpdateVisitDraw_, ImVec2(appWidth * 0.33f, 0.0f));
			ImGui::PopStyleColor();
		}

		if (plotAdditionalFrameValues_)
		{
			ImGui::Separator();
			ImGui::PlotLines("onFrameStart", plotValues_[ValuesType::FRAME_START].get(), numValues_, 0, nullptr, 0.0f, maxUpdateVisitDraw_, ImVec2(appWidth * 0.33f, 0.0f));
			ImGui::PlotLines("onFrameEnd", plotValues_[ValuesType::FRAME_END].get(), numValues_, 0, nullptr, 0.0f, maxUpdateVisitDraw_, ImVec2(appWidth * 0.33f, 0.0f));
			ImGui::PlotLines("ImGui", plotValues_[ValuesType::IMGUI].get(), numValues_, 0, nullptr, 0.0f, maxUpdateVisitDraw_, ImVec2(appWidth * 0.33f, 0.0f));
#ifdef WITH_NUKLEAR
			ImGui::PlotLines("Nuklear", plotValues_[ValuesType::NUKLEAR].get(), numValues_, 0, nullptr, 0.0f, maxUpdateVisitDraw_, ImVec2(appWidth * 0.33f, 0.0f));
#endif
		}
	}
	ImGui::End();
}

void ImGuiDebugOverlay::initPlotValues()
{
	for (unsigned int type = 0; type < ValuesType::COUNT; type++)
	{
		plotValues_[type] = nctl::makeUnique<float[]>(numValues_);

		for (unsigned int i = index_; i < numValues_; i++)
			plotValues_[type][i] = 0.0f;
	}
}

void ImGuiDebugOverlay::updateOverlayTimings()
{
	const RenderStatistics::Buffers &vboBuffers = RenderStatistics::buffers(RenderBuffersManager::BufferTypes::ARRAY);
	const RenderStatistics::Buffers &iboBuffers = RenderStatistics::buffers(RenderBuffersManager::BufferTypes::ELEMENT_ARRAY);
	const RenderStatistics::Buffers &uboBuffers = RenderStatistics::buffers(RenderBuffersManager::BufferTypes::UNIFORM);

	const RenderStatistics::Commands &spriteCommands = RenderStatistics::commands(RenderCommand::CommandTypes::SPRITE);
	const RenderStatistics::Commands &meshspriteCommands = RenderStatistics::commands(RenderCommand::CommandTypes::MESH_SPRITE);
	const RenderStatistics::Commands &particleCommands = RenderStatistics::commands(RenderCommand::CommandTypes::PARTICLE);
	const RenderStatistics::Commands &textCommands = RenderStatistics::commands(RenderCommand::CommandTypes::TEXT);
	const RenderStatistics::Commands &imguiCommands = RenderStatistics::commands(RenderCommand::CommandTypes::IMGUI);
#ifdef WITH_NUKLEAR
	const RenderStatistics::Commands &nuklearCommands = RenderStatistics::commands(RenderCommand::CommandTypes::NUKLEAR);
#endif
	const RenderStatistics::Commands &allCommands = RenderStatistics::allCommands();

	plotValues_[ValuesType::CULLED_NODES][index_] = static_cast<float>(RenderStatistics::culled());
	plotValues_[ValuesType::VBO_USED][index_] = vboBuffers.usedSpace / 1024.0f;
	plotValues_[ValuesType::IBO_USED][index_] = iboBuffers.usedSpace / 1024.0f;
	plotValues_[ValuesType::UBO_USED][index_] = uboBuffers.usedSpace / 1024.0f;

	plotValues_[ValuesType::SPRITE_VERTICES][index_] = static_cast<float>(spriteCommands.vertices);
	plotValues_[ValuesType::MESHSPRITE_VERTICES][index_] = static_cast<float>(meshspriteCommands.vertices);
	plotValues_[ValuesType::PARTICLE_VERTICES][index_] = static_cast<float>(particleCommands.vertices);
	plotValues_[ValuesType::TEXT_VERTICES][index_] = static_cast<float>(textCommands.vertices);
	plotValues_[ValuesType::IMGUI_VERTICES][index_] = static_cast<float>(imguiCommands.vertices);
#ifdef WITH_NUKLEAR
	plotValues_[ValuesType::NUKLEAR_VERTICES][index_] = static_cast<float>(nuklearCommands.vertices);
#endif
	plotValues_[ValuesType::TOTAL_VERTICES][index_] = static_cast<float>(allCommands.vertices);

#ifdef WITH_LUA
	plotValues_[ValuesType::LUA_USED][index_] = LuaStatistics::usedMemory() / 1024.0f;
	plotValues_[ValuesType::LUA_OPERATIONS][index_] = static_cast<float>(LuaStatistics::operations());
#endif
}

}
