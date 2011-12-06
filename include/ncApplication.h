#ifndef CLASS_NCAPPLICATION
#define CLASS_NCAPPLICATION

#include <cstdio> // for NULL
#include "ncFrameTimer.h"
#ifdef __ANDROID__
	#include "ncEGLGfxDevice.h"
#else
	#include "ncSDLGfxDevice.h"
#endif
#include "ncSceneNode.h"
#include "ncRenderQueue.h"
#include "ncProfilePlotter.h"
#include "ncIInputManager.h"
#include "ncIAppEventHandler.h"

/// Main entry point and handler for nCine applications
class ncApplication
{
private:
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
	static void Init(struct android_app* state, ncIAppEventHandler* (*pCreateAppEventHandler)());
	static void ForwardAEvent(const struct AInputEvent* event) { m_pInputManager->ParseEvent(event); }
	static bool ShouldQuit() { return m_bShouldQuit; }
#else
	static void Init(ncIAppEventHandler* (*pCreateAppEventHandler)());
#endif

	static void Run();
	static void Step();
	static void Shutdown();

	// HACK: wrapping a lot of getters
	static inline ncIGfxDevice& GfxDevice() { return *m_pGfxDevice; }
	static inline ncSceneNode& RootNode() { return *m_pRootNode; }
	static inline ncIInputManager& InputManager() { return *m_pInputManager; }
	static inline unsigned long int Interval() { return m_pFrameTimer->Interval(); }
	static inline float AverageFPS() { return m_pFrameTimer->AverageFPS(); }
	static inline int Width() { return m_pGfxDevice->Width(); }
	static inline int Height() { return m_pGfxDevice->Height(); }
	static inline void Quit() { m_bShouldQuit = true; };

	static inline void SetInputHandler(ncIInputEventHandler *pInputEventHandler)
	{
		m_pInputManager->SetHandler(pInputEventHandler);
	}
};

#endif
