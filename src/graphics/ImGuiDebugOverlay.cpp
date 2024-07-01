#include <nctl/HashMapIterator.h>

#include "imgui.h"
#include "ImGuiDebugOverlay.h"
#include "Application.h"
#include "IInputManager.h"
#include "InputEvents.h"
#include "Vector2.h"

#include "Viewport.h"
#include "Camera.h"
#include "DrawableNode.h"
#include "MeshSprite.h"
#include "ParticleSystem.h"
#include "TextNode.h"

#ifdef WITH_AUDIO
	#include "IAudioPlayer.h"
#endif

#include "IFrameTimer.h"
#include "RenderStatistics.h"
#include "RenderResources.h"
#include "BinaryShaderCache.h"
#include "Hash64.h"

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
			case Object::ObjectType::PARTICLE: return "Particle";
			case Object::ObjectType::PARTICLE_SYSTEM: return "ParticleSystem";
			case Object::ObjectType::TEXTNODE: return "TextNode";
			default: return "N/A";
		}
	}

#if defined(WITH_OPENGLES) || defined(__EMSCRIPTEN__)
	const char *openglApiName = "OpenGL ES";
#else
	const char *openglApiName = "OpenGL";
#endif
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
		if (appCfg.withScenegraph)
			plotValues_[ValuesType::POST_UPDATE][index_] = timings[Application::Timings::POST_UPDATE];
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
			case ButtonName::MISC1: return "Misc1";
			case ButtonName::PADDLE1: return "Paddle1";
			case ButtonName::PADDLE2: return "Paddle2";
			case ButtonName::PADDLE3: return "Paddle3";
			case ButtonName::PADDLE4: return "Paddle4";
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
	ImGui::Begin("Debug Overlay", &settings_.showInterface);

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
	guiFrameTimer();
	guiLog();
	guiGraphicsCapabilities();
	guiApplicationConfiguration();
	if (appCfg.withScenegraph)
		guiRenderingSettings();
	guiWindowSettings();
	guiAudioCapabilities();
	guiAudioPlayers();
	guiInputState();
	guiBinaryShaderCache();
	guiRenderDoc();
	guiAllocators();
	if (appCfg.withScenegraph)
		guiNodeInspector();

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
			ImGui::SliderFloat("Graphs update time", &updateTime_, 0.0f, 1.0f, "%.3f s");
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
			windowBorder = (style.WindowBorderSize > 0.0f);
			ImGui::Checkbox("Window Border", &windowBorder);
			style.WindowBorderSize = windowBorder ? 1.0f : 0.0f;
			ImGui::SameLine();
			static bool frameBorder = true;
			frameBorder = (style.FrameBorderSize > 0.0f);
			ImGui::Checkbox("Frame Border", &frameBorder);
			style.FrameBorderSize = frameBorder ? 1.0f : 0.0f;
			ImGui::SameLine();
			static bool popupBorder = true;
			popupBorder = (style.PopupBorderSize > 0.0f);
			ImGui::Checkbox("Popup Border", &popupBorder);
			style.PopupBorderSize = popupBorder ? 1.0f : 0.0f;

			const float MinScaling = 0.5f;
			const float MaxScaling = 2.0f;
			static float scaling = ImGui::GetIO().FontGlobalScale;
			ImGui::SliderFloat("Scaling", &scaling, MinScaling, MaxScaling, "%.1f");
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
			ImGui::TextUnformatted("__linux__");
#endif
#ifdef _WIN32
			ImGui::TextUnformatted("_WIN32");
#endif
#ifdef _WIN64
			ImGui::TextUnformatted("_WIN64");
#endif
#ifdef __ANDROID__
			ImGui::TextUnformatted("__ANDROID__");
#endif
#ifdef __ANDROID_API__
			ImGui::Text("__ANDROID_API__ = %d", static_cast<int>(__ANDROID_API__));
#endif
#ifdef __APPLE__
			ImGui::TextUnformatted("__APPLE__");
#endif
#ifdef _MSC_VER
			ImGui::Text("_MSC_VER = %d", _MSC_VER);
#endif
#ifdef __MINGW32__
			ImGui::TextUnformatted("__MINGW32__");
#endif
#ifdef __MINGW64__
			ImGui::TextUnformatted("__MINGW64__");
#endif
#ifdef __EMSCRIPTEN__
			ImGui::TextUnformatted("__EMSCRIPTEN__");
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
			ImGui::TextUnformatted("WITH_THREADS");
#endif
#ifdef WITH_OPENGLES
			ImGui::TextUnformatted("WITH_OPENGLES");
#endif
#ifdef WITH_ANGLE
			ImGui::TextUnformatted("WITH_ANGLE");
#endif
#ifdef WITH_GLEW
			ImGui::TextUnformatted("WITH_GLEW");
#endif
#ifdef WITH_GLFW
			ImGui::TextUnformatted("WITH_GLFW");
#endif
#ifdef WITH_SDL
			ImGui::TextUnformatted("WITH_SDL");
#endif
#ifdef WITH_QT5
			ImGui::TextUnformatted("WITH_QT5");
#endif
#ifdef WITH_AUDIO
			ImGui::TextUnformatted("WITH_AUDIO");
#endif
#ifdef WITH_VORBIS
			ImGui::TextUnformatted("WITH_VORBIS");
#endif
#ifdef WITH_PNG
			ImGui::TextUnformatted("WITH_PNG");
#endif
#ifdef WITH_WEBP
			ImGui::TextUnformatted("WITH_WEBP");
#endif
#ifdef WITH_LUA
			ImGui::TextUnformatted("WITH_LUA");
#endif
#ifdef WITH_SCRIPTING_API
			ImGui::TextUnformatted("WITH_SCRIPTING_API");
#endif
#ifdef WITH_ALLOCATORS
			ImGui::TextUnformatted("WITH_ALLOCATORS");
#endif
#ifdef WITH_IMGUI
			ImGui::TextUnformatted("WITH_IMGUI");
#endif
#ifdef WITH_NUKLEAR
			ImGui::TextUnformatted("WITH_NUKLEAR");
#endif
#ifdef WITH_TRACY
			ImGui::TextUnformatted("WITH_TRACY");
