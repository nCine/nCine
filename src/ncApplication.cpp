#include "ncApplication.h"
#include "ncServiceLocator.h"
#include "ncArrayIndexer.h"
#include "ncFileLogger.h"

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

void (*ncApplication::m_pGameCallback)(eCommand cmd) = NULL;
ncFrameTimer *ncApplication::m_pFrameTimer = NULL;
ncGfxDevice *ncApplication::m_pGfxDevice = NULL;
ncSceneNode *ncApplication::m_pRootNode = NULL;
ncRenderGraph *ncApplication::m_pRenderGraph = NULL;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncApplication::Init(void (*pGameCallback)(eCommand cmd))
{
	m_pFrameTimer = new ncFrameTimer(5, 0);
	m_pGfxDevice = new ncGfxDevice(960, 640);
	m_pRootNode = new ncSceneNode();
	m_pRenderGraph = new ncRenderGraph();

	ncServiceLocator::RegisterIndexer(new ncArrayIndexer());
	ncServiceLocator::RegisterLogger(new ncFileLogger("log.txt", ncILogger::LOG_VERBOSE, ncILogger::LOG_OFF));
	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, (char *)"ncApplication::Init - ncApplication initialized");

	m_pGameCallback = pGameCallback;
	(*m_pGameCallback)(CMD_INIT);
	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, (char *)"ncApplication::Init - external app initialized");
}

void ncApplication::Run()
{
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

		m_pFrameTimer->AddFrame();
		m_pGfxDevice->Clear();
		m_pGameCallback(CMD_FRAMESTART);

		m_pRootNode->Update(m_pFrameTimer->GetInterval());
		m_pRenderGraph->Traverse(*m_pRootNode);
		m_pRenderGraph->Draw();

		m_pGfxDevice->Update();
		m_pGameCallback(CMD_FRAMEEND);

		m_pGfxDevice->Update();
	}
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
