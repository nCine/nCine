#include "ncApplication.h"
#include "ncServiceLocator.h"
#include "ncArrayIndexer.h"
#include "ncFileLogger.h"

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

void (*ncApplication::m_pGameCallback)(eCommand cmd) = NULL;
ncFrameTimer *ncApplication::m_pFrameTimer = NULL;
ncIGfxDevice *ncApplication::m_pGfxDevice = NULL;
ncSceneNode *ncApplication::m_pRootNode = NULL;
ncRenderGraph *ncApplication::m_pRenderGraph = NULL;
ncLinePlotter *ncApplication::m_pLinePlotter = NULL;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

#ifdef __ANDROID__
void ncApplication::Init(struct android_app* state, void (*pGameCallback)(eCommand cmd))
{
//	m_state = state;
	m_pGfxDevice = new ncEGLGfxDevice(state, ncDisplayMode(5, 6, 5));
#else
void ncApplication::Init(void (*pGameCallback)(eCommand cmd))
{
	m_pGfxDevice = new ncSDLGfxDevice(960, 640);
#endif
	m_pFrameTimer = new ncFrameTimer(5, 0);
	m_pRootNode = new ncSceneNode();
	m_pRenderGraph = new ncRenderGraph();
	m_pLinePlotter = new ncLinePlotter(ncRect(Width()*0.1f, Height()*0.1f, Width()*0.8f, Height()*0.15f));
	m_pLinePlotter->SetBackgroundColor(ncColor(0.5f, 0.5f, 0.5f, 0.5f));
	m_pLinePlotter->AddVariable(32, 250);
	m_pLinePlotter->Variable(0).SetGraphColor(ncColor(1.0f, 1.0f, 1.0f));
	m_pLinePlotter->Variable(0).SetMeanColor(ncColor(0.75f, 0.75f, 0.0f));
	m_pLinePlotter->AddVariable(64, 150);
	m_pLinePlotter->Variable(1).SetGraphColor(ncColor(0.0f, 1.0f, 1.0f));
	m_pLinePlotter->Variable(1).SetMeanColor(ncColor(0.75f, 0.1f, 0.0f));

	ncServiceLocator::RegisterIndexer(new ncArrayIndexer());
	ncServiceLocator::RegisterLogger(new ncFileLogger("log.txt", ncILogger::LOG_VERBOSE, ncILogger::LOG_OFF));
	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, (char *)"ncApplication::Init - ncApplication initialized");

	m_pGameCallback = pGameCallback;
	(*m_pGameCallback)(CMD_INIT);
	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, (char *)"ncApplication::Init - external app initialized");
}

void ncApplication::Run()
{
#ifndef __ANDROID__
	bool bQuit = false;
	SDL_Event event;

	m_pFrameTimer->Reset();

	while (!bQuit) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				bQuit = true;
				break;
			case SDL_KEYDOWN:
				switch( event.key.keysym.sym ){
				case SDLK_ESCAPE:
				case SDLK_q:
					bQuit = true;
					break;
				case SDLK_F1:
					m_pGfxDevice->ToggleFullScreen();
					break;
				default:
					break;
				}
			}
		}

		Step();
	}
#endif
}

void ncApplication::Step()
{
	m_pFrameTimer->AddFrame();
	m_pGfxDevice->Clear();
	m_pGameCallback(CMD_FRAMESTART);

	m_pRootNode->Update(m_pFrameTimer->Interval());
	m_pRenderGraph->Traverse(*m_pRootNode);
	m_pRenderGraph->Draw();

	m_pLinePlotter->AddValue(0, m_pFrameTimer->Interval());
	m_pLinePlotter->AddValue(1, 1.0f * (rand()%100));
	m_pLinePlotter->Draw();

	m_pGfxDevice->Update();
	m_pGameCallback(CMD_FRAMEEND);
}

void ncApplication::Shutdown()
{	
	m_pGameCallback(CMD_SHUTDOWN);
	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, (char *)"ncApplication::Shutdown - external app shutted down");

	delete m_pRenderGraph;
	delete m_pRootNode; // deletes every child too
	delete m_pGfxDevice;
	delete m_pFrameTimer;

	if (ncServiceLocator::GetIndexer().isEmpty() == false)
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_WARN, (char *)"ncApplication::Shutdown - The object indexer is not empty");

	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, (char *)"ncApplication::Shutdown - ncApplication shutted down");
}
