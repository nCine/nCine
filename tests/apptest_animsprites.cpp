#include "apptest_animsprites.h"
#include "ncApplication.h"
#include "ncAudioStreamPlayer.h"
#include "ncTexture.h"
#include "ncAnimatedSprite.h"
#include "ncFont.h"
#include "ncTextNode.h"
#include "ncIInputManager.h"

//#define WITH_8DIRECTIONS

ncIAppEventHandler* create_apphandler()
{
	return new MyEventHandler;
}

void MyEventHandler::OnInit()
{
	ncIInputManager::SetHandler(this);
	ncSceneNode &rRootNode = ncApplication::RootNode();

#ifdef __ANDROID__
	m_pAudioPlayer = new ncAudioStreamPlayer("sdcard/ncine/music.ogg");
	m_pTexture = new ncTexture("/sdcard/ncine/abta_playertwo.dds");
//	m_pAudioPlayer = new ncAudioStreamPlayer("asset::bomb.ogg");
//	m_pTexture = new ncTexture("asset::abta_player.dds.mp3");
#else
	m_pAudioPlayer = new ncAudioStreamPlayer("sounds/music.ogg");
	m_pTexture = new ncTexture("textures/abta_playertwo.png");
//	m_pTexture = new ncTexture("textures/abta_playertwo_bc3.dds");
#endif

	m_pAudioPlayer->SetLooping(true);
	m_pAudioPlayer->Play();
	m_pAnimSprite = new ncAnimatedSprite(&rRootNode, m_pTexture);
	// Up
	ncRectAnimation *pAnimation = new ncRectAnimation(0.06f, true, true);
	pAnimation->AddRect(0, 0, 32, 32);
	pAnimation->AddRect(32, 0, 32, 32);
	pAnimation->AddRect(64, 0, 32, 32);
	pAnimation->AddRect(96, 0, 32, 32);
	pAnimation->AddRect(128, 0, 32, 32);
	m_pAnimSprite->AddAnimation(pAnimation);
#ifdef WITH_8DIRECTIONS
	// Up-right
	pAnimation = new ncRectAnimation(0.06f, true, true);
	pAnimation->AddRect(160, 0, 32, 32);
	pAnimation->AddRect(192, 0, 32, 32);
	pAnimation->AddRect(224, 0, 32, 32);
	pAnimation->AddRect(256, 0, 32, 32);
	pAnimation->AddRect(288, 0, 32, 32);
	m_pAnimSprite->AddAnimation(pAnimation);
	// Right
	pAnimation = new ncRectAnimation(0.06f, true, true);
	pAnimation->AddRect(320, 0, 32, 32);
	pAnimation->AddRect(352, 0, 32, 32);
	pAnimation->AddRect(384, 0, 32, 32);
	pAnimation->AddRect(416, 0, 32, 32);
	pAnimation->AddRect(448, 0, 32, 32);
	m_pAnimSprite->AddAnimation(pAnimation);
	// Down-right
	pAnimation = new ncRectAnimation(0.06f, true, true);
	pAnimation->AddRect(480, 0, 32, 32);
	pAnimation->AddRect(512, 0, 32, 32);
	pAnimation->AddRect(544, 0, 32, 32);
	pAnimation->AddRect(576, 0, 32, 32);
	pAnimation->AddRect(608, 0, 32, 32);
	m_pAnimSprite->AddAnimation(pAnimation);
	// Down
	pAnimation = new ncRectAnimation(0.06f, true, true);
	pAnimation->AddRect(640, 0, 32, 32);
	pAnimation->AddRect(0, 32, 32, 32);
	pAnimation->AddRect(32, 32, 32, 32);
	pAnimation->AddRect(64, 32, 32, 32);
	pAnimation->AddRect(96, 32, 32, 32);
	m_pAnimSprite->AddAnimation(pAnimation);
	// Down-left
	pAnimation = new ncRectAnimation(0.06f, true, true);
	pAnimation->AddRect(128, 32, 32, 32);
	pAnimation->AddRect(160, 32, 32, 32);
	pAnimation->AddRect(192, 32, 32, 32);
	pAnimation->AddRect(224, 32, 32, 32);
	pAnimation->AddRect(256, 32, 32, 32);
	m_pAnimSprite->AddAnimation(pAnimation);
	// Left
	pAnimation = new ncRectAnimation(0.06f, true, true);
	pAnimation->AddRect(288, 32, 32, 32);
	pAnimation->AddRect(320, 32, 32, 32);
	pAnimation->AddRect(352, 32, 32, 32);
	pAnimation->AddRect(384, 32, 32, 32);
	pAnimation->AddRect(416, 32, 32, 32);
	m_pAnimSprite->AddAnimation(pAnimation);
	// Up-left
	pAnimation = new ncRectAnimation(0.06f, true, true);
	pAnimation->AddRect(448, 32, 32, 32);
	pAnimation->AddRect(480, 32, 32, 32);
	pAnimation->AddRect(512, 32, 32, 32);
	pAnimation->AddRect(544, 32, 32, 32);
	pAnimation->AddRect(576, 32, 32, 32);
	m_pAnimSprite->AddAnimation(pAnimation);
	// Special
	pAnimation = new ncRectAnimation(0.06f, true, true);
	pAnimation->AddRect(608, 32, 32, 32);
	pAnimation->AddRect(640, 32, 32, 32);
	pAnimation->AddRect(0, 64, 32, 32);
	pAnimation->AddRect(32, 64, 32, 32);
	pAnimation->AddRect(64, 64, 32, 32);
	m_pAnimSprite->AddAnimation(pAnimation);
#endif

	m_pAnimSprite->SetPosition(ncApplication::Width() * 0.5f, ncApplication::Height() * 0.5f);
	m_pAnimSprite->SetAnimation(0);
	m_pAnimSprite->SetFrame(0);
	m_pAnimSprite->SetPaused(true);
	m_destVector = m_pAnimSprite->Position();
}

