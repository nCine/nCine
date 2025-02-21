#ifndef CLASS_NCINE_APPLICATION
#define CLASS_NCINE_APPLICATION

#include "IGfxDevice.h"
#include "AppConfiguration.h"
#include "IDebugOverlay.h"
#include "TimeStamp.h"
#include <nctl/UniquePtr.h>

namespace ncine {

class IFrameTimer;
class FrameTimer;
class SceneNode;
class Viewport;
class ScreenViewport;
class IInputManager;
class IAppEventHandler;
class ImGuiDrawing;
class NuklearDrawing;

/// Main entry point and handler for nCine applications
class DLL_PUBLIC Application
{
  public:
	/// Rendering settings that can be changed at run-time
	struct RenderingSettings
	{
		RenderingSettings()
		    : batchingEnabled(true), batchingWithIndices(false),
		      cullingEnabled(true), minBatchSize(4), maxBatchSize(512) {}

		/// True if batching is enabled
		bool batchingEnabled;
		/// True if using indices for vertex batching
		bool batchingWithIndices;
		/// True if node culling is enabled
		bool cullingEnabled;
		/// Minimum size for a batch to be collected
		unsigned int minBatchSize;
		/// Maximum size for a batch before a forced split
		unsigned int maxBatchSize;
	};

	/// GUI settings (for ImGui and Nuklear) that can be changed at run-time
	struct GuiSettings
	{
		GuiSettings();

		/// ImGui drawable node layer
		uint16_t imguiLayer;
		/// Nuklear drawable node layer
		uint16_t nuklearLayer;
		/// ImGui viewport
		/*! \note The viewport should mirror the screen dimensions or mouse input would not work. Setting `nullptr` is the same as setting the screen */
		Viewport *imguiViewport;
		/// Nuklear viewport
		/*! \note The viewport should mirror the screen dimensions or mouse input would not work. Setting `nullptr` is the same as setting the screen */
		Viewport *nuklearViewport;
	};

	struct Timings
	{
		enum
		{
			PRE_INIT,
			INIT_COMMON,
			APP_INIT,
			FRAME_START,
			UPDATE_VISIT_DRAW,
			UPDATE,
			POST_UPDATE,
			VISIT,
			DRAW,
			IMGUI,
			NUKLEAR,
			FRAME_END,

			COUNT
		};
	};

	/// Returns the configuration used to initialize the application
	inline const AppConfiguration &appConfiguration() const { return appCfg_; }
	/// Returns the run-time rendering settings
	inline RenderingSettings &renderingSettings() { return renderingSettings_; }
	/// Returns the run-time GUI settings
	inline GuiSettings &guiSettings() { return guiSettings_; }
	/// Returns the run-time debug overlay settings, if debug overlay is available
	inline IDebugOverlay::DisplaySettings &debugOverlaySettings()
	{
		return (debugOverlay_) ? debugOverlay_->settings() : debugOverlayNullSettings_;
	}

	/// Returns all timings
	inline const float *timings() const { return timings_; }

	/// Returns the graphics device instance
	inline IGfxDevice &gfxDevice() { return *gfxDevice_; }
	/// Returns the root node of the transformation graph
	inline SceneNode &rootNode() { return *rootNode_; }
	/// Returns the screen viewport
	Viewport &screenViewport();
	/// Returns the input manager instance
	inline IInputManager &inputManager() { return *inputManager_; }

	/// Returns the total number of frames already rendered
	unsigned long int numFrames() const;
	/// Returns the time in seconds that last frame took to complete
	float frameTime() const;
	/// Returns the frame timer interface
	IFrameTimer &frameTimer();

	/// Returns the drawable screen width as a float number
	inline float width() const { return static_cast<float>(gfxDevice_->drawableWidth()); }
	/// Returns the drawable screen height as a float number
	inline float height() const { return static_cast<float>(gfxDevice_->drawableHeight()); }
	/// Returns the drawable screen resolution as a `Vector2f` object
	inline Vector2f resolution() const { return Vector2f(width(), height()); }

	/// Returns the drawable screen width as an integer number
	inline int widthInt() const { return gfxDevice_->drawableWidth(); }
	/// Returns the drawable screen height as an integer number
	inline int heightInt() const { return gfxDevice_->drawableHeight(); }
	/// Returns the drawable screen resolution as a `Vector2i` object
	inline Vector2i resolutionInt() const { return gfxDevice_->drawableResolution(); }

	/// Returns the value of the suspension flag
	/*! If `true` the application is suspended, it will neither update nor receive events */
	inline bool isSuspended() const { return isSuspended_; }
	/// Sets the suspension flag value
	inline void setSuspended(bool suspended) { isSuspended_ = suspended; }

	/// Returns the value of the auto-suspension flag
	/*! If `true` the application will be suspended when it loses focus */
	inline bool autoSuspension() const { return autoSuspension_; }
	/// Sets the auto-suspension flag value
	inline void setAutoSuspension(bool autoSuspension) { autoSuspension_ = autoSuspension; }

	/// Asks the application to quit as soon as possible (by raising the quit flag)
	inline void quit() { shouldQuit_ = true; }
	/// Returns the value of the quit flag
	inline bool shouldQuit() const { return shouldQuit_; }

	/// Returns the focus flag value
	inline bool hasFocus() const { return hasFocus_; }

  protected:
	bool isSuspended_;
	bool autoSuspension_;
	bool hasFocus_;
	bool shouldQuit_;
	const AppConfiguration appCfg_;
	RenderingSettings renderingSettings_;
	GuiSettings guiSettings_;
	float timings_[Timings::COUNT];
	IDebugOverlay::DisplaySettings debugOverlayNullSettings_;

	TimeStamp profileStartTime_;
	nctl::UniquePtr<FrameTimer> frameTimer_;
	nctl::UniquePtr<IGfxDevice> gfxDevice_;
	nctl::UniquePtr<SceneNode> rootNode_;
	nctl::UniquePtr<ScreenViewport> screenViewport_;
	nctl::UniquePtr<IDebugOverlay> debugOverlay_;
	nctl::UniquePtr<IInputManager> inputManager_;
	nctl::UniquePtr<IAppEventHandler> appEventHandler_;
#ifdef WITH_IMGUI
	nctl::UniquePtr<ImGuiDrawing> imguiDrawing_;
#endif
#ifdef WITH_NUKLEAR
	nctl::UniquePtr<NuklearDrawing> nuklearDrawing_;
#endif

	Application();
	~Application();

	/// Must be called before giving control to the application
	void initCommon();
	/// A single step of the game loop made to render a frame
	void step();
	/// Must be called before exiting to shut down the application
	void shutdownCommon();

	/// Called when the application gets suspended
	void suspend();
	/// Called when the application resumes execution
	void resume();

	/// Sets the focus flag
	virtual void setFocus(bool hasFocus);

	/// Resizes the screen viewport, if it exists
	bool resizeScreenViewport(int width, int height);

	/// Checks if the window scaling factor has changed this frame
	bool updateScalingFactor();

  private:
	/// Deleted copy constructor
	Application(const Application &) = delete;
	/// Deleted assignment operator
	Application &operator=(const Application &) = delete;

	bool shouldSuspend();

	friend class PCApplication;
	friend class AndroidApplication;
#ifdef __EMSCRIPTEN__
	friend class IGfxDevice;
#endif
	friend class Viewport; // for `onDrawViewport()`
	friend class GlfwInputManager; // for `resizeScreenViewport()`
	friend class Qt5Widget; // for `resizeScreenViewport()`
};

// Meyers' Singleton
extern DLL_PUBLIC Application &theApplication();

}

#endif
