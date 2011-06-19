#ifndef NO_GLEW
	#include <GL/glew.h>
#else
	#include <GL/gl.h>
	#include <GL/glext.h>
#endif
#include <SDL/SDL.h>

#include "base/Rect.h"
#include "FrameTimer.h"
#include "ServiceLocator.h"
#include "FileLogger.h"
#include "graphics/GfxDevice.h"
#include "graphics/Texture.h"

int main(int argc, char **argv)
{
	SDL_Event event;
	int iWidth = 640;
	int iHeight = 480;
	bool bQuit = 0;

// ----- Init ----------------------
	FrameTimer t(5, 0);
	ServiceLocator::RegisterLogger(new FileLogger("log.txt", 5, -1));
	GfxDevice gfxDevice(iWidth, iHeight);
	SDL_WM_SetCaption("Test", NULL);

	Texture tex("texture.png");
	tex.Bind();

	t.Reset();


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

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-1.0f,1.0f,0.0);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.0f,-1.0f,0.0);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(1.0f,-1.0f,0.0);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(1.0f,1.0f,0.0);
		glEnd();

		gfxDevice.Update();
	}


// ----- Quitting ----------------------
//	SDL_Quit(); // GfxDevice should call this
	return 0;	
}
