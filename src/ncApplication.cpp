#include <ctime>
#include "ncApplication.h"
#include "ncServiceLocator.h"
#include "ncArrayIndexer.h"
#include "ncFileLogger.h"
#include "ncLinePlotter.h"
#include "ncStackedBarPlotter.h"

#ifdef __ANDROID__
	#include "ncAndroidInputManager.h"
#else
	#include "ncSDLInputManager.h"
#endif

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

bool ncApplication::m_bShouldQuit = false;
ncFrameTimer *ncApplication::m_pFrameTimer = NULL;
ncIGfxDevice *ncApplication::m_pGfxDevice = NULL;
ncSceneNode *ncApplication::m_pRootNode = NULL;
ncRenderQueue *ncApplication::m_pRenderQueue = NULL;
ncTimer *ncApplication::m_pTimer = NULL;
ncProfilePlotter *ncApplication::m_pProfilePlotter = NULL;
ncIInputManager *ncApplication::m_pInputManager = NULL;
ncIAppEventHandler *ncApplication::m_pAppEventHandler = NULL;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

#ifdef __ANDROID__
void ncApplication::Init(struct android_app* state, ncIAppEventHandler* (*pCreateAppEventHandler)())
{
	m_pGfxDevice = new ncEGLGfxDevice(state, ncDisplayMode(5, 6, 5));
	m_pInputManager = new ncAndroidInputManager();
#else
void ncApplication::Init(ncIAppEventHandler* (*pCreateAppEventHandler)())
{
	m_pGfxDevice = new ncSDLGfxDevice(960, 640);
	m_pInputManager = new ncSDLInputManager();
#endif
	m_pFrameTimer = new ncFrameTimer(5, 100);
	m_pRootNode = new ncSceneNode();
	m_pRenderQueue = new ncRenderQueue();
	m_pTimer = new ncTimer();

	m_pProfilePlotter = new ncStackedBarPlotter(ncRect(Width()*0.1f, Height()*0.1f, Width()*0.8f, Height()*0.15f));
	m_pProfilePlotter->SetBackgroundColor(ncColor(0.35f, 0.35f, 0.45f, 0.5f));
	m_pProfilePlotter->AddVariable(50, 200);
	m_pProfilePlotter->Variable(0).SetGraphColor(ncColor(0.8f, 0.0f, 0.0f));
	m_pProfilePlotter->Variable(0).SetMeanColor(ncColor(1.0f, 0.0f, 0.0f));
	m_pProfilePlotter->AddVariable(50, 200);
	m_pProfilePlotter->Variable(1).SetGraphColor(ncColor(0.0f, 0.8f, 0.0f));
	m_pProfilePlotter->Variable(1).SetMeanColor(ncColor(0.0f, 1.0f, 0.0f));
	m_pProfilePlotter->AddVariable(50, 200);
	m_pProfilePlotter->Variable(2).SetGraphColor(ncColor(0.0f, 0.0f, 0.8f));
	m_pProfilePlotter->Variable(2).SetMeanColor(ncColor(0.0f, 0.0f, 1.0f));

	ncServiceLocator::RegisterIndexer(new ncArrayIndexer());
	ncServiceLocator::RegisterLogger(new ncFileLogger("log.txt", ncILogger::LOG_VERBOSE, ncILogger::LOG_OFF));
	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, (char *)"ncApplication::Init - ncApplication initialized");

	m_pAppEventHandler = pCreateAppEventHandler();
	m_pAppEventHandler->OnInit();
	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, (char *)"ncApplication::Init - ncIAppEventHandler::OnInit() invoked");

	// HACK: Init of the random seed
	// In the future there could be a random generator service
	srand(time(NULL));
}

void ncApplication::Run()
{
#ifndef __ANDROID__
	bool bPaused = false;
	SDL_Event event;

	m_pFrameTimer->Reset();

	while (!m_bShouldQuit) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				m_bShouldQuit = true;
				break;
			case SDL_ACTIVEEVENT:
				if (event.active.state != SDL_APPMOUSEFOCUS)
					bPaused = !event.active.gain;
				break;
/*
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
				case SDLK_q:
					// TODO: add a quit request from the game
					bQuit = true;
					break;
				case SDLK_F1:
					m_pGfxDevice->ToggleFullScreen();
					break;
				default:
					break;
				}
				break;
*/
			default:
				m_pInputManager->ParseEvent(event);
				break;
			}
		}

		if (!bPaused)
			Step();
	}
#endif
}

void ncApplication::Step()
{
	unsigned long int uStartTime = 0L;
	m_pTimer->Start();

	uStartTime = m_pTimer->Now();
	m_pFrameTimer->AddFrame();
	m_pGfxDevice->Clear();
	m_pAppEventHandler->OnFrameStart();
	m_pProfilePlotter->AddValue(0, m_pTimer->Now() - uStartTime);
//	m_pProfilePlotter->AddValue(0, 1.0f * abs(rand()%34));

	uStartTime = m_pTimer->Now();
	m_pRootNode->Update(m_pFrameTimer->Interval());
	m_pRootNode->Visit(*m_pRenderQueue);
	m_pProfilePlotter->AddValue(1, m_pTimer->Now() - uStartTime);
//	m_pProfilePlotter->AddValue(1, 1.0f * abs(rand()%33));

	uStartTime = m_pTimer->Now();
	m_pRenderQueue->Draw();
	m_pProfilePlotter->AddValue(2, m_pTimer->Now() - uStartTime);
//	m_pProfilePlotter->AddValue(2, 1.0f * abs(rand()%33));

	m_pProfilePlotter->Draw();

	m_pGfxDevice->Update();
	m_pAppEventHandler->OnFrameEnd();
}

void ncApplication::Shutdown()
{
	m_pAppEventHandler->OnShutdown();
	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, (char *)"ncApplication::Shutdown - ncIAppEventHandler::OnShutdown() invoked");

	delete m_pInputManager;;
	if (m_pAppEventHandler)
		delete m_pAppEventHandler;

	delete m_pRenderQueue;
	delete m_pRootNode; // deletes every child too
	delete m_pGfxDevice;
	delete m_pFrameTimer;

	if (ncServiceLocator::GetIndexer().isEmpty() == false)
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_WARN, (char *)"ncApplication::Shutdown - The object indexer is not empty");

	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, (char *)"ncApplication::Shutdown - ncApplication shutted down");
}
