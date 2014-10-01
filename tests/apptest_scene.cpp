#define _USE_MATH_DEFINES // for M_PI in MSVC
#include <cmath>
#include "apptest_scene.h"
#include "ncTexture.h"
#include "ncSprite.h"
#include "ncApplication.h"

static const int numTextures = 4;
static const int numSprites = 8;

ncIAppEventHandler* create_apphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::OnInit()
{
	ncIInputManager::SetHandler(this);

	m_fAngles = new float[numTextures];
	for (int i = 0; i < numTextures; i++)
	{
		m_fAngles[i] = 0.0f;
	}
	m_pTextures = new ncTexture*[numTextures];
	m_pSprites = new ncSprite*[numSprites];

	ncSceneNode &rRootNode = ncApplication::RootNode();

#ifdef __ANDROID__
	m_pTextures[0] = new ncTexture("/sdcard/ncine/texture1.pkm"); // 145x121
	m_pTextures[1] = new ncTexture("/sdcard/ncine/texture2.pkm"); // 100x100
	m_pTextures[2] = new ncTexture("/sdcard/ncine/texture3.pkm"); // 96x96
	m_pTextures[3] = new ncTexture("/sdcard/ncine/texture4.pkm"); // 96x96
#else
	m_pTextures[0] = new ncTexture("textures/texture1.png");
	m_pTextures[1] = new ncTexture("textures/texture2.png");
	m_pTextures[2] = new ncTexture("textures/texture3.png");
	m_pTextures[3] = new ncTexture("textures/texture4.png");
#endif

	m_pSprites[0] = new ncSprite(m_pTextures[0], 0, 0);
	m_pSprites[0]->SetTexRect(ncRect(40, 0, m_pSprites[0]->Width() - 80, m_pSprites[0]->Height() - 30));
	m_pSprites[0]->SetScale(0.75f);
	rRootNode.AddChildNode(m_pSprites[0]);

	for (int i = 1; i < numSprites; i++)
	{
		m_pSprites[i] = new ncSprite(m_pSprites[i - 1], m_pTextures[i % numTextures], 0, 0);
		m_pSprites[i]->SetScale(0.5f);
	}
}

void MyEventHandler::OnFrameStart()
{
	float fSinus[numTextures];
	float fCosine[numTextures];

	for (int i = 0; i < numTextures; i++)
	{
		m_fAngles[i] += (250 + 25 * i) * ncApplication::Interval();
		fSinus[i] = sinf(m_fAngles[i] * 0.01f);
		fCosine[i] = cosf(2 * m_fAngles[i] * 0.01f);
	}

	m_pSprites[0]->x = ncApplication::Width() * 0.5f + fSinus[0] * 100.0f;
	m_pSprites[0]->y = ncApplication::Height() * 0.5f + fCosine[0] * 50.0f;

	for (int i = 1; i < numSprites; i++)
	{
		m_pSprites[i]->x = fSinus[i % numTextures] * 50;
		m_pSprites[i]->y = fCosine[i % numTextures] * 50;
	}
}

void MyEventHandler::OnShutdown()
{
	delete m_pSprites[0]; // and all its children
	for (int i = 0; i < numTextures; i++)
	{
		delete m_pTextures[i];
	}

	delete[] m_pSprites;
	delete[] m_pTextures;
	delete[] m_fAngles;
}

#ifndef __ANDROID__
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
#endif
