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
	const int Width = 960;
	const int Height = 640;
	bool shouldQuit = false;

	const int NumTextures = 4;
	const int NumSprites = 1000;
	ncSprite *sprites[NumSprites];
	int radii[NumSprites];
	int xCoords[NumSprites];
	int yCoords[NumSprites];

// ----- Init ----------------------
	float angle = 0.0f;
	ncFrameTimer t(5, 0);
	ncServiceLocator::registerLogger(new ncFileLogger("log.txt", ncILogger::LOG_VERBOSE, ncILogger::LOG_OFF));
	ncSDLGfxDevice gfxDevice(Width, Height);
	gfxDevice.setWindowTitle("Test");

	ncRenderQueue renderQueue;
	ncSceneNode rootNode;

#ifdef WITH_BATCH
	ncTexture *megaTexture = new ncTexture("textures/megatexture_256.png");
	ncRect texRects[NUM_TEXTURES];
	texRects[0] = ncRect(0, 0, 145, 121);
	texRects[1] = ncRect(256 - 100, 0, 100, 100);
	texRects[2] = ncRect(0, 256 - 96, 96, 96);
	texRects[3] = ncRect(256 - 96, 256 - 96, 96, 96);
	ncSpriteBatchNode spriteBatch(&rootNode, megaTexture);
#else
	ncTexture *textures[NumTextures];
	textures[0] = new ncTexture("textures/texture1.png");
	textures[1] = new ncTexture("textures/texture2.png");
	textures[2] = new ncTexture("textures/texture3.png");
	textures[3] = new ncTexture("textures/texture4.png");
#endif

	for (int i = 0; i < NumSprites; i++)
	{
		int numTex = rand() % NumTextures;
		radii[i] = 20 + rand() % 50;
		xCoords[i] = rand() % Width;
		yCoords[i] = rand() % Height;
#ifdef WITH_BATCH
		sprites[i] = new ncSprite(&spriteBatch, megaTexture, xCoords[i], yCoords[i]);
		sprites[i]->setTexRect(texRects[numTex]);
#else
		ncTexture *texture = textures[numTex];
		sprites[i] = new ncSprite(texture, xCoords[i], yCoords[i]);
		sprites[i]->setPriority(numTex);
		rootNode.addChildNode(sprites[i]);
#endif
		float scale = 0.35f + 0.1f * (rand() % 3);
		sprites[i]->setScale(scale);
		float rot = rand() % 360;
		sprites[i]->setRotation(rot);
		unsigned char ucRandAlpha = rand() % 256;
		sprites[i]->setColorF(1.0f, 1.0f, 1.0f, ucRandAlpha / 255.0f);
	}


// ----- Event cycle --------------------
	while (!shouldQuit)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					shouldQuit = true;
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
						case SDLK_q:
							shouldQuit = true;
							break;
						case SDLK_F1:
							gfxDevice.toggleFullScreen();
							break;
						default:
							break;
					}
					break;
			}
		}


// ----- Blitting on the screen --------
		t.addFrame();
		angle += 250.0f * t.interval();

		gfxDevice.clear();

		float sine = sinf(angle * 0.01f);
		float cosine = cosf(angle * 0.01f);

		for (int i = 0; i < NumSprites; i++)
		{
			sprites[i]->x = xCoords[i] + sine * radii[i];
			sprites[i]->y = yCoords[i] + cosine * radii[i];
		}

		rootNode.update(t.interval());
		rootNode.visit(renderQueue);
		renderQueue.draw();

		gfxDevice.update();
	}


// ----- Quitting ----------------------
	for (int i = 0; i < NumSprites; i++)
	{
		delete sprites[i];
	}

#ifdef WITH_BATCH
	delete megaTexture;
#else
	for (int i = 0; i < NumTextures; i++)
	{
		delete textures[i];
	}
#endif

	ncServiceLocator::unregisterAll();
	return 0;
}
