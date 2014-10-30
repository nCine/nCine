#ifdef WITH_GLEW
	#include <GL/glew.h>
#else
	#include <GL/gl.h>
	#include <GL/glext.h>
#endif
#include <SDL/SDL.h>

#define _USE_MATH_DEFINES // for M_PI in MSVC
#include <cmath>

#include "FrameTimer.h"
#include "ServiceLocator.h"
#include "FileLogger.h"
#include "SdlGfxDevice.h"
#include "Texture.h"
#include "RenderQueue.h"
#include "SceneNode.h"
#include "ParticleSystem.h"

namespace nc = ncine;

const int NumParticles = 400;

int main(int argc, char **argv)
{
	SDL_Event event;
	const int Width = 960;
	const int Height = 640;
	bool shouldQuit = false;

// ----- Init ----------------------
	nc::FrameTimer t(5.0f, 0.0f);
	nc::ServiceLocator::registerLogger(new nc::FileLogger("log.txt", nc::ILogger::LOG_VERBOSE, nc::ILogger::LOG_OFF));
	nc::SdlGfxDevice gfxDevice(Width, Height);
	gfxDevice.setWindowTitle("Test");

	nc::RenderQueue renderQueue;
	nc::SceneNode rootNode;

	nc::Texture texture("textures/smoke_256.png");

	nc::ParticleSystem particleSystem(&rootNode, NumParticles, &texture, texture.rect());
	particleSystem.setPosition(Width * 0.5f, Height * 0.5f);

//	particleSystem.addAffector(new nc::AccelerationAffector(0.000025f, 0.0f));
	nc::ColorAffector *colAffector = new nc::ColorAffector();
//	colAffector->addColorStep(0.0f, nc::Color(1.0f, 1.0f, 1.0f, 0.0f)); // GRAY
//	colAffector->addColorStep(1.0f, nc::Color(1.0f, 1.0f, 1.0f, 1.0f)); // SMOKE
	colAffector->addColorStep(0.0f, nc::Color(0.86f, 0.39f, 0.0f, 0.05f));
	colAffector->addColorStep(0.65f, nc::Color(0.86f, 0.59f, 0.0f, 0.55f));
	colAffector->addColorStep(0.7f, nc::Color(0.86f, 0.7f, 0.0f, 0.295));
	colAffector->addColorStep(1.0f, nc::Color(0.0f, 0.0f, 1.0f, 0.59));
	particleSystem.addAffector(colAffector);
	nc::SizeAffector *sizeAffector = new nc::SizeAffector(0.25f);
	sizeAffector->addSizeStep(0.0f, 0.01f);
	sizeAffector->addSizeStep(0.7f, 1.6f);
	sizeAffector->addSizeStep(1.0f, 0.4f);
	particleSystem.addAffector(sizeAffector);

	srand(time(NULL));
	nc::Timer emitTimer;
	emitTimer.start();


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

		gfxDevice.clear();

		if (emitTimer.interval() > 0.25f)
		{
			emitTimer.start();
			particleSystem.emitParticles(25, 3.0f, nc::Vector2f(0.0f, 100.0f));
		}

		rootNode.update(t.interval());
		rootNode.visit(renderQueue);
		renderQueue.draw();

		gfxDevice.update();
	}


// ----- Quitting ----------------------
	nc::ServiceLocator::unregisterAll();
	return 0;
}
