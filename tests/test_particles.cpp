#ifndef NO_GLEW
	#include <GL/glew.h>
#else
	#include <GL/gl.h>
	#include <GL/glext.h>
#endif
#include <SDL/SDL.h>

#include <cmath>

#include "ncFrameTimer.h"
#include "ncServiceLocator.h"
#include "ncFileLogger.h"
#include "ncSDLGfxDevice.h"
#include "ncTexture.h"
#include "ncRenderQueue.h"
#include "ncSceneNode.h"
#include "ncParticleSystem.h"

const int NUM_PARTICLES = 400;

int main(int argc, char **argv)
{
	SDL_Event event;
	int iWidth = 960;
	int iHeight = 640;
	bool bQuit = false;

// ----- Init ----------------------
	ncFrameTimer t(5, 0);
	ncServiceLocator::RegisterLogger(new ncFileLogger("log.txt", ncILogger::LOG_VERBOSE, ncILogger::LOG_OFF));
	ncSDLGfxDevice gfxDevice(iWidth, iHeight);
	SDL_WM_SetCaption("Test", NULL);

	ncRenderQueue renderQueue;
	glEnable(GL_BLEND); // HACK: for alpha blending
	ncSceneNode rootNode;

	ncTexture texture("smoke.png");
//	ncTexture texture("smoke_rgb.png");
//	ncTexture textureAlpha("smoke_red.png");
//	texture.SetAlphaFromRed(&textureAlpha);

	ncParticleSystem particleSys(NUM_PARTICLES, &texture, 0.25f);
	particleSys.SetPosition(iWidth*0.5f, iHeight*0.5f);

//	particleSys.AddAffector(new ncAccelerationAffector(0.000025f, 0.0f));
	ncColorAffector *colAffector = new ncColorAffector();
//	colAffector->AddColorStep(0.0f, ncColor(1.0f, 1.0f, 1.0f, 0.0f)); // GRAY
//	colAffector->AddColorStep(1.0f, ncColor(1.0f, 1.0f, 1.0f, 1.0f)); // SMOKE
	colAffector->AddColorStep(0.0f, ncColor(0.86f, 0.39f, 0.0f, 0.05f));
	colAffector->AddColorStep(0.65f, ncColor(0.86f, 0.59f, 0.0f, 0.55f));
	colAffector->AddColorStep(0.7f, ncColor(0.86f, 0.7f, 0.0f, 0.295));
	colAffector->AddColorStep(1.0f, ncColor(0.0f, 0.0f, 1.0f, 0.59));
	particleSys.AddAffector(colAffector);
	ncSizeAffector *sizeAffector = new ncSizeAffector(0.25f);
	sizeAffector->AddSizeStep(0.0f, 0.01f);
	sizeAffector->AddSizeStep(0.7f, 1.6f);
	sizeAffector->AddSizeStep(1.0f, 0.4f);
	particleSys.AddAffector(sizeAffector);

	rootNode.AddChildNode(&particleSys);

	srand(time(NULL));
	t.Reset();
	unsigned long int ulStartTime = t.Now();


// ----- Event cycle --------------------
	while (!bQuit) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				bQuit = true;
				break;
			case SDL_KEYDOWN:
				switch( event.key.keysym.sym ){
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
			}
		}


// ----- Blitting on the screen --------
		t.AddFrame();

		gfxDevice.Clear();

		if (t.Now() - ulStartTime > 150)
		{
			ulStartTime = t.Now();
			particleSys.Emit(25, 3000, ncVector2f(0.0f, 0.1f));
		}

		rootNode.Update(t.Interval());
		rootNode.Visit(renderQueue);
		renderQueue.Draw();

		gfxDevice.Update();
	}


// ----- Quitting ----------------------
//	SDL_Quit(); // GfxDevice should call this

	return 0;	
}
