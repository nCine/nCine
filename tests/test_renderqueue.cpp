#ifdef WITH_GLEW
	#include <GL/glew.h>
#else
	#include <GL/gl.h>
	#include <GL/glext.h>
#endif
#include <SDL/SDL.h>

#define _USE_MATH_DEFINES // for M_PI in MSVC
#include <cmath>

#include "ncRect.h"
#include "ncFrameTimer.h"
#include "ncServiceLocator.h"
#include "ncFileLogger.h"
#include "ncSDLGfxDevice.h"
#include "ncTexture.h"
#include "ncRenderQueue.h"
#include "ncSceneNode.h"
#include "ncSprite.h"
#include "ncSpriteBatchNode.h"

//#define WITH_BATCH

int main(int argc, char **argv)
{
	SDL_Event event;
	int iWidth = 960;
	int iHeight = 640;
	bool bQuit = false;

	const int NUM_TEXTURES = 4;
	const int NUM_SPRITES = 1000;
	ncSprite *pSprites[NUM_SPRITES];
	int vRadius[NUM_SPRITES];
	int vX[NUM_SPRITES];
	int vY[NUM_SPRITES];

// ----- Init ----------------------
	float fAngle = 0.0f;
	ncFrameTimer t(5, 0);
	ncServiceLocator::RegisterLogger(new ncFileLogger("log.txt", ncILogger::LOG_VERBOSE, ncILogger::LOG_OFF));
	ncSDLGfxDevice gfxDevice(iWidth, iHeight);
	gfxDevice.SetWindowTitle("Test");

	ncRenderQueue renderQueue;
	ncSceneNode rootNode;

#ifdef WITH_BATCH
	ncTexture *pMegaTexture = new ncTexture("textures/megatexture_256.png");
	ncRect texRects[NUM_TEXTURES];
	texRects[0] = ncRect(0, 0, 145, 121);
	texRects[1] = ncRect(256 - 100, 0, 100, 100);
	texRects[2] = ncRect(0, 256 - 96, 96, 96);
	texRects[3] = ncRect(256 - 96, 256 - 96, 96, 96);
	ncSpriteBatchNode spriteBatch(&rootNode, pMegaTexture);
#else
	ncTexture *pTextures[NUM_TEXTURES];
	pTextures[0] = new ncTexture("textures/texture1.png");
	pTextures[1] = new ncTexture("textures/texture2.png");
	pTextures[2] = new ncTexture("textures/texture3.png");
	pTextures[3] = new ncTexture("textures/texture4.png");
#endif

	for (int i = 0; i < NUM_SPRITES; i++)
	{
		int numTex = rand() % NUM_TEXTURES;
		vRadius[i] = 20 + rand() % 50;
		vX[i] = rand() % iWidth;
		vY[i] = rand() % iHeight;
#ifdef WITH_BATCH
		pSprites[i] = new ncSprite(&spriteBatch, pMegaTexture, vX[i], vY[i]);
		pSprites[i]->SetTexRect(texRects[numTex]);
#else
		ncTexture *pTexture = pTextures[numTex];
		pSprites[i] = new ncSprite(pTexture, vX[i], vY[i]);
		pSprites[i]->SetPriority(numTex);
		rootNode.AddChildNode(pSprites[i]);
#endif
		float scale = 0.35f + 0.1f * (rand() % 3);
		pSprites[i]->SetScale(scale);
		float rot = rand() % 360;
		pSprites[i]->SetRotation(rot);
		unsigned char ucRandAlpha = rand() % 256;
		pSprites[i]->SetColorF(1.0f, 1.0f, 1.0f, ucRandAlpha / 255.0f);
	}


// ----- Event cycle --------------------
	while (!bQuit)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					bQuit = true;
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
						case SDLK_q:
							bQuit = true;
							break;
						case SDLK_F1:
							gfxDevice.ToggleFullScreen();
							break;
						default:
							break;
					}
					break;
			}
		}


// ----- Blitting on the screen --------
		t.AddFrame();
		fAngle += 250.0f * t.Interval();

		gfxDevice.Clear();

		float fSinus = sinf(fAngle * 0.01f);
		float fCosine = cosf(fAngle * 0.01f);

		for (int i = 0; i < NUM_SPRITES; i++)
		{
			pSprites[i]->x = vX[i] + fSinus * vRadius[i];
			pSprites[i]->y = vY[i] + fCosine * vRadius[i];
		}

		rootNode.Update(t.Interval());
		rootNode.Visit(renderQueue);
		renderQueue.Draw();

		gfxDevice.Update();
	}


// ----- Quitting ----------------------
	for (int i = 0; i < NUM_SPRITES; i++)
	{
		delete pSprites[i];
	}

#ifdef WITH_BATCH
	delete pMegaTexture;
#else
	for (int i = 0; i < NUM_TEXTURES; i++)
	{
		delete pTextures[i];
	}
#endif

	ncServiceLocator::UnregisterAll();
	return 0;
}