#endif
#ifdef WITH_RENDERDOC
			ImGui::TextUnformatted("WITH_RENDERDOC");
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

		ImGui::Text("Pre-Init Time: %.3f s", timings[Application::Timings::PRE_INIT]);
		ImGui::Text("Init Time: %.3f s", timings[Application::Timings::INIT_COMMON]);
		ImGui::Text("Application Init Time: %.3f s", timings[Application::Timings::APP_INIT]);
	}
}

void ImGuiDebugOverlay::guiFrameTimer()
{
	if (ImGui::CollapsingHeader("Frame Timer"))
	{
		IFrameTimer &frameTimer = theApplication().frameTimer();

		float averageInterval = frameTimer.averageInterval();
		bool averageEnabled = frameTimer.averageEnabled();
		ImGui::BeginDisabled(averageEnabled == false);
		ImGui::SliderFloat("Average Update Time", &averageInterval, 0.05f, 1.0f, "%.2f s", ImGuiSliderFlags_AlwaysClamp);
		frameTimer.setAverageInterval(averageInterval);
		ImGui::EndDisabled();
		ImGui::SameLine();
		if (ImGui::Checkbox("##EnableAvgUpdate", &averageEnabled))
			averageInterval = averageEnabled ? theApplication().appConfiguration().profileTextUpdateTime() : 0.0f;
		frameTimer.setAverageInterval(averageInterval);

		int logLevel = static_cast<int>(frameTimer.logLevel());
		ImGui::Combo("Log Level", &logLevel, "Off\0Verbose\0Debug\0Info\0Warning\0Error\0Fatal\0");
		frameTimer.setLogLevel(static_cast<ILogger::LogLevel>(logLevel ));

		float loggingInterval = frameTimer.loggingInterval();
		bool loggingEnabled = frameTimer.loggingEnabled();
		ImGui::BeginDisabled(loggingEnabled == false);
		ImGui::SliderFloat("Print Log Time", &loggingInterval, 1.0f, 10.0f, "%.2f s", ImGuiSliderFlags_AlwaysClamp);
		frameTimer.setLoggingInterval(loggingInterval);
		ImGui::EndDisabled();
		ImGui::SameLine();
		if (ImGui::Checkbox("##EnableLogging", &loggingEnabled))
			loggingInterval = loggingEnabled ? theApplication().appConfiguration().frameTimerLogInterval : 0.0f;
		frameTimer.setLoggingInterval(loggingInterval);

		ImGui::Text("Total Number of Frames: %lu", frameTimer.totalNumberFrames());
		ImGui::Text("Average: %.2f FPS (%.2f ms)", frameTimer.averageFps(), frameTimer.averageFrameTime() * 1000.0f);
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
		ImGui::Text("%s Vendor: %s", openglApiName, glInfoStrings.vendor);
		ImGui::Text("%s Renderer: %s", openglApiName, glInfoStrings.renderer);
		ImGui::Text("%s Version: %s", openglApiName, glInfoStrings.glVersion);
		ImGui::Text("GLSL Version: %s", glInfoStrings.glslVersion);

		ImGui::Text("%s parsed version: %d.%d.%d", openglApiName,
		            gfxCaps.glVersion(IGfxCapabilities::GLVersion::MAJOR),
		            gfxCaps.glVersion(IGfxCapabilities::GLVersion::MINOR),
		            gfxCaps.glVersion(IGfxCapabilities::GLVersion::RELEASE));

		ImGui::Separator();
		ImGui::Text("GL_MAX_TEXTURE_SIZE: %d", gfxCaps.value(IGfxCapabilities::GLIntValues::MAX_TEXTURE_SIZE));
		ImGui::Text("GL_MAX_TEXTURE_IMAGE_UNITS: %d", gfxCaps.value(IGfxCapabilities::GLIntValues::MAX_TEXTURE_IMAGE_UNITS));

		const int UNCLAMPED_MAX_UNIFORM_BLOCK_SIZE = gfxCaps.value(IGfxCapabilities::GLIntValues::UNCLAMPED_MAX_UNIFORM_BLOCK_SIZE);
		const int MAX_UNIFORM_BLOCK_SIZE = gfxCaps.value(IGfxCapabilities::GLIntValues::MAX_UNIFORM_BLOCK_SIZE);
		if (UNCLAMPED_MAX_UNIFORM_BLOCK_SIZE != MAX_UNIFORM_BLOCK_SIZE)
			ImGui::Text("Unclamped GL_MAX_UNIFORM_BLOCK_SIZE: %d", UNCLAMPED_MAX_UNIFORM_BLOCK_SIZE);
		ImGui::Text("GL_MAX_UNIFORM_BLOCK_SIZE: %d", MAX_UNIFORM_BLOCK_SIZE);

		ImGui::Text("GL_MAX_UNIFORM_BUFFER_BINDINGS: %d", gfxCaps.value(IGfxCapabilities::GLIntValues::MAX_UNIFORM_BUFFER_BINDINGS));
		ImGui::Text("GL_MAX_VERTEX_UNIFORM_BLOCKS: %d", gfxCaps.value(IGfxCapabilities::GLIntValues::MAX_VERTEX_UNIFORM_BLOCKS));
		ImGui::Text("GL_MAX_FRAGMENT_UNIFORM_BLOCKS: %d", gfxCaps.value(IGfxCapabilities::GLIntValues::MAX_FRAGMENT_UNIFORM_BLOCKS));
		ImGui::Text("GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT: %d", gfxCaps.value(IGfxCapabilities::GLIntValues::UNIFORM_BUFFER_OFFSET_ALIGNMENT));
#if !defined(EMSCRIPTEN) && (!defined(WITH_OPENGLES) || (defined(WITH_OPENGLES) && GL_ES_VERSION_3_1))
		ImGui::Text("GL_MAX_VERTEX_ATTRIB_STRIDE: %d", gfxCaps.value(IGfxCapabilities::GLIntValues::MAX_VERTEX_ATTRIB_STRIDE));
#endif
		ImGui::Text("GL_MAX_COLOR_ATTACHMENTS: %d", gfxCaps.value(IGfxCapabilities::GLIntValues::MAX_COLOR_ATTACHMENTS));
		ImGui::Text("GL_NUM_PROGRAM_BINARY_FORMATS: %d", gfxCaps.value(IGfxCapabilities::GLIntValues::NUM_PROGRAM_BINARY_FORMATS));

#if defined(__ANDROID__) || defined(WITH_ANGLE)
		const char *getProgramBinaryExtString = "GL_OES_get_program_binary";
#else
		const char *getProgramBinaryExtString = "GL_ARB_get_program_binary";
#endif
		ImGui::Separator();
		ImGui::Text("GL_KHR_debug: %d", gfxCaps.hasExtension(IGfxCapabilities::GLExtensions::KHR_DEBUG));
		ImGui::Text("GL_ARB_texture_storage: %d", gfxCaps.hasExtension(IGfxCapabilities::GLExtensions::ARB_TEXTURE_STORAGE));
		ImGui::Text("%s: %d", getProgramBinaryExtString, gfxCaps.hasExtension(IGfxCapabilities::GLExtensions::ARB_GET_PROGRAM_BINARY));
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
#if !defined(WITH_OPENGLES) && !defined(__EMSCRIPTEN__)
		ImGui::Text("OpenGL Core: %s", appCfg.glCoreProfile() ? "true" : "false");
		ImGui::Text("OpenGL Forward: %s", appCfg.glForwardCompatible() ? "true" : "false");
#endif
		ImGui::Text("%s Major: %d", openglApiName, appCfg.glMajorVersion());
		ImGui::Text("%s Minor: %d", openglApiName, appCfg.glMinorVersion());

		ImGui::Separator();
		ImGui::Text("Data path: \"%s\"", appCfg.dataPath().data());
		ImGui::Text("Log file: \"%s\"", appCfg.logFile.data());
		ImGui::Text("Console log level: %d", static_cast<int>(appCfg.consoleLogLevel));
		ImGui::Text("File log level: %d", static_cast<int>(appCfg.fileLogLevel));
		ImGui::Text("Frametimer Log interval: %f", appCfg.frameTimerLogInterval);
		ImGui::Text("Profile text update time: %f", appCfg.profileTextUpdateTime());
		ImGui::Text("Resolution: %d x %d", appCfg.resolution.x, appCfg.resolution.y);
		ImGui::Text("Refresh Rate: %f", appCfg.refreshRate);
		widgetName_.assign("Window Position: ");
		if (appCfg.windowPosition.x == AppConfiguration::WindowPositionIgnore)
			widgetName_.append("Ignore x ");
		else
			widgetName_.formatAppend("%d x ", appCfg.windowPosition.x);
		if (appCfg.windowPosition.y == AppConfiguration::WindowPositionIgnore)
			widgetName_.append("Ignore");
		else
			widgetName_.formatAppend("%d", appCfg.windowPosition.y);
		ImGui::TextUnformatted(widgetName_.data());
		ImGui::Text("Full Screen: %s", appCfg.fullScreen ? "true" : "false");
		ImGui::Text("Resizable: %s", appCfg.resizable ? "true" : "false");
		ImGui::Text("Window Scaling: %s", appCfg.windowScaling ? "true" : "false");
		ImGui::Text("Frame Limit: %u", appCfg.frameLimit);

		ImGui::Separator();
		ImGui::Text("Window title: \"%s\"", appCfg.windowTitle.data());
		ImGui::Text("Window icon: \"%s\"", appCfg.windowIconFilename.data());

		ImGui::Separator();
		ImGui::Text("Buffer mapping: %s", appCfg.useBufferMapping ? "true" : "false");
		ImGui::Text("Defer shader queries: %s", appCfg.deferShaderQueries ? "true" : "false");
#if defined(__EMSCRIPTEN__) || defined(WITH_ANGLE)
		ImGui::Text("Fixed batch size: %u", appCfg.fixedBatchSize);
#endif
		ImGui::Text("Binary shader cache: %s", appCfg.useBinaryShaderCache ? "true" : "false");
		ImGui::Text("Shader cache directory name: \"%s\"", appCfg.shaderCacheDirname.data());
		ImGui::Text("Compile batched shaders twice: %s", appCfg.compileBatchedShadersTwice ? "true" : "false");
		ImGui::Text("VBO size: %lu", appCfg.vboSize);
		ImGui::Text("IBO size: %lu", appCfg.iboSize);
		ImGui::Text("Vao pool size: %u", appCfg.vaoPoolSize);
		ImGui::Text("RenderCommand pool size: %u", appCfg.renderCommandPoolSize);

		ImGui::Separator();
		ImGui::Text("Output audio frequency: %u", appCfg.outputAudioFrequency);
		ImGui::Text("Mono audio sources: %u", appCfg.monoAudioSources);
		ImGui::Text("Stereo audio sources: %u", appCfg.stereoAudioSources);

		ImGui::Separator();
		ImGui::Text("Debug Overlay: %s", appCfg.withDebugOverlay ? "true" : "false");
		ImGui::Text("Audio: %s", appCfg.withAudio ? "true" : "false");
		ImGui::Text("Threads: %s", appCfg.withThreads ? "true" : "false");
		ImGui::Text("Scenegraph: %s", appCfg.withScenegraph ? "true" : "false");
		ImGui::Text("VSync: %s", appCfg.withVSync ? "true" : "false");
		ImGui::Text("%s Debug Context: %s", openglApiName, appCfg.withGlDebugContext ? "true" : "false");
		ImGui::Text("Console Colors: %s", appCfg.withConsoleColors ? "true" : "false");
	}
}

