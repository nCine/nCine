#include <ctime>
#include "ncApplication.h"
#include "ncIAppEventHandler.h"
#include "ncServiceLocator.h"
#include "ncArrayIndexer.h"
#include "ncFileLogger.h"
#include "ncALAudioDevice.h"
#include "ncLinePlotter.h"
#include "ncStackedBarPlotter.h"
#include "ncFont.h"
#include "ncTextNode.h"
#include "ncThreadPool.h"

#if defined(__ANDROID__)
	#include "ncEGLGfxDevice.h"
	#include "ncAndroidInputManager.h"
	#include "ncAssetFile.h"
#elif defined(WITH_SDL)
	#include "ncSDLGfxDevice.h"
	#include "ncSDLInputManager.h"
#elif defined(WITH_GLFW)
	#include "ncGLFWGfxDevice.h"
	#include "ncGLFWInputManager.h"
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
ncTimer *ncApplication::m_pProfileTimer = NULL;
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
	// Registering the logger as early as possible
	ncServiceLocator::RegisterLogger(new ncFileLogger("/sdcard/ncine_log.txt", ncILogger::LOG_VERBOSE, ncILogger::LOG_VERBOSE));
	if (ncEGLGfxDevice::isModeSupported(state, ncDisplayMode(8, 8, 8)))
		m_pGfxDevice = new ncEGLGfxDevice(state, ncDisplayMode(8, 8, 8));
	else
		m_pGfxDevice = new ncEGLGfxDevice(state, ncDisplayMode(5, 6, 5));
	m_pInputManager = new ncAndroidInputManager();
	ncAssetFile::InitAssetManager(state);
#else
void ncApplication::Init(ncIAppEventHandler* (*pCreateAppEventHandler)())
{
	// Registering the logger as early as possible
	ncServiceLocator::RegisterLogger(new ncFileLogger("ncine_log.txt", ncILogger::LOG_VERBOSE, ncILogger::LOG_OFF));
	// Graphics device should always be created before the input manager!
#if defined(WITH_SDL)
	m_pGfxDevice = new ncSDLGfxDevice(960, 640);
	m_pInputManager = new ncSDLInputManager();
#elif defined(WITH_GLFW)
	m_pGfxDevice = new ncGLFWGfxDevice(960, 640);
	m_pInputManager = new ncGLFWInputManager();
#endif
	m_pGfxDevice->SetWindowTitle("nCine");
#endif

	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncApplication::Init - nCine compiled on " __DATE__ " at " __TIME__);
	ncServiceLocator::RegisterIndexer(new ncArrayIndexer());
	ncServiceLocator::RegisterAudioDevice(new ncALAudioDevice());
#ifdef WITH_THREADS
	ncServiceLocator::RegisterThreadPool(new ncThreadPool());
#endif
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncApplication::Init - Data path: %s", ncIFile::DataPath());

	m_pFrameTimer = new ncFrameTimer(5, 100);
	m_pRootNode = new ncSceneNode();
	m_pRenderQueue = new ncRenderQueue();
	m_pProfileTimer = new ncTimer();

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

#if defined(__ANDROID__)
//	m_pFont = new ncFont("asset::trebuchet16_128.dds.mp3", "asset::trebuchet16_128.fnt");
	m_pFont = new ncFont("/sdcard/ncine/trebuchet32_256_4444.pvr", "/sdcard/ncine/trebuchet32_256.fnt");
//	m_pFont = new ncFont("/sdcard/ncine/trebuchet16_128.dds", "/sdcard/ncine/trebuchet16_128.fnt");
#else
	m_pFont = new ncFont("fonts/trebuchet32_256.png", "fonts/trebuchet32_256.fnt");
#endif
	m_pTextLines = new ncTextNode(m_pRootNode, m_pFont);
	m_pTextLines->SetPosition(0, Height());

	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncApplication::Init - ncApplication initialized");

	m_pAppEventHandler = pCreateAppEventHandler();
	m_pAppEventHandler->OnInit();
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncApplication::Init - ncIAppEventHandler::OnInit() invoked");

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
	while (!m_bShouldQuit) {
#if defined(WITH_SDL)
		SDL_Event event;

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
				ncSDLInputManager::ParseEvent(event);
				break;
			}
		}
