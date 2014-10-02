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
	static void Init(struct android_app* state, ncIAppEventHandler* (*pCreateAppEventHandler)());
#else
	// Must be called at start to init the application
	static void Init(ncIAppEventHandler* (*pCreateAppEventHandler)());
#endif

	// The main game loop, handling events and rendering
	static void Run();
	// A single step of the game loop made to render a frame
	static void Step();
	// Must be called before exiting to shut down the application
	static void Shutdown();

	// HACK: wrapping a lot of getters
	/// Returns the graphics device instance
	inline static ncIGfxDevice& GfxDevice() { return *m_pGfxDevice; }
	/// Returns the root of the transformation graph
	inline static ncSceneNode& RootNode() { return *m_pRootNode; }
	/// Returns the input manager instance
	inline static ncIInputManager& InputManager() { return *m_pInputManager; }

	/// Returns the elapsed time since the end of the previous frame in milliseconds
	inline static float Interval() { return m_pFrameTimer->Interval(); }
	/// Returns the average FPS
	inline static float AverageFPS() { return m_pFrameTimer->AverageFPS(); }

	/// Returns the screen width
	inline static int Width() { return m_pGfxDevice->Width(); }
	/// Returns the screen height
	inline static int Height() { return m_pGfxDevice->Height(); }

	/// Returns the value of the pause flag
	inline static bool IsPaused() { return m_bPaused; }
	// Sets the pause flag value
	static void SetPause(bool bPaused);
	// Toggles the pause flag on and off
	static void TogglePause();

	/// Raises the quit flag
	inline static void Quit() { m_bShouldQuit = true; }
	/// Returns the quit flag value
	static bool ShouldQuit() { return m_bShouldQuit; }

	/// Returns the focus flag value
	static bool HasFocus() { return m_bHasFocus; }
	// Sets the focus flag
	static void SetFocus(bool bHasFocus);

	// Shows or hides profiling graphs
	static void ShowProfileGraphs(bool bShouldDraw);
	// Shows or hides profiling information text
	static void ShowProfileInfo(bool bShouldDraw);

 private:
	static bool m_bPaused;
	static bool m_bHasFocus;
	static bool m_bShouldQuit;
	static ncFrameTimer *m_pFrameTimer;
	static ncIGfxDevice *m_pGfxDevice;
	static ncSceneNode *m_pRootNode;
	static ncRenderQueue *m_pRenderQueue;
	static ncTimer *m_pProfileTimer;
	static ncProfilePlotter *m_pProfilePlotter;
	static ncFont *m_pFont;
	static ncTextNode *m_pTextLines;
	static float m_fTextUpdateTime;
	static char m_vTextChars[256];
	static ncIInputManager *m_pInputManager;
	static ncIAppEventHandler *m_pAppEventHandler;

	ncApplication();
	~ncApplication();
};

#endif
