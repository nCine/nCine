#ifdef WITH_GLEW
	#include <GL/glew.h>
#else
	#include <GL/gl.h>
	#include <GL/glext.h>
#endif
#include <SDL/SDL.h>

#include "ncFrameTimer.h"
#include "ncServiceLocator.h"
#include "ncFileLogger.h"
#include "ncSDLGfxDevice.h"
#include "ncFont.h"
#include "ncTextNode.h"

int main(int argc, char **argv)
{
	SDL_Event event;
	int iWidth = 960;
	int iHeight = 640;
	bool bQuit = false;

// ----- Init ----------------------
	ncFrameTimer t(5, 100);
	ncServiceLocator::RegisterLogger(new ncFileLogger("log.txt", ncILogger::LOG_VERBOSE, ncILogger::LOG_OFF));
	ncSDLGfxDevice gfxDevice(iWidth, iHeight);
	SDL_WM_SetCaption("Test", NULL);

	ncRenderQueue renderQueue;
	ncSceneNode rootNode;

	ncFont font1("fonts/trebuchet32_256.png", "fonts/trebuchet32_256.fnt");
	ncFont font2("fonts/arial32_256.png", "fonts/arial32_256.fnt");
	ncFont font3("fonts/verdana32_256.png", "fonts/verdana32_256.fnt");

	char vFPS[16];
	ncTextNode fpsText(&rootNode, &font1);
	fpsText.SetScale(0.85f);
	fpsText.SetString("FPS: ");
	fpsText.SetPosition((iWidth - fpsText.Width()), iHeight);

	char vTestString[] = "WAY.P.ATAV";
	int iTextHeight = iHeight * 0.7f;
	ncTextNode text1(&rootNode, &font1);
	text1.SetScale(2.0f);
	text1.SetString(vTestString);
	text1.SetPosition((iWidth - text1.Width())*0.5f, iTextHeight);
	text1.SetColor(255, 0, 0, 255);

	ncTextNode text2(&rootNode, &font1);
	text2.SetScale(2.0f);
	text2.SetString(vTestString);
	iTextHeight -= text2.FontBase();
	text2.SetPosition((iWidth - text1.Width())*0.5f, iTextHeight);
	text2.EnableKerning(false);
	text2.SetColor(255, 0, 0, 128);

	ncTextNode text3(&rootNode, &font2);
	text3.SetScale(2.0f);
	text3.SetString(vTestString);
	iTextHeight -= text3.FontBase();
	text3.SetPosition((iWidth - text3.Width())*0.5f, iTextHeight);
	text3.SetColor(0, 255, 0, 255);

	ncTextNode text4(&rootNode, &font2);
	text4.SetScale(2.0f);
	text4.SetString(vTestString);
	iTextHeight -= text4.FontBase();
	text4.SetPosition((iWidth - text3.Width())*0.5f, iTextHeight);
	text4.EnableKerning(false);
	text4.SetColor(0, 255, 0, 128);

	ncTextNode text5(&rootNode, &font3);
	text5.SetScale(2.0f);
	text5.SetString(vTestString);
	iTextHeight -= text5.FontBase();
	text5.SetPosition((iWidth - text5.Width())*0.5f, iTextHeight);
	text5.SetColor(0, 0, 255, 255);

	ncTextNode text6(&rootNode, &font3);
	text6.SetScale(2.0f);
	text6.SetString(vTestString);
	iTextHeight -= text6.FontBase();
	text6.SetPosition((iWidth - text5.Width())*0.5f, iTextHeight);
	text6.EnableKerning(false);
	text6.SetColor(0, 0, 255, 128);

	t.Reset();
	unsigned long int ulUpdateTime = t.Now();


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

		// Updating string every 100ms
		if (t.Now() - ulUpdateTime > 100)
		{
			ulUpdateTime = t.Now();
			sprintf(vFPS, (const char *)"FPS: %.0f", t.AverageFPS());
			fpsText.SetString(vFPS);
			fpsText.SetPosition((iWidth - fpsText.Width()), iHeight);
		}

		gfxDevice.Clear();

		rootNode.Update(t.Interval());
		rootNode.Visit(renderQueue);
		renderQueue.Draw();

		gfxDevice.Update();
	}


// ----- Quitting ----------------------
//	SDL_Quit(); // GfxDevice should call this

	return 0;	
}
