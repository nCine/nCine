#ifndef CLASS_NCAPPLICATION
#define CLASS_NCAPPLICATION

#include <cstdio> // for NULL
#include "ncFrameTimer.h"
#include "ncIGfxDevice.h"
#include "ncSceneNode.h"
#include "ncRenderQueue.h"
#include "ncProfilePlotter.h"
#include "ncIInputManager.h"
#include "ncIAppEventHandler.h"

/// Main entry point and handler for nCine applications
class ncApplication
{
private:
	static bool m_bPaused;
	static bool m_bShouldQuit;
	static ncFrameTimer *m_pFrameTimer;
	static ncIGfxDevice *m_pGfxDevice;
	static ncSceneNode *m_pRootNode;
	static ncRenderQueue *m_pRenderQueue;
	static ncTimer *m_pTimer;
	static ncProfilePlotter *m_pProfilePlotter;
	static ncIInputManager *m_pInputManager;
	static ncIAppEventHandler *m_pAppEventHandler;

	ncApplication();
	~ncApplication();

public:
#ifdef __ANDROID__
	// Must be called at start to init the application
	static void Init(struct android_app* state, ncIAppEventHandler* (*pCreateAppEventHandler)());
	/// Called in the Android input event handler to forward events to the application
	static void ForwardAEvent(const struct AInputEvent* event) { m_pInputManager->ParseEvent(event); }
	/// Return the quit flag value
	static bool ShouldQuit() { return m_bShouldQuit; }
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

	/// Return the value of the pause flag
	static inline bool IsPaused() { return m_bPaused; }

	// HACK: wrapping a lot of getters
	/// Returns the graphics device instance
	static inline ncIGfxDevice& GfxDevice() { return *m_pGfxDevice; }
	/// Returns the root of the transformation graph
	static inline ncSceneNode& RootNode() { return *m_pRootNode; }
	/// Returns the input manager instance
	static inline ncIInputManager& InputManager() { return *m_pInputManager; }

	/// Returns the elapsed time since the end of the previous frame in milliseconds
	static inline unsigned long int Interval() { return m_pFrameTimer->Interval(); }
	/// Returns the average FPS
	static inline float AverageFPS() { return m_pFrameTimer->AverageFPS(); }
	/// Returns the screen width
	static inline int Width() { return m_pGfxDevice->Width(); }
	/// Returns the screen height
	static inline int Height() { return m_pGfxDevice->Height(); }
	/// Sets the pause flag value
	static inline void SetPause(bool bPaused) { m_bPaused = bPaused; }
	/// Toggles the pause flag on and off
	static inline void TogglePause() { m_bPaused = !m_bPaused; }
	/// Raises the quit flag
	static inline void Quit() { m_bShouldQuit = true; };

	/// Sets the input handler object
	static inline void SetInputHandler(ncIInputEventHandler *pInputEventHandler)
	{
		m_pInputManager->SetHandler(pInputEventHandler);
	}
};

#endif
