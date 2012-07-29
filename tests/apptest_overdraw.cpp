#define _USE_MATH_DEFINES // for M_PI in MSVC
#include <cmath>
#include "apptest_overdraw.h"
#include "ncSceneNode.h"
#include "ncTexture.h"
#include "ncSprite.h"
#include "ncApplication.h"

static const int numSprites = 100;

ncIAppEventHandler* create_apphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::OnInit()
{
	ncIInputManager::SetHandler(this);
	ncSceneNode &rRootNode = ncApplication::RootNode();

#ifdef __ANDROID__
	m_pMegaTexture = new ncTexture("/sdcard/ncine/megatexture_256.dds");
	m_pAlphaTexture = new ncTexture("/sdcard/ncine/transparent_128.png");
#else
	m_pMegaTexture = new ncTexture("textures/megatexture_256.png");
	m_pAlphaTexture = new ncTexture("textures/transparent_128.png");
#endif

	m_pDummy = new ncSceneNode(&rRootNode, ncApplication::Width()*0.5f, ncApplication::Height()*0.5f);

	m_pSprites = new ncSprite*[numSprites];
	for (int i = 0; i < numSprites; i++)
	{
		m_pSprites[i] = new ncSprite(m_pDummy, m_pMegaTexture, 0, 0);
		m_pSprites[i]->SetScale(1.35f);
		m_pSprites[i]->SetPriority(ncDrawableNode::SCENE_PRIORITY + 5);
	}

	m_pAlphaSpriteBottom = new ncSprite(m_pDummy, m_pAlphaTexture, 0, 150);
	m_pAlphaSpriteBottom->SetScale(2.0f);
	m_pAlphaSpriteBottom->SetPriority(ncDrawableNode::SCENE_PRIORITY + 1);
	m_pAlphaSpriteBottom->SetColor(0, 0, 255, 255);
	m_pAlphaSpriteTop = new ncSprite(m_pDummy, m_pAlphaTexture, -0, -150);
	m_pAlphaSpriteTop->SetScale(2.0f);
	m_pAlphaSpriteTop->SetPriority(ncDrawableNode::SCENE_PRIORITY + 10);
	m_pAlphaSpriteTop->SetColor(255, 0, 0, 255);
}

void MyEventHandler::OnShutdown()
{
	delete m_pDummy; // and all its children
	delete[] m_pSprites;
	delete m_pMegaTexture;
	delete m_pAlphaTexture;
}

#ifndef __ANDROID__
void MyEventHandler::OnKeyReleased(const ncKeyboardEvent &event)
{
	if (event.sym == NCKEY_ESCAPE || event.sym == NCKEY_Q)
		ncApplication::Quit();
}
#endif
