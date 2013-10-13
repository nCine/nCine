#define _USE_MATH_DEFINES // for M_PI in MSVC
#include <cmath>
#include "apptest_mipmapping.h"
#include "ncSceneNode.h"
#include "ncTexture.h"
#include "ncSprite.h"
#include "ncApplication.h"

static const unsigned int maxDivider = 16;

ncIAppEventHandler* create_apphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::OnInit()
{
	ncIInputManager::SetHandler(this);

	m_bPause = false;

	ncSceneNode &rRootNode = ncApplication::RootNode();

#ifdef __ANDROID__
	m_pTexture = new ncTexture("/sdcard/ncine/checker_256_ETC1_MIP.ktx");
	//m_pTexture = new ncTexture("/sdcard/ncine/megatexture_256.dds");
#else
	//m_pTexture = new ncTexture("textures/checker_256_MIP.pvr");
	//m_pTexture = new ncTexture("textures/compressed/checker_256_MIP.dds");
	m_pTexture = new ncTexture("textures/compressed/checker_256_DXT1_MIP.ktx");
	//m_pTexture = new ncTexture("textures/checker_256.png");
#endif

	m_pDummy = new ncSceneNode(&rRootNode, ncApplication::Width()*0.5f, ncApplication::Height()*0.5f);
	m_uDivider = 1;

	m_pSprite = new ncSprite(m_pDummy, m_pTexture);
}

void MyEventHandler::OnFrameStart()
{

}

void MyEventHandler::OnShutdown()
{
	delete m_pDummy; // and all its children
	delete m_pTexture;
}

#ifdef __ANDROID__
void MyEventHandler::OnTouchUp(const ncTouchEvent &event)
{
	m_uDivider *= 2;
	if (m_uDivider > maxDivider)
		m_uDivider = 1;

	m_pSprite->SetScale(1.0f / m_uDivider);
}
#else
void MyEventHandler::OnMouseButtonReleased(const ncMouseEvent &event)
{
	if (event.isLeftButton())
	{
		m_uDivider *= 2;
		if (m_uDivider > maxDivider)
			m_uDivider = 1;
	}
	else if (event.isRightButton())
	{
		m_uDivider /= 2;
		if (m_uDivider < 1)
			m_uDivider = maxDivider;
	}

	m_pSprite->SetScale(1.0f / m_uDivider);
}

void MyEventHandler::OnKeyReleased(const ncKeyboardEvent &event)
{
	if (event.sym == NCKEY_ESCAPE || event.sym == NCKEY_Q)
		ncApplication::Quit();
	else if (event.sym == NCKEY_SPACE)
		ncApplication::TogglePause();
}
#endif