void ImGuiDebugOverlay::guiRenderingSettings()
{
	if (ImGui::CollapsingHeader("Rendering Settings"))
	{
		Application::RenderingSettings &settings = theApplication().renderingSettings();

		ImGui::Checkbox("Batching", &settings.batchingEnabled);
		ImGui::SameLine();
		ImGui::Checkbox("Batching with indices", &settings.batchingWithIndices);
		ImGui::SameLine();
		ImGui::Checkbox("Culling", &settings.cullingEnabled);

		int minBatchSize = settings.minBatchSize;
		int maxBatchSize = settings.maxBatchSize;
#if defined(__EMSCRIPTEN__) || defined(WITH_ANGLE)
		const unsigned int fixedBatchSize = theApplication().appConfiguration().fixedBatchSize;
		const int maxBatchSizeRange = fixedBatchSize;
#else
		const int maxBatchSizeRange = 512;
#endif
		ImGui::DragIntRange2("Batch size", &minBatchSize, &maxBatchSize, 1.0f, 0, maxBatchSizeRange);
#if defined(__EMSCRIPTEN__) || defined(WITH_ANGLE)
		ImGui::Text("Fixed batch size: %u", fixedBatchSize);
#endif
		settings.minBatchSize = minBatchSize;
		settings.maxBatchSize = maxBatchSize;
	}
}

