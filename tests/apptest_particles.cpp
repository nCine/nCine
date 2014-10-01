#include "apptest_particles.h"
#include "ncApplication.h"
#include "ncTexture.h"
#include "ncParticleSystem.h"
#include "ncIInputManager.h"
#ifdef __ANDROID__
	#include "ncAndroidInputManager.h"
#endif

static const int numParticles = 50;

ncIAppEventHandler* create_apphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::OnInit()
{
	ncIInputManager::SetHandler(this);
	ncSceneNode &rRootNode = ncApplication::RootNode();

#ifdef __ANDROID__
	ncAndroidInputManager::EnableAccelerometer(true);
//	m_pTexture = new ncTexture("/sdcard/ncine/smoke_128.dds"); // Adreno SD
	m_pTexture = new ncTexture("/sdcard/ncine/smoke2_256_8888.pvr"); // Mali HD
#else
//	m_pTexture = new ncTexture("textures/smoke_256.webp");
//	m_pTexture = new ncTexture("textures/smoke_256_4444.pvr");
	m_pTexture = new ncTexture("textures/smoke_256.png");
#endif

	m_pParticleSys = new ncParticleSystem(&rRootNode, numParticles, m_pTexture, m_pTexture->Rect());
	m_pParticleSys->SetPosition(ncApplication::Width() * 0.5f, ncApplication::Height() * 0.33f);

//	pParticleSys->AddAffector(new ncAccelerationAffector(0.000025f, 0.0f));
	ncColorAffector *colAffector = new ncColorAffector();
	colAffector->AddColorStep(0.0f, ncColor(0.86f, 0.39f, 0.0f, 0.7f)); // 0.05
	colAffector->AddColorStep(0.65f, ncColor(0.86f, 0.59f, 0.0f, 0.75f)); // 0.55
	colAffector->AddColorStep(0.7f, ncColor(0.86f, 0.7f, 0.0f, 0.6)); // 0.295
	colAffector->AddColorStep(1.0f, ncColor(0.0f, 0.0f, 1.0f, 0.85)); // 0.59
	m_pParticleSys->AddAffector(colAffector);
	ncSizeAffector *sizeAffector = new ncSizeAffector(0.45f); // 0.25
	sizeAffector->AddSizeStep(0.0f, 0.01f);
	sizeAffector->AddSizeStep(0.7f, 1.6f);
	sizeAffector->AddSizeStep(1.0f, 0.4f);
	m_pParticleSys->AddAffector(sizeAffector);
	m_emitVector.Set(0.0f, 350.0f);

	m_pEmitTimer = new ncTimer();
	m_pEmitTimer->Start();
}

void MyEventHandler::OnFrameStart()
{
	if (m_pEmitTimer->Interval() > 0.085f) // 0.150f
	{
		m_pEmitTimer->Start();
		m_pParticleSys->Emit(3, 1.0f, m_emitVector); // (25, 3.0f, ncVector2f(0.0f, 100.0f))
	}
}

void MyEventHandler::OnFrameEnd()
{
#ifndef __ANDROID__
	const ncKeyboardState &keyState = ncApplication::InputManager().KeyboardState();

	if (keyState.isKeyDown(NCKEY_RIGHT))
	{
		m_pParticleSys->x += 0.1f * ncApplication::Interval();
	}
	else if (keyState.isKeyDown(NCKEY_LEFT))
	{
		m_pParticleSys->x -= 0.1f * ncApplication::Interval();
	}
	else if (keyState.isKeyDown(NCKEY_UP))
	{
		m_pParticleSys->y += 0.1f * ncApplication::Interval();
	}
	else if (keyState.isKeyDown(NCKEY_DOWN))
	{
		m_pParticleSys->y -= 0.1f * ncApplication::Interval();
	}
#endif
}

void MyEventHandler::OnShutdown()
{
	delete m_pEmitTimer;
	delete m_pParticleSys;
	delete m_pTexture;
}

#ifdef __ANDROID__
void MyEventHandler::OnTouchDown(const ncTouchEvent &event)
{
	m_pParticleSys->x = event.x;
	m_pParticleSys->y = event.y;
}

void MyEventHandler::OnTouchMove(const ncTouchEvent &event)
{
	m_pParticleSys->x = event.x;
	m_pParticleSys->y = event.y;

	if (event.count > 1)
	{
		m_emitVector.x = (event.x2 - m_pParticleSys->x) * 2.5f;
		m_emitVector.y = (event.y2 - m_pParticleSys->y) * 2.5f;
	}
}
void MyEventHandler::OnAcceleration(const ncAccelerometerEvent &event)
{
	m_pParticleSys->x += event.y * 0.75f;
	m_pParticleSys->y += -event.x * 0.75f;
}
#else
void MyEventHandler::OnKeyReleased(const ncKeyboardEvent &event)
{
	if (event.sym == NCKEY_ESCAPE || event.sym == NCKEY_Q)
	{
		ncApplication::Quit();
	}
	else if (event.sym == NCKEY_SPACE)
	{
		ncApplication::TogglePause();
	}
}

void MyEventHandler::OnMouseButtonPressed(const ncMouseEvent &event)
{
	if (event.isLeftButton())
	{
		m_pParticleSys->x = event.x;
		m_pParticleSys->y = event.y;
	}
}

void MyEventHandler::OnMouseMoved(const ncMouseState &state)
{
	if (state.isLeftButtonDown())
	{
		m_pParticleSys->x = state.x;
		m_pParticleSys->y = state.y;
	}
	else if (state.isRightButtonDown())
	{
		m_emitVector.x = (state.x - m_pParticleSys->x) * 2.5f;
		m_emitVector.y = (state.y - m_pParticleSys->y) * 2.5f;
	}
}
#endif