void MyEventHandler::OnFrameStart()
{
	ncVector2f reachVector = m_destVector - m_pAnimSprite->AbsPosition();
	if (reachVector.Length() > 1.0f)
	{
		reachVector.Normalize();
		m_pAnimSprite->SetPaused(false);

#ifdef WITH_8DIRECTIONS
		const float dirTolerance = 0.3f;
		if (reachVector.x < -dirTolerance) // Right
		{
			if (reachVector.y > dirTolerance)
			{
				m_pAnimSprite->SetAnimation(1);    // Up-right
			}
			else if (reachVector.y < -dirTolerance)
			{
				m_pAnimSprite->SetAnimation(3);    // Down-right
			}
			else
			{
				m_pAnimSprite->SetAnimation(2);    // Right
			}
		}
		else if (reachVector.x > dirTolerance) // Left
		{
			if (reachVector.y > dirTolerance)
			{
				m_pAnimSprite->SetAnimation(7);    // Up-left
			}
			else if (reachVector.y < -dirTolerance)
			{
				m_pAnimSprite->SetAnimation(5);    // Down-left
			}
			else
			{
				m_pAnimSprite->SetAnimation(6);    // Left
			}
		}
		else // Pure up or down
		{
			if (reachVector.y > 0.0f)
			{
				m_pAnimSprite->SetAnimation(0);    // Up
			}
			else
			{
				m_pAnimSprite->SetAnimation(4);    // Down
			}
		}
#else
		float fAngle = -(atan2(reachVector.y, reachVector.x) - atan2(1.0f, 0.0f)) * 180.0f / M_PI;
		if (fAngle < 0.0f)
		{
			fAngle += 360.0f;
		}
		m_pAnimSprite->SetRotation(fAngle);
#endif

		reachVector *= ncApplication::Interval() * 100.0f;
		m_pAnimSprite->Move(reachVector);
	}
	else
	{
		m_pAnimSprite->SetFrame(0);
		m_pAnimSprite->SetPaused(true);
	}
}

void MyEventHandler::OnShutdown()
{
	delete m_pAudioPlayer;
	delete m_pAnimSprite;
	delete m_pTexture;
}

#ifdef __ANDROID__
void MyEventHandler::OnTouchDown(const ncTouchEvent &event)
{
	m_destVector.x = event.x;
	m_destVector.y = event.y;
}

void MyEventHandler::OnTouchMove(const ncTouchEvent &event)
{
	m_destVector.x = event.x;
	m_destVector.y = event.y;
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
		m_destVector.x = event.x;
		m_destVector.y = event.y;
	}
}

void MyEventHandler::OnMouseMoved(const ncMouseState &state)
{
	if (state.isLeftButtonDown())
	{
		m_destVector.x = state.x;
		m_destVector.y = state.y;
	}
}
#endif
