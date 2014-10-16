#ifndef CLASS_NCAPPLICATION
#define CLASS_NCAPPLICATION

#include <cstdio> // for NULL
#include "ncFrameTimer.h"
#include "ncIGfxDevice.h"
class FrameTimer;
class ncSceneNode;
class ncRenderQueue;
class ncProfilePlotter;
class ncFont;
class ncTextNode;
class ncIInputManager;
class ncIAppEventHandler;

/// Main entry point and handler for nCine applications
class ncApplication
{
  public:
#ifdef __ANDROID__
	// Must be called at start to init the application
	static void init(struct android_app* state, ncIAppEventHandler* (*createAppEventHandler)());
#else
	// Must be called at start to init the application
	static void init(ncIAppEventHandler* (*createAppEventHandler)());
#endif

	// The main game loop, handling events and rendering
	static void run();
	// A single step of the game loop made to render a frame
	static void step();
	// Must be called before exiting to shut down the application
	static void shutdown();

	// HACK: wrapping a lot of getters
	/// Returns the graphics device instance
	inline static ncIGfxDevice& gfxDevice() { return *gfxDevice_; }
	/// Returns the root of the transformation graph
	inline static ncSceneNode& rootNode() { return *rootNode_; }
	/// Returns the input manager instance
	inline static ncIInputManager& inputManager() { return *inputManager_; }

	/// Returns the elapsed time since the end of the previous frame in milliseconds
	inline static float interval() { return frameTimer_->interval(); }
	/// Returns the average FPS
	inline static float averageFps() { return frameTimer_->averageFps(); }

	/// Returns the screen width
	inline static int width() { return gfxDevice_->width(); }
	/// Returns the screen height
	inline static int height() { return gfxDevice_->height(); }

	/// Returns the value of the pause flag
	inline static bool isPaused() { return isPaused_; }
	// Sets the pause flag value
	static void setPause(bool paused);
	// Toggles the pause flag on and off
	static void togglePause();

	/// Raises the quit flag
	inline static void quit() { shouldQuit_ = true; }
	/// Returns the quit flag value
	static bool shouldQuit() { return shouldQuit_; }

	/// Returns the focus flag value
	static bool hasFocus() { return hasFocus_; }
	// Sets the focus flag
	static void setFocus(bool hasFocus);

	// Shows or hides profiling graphs
	static void showProfileGraphs(bool shouldDraw);
	// Shows or hides profiling information text
	static void showProfileInfo(bool shouldDraw);

  private:
	static bool isPaused_;
	static bool hasFocus_;
	static bool shouldQuit_;
	static ncFrameTimer *frameTimer_;
	static ncIGfxDevice *gfxDevice_;
	static ncSceneNode *rootNode_;
	static ncRenderQueue *renderQueue_;
	static ncTimer *profileTimer_;
	static ncProfilePlotter *profilePlotter_;
	static ncFont *font_;
	static ncTextNode *textLines_;
	static float textUpdateTime;
	static char textChars_[256];
	static ncIInputManager *inputManager_;
	static ncIAppEventHandler *appEventHandler_;

	ncApplication();
	~ncApplication();
};

#endif
