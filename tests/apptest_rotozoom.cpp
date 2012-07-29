#define _USE_MATH_DEFINES // for M_PI in MSVC
#include <cmath>
#include "apptest_rotozoom.h"
#include "ncSceneNode.h"
#include "ncTexture.h"
#include "ncSprite.h"
#include "ncApplication.h"

static const int numTextures = 4;
static const int numRowSprites = 8;
static const int numColSprites = 8;

ncIAppEventHandler* create_apphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::OnInit()
{
	ncIInputManager::SetHandler(this);

	m_bPause = false;
	m_fAngle = 0.0f;
	ncRect texRects[numTextures];
	m_pSprites = new ncSprite*[numRowSprites*numColSprites];

	ncSceneNode &rRootNode = ncApplication::RootNode();

#ifdef __ANDROID__
	m_pMegaTexture = new ncTexture("/sdcard/ncine/megatexture_256.dds");
#else
	m_pMegaTexture = new ncTexture("textures/megatexture_256.png");
#endif
	texRects[0] = ncRect(0, 0, 145, 121);
	texRects[1] = ncRect(256-100, 0, 100, 100);
	texRects[2] = ncRect(0, 256-96, 96, 96);
	texRects[3] = ncRect(256-96, 256-96, 96, 96);

	m_pDummy = new ncSceneNode(&rRootNode, ncApplication::Width()*0.5f, ncApplication::Height()*0.5f);
	m_pDummy->SetScale(0.75f);

	int iIdx = 0;
	for (int i = -numRowSprites*0.5f; i < numRowSprites*0.5f; i++)
	{
		for (int j = -numColSprites*0.5f; j < numColSprites*0.5f; j++)
		{
			m_pSprites[iIdx] = new ncSprite(m_pDummy, m_pMegaTexture, i*100, j*100);
			m_pSprites[iIdx]->SetTexRect(texRects[iIdx%numTextures]);
			m_pSprites[iIdx]->SetScale(0.75f);
			iIdx++;
		}
	}
}

void MyEventHandler::OnFrameStart()
{
	if (m_bPause == false)
		m_fAngle += 0.0025f * ncApplication::Interval();
	float fSinus = sinf(m_fAngle);
	float fCosine = cosf(m_fAngle);

	m_pDummy->x = ncApplication::Width()*0.5f + fSinus*100.0f;
	m_pDummy->y = ncApplication::Height()*0.5f + fCosine*150.0f;
	m_pDummy->SetRotation(m_fAngle*8.0f);
	m_pDummy->SetScale(((fSinus*0.15f)+1.0f)*0.5f);

	for (int i = 0; i < numColSprites*numRowSprites; i++)
		m_pSprites[i]->SetRotation(-m_fAngle*8.0f);

	ncApplication::RootNode().x = (fSinus+1.0f)*50.0f;
	ncApplication::RootNode().y = (fCosine+1.0f)*50.0f;
	ncApplication::RootNode().SetRotation(fSinus * 10.0f);
	ncApplication::RootNode().SetScale(((fCosine*0.1f)+1.0f)*0.75f);
}

void MyEventHandler::OnShutdown()
{
	delete m_pDummy; // and all its children
	delete[] m_pSprites;
	delete m_pMegaTexture;
}

#ifdef __ANDROID__
void MyEventHandler::OnTouchDown(const ncTouchEvent &event)
{
	m_bPause = true;
}

void MyEventHandler::OnTouchUp(const ncTouchEvent &event)
{
	m_bPause = false;
}
#else
void MyEventHandler::OnMouseButtonPressed(const ncMouseEvent &event)
{
	if (event.isLeftButton())
		m_bPause = true;
}

void MyEventHandler::OnMouseButtonReleased(const ncMouseEvent &event)
{
	if (event.isLeftButton())
		m_bPause = false;
}

void MyEventHandler::OnKeyReleased(const ncKeyboardEvent &event)
{
	if (event.sym == NCKEY_ESCAPE || event.sym == NCKEY_Q)
		ncApplication::Quit();
	else if (event.sym == NCKEY_SPACE)
		ncApplication::TogglePause();
}
#endif
