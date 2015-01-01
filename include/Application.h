#ifndef CLASS_NCINE_APPLICATION
#define CLASS_NCINE_APPLICATION

#include <cstdio> // for NULL
#include "IGfxDevice.h"

namespace ncine {

class FrameTimer;
class Timer;
class SceneNode;
class RenderQueue;
class ProfilePlotter;
class Font;
class TextNode;
class String;
class IInputManager;
class IAppEventHandler;

/// Main entry point and handler for nCine applications
class Application
{
  public:
	// Must be called at start to init the application
	static void init(IAppEventHandler* (*createAppEventHandler)());

	// The main game loop, handling events and rendering
	static void run();
	// A single step of the game loop made to render a frame
	static void step();
	// Must be called before exiting to shut down the application
	static void shutdown();

	// HACK: wrapping a lot of getters
	/// Returns the graphics device instance
	inline static IGfxDevice& gfxDevice() { return *gfxDevice_; }
	/// Returns the root of the transformation graph
	inline static SceneNode& rootNode() { return *rootNode_; }
	/// Returns the input manager instance
	inline static IInputManager& inputManager() { return *inputManager_; }

	// Returns the elapsed time since the end of the previous frame in milliseconds
	static float interval();

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

  protected:
	/// Maximum length for the profile string
	static const int MaxTextLength = 256;

	static bool isPaused_;
	static bool hasFocus_;
	static bool shouldQuit_;
	static FrameTimer *frameTimer_;
	static IGfxDevice *gfxDevice_;
	static SceneNode *rootNode_;
	static RenderQueue *renderQueue_;
	static Timer *profileTimer_;
	static ProfilePlotter *profilePlotter_;
	static Font *font_;
	static TextNode *textLines_;
	static float textUpdateTime;
	static String textString_;
	static IInputManager *inputManager_;
	static IAppEventHandler *appEventHandler_;

	static const char* fontTexFilename_;
	static const char* fontFntFilename_;

	static void initCommon(IAppEventHandler* (*createAppEventHandler)());

  private:
	Application();
	~Application();
	/// Private copy constructor
	Application(const Application&);
	/// Private assignment operator
	Application& operator=(const Application&);
};

}

#endif