void ImGuiDebugOverlay::guiWindowSettings()
{
	if (ImGui::CollapsingHeader("Window Settings"))
	{
		IGfxDevice &gfxDevice = theApplication().gfxDevice();
		const unsigned int numMonitors = gfxDevice.numMonitors();
		for (unsigned int i = 0; i < numMonitors; i++)
		{
			const IGfxDevice::Monitor &monitor = gfxDevice.monitor(i);
			widgetName_.format("Monitor #%u: \"%s\"", i, monitor.name);
			if (i == gfxDevice.primaryMonitorIndex())
				widgetName_.append(" [Primary]");
			if (i == gfxDevice.windowMonitorIndex())
				widgetName_.formatAppend(" [%s]", gfxDevice.isFullScreen() ? "Full Screen" : "Window");

			if (ImGui::TreeNode(widgetName_.data()))
			{
				ImGui::Text("Position: <%d, %d>", monitor.position.x, monitor.position.y);
				ImGui::Text("DPI: <%d, %d>", monitor.dpi.x, monitor.dpi.y);
				ImGui::Text("Scale: <%.2f, %.2f>", monitor.scale.x, monitor.scale.y);

				const unsigned int numVideoModes = monitor.numVideoModes;
				widgetName_.format("%u Video Modes", numVideoModes);
				if (ImGui::TreeNode(widgetName_.data()))
				{
					for (unsigned int j = 0; j < numVideoModes; j++)
					{
						const IGfxDevice::VideoMode &videoMode = monitor.videoModes[j];
						widgetName_.format("#%u: %u x %u, %.2f Hz", j, videoMode.width, videoMode.height, videoMode.refreshRate);
						if (videoMode.redBits != 8 || videoMode.greenBits != 8 || videoMode.blueBits != 8)
							widgetName_.formatAppend(" (R%uG%uB%u)", videoMode.redBits, videoMode.greenBits, videoMode.blueBits);
						ImGui::TextUnformatted(widgetName_.data());
					}
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
		}

#if !defined(__ANDROID__) || (defined(__ANDROID__) && __ANDROID_API__ >= 30)
		static Vector2i resolution = theApplication().resolutionInt();
		static Vector2i winPosition = gfxDevice.windowPosition();
		static bool fullScreen = gfxDevice.isFullScreen();

		static int selectedVideoMode = -1;
		const IGfxDevice::VideoMode currentVideoMode = gfxDevice.currentVideoMode();
		if (fullScreen == false)
		{
			ImGui::InputInt2("Resolution", resolution.data());
			ImGui::InputInt2("Position", winPosition.data());
			selectedVideoMode = -1;
		}
		else
		{
			const int monitorIndex = gfxDevice.windowMonitorIndex();
			const IGfxDevice::Monitor &monitor = gfxDevice.monitor(monitorIndex);

			unsigned int currentVideoModeIndex = 0;
			const unsigned int numVideoModes = monitor.numVideoModes;
			comboVideoModes_.clear();
			for (unsigned int i = 0; i < numVideoModes; i++)
			{
				const IGfxDevice::VideoMode &mode = monitor.videoModes[i];
				comboVideoModes_.formatAppend("%u: %u x %u, %.2f Hz", i, mode.width, mode.height, mode.refreshRate);
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
			resolution.x = monitor.videoModes[selectedVideoMode].width;
			resolution.y = monitor.videoModes[selectedVideoMode].height;
		}

	#ifdef __ANDROID__
		ImGui::TextUnformatted("Full Screen: true");
	#else
		ImGui::Checkbox("Full Screen", &fullScreen);
	#endif
		ImGui::SameLine();
		if (ImGui::Button("Apply"))
		{
			if (fullScreen == false)
			{
				// Disable full screen, then change window size and position
				gfxDevice.setFullScreen(fullScreen);
				gfxDevice.setWindowSize(resolution);
				gfxDevice.setWindowPosition(winPosition);
			}
			else
			{
				// Set the video mode, then enable full screen
				winPosition = gfxDevice.windowPosition();
				gfxDevice.setVideoMode(selectedVideoMode);
				gfxDevice.setFullScreen(fullScreen);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
		{
	#ifndef __ANDROID__
			resolution = theApplication().appConfiguration().resolution;
			fullScreen = theApplication().appConfiguration().fullScreen;
	#endif
			gfxDevice.setFullScreen(fullScreen);
			gfxDevice.setWindowSize(resolution[0], resolution[1]);
			gfxDevice.setWindowPosition(winPosition[0], winPosition[1]);
		}
		ImGui::SameLine();
		if (ImGui::Button("Current"))
		{
			resolution = theApplication().resolutionInt();
			winPosition = gfxDevice.windowPosition();
			fullScreen = gfxDevice.isFullScreen();
			selectedVideoMode = -1;
		}

		ImGui::Text("Resizable: %s", gfxDevice.isResizable() ? "true" : "false");
#endif
	}
}

void ImGuiDebugOverlay::guiAudioCapabilities()
{
#ifdef WITH_AUDIO
	if (ImGui::CollapsingHeader("Audio Capabilities"))
	{
		const IAudioDevice &audioDevice = theServiceLocator().audioDevice();
		const IAudioDevice::Attributes &audioProps = audioDevice.attributes();

		ImGui::Text("OpenAL device name: %s", audioProps.deviceName);
		ImGui::Text("OpenAL Version: %d.%d", audioProps.majorVersion, audioProps.minorVersion);
		ImGui::Text("Output Frequency: %d", audioProps.outputFrequency);
		ImGui::Text("Mono Sources: %d", audioProps.numMonoSources);
		ImGui::Text("Stereo Sources: %d", audioProps.numStereoSources);
		ImGui::Text("Refresh Rate: %d", audioProps.refreshRate);
		ImGui::Text("Synchronous: %s", audioProps.synchronous ? "true" : "false");

		ImGui::Separator();
		ImGui::Text("ALC_EXT_EFX: %d", audioDevice.hasExtension(IAudioDevice::ALExtensions::EXT_EFX));
		ImGui::Text("ALC_SOFT_PAUSE_DEVICE: %d", audioDevice.hasExtension(IAudioDevice::ALExtensions::SOFT_PAUSE_DEVICE));
		ImGui::Text("AL_SOFT_DEFERRED_UPDATES: %d", audioDevice.hasExtension(IAudioDevice::ALExtensions::SOFT_DEFERRED_UPDATES));
		ImGui::Text("AL_SOFT_SOURCE_SPATIALIZE: %d", audioDevice.hasExtension(IAudioDevice::ALExtensions::SOFT_SOURCE_SPATIALIZE));

		ImGui::NewLine();
		if (audioDevice.hasExtension(IAudioDevice::ALExtensions::EXT_EFX))
		{
			ImGui::Text("EFX Version: %d.%d", audioProps.efxMajorVersion, audioProps.efxMinorVersion);
			ImGui::Text("Max Auxiliary Sends: %d", audioProps.maxAuxiliarySends);
		}
	}
#endif
}

void ImGuiDebugOverlay::guiAudioPlayers()
{
#ifdef WITH_AUDIO
	if (ImGui::CollapsingHeader("Audio Players"))
	{
		IAudioDevice &audioDevice = theServiceLocator().audioDevice();

		float gain = audioDevice.gain();
		ImGui::SliderFloat("Gain", &gain, IAudioPlayer::MinGain, IAudioPlayer::MaxGain, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		audioDevice.setGain(gain);

		const unsigned int maxNumSources = audioDevice.maxNumSources();
		const unsigned int numAvailableSources = audioDevice.numAvailableSources();
		const float availableSourcesFraction = numAvailableSources / float(maxNumSources);
		ImGui::ProgressBar(availableSourcesFraction, ImVec2(0.0f, 0.0f));
		ImGui::Text("Available Sources: %u / %u", numAvailableSources, maxNumSources);

		unsigned int numPlayers = audioDevice.numPlayers();
		ImGui::Text("Active Players: %u", numPlayers);

		if (numPlayers > 0)
		{
			if (ImGui::Button("Stop All"))
				audioDevice.stopPlayers();
			ImGui::SameLine();
			if (ImGui::Button("Pause All"))
				audioDevice.pausePlayers();
		}

		// Stopping or pausing players change the number of active ones
		numPlayers = audioDevice.numPlayers();
		for (unsigned int i = 0; i < numPlayers; i++)
		{
			IAudioPlayer *player = audioDevice.player(i);
			// Check if the pointer is valid as a player could have been stopped
			if (player == nullptr)
				continue;

			widgetName_.format("Player %d", i);
			if (ImGui::TreeNode(widgetName_.data()))
			{
				ImGui::Text("Source Id: %u (locked: %s)", player->sourceId(), player->isSourceLocked() ? "true" : "false");
				ImGui::Text("Buffer Id: %u", player->bufferId());
				ImGui::Text("Channels: %d", player->numChannels());
				ImGui::Text("Frequency: %d Hz", player->frequency());
				ImGui::Text("Samples: %lu", player->numSamples());
				ImGui::Text("Duration: %.3f s", player->duration());
				ImGui::Text("Buffer Size: %lu bytes", player->bufferSize());
				ImGui::NewLine();

				ImGui::Text("State: %s", audioPlayerStateToString(player->state()));
				ImGui::Text("Looping: %s", player->isLooping() ? "true" : "false");
				ImGui::Text("Gain: %f", player->gain());
				ImGui::Text("Pitch: %f", player->pitch());
				const Vector3f &pos = player->position();
				ImGui::Text("Position: <%f, %f, %f>", pos.x, pos.y, pos.z);
				const Vector3f &vel = player->velocity();
				ImGui::Text("Velocity: <%f, %f, %f>", vel.x, vel.y, vel.z);
				const Vector3f &dir = player->direction();
				ImGui::Text("Direction: <%f, %f, %f>", dir.x, dir.y, dir.z);
				ImGui::Text("Cone Inner Angle: %f", player->coneInnerAngle());
				ImGui::Text("Cone Outer Angle: %f", player->coneOuterAngle());
				ImGui::Text("Cone Outer Gain: %f", player->coneOuterGain());

	#ifdef WITH_OPENAL_EXT
				if (audioDevice.hasExtension(IAudioDevice::ALExtensions::EXT_EFX))
				{
					ImGui::NewLine();
					ImGui::Text("Air Absorption Factor: %f", player->airAbsorptionFactor());
					ImGui::Text("Room Rolloff Factor: %f", player->roomRolloffFactor());
					ImGui::Text("Cone Outer Gain HF: %f", player->coneOuterGainHF());
					ImGui::NewLine();
					ImGui::Text("Effect Slot: %d", player->effectSlotId());
					ImGui::Text("Aux Filter: %d", player->auxFilterId());
					ImGui::Text("Direct Filter: %d", player->directFilterId());
				}
	#endif
				ImGui::NewLine();

				ImGui::PushID(player);
				const bool canPlay = (player->isPlaying() == false);
				const bool canPause = (player->isPaused() == false);
				const bool canStop = (player->isStopped() == false);
				ImGui::BeginDisabled(canPlay == false);
				if (ImGui::Button("Play"))
					player->play();
				ImGui::EndDisabled();
				ImGui::SameLine();
				ImGui::BeginDisabled(canPause == false);
				if (ImGui::Button("Pause"))
					player->pause();
				ImGui::EndDisabled();
				ImGui::SameLine();
				ImGui::BeginDisabled(canStop == false);
				if (ImGui::Button("Stop"))
					player->stop();
				ImGui::EndDisabled();
				ImGui::PopID();

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
			ImGui::TextUnformatted("No joysticks connected");
	}
}

void ImGuiDebugOverlay::guiBinaryShaderCache()
{
	if (ImGui::CollapsingHeader("Binary Shader Cache"))
	{
		BinaryShaderCache &cache = RenderResources::binaryShaderCache();
		const BinaryShaderCache::Statistics &stats = cache.statistics();

		const bool isAvailable = cache.isAvailable();
		const bool isEnabled = cache.isEnabled();
		const bool canBePruned = (stats.TotalFilesCount > stats.PlatformFilesCount);
		const bool canBeCleared = (stats.TotalFilesCount > 0);

		ImGui::TextUnformatted("Available:");
		ImGui::SameLine();
		if (isAvailable)
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "true");
		else
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "false");

		if (isAvailable)
		{
			ImGui::SameLine();
			if (ImGui::Button(isEnabled ? "Disable" : "Enable"))
				cache.setEnabled(!isEnabled);
		}
		ImGui::Text("Shader source directory name: \"%s\"", RenderResources::ShadersDir);

		if (isAvailable)
		{
			ImGui::BeginDisabled(isEnabled == false);

			ImGui::Text("Directory: %s", cache.directory().data());
			// Reporting statistics for shaders hashing
			const Hash64::Statistics &hash64Stats = RenderResources::hash64().statistics();
			ImGui::Text("Hashed: %u sources (%u strings, %u characters), %u files, %u scanned",
						hash64Stats.HashStringCalls, hash64Stats.HashedStrings, hash64Stats.HashedCharacters, hash64Stats.HashedFiles, hash64Stats.ScannedHashStrings);
			ImGui::Text("Requests: %u loaded, %u saved", stats.LoadedShaders, stats.SavedShaders);
			ImGui::Text("Count: %u (total: %u)", stats.PlatformFilesCount, stats.TotalFilesCount);
			ImGui::Text("Size: %u Kb (total: %u Kb)", stats.PlatformBytesCount / 1024, stats.TotalBytesCount / 1024);

			const unsigned int numDefaultVertexShaders = static_cast<unsigned int>(RenderResources::DefaultVertexShader::COUNT);
			widgetName_.format("Default vertex shaders (%u)", numDefaultVertexShaders);
			if (ImGui::TreeNode(widgetName_.data()))
			{
				for (unsigned int i = 0; i < numDefaultVertexShaders; i++)
				{
					const RenderResources::ShaderProgramCompileInfo::ShaderCompileInfo &vertexInfo = RenderResources::defaultVertexShaderInfo(i);
					if (ImGui::TreeNode(&vertexInfo, "#%u", i))
					{
#ifndef WITH_EMBEDDED_SHADERS
						ImGui::Text("Filename: %s", vertexInfo.shaderFile);
#else
						ImGui::TextUnformatted("Source:");
						ImGui::Separator();
						ImGui::Text("%s", vertexInfo.shaderString);
						ImGui::Separator();
#endif
						if (vertexInfo.hash != 0)
							ImGui::Text("Hash: 0x%016llx", vertexInfo.hash);
						if (vertexInfo.hashString)
							ImGui::Text("Hash string: 0x%s", vertexInfo.hashString);
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}

			const unsigned int numDefaultFragmentShaders = static_cast<unsigned int>(RenderResources::DefaultFragmentShader::COUNT);
			widgetName_.format("Default fragment shaders (%u)", numDefaultFragmentShaders);
			if (ImGui::TreeNode(widgetName_.data()))
			{
				for (unsigned int i = 0; i < numDefaultFragmentShaders; i++)
				{
					const RenderResources::ShaderProgramCompileInfo::ShaderCompileInfo &fragmentInfo = RenderResources::defaultFragmentShaderInfo(i);
					if (ImGui::TreeNode(&fragmentInfo, "#%u", i))
					{
#ifndef WITH_EMBEDDED_SHADERS
						ImGui::Text("Filename: %s", fragmentInfo.shaderFile);
#else
						ImGui::TextUnformatted("Source:");
						ImGui::Separator();
						ImGui::Text("%s", fragmentInfo.shaderString);
						ImGui::Separator();
#endif
						if (fragmentInfo.hash != 0)
							ImGui::Text("Hash: 0x%016llx", fragmentInfo.hash);
						if (fragmentInfo.hashString)
							ImGui::Text("Hash string: 0x%s", fragmentInfo.hashString);
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}

			widgetName_.format("Shader Info Hashmap (%u)", cache.shaderInfoHashMap().size());
			if (ImGui::TreeNode(widgetName_.data()))
			{
				for (BinaryShaderCache::ShaderInfoHashMapType::ConstIterator i = cache.shaderInfoHashMap().begin(); i != cache.shaderInfoHashMap().end(); ++i)
				{
					const BinaryShaderCache::ShaderInfo &shaderInfo = i.value();
					if (ImGui::TreeNode(&shaderInfo, "%s", shaderInfo.objectLabel.data()))
					{
						ImGui::Text("Binary file: %s", shaderInfo.binaryFilename.data());
						ImGui::Text("Batch size: %u", shaderInfo.batchSize);
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}

			ImGui::BeginDisabled(canBePruned == false);
			if (ImGui::Button("Prune"))
				cache.prune();
			ImGui::EndDisabled();

			ImGui::SameLine();
			ImGui::BeginDisabled(canBeCleared == false);
			if (ImGui::Button("Clear"))
				cache.clear();
			ImGui::EndDisabled();

			ImGui::EndDisabled();
		}
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
			ImGui::TextUnformatted("Capturing a frame...");
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
			ImGui::TextUnformatted("Replay UI is connected");
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
			ImGui::TextUnformatted("Crash handler not loaded");
	}
#endif
}

#ifdef RECORD_ALLOCATIONS
void guiAllocator(nctl::IAllocator &alloc)
{
	ImGui::Text("Allocations - Recorded: %lu, Active: %lu, Used Memory: %lu",
	            alloc.numEntries(), alloc.numAllocations(), alloc.usedMemory());

	enum FilterState { ALL, ACTIVE, NOT_ACTIVE, FREE };
	static int selectedFilter = 0;
	ImGui::Combo("Filter", &selectedFilter, "All\0Active\0Not Active\0Free\0");

	const int tableNumRows = alloc.numEntries() > 32 ? 32 : alloc.numEntries();
	if (ImGui::BeginTable("allocatorEntries", 6, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
	                      ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_ScrollY, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * tableNumRows)))
	{
		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableSetupColumn("Entry", ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoHide);
		ImGui::TableSetupColumn("Time");
		ImGui::TableSetupColumn("Pointer");
		ImGui::TableSetupColumn("Bytes");
		ImGui::TableSetupColumn("Alignment");
		ImGui::TableSetupColumn("State");
		ImGui::TableHeadersRow();

		for (size_t i = 0; i < alloc.numEntries(); i++)
		{
			const nctl::IAllocator::Entry &e = alloc.entry(i);
			const size_t deallocationIndex = alloc.findDeallocation(i);

			if (selectedFilter != FilterState::ALL)
			{
				const bool entryIsActive = (e.alignment > 0 && deallocationIndex == nctl::IAllocator::InvalidEntryIndex);
				const bool entryIsNotActive = (e.alignment > 0 && deallocationIndex != nctl::IAllocator::InvalidEntryIndex);
				const bool entryIsFree = (e.alignment == 0);

				if ((entryIsActive && selectedFilter != FilterState::ACTIVE) ||
				    (entryIsNotActive && selectedFilter != FilterState::NOT_ACTIVE) ||
				    (entryIsFree && selectedFilter != FilterState::FREE))
				{
					continue;
				}
			}

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("#%u", i);
			ImGui::TableNextColumn();
			ImGui::Text("%f s", e.timestamp.seconds());
			ImGui::TableNextColumn();
			ImGui::Text("0x%lx", uintptr_t(e.ptr));
			ImGui::TableNextColumn();
			ImGui::Text("%lu", e.bytes);
			ImGui::TableNextColumn();
			ImGui::Text("%u", e.alignment);
			ImGui::TableNextColumn();
			if (deallocationIndex != nctl::IAllocator::InvalidEntryIndex)
			{
				const TimeStamp diffStamp = alloc.entry(deallocationIndex).timestamp - e.timestamp;
				ImGui::Text("Freed by #%lu after %f s", deallocationIndex, diffStamp.seconds());
			}
			else
			{
				if (e.alignment > 0)
					ImGui::TextUnformatted("Active");
				else
				{
					const size_t allocationIndex = alloc.findAllocationBeforeIndex(e.ptr, i);
					if (allocationIndex != nctl::IAllocator::InvalidEntryIndex)
					{
						const TimeStamp diffStamp = e.timestamp - alloc.entry(allocationIndex).timestamp;
						ImGui::Text("Free for #%lu after %f s", allocationIndex, diffStamp.seconds());
					}
					else
						ImGui::TextUnformatted("Free");
				}
			}
		}

		ImGui::EndTable();
	}
}
#endif

void ImGuiDebugOverlay::guiAllocators()
{
#ifdef WITH_ALLOCATORS
	const unsigned int NumAllocators = 6;
	const char *allocatorNames[NumAllocators] = { "Default", "String", "ImGui", "Nuklear", "Lua", "GLFW" };
	nctl::IAllocator *allocators[NumAllocators] = { &nctl::theDefaultAllocator(), &nctl::theStringAllocator(),
	                                                nullptr, nullptr, nullptr, nullptr };

	#ifdef WITH_IMGUI
	allocators[2] = &nctl::theImGuiAllocator();
	#endif
	#ifdef WITH_NUKLEAR
	allocators[3] = &nctl::theNuklearAllocator();
	#endif
	#ifdef WITH_LUA
	allocators[4] = &nctl::theLuaAllocator();
	#endif
	#ifdef WITH_GLFW
	allocators[5] = &nctl::theGlfwAllocator();
	#endif

	if (ImGui::CollapsingHeader("Memory Allocators"))
	{
		for (unsigned int i = 0; i < NumAllocators; i++)
		{
			if (allocators[i] == nullptr)
				continue;

			if (i == 0 || allocators[i] != &nctl::theDefaultAllocator())
			{
				widgetName_.format("%s Allocator \"%s\" (%d allocations, %lu bytes)",
				                   allocatorNames[i], allocators[i]->name(), allocators[i]->numAllocations(), allocators[i]->usedMemory());

	#ifdef RECORD_ALLOCATIONS
				if (allocators[i]->numEntries() > 0)
				{
					widgetName_.formatAppend("###%sAllocator", allocatorNames[i]);
					if (ImGui::TreeNode(widgetName_.data()))
					{
						guiAllocator(*allocators[i]);
						ImGui::TreePop();
					}
				}
				else
	#endif
					ImGui::BulletText("%s", widgetName_.data());
			}
			else
				ImGui::Text("The %s allocator is the default one", allocatorNames[i]);
		}
	}
#endif
}

void ImGuiDebugOverlay::guiViewports(Viewport *viewport, unsigned int viewportId)
{
	widgetName_.format("#%u Viewport", viewportId);
	if (viewport->type() != Viewport::Type::NO_TEXTURE)
		widgetName_.formatAppend(" - size: %d x %d", viewport->width(), viewport->height());
	const Recti viewportRect = viewport->viewportRect();
	widgetName_.formatAppend(" - rect: pos <%d, %d>, size %d x %d", viewportRect.x, viewportRect.y, viewportRect.w, viewportRect.h);
	const Rectf cullingRect = viewport->cullingRect();
	widgetName_.formatAppend(" - culling: pos <%.2f, %.2f>, size %.2f x %.2f", cullingRect.x, cullingRect.y, cullingRect.w, cullingRect.h);
	widgetName_.formatAppend("###0x%x", uintptr_t(viewport));

	SceneNode *rootNode = viewport->rootNode();
	if (rootNode != nullptr && ImGui::TreeNode(widgetName_.data()))
	{
		if (viewport->camera() != nullptr)
		{
			const Camera::ViewValues &viewValues = viewport->camera()->viewValues();
			ImGui::Text("Camera view - position: <%.2f, %.2f>, rotation: %.2f, scale: %.2f", viewValues.position.x, viewValues.position.y, viewValues.rotation, viewValues.scale);
			const Camera::ProjectionValues &projValues = viewport->camera()->projectionValues();
			ImGui::Text("Camera projection - left: %.2f, right: %.2f, top: %.2f, bottom: %.2f", projValues.left, projValues.right, projValues.top, projValues.bottom);
		}

		guiRecursiveChildrenNodes(rootNode, 0);
		ImGui::TreePop();
	}
}

void ImGuiDebugOverlay::guiRecursiveChildrenNodes(SceneNode *node, unsigned int childId)
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
	if (node->name() != nullptr)
		widgetName_.formatAppend("\"%s\" ", node->name());
	widgetName_.formatAppend("%s", nodeTypeToString(node->type()));
	const unsigned int numChildren = node->children().size();
	if (numChildren > 0)
		widgetName_.formatAppend(" (%u children)", node->children().size());
	widgetName_.formatAppend(" - position: %.1f x %.1f", node->position().x, node->position().y);
	if (drawable)
	{
		widgetName_.formatAppend(" - size: %.1f x %.1f", drawable->width(), drawable->height());
		if (drawable->isDrawEnabled() && drawable->lastFrameRendered() < theApplication().numFrames() - 1)
			widgetName_.append(" - culled");
	}
	widgetName_.formatAppend("###0x%x", uintptr_t(node));

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
			if (layer < 0)
				layer = 0;
			else if (layer > 0xffff)
				layer = 0xffff;
			drawable->setLayer(static_cast<uint16_t>(layer));

			ImGui::SameLine();
			ASSERT(childId == node->childOrderIndex());
			ImGui::Text("Visit order: %u", node->visitOrderIndex());

			ImGui::SameLine();
			bool isBlendingEnabled = drawable->isBlendingEnabled();
			ImGui::Checkbox("Blending", &isBlendingEnabled);
			drawable->setBlendingEnabled(isBlendingEnabled);
		}

		if (baseSprite)
		{
			ImGui::Text("Texture: \"%s\" (%d x %d)", baseSprite->texture()->name(),
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
			nctl::String textnodeString(textnode->string());
			if (ImGui::InputTextMultiline("String", textnodeString.data(), textnodeString.capacity(),
			                              ImVec2(0.0f, 3.0f * ImGui::GetTextLineHeightWithSpacing()),
			                              ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackResize,
			                              inputTextCallback, &textnodeString))
			{
				textnode->setString(textnodeString);
			}
		}

		bool updateEnabled = node->isUpdateEnabled();
		ImGui::Checkbox("Update", &updateEnabled);
		node->setUpdateEnabled(updateEnabled);
		ImGui::SameLine();
		bool drawEnabled = node->isDrawEnabled();
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
					guiRecursiveChildrenNodes(children[i], i);
				ImGui::TreePop();
			}
		}

		ImGui::PopID();
		ImGui::TreePop();
	}
}

void ImGuiDebugOverlay::guiNodeInspector()
{
	if (ImGui::CollapsingHeader("Node Inspector"))
	{
		guiViewports(&theApplication().screenViewport(), 0);
		for (unsigned int i = 0; i < Viewport::chain().size(); i++)
			guiViewports(Viewport::chain()[i], i + 1);
	}
}

void ImGuiDebugOverlay::guiTopLeft()
{
	const RenderStatistics::VaoPool &vaoPool = RenderStatistics::vaoPool();
	const RenderStatistics::CommandPool &commandPool = RenderStatistics::commandPool();
	const RenderStatistics::Textures &textures = RenderStatistics::textures();
	const RenderStatistics::CustomBuffers &customVbos = RenderStatistics::customVBOs();
	const RenderStatistics::CustomBuffers &customIbos = RenderStatistics::customIBOs();
	const RenderStatistics::Buffers &vboBuffers = RenderStatistics::buffers(RenderBuffersManager::BufferTypes::ARRAY);
	const RenderStatistics::Buffers &iboBuffers = RenderStatistics::buffers(RenderBuffersManager::BufferTypes::ELEMENT_ARRAY);
	const RenderStatistics::Buffers &uboBuffers = RenderStatistics::buffers(RenderBuffersManager::BufferTypes::UNIFORM);
	const BinaryShaderCache::Statistics &shaderCacheStats = RenderResources::binaryShaderCache().statistics();

	const ImVec2 windowPos = ImVec2(Margin, Margin);
	const ImVec2 windowPosPivot = ImVec2(0.0f, 0.0f);
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_FirstUseEver, windowPosPivot);
	ImGui::SetNextWindowBgAlpha(Transparency);
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	if (lockOverlayPositions_)
		windowFlags |= ImGuiWindowFlags_NoMove;
	if (showTopLeftOverlay_)
	{
		ImGui::Begin("###Top-Left", nullptr, windowFlags);

		ImGui::Text("Culled nodes: %u", RenderStatistics::culled());
		if (plotOverlayValues_)
		{
			ImGui::SameLine();
			ImGui::PlotLines("", plotValues_[ValuesType::CULLED_NODES].get(), numValues_, 0, nullptr, 0.0f, FLT_MAX);
		}

		ImGui::Text("%u/%u VAOs (%u reuses, %u bindings)", vaoPool.size, vaoPool.capacity, vaoPool.reuses, vaoPool.bindings);
		ImGui::Text("%u/%u RenderCommands in the pool (%u retrievals)", commandPool.usedSize, commandPool.usedSize + commandPool.freeSize, commandPool.retrievals);
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

		if (RenderResources::binaryShaderCache().isAvailable())
			ImGui::Text("Binary Shaders: %u Kb in %u file(s)", shaderCacheStats.TotalBytesCount / 1024, shaderCacheStats.TotalFilesCount);
		ImGui::Text("Viewport chain length: %u", Viewport::chain().size());

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
	if (showTopRightOverlay_)
	{
		ImGui::Begin("###Top-Right", nullptr, windowFlags);

		ImGui::Text("FPS: %.0f (%.2f ms)", 1.0f / theApplication().interval(), theApplication().interval() * 1000.0f);
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
	if (showBottomLeftOverlay_)
	{
		ImGui::Begin("###Bottom-Left", nullptr, windowFlags);
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
	if (showBottomRightOverlay_)
	{
		ImGui::Begin("###Bottom-Right", nullptr, windowFlags);

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
	ImGui::Begin("###Plots", nullptr, windowFlags);

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
		if (appCfg.withScenegraph)
			ImGui::PlotLines("onPostUpdate", plotValues_[ValuesType::POST_UPDATE].get(), numValues_, 0, nullptr, 0.0f, maxUpdateVisitDraw_, ImVec2(appWidth * 0.33f, 0.0f));
		ImGui::PlotLines("onFrameEnd", plotValues_[ValuesType::FRAME_END].get(), numValues_, 0, nullptr, 0.0f, maxUpdateVisitDraw_, ImVec2(appWidth * 0.33f, 0.0f));
		ImGui::PlotLines("ImGui", plotValues_[ValuesType::IMGUI].get(), numValues_, 0, nullptr, 0.0f, maxUpdateVisitDraw_, ImVec2(appWidth * 0.33f, 0.0f));
#ifdef WITH_NUKLEAR
		ImGui::PlotLines("Nuklear", plotValues_[ValuesType::NUKLEAR].get(), numValues_, 0, nullptr, 0.0f, maxUpdateVisitDraw_, ImVec2(appWidth * 0.33f, 0.0f));
#endif
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
