#ifdef WITH_GLEW
	#include <GL/glew.h>
#else
	#include <GL/gl.h>
	#include <GL/glext.h>
#endif
#include <SDL/SDL.h>

#define _USE_MATH_DEFINES // for M_PI in MSVC
#include <cmath>

#include "Rect.h"
#include "FrameTimer.h"
#include "ServiceLocator.h"
#include "FileLogger.h"
#include "SdlGfxDevice.h"
#include "Texture.h"
#include "RenderQueue.h"
#include "SceneNode.h"
#include "Sprite.h"

namespace nc = ncine;

int main(int argc, char **argv)
{
	SDL_Event event;
	const int Width = 960;
	const int Height = 640;
	bool shouldQuit = false;

// ----- Init ----------------------
	float angle = 0.0f;
	float angle2 = 0.0f;
	nc::FrameTimer t(5.0f, 0.0f);
	nc::ServiceLocator::registerLogger(new nc::FileLogger("log.txt", nc::ILogger::LOG_VERBOSE, nc::ILogger::LOG_OFF));
	nc::SdlGfxDevice gfxDevice(Width, Height);
	gfxDevice.setWindowTitle("Test");

	nc::Texture tex1("textures/texture1.png");
	nc::Texture tex2("textures/texture2.png");
	nc::Texture tex3("textures/texture3.png");
	nc::Texture tex4("textures/texture4.png");

	nc::RenderQueue renderQueue;
	nc::SceneNode rootNode;
	nc::Sprite *sprite1 = new nc::Sprite(&tex1, 150, 170);
	nc::Sprite *sprite2 = new nc::Sprite(sprite1, &tex2, 50, 70);
	nc::Sprite *sprite3 = new nc::Sprite(sprite2, &tex3, 100, 50);
	nc::Sprite *sprite4 = new nc::Sprite(sprite3, &tex4, -50, 25);
	sprite1->setScale(0.75f);
	sprite2->setScale(0.7f);
	sprite3->setScale(0.7f);
	sprite4->setScale(0.7f);
	rootNode.addChildNode(sprite1);


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
		angle += 100.0f * t.interval();
		angle2 += 250.0f * t.interval();

		gfxDevice.clear();

		float sine = sinf(angle * 0.01f);
		float cosine = cosf(angle * 0.01f);
		float sine2 = sinf(angle2 * 0.01f);
		float cosine2 = cosf(angle2 * 0.01f);

		sprite1->x = Width * 0.5f + sine * 100;
		sprite1->y = Height * 0.5f + cosine * 100;

		sprite2->x = sine * 150;
		sprite2->y = cosine * 150;

		sprite3->x = -sine2 * 100;
		sprite3->y = cosine2 * 100;

		sprite4->x = -sine2 * 100;
		sprite4->y = cosine2 * 100;


		rootNode.update(t.interval());
		rootNode.visit(renderQueue);
		renderQueue.draw();

		gfxDevice.update();
	}


// ----- Quitting ----------------------
	delete sprite4;
	delete sprite3;
	delete sprite2;
	delete sprite1;

	nc::ServiceLocator::unregisterAll();
	return 0;
}
