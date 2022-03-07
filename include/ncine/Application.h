#ifndef CLASS_NCINE_APPLICATION
#define CLASS_NCINE_APPLICATION

#include "IGfxDevice.h"
#include "AppConfiguration.h"
#include "IDebugOverlay.h"
#include "TimeStamp.h"
#include <nctl/UniquePtr.h>

namespace ncine {

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
		      cullingEnabled(true), minBatchSize(4), maxBatchSize(500) {}

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
	/// Returns the debug overlay object, if any
	inline IDebugOverlay::DisplaySettings &debugOverlaySettings()
	{
		return (debugOverlay_) ? debugOverlay_->settings() : debugOverlayNullSettings_;
	}

	/// Returns all timings
	inline const float *timings() const { return timings_; }

	/// Returns the graphics device instance
	inline IGfxDevice &gfxDevice() { return *gfxDevice_; }
	/// Returns the root of the transformation graph
	inline SceneNode &rootNode() { return *rootNode_; }
	/// Returns the root viewport (i.e. the screen)
	Viewport &rootViewport();
	/// Returns the input manager instance
	inline IInputManager &inputManager() { return *inputManager_; }

	/// Returns the total number of frames already rendered
	unsigned long int numFrames() const;
	/// Returns the elapsed time since the end of the previous frame in seconds
	float interval() const;

	/// Returns the screen width as a float number
	inline float width() const { return static_cast<float>(gfxDevice_->width()); }
	/// Returns the screen height as a float number
	inline float height() const { return static_cast<float>(gfxDevice_->height()); }
	/// Returns the screen resolution as a `Vector2f` object
	inline Vector2f resolution() const { return Vector2f(width(), height()); }

	/// Returns the screen width as an integer number
	inline int widthInt() const { return gfxDevice_->width(); }
	/// Returns the screen height as an integer number
	inline int heightInt() const { return gfxDevice_->height(); }
	/// Returns the screen resolution as a `Vector2i` object
	inline Vector2i resolutionInt() const { return gfxDevice_->resolution(); }

	/// Resizes the root viewport if it exists
	void resizeRootViewport(int width, int height);

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

	/// Raises the quit flag
	inline void quit() { shouldQuit_ = true; }
	/// Returns the quit flag value
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
	nctl::UniquePtr<ScreenViewport> rootViewport_;
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
};

// Meyers' Singleton
extern DLL_PUBLIC Application &theApplication();

}

#endif
