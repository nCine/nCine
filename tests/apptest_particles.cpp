#include <cmath>
#include "apptest_particles.h"
#include "ncApplication.h"
#include "ncTexture.h"
#include "ncParticleSystem.h"
#include "ncFont.h"
#include "ncTextNode.h"
#include "ncIInputManager.h"

static const int numParticles = 50;

ncIAppEventHandler* create_apphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::OnInit()
{
	ncApplication::SetInputHandler(this);
	ncSceneNode &rRootNode = ncApplication::RootNode();

#ifdef __ANDROID__
	m_pFont = new ncFont("/sdcard/ncine/trebuchet16_128.dds", "/sdcard/ncine/trebuchet16_128.fnt");
	m_pTexture = new ncTexture("/sdcard/ncine/smoke_128.dds");
#else
	m_pFont = new ncFont("fonts/trebuchet32_256.png", "fonts/trebuchet32_256.fnt");
	m_pTexture = new ncTexture("textures/smoke_256.png");
//	ncTexture textureAlpha("textures/smoke_256_red.png");
//	m_pTexture->SetAlphaFromRed(&textureAlpha);
#endif

	m_pFpsText = new ncTextNode(&rRootNode, m_pFont);
	m_pFpsText->SetString("FPS: ");
	m_pFpsText->SetPosition((ncApplication::Width() - m_pFpsText->XAdvanceSum()), ncApplication::Height() - m_pFpsText->FontBase());

	m_pParticleSys = new ncParticleSystem(numParticles, m_pTexture, 0.225f);
	m_pParticleSys->SetPosition(ncApplication::Width()*0.5f, ncApplication::Height()*0.33f);

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
	m_emitVector.Set(0.0f, 0.35f);

	rRootNode.AddChildNode(m_pParticleSys);

	m_pTimer = new ncTimer();
	m_pTimer->Reset();
	unsigned long int m_ulStartTime = m_pTimer->Now();
}

void MyEventHandler::OnFrameStart()
{
	if (m_pTimer->Now() - m_ulUpdateFpsTime > 100)
	{
		m_ulUpdateFpsTime = m_pTimer->Now();
		sprintf(m_vFPS, (const char *)"FPS: %.0f", ncApplication::AverageFPS());
		m_pFpsText->SetString(m_vFPS);
		m_pFpsText->SetPosition((ncApplication::Width() - m_pFpsText->XAdvanceSum()), ncApplication::Height() - m_pFpsText->FontBase());
	}

	if (m_pTimer->Now() - m_ulEmitTime > 85) // 150
	{
		m_ulEmitTime = m_pTimer->Now();
		m_pParticleSys->Emit(3, 1000, m_emitVector); // (25, 3000, ncVector2f(0.0, 0.1))
	}
}

void MyEventHandler::OnFrameEnd()
{
#ifndef __ANDROID__
	unsigned char *keyState = ncApplication::InputManager().KeyboardState();

	if (keyState[SDLK_RIGHT])
		m_pParticleSys->x += 0.1f * ncApplication::Interval();
	else if (keyState[SDLK_LEFT])
		m_pParticleSys->x -= 0.1f * ncApplication::Interval();
	else if (keyState[SDLK_UP])
		m_pParticleSys->y += 0.1f * ncApplication::Interval();
	else if (keyState[SDLK_DOWN])
		m_pParticleSys->y -= 0.1f * ncApplication::Interval();
#endif
}

void MyEventHandler::OnShutdown()
{
	delete m_pTimer;
	delete m_pParticleSys;
	delete m_pFpsText;
	delete m_pTexture;
	delete m_pFont;
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
		m_emitVector.x = (event.x2 - m_pParticleSys->x) * 0.0025f;
		m_emitVector.y = (event.y2 - m_pParticleSys->y) * 0.0025f;
	}
}
#else
void MyEventHandler::OnKeyReleased(const ncKeyboardEvent &event)
{
	if (event.sym == NCKEY_ESCAPE || event.sym == NCKEY_Q)
		ncApplication::Quit();
	else if (event.sym == NCKEY_SPACE)
		ncApplication::TogglePause();
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
	if (state.isButtonDown(ncMouseState::RIGHT_BUTTON))
	{
		m_emitVector.x = (state.x - m_pParticleSys->x) * 0.0025f;
		m_emitVector.y = (state.y - m_pParticleSys->y) * 0.0025f;
	}
}
#endif
