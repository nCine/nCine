#ifndef CLASS_NCINE_APPLICATION
#define CLASS_NCINE_APPLICATION

#include "IGfxDevice.h"
#include "AppConfiguration.h"
#include "nctl/UniquePtr.h"

namespace ncine {

class FrameTimer;
class Timer;
class SceneNode;
class RenderQueue;
class ProfilePlotter;
class Font;
class TextNode;
class IInputManager;
class IAppEventHandler;

/// Main entry point and handler for nCine applications
class DLL_PUBLIC Application
{
  public:
	/// Rendering settings that can be changed at run-time
	struct RenderingSettings
	{
		RenderingSettings() :
			batchingEnabled(true), batchingWithIndices(false), cullingEnabled(true),
			minBatchSize(4), maxBatchSize(500), showProfilerGraphs(true), showInfoText(true) { }

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
		/// True if showing the profiler graphc
		bool showProfilerGraphs;
		/// True if showing the information text
		bool showInfoText;
	};

	/// Returns the configuration used to initialize the application
	inline const AppConfiguration &appConfiguration() const { return appCfg_; }
	/// Returns the run-time rendering settings
	inline RenderingSettings &renderingSettings() { return renderingSettings_; }

	/// Returns the graphics device instance
	inline IGfxDevice &gfxDevice() { return *gfxDevice_; }
	/// Returns the root of the transformation graph
	inline SceneNode &rootNode() { return *rootNode_; }
	/// Returns the input manager instance
	inline IInputManager &inputManager() { return *inputManager_; }

	/// Returns the elapsed time since the end of the previous frame in milliseconds
	float interval() const;

	/// Returns the screen width as a float number
	inline float width() const { return static_cast<float>(gfxDevice_->width()); }
	/// Returns the screen height as a float number
	inline float height() const { return static_cast<float>(gfxDevice_->height()); }

	/// Returns the screen width as an integer number
	inline int widthInt() const { return gfxDevice_->width(); }
	/// Returns the screen height as an integer number
	inline int heightInt() const { return gfxDevice_->height(); }

	/// Returns the value of the pause flag
	inline bool isPaused() const { return isPaused_; }
	/// Sets the pause flag value
	void setPaused(bool paused);
	/// Toggles the pause flag on and off
	void togglePause();

	/// Raises the quit flag
	inline void quit() { shouldQuit_ = true; }
	/// Returns the quit flag value
	inline bool shouldQuit() const { return shouldQuit_; }

	/// Returns the focus flag value
	inline bool hasFocus() const { return hasFocus_; }

  protected:
	/// Maximum length for the information strings
	static const unsigned int MaxTextLength = 256;

	bool isPaused_;
	bool hasFocus_;
	bool shouldQuit_;
	AppConfiguration appCfg_;
	RenderingSettings renderingSettings_;
	nctl::UniquePtr<FrameTimer> frameTimer_;
	nctl::UniquePtr<IGfxDevice> gfxDevice_;
	nctl::UniquePtr<RenderQueue> renderQueue_;
	nctl::UniquePtr<SceneNode> rootNode_;
	nctl::UniquePtr<Timer> profileTimer_;
	nctl::UniquePtr<ProfilePlotter> profilePlotter_;
	nctl::UniquePtr<Font> font_;
	nctl::UniquePtr<TextNode> infoLineTopRight_;
	nctl::UniquePtr<TextNode> infoLineTopLeft_;
	nctl::UniquePtr<TextNode> infoLineBottomLeft_;
	float textUpdateTime_;
	nctl::String infoStringTopRight_;
	nctl::String infoStringTopLeft_;
	nctl::UniquePtr<IInputManager> inputManager_;
	nctl::UniquePtr<IAppEventHandler> appEventHandler_;

	Application();
	~Application();

	/// Must be called before giving control to the application
	void initCommon();
	/// A single step of the game loop made to render a frame
	void step();
	/// Must be called before exiting to shut down the application
	void shutdownCommon();

	/// Sets the focus flag
	virtual void setFocus(bool hasFocus);

  private:
	/// Deleted copy constructor
	Application(const Application &) = delete;
	/// Deleted assignment operator
	Application &operator=(const Application &) = delete;

	friend class PCApplication;
	friend class AndroidApplication;
};

// Meyers' Singleton
extern DLL_PUBLIC Application &theApplication();

}

#endif
