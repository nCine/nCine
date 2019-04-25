#include "imgui.h"
#include "ImGuiDebugOverlay.h"
#include "Application.h"
#include "IInputManager.h"
#include "InputEvents.h"

#include "RenderStatistics.h"
#ifdef WITH_LUA
	#include "LuaStatistics.h"
#endif

#include "version.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
///////////////////////////////////////////////////////////

ImGuiDebugOverlay::ImGuiDebugOverlay(const AppConfiguration &appCfg)
	: IDebugOverlay(appCfg), disableAppInputEvents_(false),
	  appInputHandler_(nullptr), lockOverlayPositions_(true),
	  showTopLeftOverlay_(true), showTopRightOverlay_(true),
	  showBottomLeftOverlay_(true), showBottomRightOverlay_(true),
	  numValues_(128), maxFrameTime_(0.0f), maxUpdateVisitDraw_(0.0f),
	  index_(0), plotAdditionalFrameValues_(false), plotOverlayValues_(false)
{
	if (appCfg.withProfilerGraphs)
		initPlotValues();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ImGuiDebugOverlay::update()
{
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

	const AppConfiguration &appCfg = theApplication().appConfiguration();

	guiWindow();

	if (appCfg.withInfoText && settings_.showInfoText)
	{
		guiTopLeft();
		guiTopRight();
		guiBottomLeft();
		guiBottomRight();
	}

	if (appCfg.withProfilerGraphs && settings_.showProfilerGraphs)
		guiPlots();
}

void ImGuiDebugOverlay::updateFrameTimings()
{
	if (updateTimer_.interval() > updateTime_)
	{
		const float *timings = theApplication().timings();

		plotValues_[ValuesType::FRAME_TIME][index_] = theApplication().interval();
		plotValues_[ValuesType::FRAME_START][index_] = timings[Application::Timings::FRAME_START];
		plotValues_[ValuesType::UPDATE_VISIT_DRAW][index_] = timings[Application::Timings::UPDATE] +
		                                                     timings[Application::Timings::VISIT] +
		                                                     timings[Application::Timings::DRAW];
		plotValues_[ValuesType::UPDATE][index_] = timings[Application::Timings::UPDATE];
		plotValues_[ValuesType::VISIT][index_] = timings[Application::Timings::VISIT];
		plotValues_[ValuesType::DRAW][index_] = timings[Application::Timings::DRAW];
		plotValues_[ValuesType::IMGUI][index_] = timings[Application::Timings::IMGUI];
		plotValues_[ValuesType::FRAME_END][index_] = timings[Application::Timings::FRAME_END];

		for (unsigned int i = 0; i < index_; i++)
		{
			if (plotValues_[ValuesType::FRAME_TIME][i] > maxFrameTime_)
				maxFrameTime_ = plotValues_[ValuesType::FRAME_TIME][i];
			if (plotValues_[ValuesType::UPDATE_VISIT_DRAW][i] > maxUpdateVisitDraw_)
				maxUpdateVisitDraw_ = plotValues_[ValuesType::UPDATE_VISIT_DRAW][i];
		}

		updateOverlayTimings();

		index_ = (index_ + 1) % numValues_;
		updateTimer_.start();
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

namespace {

const char *mouseCursorModeToString(IInputManager::MouseCursorMode mode)
{
	switch (mode)
	{
		case IInputManager::MouseCursorMode::NORMAL: return "Normal";
		case IInputManager::MouseCursorMode::HIDDEN: return "Hidden";
		case IInputManager::MouseCursorMode::DISABLED: return "Disabled";
	}
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
}

}

void ImGuiDebugOverlay::guiWindow()
{
	if (settings_.showInterface == false)
		return;

	const ImVec2 windowPos = ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y - 0.5f);
	const ImVec2 windowPosPivot = ImVec2(0.5f, 0.5f);
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_FirstUseEver, windowPosPivot);
	if	(ImGui::Begin("Debug Overlay", &settings_.showInterface))
	{
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
		guiRenderingSettings();
		guiWindowSettings();
		guiInputState();
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
		if (appCfg.withInfoText)
		{
			if (ImGui::TreeNodeEx("Overlays", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Checkbox("Show overlays", &settings_.showInfoText);
				ImGui::Checkbox("Lock overlay positions", &lockOverlayPositions_);
				ImGui::Checkbox("Show Top-Left", &showTopLeftOverlay_);
				ImGui::SameLine();
				ImGui::Checkbox("Show Top-Right", &showTopRightOverlay_);
				ImGui::Checkbox("Show Bottom-Left", &showBottomLeftOverlay_);
			#ifdef WITH_LUA
				ImGui::SameLine();
				ImGui::Checkbox("Show Bottom-Right", &showBottomRightOverlay_);
			#endif
				ImGui::TreePop();
			}
		}
		if (appCfg.withProfilerGraphs)
		{
			if (ImGui::TreeNodeEx("Profiling Graphs", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Checkbox("Show profiling graphs", &settings_.showProfilerGraphs);
				ImGui::Checkbox("Plot additional frame values", &plotAdditionalFrameValues_);
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
			ImGuiStyle &style =  ImGui::GetStyle();
			ImGui::SliderFloat("Frame Rounding", &style.FrameRounding, MinFrameRounding, MaxFrameRounding, "%.0f");

			if (style.FrameRounding < MinFrameRounding)
				style.FrameRounding = MinFrameRounding;
			else if (style.FrameRounding >MaxFrameRounding)
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
			ImGui::PushItemWidth(100);
			ImGui::DragFloat("Scaling", &scaling, 0.005f, MinScaling, MaxScaling, "%.1f");
			ImGui::PopItemWidth();
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
		#ifdef WITH_GLEW
			ImGui::Text("WITH_GLEW");
		#endif
		#ifdef WITH_GLFW
			ImGui::Text("WITH_GLFW");
		#endif
		#ifdef WITH_SDL
			ImGui::Text("WITH_SDL");
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
		#ifdef WITH_IMGUI
			ImGui::Text("WITH_IMGUI");
		#endif
		#ifdef WITH_TRACY
			ImGui::Text("WITH_TRACY");
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
		ImGui::Text("Text update time: %f", appCfg.profileTextUpdateTime());
		ImGui::Text("Resolution: %u x %u", appCfg.xResolution, appCfg.yResolution);
		ImGui::Text("Full Screen: %s", appCfg.inFullscreen ? "true" : "false");
		ImGui::Text("Resizable: %s", appCfg.isResizable ? "true" : "false");

		ImGui::Separator();
		ImGui::Text("Window title: %s", appCfg.windowTitle.data());
		ImGui::Text("Window icon: %s", appCfg.windowIconFilename.data());
		ImGui::Text("Font texture: %s", appCfg.fontTexFilename.data());
		ImGui::Text("Font Fnt file: %s", appCfg.fontFntFilename.data());

		ImGui::Separator();
		ImGui::Text("VBO size: %u", appCfg.vboSize);
		ImGui::Text("IBO size: %u", appCfg.iboSize);
		ImGui::Text("Vao pool size: %u", appCfg.vaoPoolSize);

		ImGui::Separator();
		ImGui::Text("Profiler graphs: %s", appCfg.withProfilerGraphs ? "true" : "false");
		ImGui::Text("Information text: %s", appCfg.withInfoText ? "true" : "false");
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
		ImGui::InputInt2("Resolution", resolution);

		static bool fullScreen = theApplication().gfxDevice().isFullScreen();
		ImGui::Checkbox("Full Screen", &fullScreen);

		ImGui::SameLine();
		if (ImGui::Button("Apply"))
		{
			theApplication().gfxDevice().setResolution(resolution[0], resolution[1]);
			theApplication().gfxDevice().setFullScreen(fullScreen);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
		{
			resolution[0] = theApplication().appConfiguration().xResolution;
			resolution[1] = theApplication().appConfiguration().yResolution;
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
		}

		ImGui::Text("Resizable: %s", theApplication().gfxDevice().isResizable() ? "true" : "false");
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
			nctl::String widgetName;
			widgetName.format("%d Joystick(s)", numConnectedJoysticks);
			if (ImGui::TreeNode(widgetName.data()))
			{
				ImGui::Text("Joystick mappings: %u", input.numJoyMappings());

				for (int joyId = 0; joyId < IInputManager::MaxNumJoysticks; joyId++)
				{
					if (input.isJoyPresent(joyId) == false)
						continue;

					widgetName.format("Joystick %d", joyId);
					if (ImGui::TreeNode(widgetName.data()))
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
	const RenderStatistics::Commands &spriteCommands = RenderStatistics::commands(RenderCommand::CommandTypes::SPRITE);
	const RenderStatistics::Commands &meshspriteCommands = RenderStatistics::commands(RenderCommand::CommandTypes::MESH_SPRITE);
	const RenderStatistics::Commands &particleCommands = RenderStatistics::commands(RenderCommand::CommandTypes::PARTICLE);
	const RenderStatistics::Commands &textCommands = RenderStatistics::commands(RenderCommand::CommandTypes::TEXT);
	const RenderStatistics::Commands &imguiCommands = RenderStatistics::commands(RenderCommand::CommandTypes::IMGUI);
	const RenderStatistics::Commands &allCommands = RenderStatistics::allCommands();

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

		ImGui::Text("Total: %uV, %uDC (%u Tr), %uI/%uB", allCommands.vertices, allCommands.commands, allCommands.transparents, allCommands.instances, allCommands.batchSize);
		if (plotOverlayValues_)
		{
			ImGui::SameLine();
			ImGui::PlotLines("", plotValues_[ValuesType::TOTAL_VERTICES].get(), numValues_, 0, nullptr, 0.0f, FLT_MAX);
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
		ImGui::Separator();
		if (plotAdditionalFrameValues_)
		{
			ImGui::PlotLines("onFrameStart", plotValues_[ValuesType::FRAME_START].get(), numValues_, 0, nullptr, 0.0f, maxUpdateVisitDraw_, ImVec2(appWidth * 0.33f, 0.0f));
			ImGui::PlotLines("onFrameEnd", plotValues_[ValuesType::FRAME_END].get(), numValues_, 0, nullptr, 0.0f, maxUpdateVisitDraw_, ImVec2(appWidth * 0.33f, 0.0f));
		}
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
		if (plotAdditionalFrameValues_)
			ImGui::PlotLines("ImGui", plotValues_[ValuesType::IMGUI].get(), numValues_, 0, nullptr, 0.0f, maxUpdateVisitDraw_, ImVec2(appWidth * 0.33f, 0.0f));
	}
	ImGui::End();
}

void ImGuiDebugOverlay::initPlotValues()
{
	for (unsigned int type = 0; type < ValuesType::COUNT; type++)
	{
		plotValues_[type] = nctl::makeUnique<float []>(numValues_);

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
	const RenderStatistics::Commands &allCommands = RenderStatistics::allCommands();

	plotValues_[ValuesType::CULLED_NODES][index_] = RenderStatistics::culled();
	plotValues_[ValuesType::VBO_USED][index_] = vboBuffers.usedSpace / 1024.0f;
	plotValues_[ValuesType::IBO_USED][index_] = iboBuffers.usedSpace / 1024.0f;
	plotValues_[ValuesType::UBO_USED][index_] = uboBuffers.usedSpace / 1024.0f;

	plotValues_[ValuesType::SPRITE_VERTICES][index_] = spriteCommands.vertices;
	plotValues_[ValuesType::MESHSPRITE_VERTICES][index_] = meshspriteCommands.vertices;
	plotValues_[ValuesType::PARTICLE_VERTICES][index_] = particleCommands.vertices;
	plotValues_[ValuesType::TEXT_VERTICES][index_] = textCommands.vertices;
	plotValues_[ValuesType::IMGUI_VERTICES][index_] = imguiCommands.vertices;
	plotValues_[ValuesType::TOTAL_VERTICES][index_] = allCommands.vertices;

#ifdef WITH_LUA
	plotValues_[ValuesType::LUA_USED][index_] = LuaStatistics::usedMemory() / 1024.0f;
	plotValues_[ValuesType::LUA_OPERATIONS][index_] = LuaStatistics::operations();
#endif
}

}