#elif defined(WITH_GLFW)
		m_bPaused = !ncGLFWInputManager::hasFocus();
		glfwPollEvents();
		ncGLFWInputManager::UpdateJoystickStates();
#endif

		if (!m_bPaused)
			Step();
	}
#endif
}

/// A single step of the game loop made to render a frame
void ncApplication::Step()
{
	m_pProfileTimer->Start();
	m_pFrameTimer->AddFrame();
	m_pGfxDevice->Clear();
	m_pAppEventHandler->OnFrameStart();
	m_pProfilePlotter->AddValue(0, m_pProfileTimer->PreciseInterval());
//	m_pProfilePlotter->AddValue(0, 1.0f * abs(rand()%34));

	m_pProfileTimer->Start();
	m_pRootNode->Update(m_pFrameTimer->Interval());
	m_pRootNode->Visit(*m_pRenderQueue);
	m_pProfilePlotter->AddValue(1, m_pProfileTimer->PreciseInterval());
//	m_pProfilePlotter->AddValue(1, 1.0f * abs(rand()%33));

	m_pProfileTimer->Start();
	m_pRenderQueue->Draw();
	m_pProfilePlotter->AddValue(2, m_pProfileTimer->PreciseInterval());
//	m_pProfilePlotter->AddValue(2, 1.0f * abs(rand()%33));

	ncServiceLocator::AudioDevice().UpdatePlayers();
	m_pGfxDevice->Update();
	m_pAppEventHandler->OnFrameEnd();

	// TODO: hard-coded 100ms update time
	if (ncTimer::Now() - m_ulTextUpdateTime > 100)
	{
		m_ulTextUpdateTime = ncTimer::Now();
		sprintf(m_vTextChars, (const char *)"FPS: %.0f (%.2fms)\nSprites: %uV, %uDC\nParticles: %uV, %uDC\nText: %uV, %uDC\nPlotter: %uV, %uDC\nTotal: %uV, %uDC",
				m_pFrameTimer->AverageFPS(), m_pFrameTimer->PreciseInterval(),
				m_pRenderQueue->NumVertices(ncRenderCommand::SPRITE_TYPE), m_pRenderQueue->NumCommands(ncRenderCommand::SPRITE_TYPE),
				m_pRenderQueue->NumVertices(ncRenderCommand::PARTICLE_TYPE), m_pRenderQueue->NumCommands(ncRenderCommand::PARTICLE_TYPE),
				m_pRenderQueue->NumVertices(ncRenderCommand::TEXT_TYPE), m_pRenderQueue->NumCommands(ncRenderCommand::TEXT_TYPE),
				m_pRenderQueue->NumVertices(ncRenderCommand::PLOTTER_TYPE), m_pRenderQueue->NumCommands(ncRenderCommand::PLOTTER_TYPE),
				m_pRenderQueue->NumVertices(), m_pRenderQueue->NumCommands());

		m_pTextLines->SetString(m_vTextChars);
		m_pTextLines->SetAlignment(ncTextNode::ALIGN_RIGHT);
		m_pTextLines->SetPosition((Width() - m_pTextLines->Width()), Height());
	}
}

/// Must be called before exiting to shut down the application
void ncApplication::Shutdown()
{
	m_pAppEventHandler->OnShutdown();
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncApplication::Shutdown - ncIAppEventHandler::OnShutdown() invoked");

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

	if (ncServiceLocator::Indexer().isEmpty() == false)
		ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"ncApplication::Shutdown - The object indexer is not empty");

	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncApplication::Shutdown - ncApplication shutted down");

	ncServiceLocator::UnregisterAll();
}

/// Sets the pause flag value
void ncApplication::SetPause(bool bPaused)
{
	m_bPaused = bPaused;
	if (m_bPaused == false)
		m_pFrameTimer->Start();
}

/// Toggles the pause flag on and off
void ncApplication::TogglePause()
{
	bool bPaused = !m_bPaused;
	SetPause(bPaused);
}

/// Shows or hides profiling graphs
void ncApplication::ShowProfileGraphs(bool bShouldDraw)
{
	m_pProfilePlotter->bShouldDraw = bShouldDraw;
}

/// Shows or hides profiling information text
void ncApplication::ShowProfileInfo(bool bShouldDraw)
{
	m_pTextLines->bShouldDraw = bShouldDraw;
}
