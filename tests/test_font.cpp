#ifdef WITH_GLEW
	#include <GL/glew.h>
#else
	#include <GL/gl.h>
	#include <GL/glext.h>
#endif
#include <SDL/SDL.h>

#include "FrameTimer.h"
#include "ServiceLocator.h"
#include "FileLogger.h"
#include "SdlGfxDevice.h"
#include "Font.h"
#include "TextNode.h"

namespace nc = ncine;

int main(int argc, char **argv)
{
	SDL_Event event;
	const int Width = 960;
	const int Height = 640;
	bool shouldQuit = false;

// ----- Init ----------------------
	nc::FrameTimer t(5.0f, 0.1f);
	nc::ServiceLocator::registerLogger(new nc::FileLogger("log.txt", nc::ILogger::LOG_VERBOSE, nc::ILogger::LOG_OFF));
	nc::SdlGfxDevice gfxDevice(Width, Height);
	gfxDevice.setWindowTitle("Test");

	nc::RenderQueue renderQueue;
	nc::SceneNode rootNode;

	nc::Font font1("fonts/trebuchet32_256.png", "fonts/trebuchet32_256.fnt");
	nc::Font font2("fonts/arial32_256.png", "fonts/arial32_256.fnt");
	nc::Font font3("fonts/verdana32_256.png", "fonts/verdana32_256.fnt");

	char fpsString[16];
	nc::TextNode fpsText(&rootNode, &font1);
	fpsText.setScale(0.85f);
	fpsText.setString("FPS: ");
	fpsText.setPosition((Width - fpsText.width()), Height);

	char testString[] = "WAY.P.ATAV";
	int textHeight = Height * 0.7f;
	nc::TextNode text1(&rootNode, &font1);
	text1.setScale(2.0f);
	text1.setString(testString);
	text1.setPosition((Width - text1.width()) * 0.5f, textHeight);
	text1.setColor(255, 0, 0, 255);

	nc::TextNode text2(&rootNode, &font1);
	text2.setScale(2.0f);
	text2.setString(testString);
	textHeight -= text2.fontBase();
	text2.setPosition((Width - text1.width()) * 0.5f, textHeight);
	text2.enableKerning(false);
	text2.setColor(255, 0, 0, 128);

	nc::TextNode text3(&rootNode, &font2);
	text3.setScale(2.0f);
	text3.setString(testString);
	textHeight -= text3.fontBase();
	text3.setPosition((Width - text3.width()) * 0.5f, textHeight);
	text3.setColor(0, 255, 0, 255);

	nc::TextNode text4(&rootNode, &font2);
	text4.setScale(2.0f);
	text4.setString(testString);
	textHeight -= text4.fontBase();
	text4.setPosition((Width - text3.width()) * 0.5f, textHeight);
	text4.enableKerning(false);
	text4.setColor(0, 255, 0, 128);

	nc::TextNode text5(&rootNode, &font3);
	text5.setScale(2.0f);
	text5.setString(testString);
	textHeight -= text5.fontBase();
	text5.setPosition((Width - text5.width()) * 0.5f, textHeight);
	text5.setColor(0, 0, 255, 255);

	nc::TextNode text6(&rootNode, &font3);
	text6.setScale(2.0f);
	text6.setString(testString);
	textHeight -= text6.fontBase();
	text6.setPosition((Width - text5.width()) * 0.5f, textHeight);
	text6.enableKerning(false);
	text6.setColor(0, 0, 255, 128);

	nc::Timer updateTimer;
	updateTimer.start();


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

		// Updating string every 100ms
		if (updateTimer.interval() > 0.1f)
		{
			updateTimer.start();
			sprintf(fpsString, static_cast<const char *>("FPS: %.0f"), t.averageFps());
			fpsText.setString(fpsString);
			fpsText.setPosition((Width - fpsText.width()), Height);
		}

		gfxDevice.clear();

		rootNode.update(t.interval());
		rootNode.visit(renderQueue);
		renderQueue.draw();

		gfxDevice.update();
	}


// ----- Quitting ----------------------
	nc::ServiceLocator::unregisterAll();
	return 0;
}
