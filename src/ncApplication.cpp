#include <ctime>
#include "ncApplication.h"
#include "ncServiceLocator.h"
#include "ncArrayIndexer.h"
#include "ncFileLogger.h"
#include "ncALAudioDevice.h"
#include "ncLinePlotter.h"
#include "ncStackedBarPlotter.h"
#include "ncFont.h"
#include "ncTextNode.h"

#ifdef __ANDROID__
	#include "ncEGLGfxDevice.h"
	#include "ncAndroidInputManager.h"
#else
	#include "ncSDLGfxDevice.h"
	#include "ncSDLInputManager.h"
#endif

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

bool ncApplication::m_bPaused = true;
bool ncApplication::m_bShouldQuit = false;
ncFrameTimer *ncApplication::m_pFrameTimer = NULL;
ncIGfxDevice *ncApplication::m_pGfxDevice = NULL;
ncSceneNode *ncApplication::m_pRootNode = NULL;
ncRenderQueue *ncApplication::m_pRenderQueue = NULL;
ncTimer *ncApplication::m_pTimer = NULL;
ncProfilePlotter *ncApplication::m_pProfilePlotter = NULL;
ncFont *ncApplication::m_pFont = NULL;
ncTextNode *ncApplication::m_pTextLines = NULL;
unsigned long int ncApplication::m_ulTextUpdateTime = 0;
char ncApplication::m_vTextChars[256] = "";
ncIInputManager *ncApplication::m_pInputManager = NULL;
ncIAppEventHandler *ncApplication::m_pAppEventHandler = NULL;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Must be called at start to init the application
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

	m_pProfilePlotter = new ncStackedBarPlotter(m_pRootNode, ncRect(Width()*0.1f, Height()*0.1f, Width()*0.8f, Height()*0.15f));
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

#ifdef __ANDROID__
	m_pFont = new ncFont("/sdcard/ncine/trebuchet16_128.dds", "/sdcard/ncine/trebuchet16_128.fnt");
#else
	m_pFont = new ncFont("fonts/trebuchet32_256.png", "fonts/trebuchet32_256.fnt");
#endif
	m_pTextLines = new ncTextNode(m_pRootNode, m_pFont);
	m_pTextLines->SetPosition(0, Height());

	ncServiceLocator::RegisterIndexer(new ncArrayIndexer());
	ncServiceLocator::RegisterLogger(new ncFileLogger("log.txt", ncILogger::LOG_VERBOSE, ncILogger::LOG_OFF));
	ncServiceLocator::RegisterAudioDevice(new ncALAudioDevice());
	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, (const char *)"ncApplication::Init - ncApplication initialized");

	m_pAppEventHandler = pCreateAppEventHandler();
	m_pAppEventHandler->OnInit();
	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, (const char *)"ncApplication::Init - ncIAppEventHandler::OnInit() invoked");

	// HACK: Init of the random seed
	// In the future there could be a random generator service
	srand(time(NULL));

	// HACK: Using the pause flag as a way to know if initialization is done (Android)
	m_bPaused = false;
}

/// The main game loop, handling events and rendering
void ncApplication::Run()
{
#ifndef __ANDROID__
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
					m_bPaused = !event.active.gain;
				break;
			default:
				m_pInputManager->ParseEvent(event);
				break;
			}
		}

		if (!m_bPaused)
			Step();
	}
#endif
}

/// A single step of the game loop made to render a frame
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

	ncServiceLocator::GetAudioDevice().UpdateStreams();
	m_pGfxDevice->Update();
	m_pAppEventHandler->OnFrameEnd();

	if (m_pTimer->Now() - m_ulTextUpdateTime > 100)
	{
		m_ulTextUpdateTime = m_pTimer->Now();
		sprintf(m_vTextChars, (const char *)"FPS: %.0f\nInterval: %lums\nDraw calls: %d\nVertices: %d",
			AverageFPS(), Interval(), m_pRenderQueue->NumCommands(), m_pRenderQueue->NumVertices());
		m_pTextLines->SetString(m_vTextChars);
		m_pTextLines->SetAlignment(ncTextNode::ALIGN_RIGHT);
		m_pTextLines->SetPosition((Width() - m_pTextLines->Width()), Height());
    }
}

/// Must be called before exiting to shut down the application
void ncApplication::Shutdown()
{
	m_pAppEventHandler->OnShutdown();
	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, (const char *)"ncApplication::Shutdown - ncIAppEventHandler::OnShutdown() invoked");

	delete m_pInputManager;
	if (m_pAppEventHandler)
		delete m_pAppEventHandler;

	delete m_pTextLines;
	delete m_pProfilePlotter;
	delete m_pRootNode; // deletes every child too
	delete m_pRenderQueue;
	delete m_pFont;
	delete m_pGfxDevice;
	delete m_pFrameTimer;

	if (ncServiceLocator::GetIndexer().isEmpty() == false)
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_WARN, (const char *)"ncApplication::Shutdown - The object indexer is not empty");

	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, (const char *)"ncApplication::Shutdown - ncApplication shutted down");
}

/// Sets the pause flag value
void ncApplication::SetPause(bool bPaused)
{
	m_bPaused = bPaused;
	if (m_bPaused == false)
		m_pFrameTimer->Continue();
	else
		m_pFrameTimer->Stop();
}

/// Toggles the pause flag on and off
void ncApplication::TogglePause()
{
	bool bPaused = !m_bPaused;
	SetPause(bPaused);
}

/// Sets the input handler object
void ncApplication::SetInputHandler(ncIInputEventHandler *pInputEventHandler)
{
	m_pInputManager->SetHandler(pInputEventHandler);
}
