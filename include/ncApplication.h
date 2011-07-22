#ifndef CLASS_NCAPPLICATION
#define CLASS_NCAPPLICATION

#include <cstdio> // for NULL
#include "ncFrameTimer.h"
#include "ncGfxDevice.h"
#include "ncSceneNode.h"
#include "ncRenderGraph.h"
#include "ncLinePlotter.h"

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
	static ncGfxDevice *m_pGfxDevice;
	static ncSceneNode *m_pRootNode;
	static ncRenderGraph *m_pRenderGraph;
	static ncLinePlotter *m_pLinePlotter;

	ncApplication();
	~ncApplication();

public:
	static void Init(void (*pCallback)(eCommand cmd));
	static void Run();
	static void Shutdown();

	// Wrappaggio pezzotto
	static inline ncSceneNode& RootNode() { return *m_pRootNode; }
	static inline unsigned long int Interval() { return m_pFrameTimer->Interval(); }
	static inline int Width() { return m_pGfxDevice->Width(); }
	static inline int Height() { return m_pGfxDevice->Height(); }
};

#endif
