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
#include "ncRenderGraph.h"
#include "ncProfilePlotter.h"

/// Main entry point and handler for nCine applications
class ncApplication
{
public:

	enum eCommand {
		CMD_NOOP = 0,
		CMD_INIT,
		CMD_FRAMESTART,
		CMD_FRAMEEND,
		CMD_SHUTDOWN
	};

private:
	static void (*m_pGameCallback)(eCommand cmd);
	static ncFrameTimer *m_pFrameTimer;
	static ncIGfxDevice *m_pGfxDevice;
	static ncSceneNode *m_pRootNode;
	static ncRenderGraph *m_pRenderGraph;
	static ncTimer *m_pTimer;
	static ncProfilePlotter *m_pProfilePlotter;

	ncApplication();
	~ncApplication();

public:
#ifdef __ANDROID__
	static void Init(struct android_app* state, void (*pGameCallback)(eCommand cmd));
#else
	static void Init(void (*pGameCallback)(eCommand cmd));
#endif
	static void Run();
	static void Step();
	static void Shutdown();

	// HACK: wrapping a lot of getters
	static inline ncSceneNode& RootNode() { return *m_pRootNode; }
	static inline unsigned long int Interval() { return m_pFrameTimer->Interval(); }
	static inline int Width() { return m_pGfxDevice->Width(); }
	static inline int Height() { return m_pGfxDevice->Height(); }
};

#endif
