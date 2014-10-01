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

int main(int argc, char **argv)
{
	SDL_Event event;
	int iWidth = 960;
	int iHeight = 640;
	bool bQuit = false;

// ----- Init ----------------------
	float fAngle = 0.0f;
	float fAngle2 = 0.0f;
	ncFrameTimer t(5.0f, 0.0f);
	ncServiceLocator::RegisterLogger(new ncFileLogger("log.txt", ncILogger::LOG_VERBOSE, ncILogger::LOG_OFF));
	ncSDLGfxDevice gfxDevice(iWidth, iHeight);
	gfxDevice.SetWindowTitle("Test");

	ncTexture tex1("textures/texture1.png");
	ncTexture tex2("textures/texture2.png");
	ncTexture tex3("textures/texture3.png");
	ncTexture tex4("textures/texture4.png");

	ncRenderQueue renderQueue;
	ncSceneNode rootNode;
	ncSprite *pSprite1 = new ncSprite(&tex1, 150, 170);
	ncSprite *pSprite2 = new ncSprite(pSprite1, &tex2, 50, 70);
	ncSprite *pSprite3 = new ncSprite(pSprite2, &tex3, 100, 50);
	ncSprite *pSprite4 = new ncSprite(pSprite3, &tex4, -50, 25);
	pSprite1->SetScale(0.75f);
	pSprite2->SetScale(0.7f);
	pSprite3->SetScale(0.7f);
	pSprite4->SetScale(0.7f);
	rootNode.AddChildNode(pSprite1);


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
		fAngle += 100.0f * t.Interval();
		fAngle2 += 250.0f * t.Interval();

		gfxDevice.Clear();

		float fSinus = sinf(fAngle * 0.01f);
		float fCosine = cosf(fAngle * 0.01f);
		float fSinus2 = sinf(fAngle2 * 0.01f);
		float fCosine2 = cosf(fAngle2 * 0.01f);

		pSprite1->x = iWidth * 0.5f + fSinus * 100;
		pSprite1->y = iHeight * 0.5f + fCosine * 100;

		pSprite2->x = fSinus * 150;
		pSprite2->y = fCosine * 150;

		pSprite3->x = -fSinus2 * 100;
		pSprite3->y = fCosine2 * 100;

		pSprite4->x = -fSinus2 * 100;
		pSprite4->y = fCosine2 * 100;


		rootNode.Update(t.Interval());
		rootNode.Visit(renderQueue);
		renderQueue.Draw();

		gfxDevice.Update();
	}


// ----- Quitting ----------------------
	delete pSprite4;
	delete pSprite3;
	delete pSprite2;
	delete pSprite1;

	ncServiceLocator::UnregisterAll();
	return 0;
}
