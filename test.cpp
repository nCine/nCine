#ifndef NO_GLEW
	#include <GL/glew.h>
#else
	#include <GL/gl.h>
	#include <GL/glext.h>
#endif
#include <SDL/SDL.h>

#include <cmath>

#include "base/Rect.h"
#include "FrameTimer.h"
#include "ServiceLocator.h"
#include "FileLogger.h"
#include "graphics/GfxDevice.h"
#include "graphics/SpriteBatch.h"
#include "graphics/Texture.h"

int main(int argc, char **argv)
{
	SDL_Event event;
	int iWidth = 640;
	int iHeight = 480;
	bool bQuit = 0;

// ----- Init ----------------------
	float fAngle = 0.0;
	FrameTimer t(5, 0);
	ServiceLocator::RegisterLogger(new FileLogger("log.txt", 5, -1));
	GfxDevice gfxDevice(iWidth, iHeight);
	SDL_WM_SetCaption("Test", NULL);

	Texture tex1("texture1.png");
	Texture tex2("texture2.png");
	Texture tex3("texture3.png");
	Texture tex4("texture4.png");
	SpriteBatch sb;

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
		fAngle += 0.1f * t.GetInterval();

		gfxDevice.Clear();

/*
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.0f, 1.0f, 0.0);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-1.0f,-1.0f, 0.0);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f( 1.0f,-1.0f, 0.0);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f( 1.0f, 1.0f, 0.0);
		glEnd();
*/

		float fSinus = sinf(fAngle * 0.01f);
		float fCosine = cosf(fAngle * 0.01f);

		sb.Begin();
		sb.Draw(&tex2, Rect(200, 65, 50*fSinus, 50*fCosine));
		sb.Draw(&tex4, Rect(100, 500, 60, 70*fSinus));
		sb.Draw(&tex3, Point(iWidth*0.5f + fSinus*100, iHeight*0.5f + fCosine*100));
		sb.Draw(&tex1, Rect(500, 500, 100, 100), Rect(50, 10, 40, 40*fSinus));

		sb.Draw(&tex2, Rect(220, 350, 60, 60), Rect(200, 65, 50*fSinus, 50*fCosine));
		sb.Draw(&tex4, Rect(100+fCosine*75, 500+fSinus*25, 60, 70), Rect(50, 50, 10, 10));
		sb.Draw(&tex3, Point(iWidth*0.25f + fSinus*10, iHeight*0.25f + fCosine*10));
		sb.Draw(&tex1, Rect(550+fSinus*20.0f, 75+fCosine*15.0f, 100, 100), Rect(50+fCosine+3.0f, 10+fSinus*10.0f, 40, 40*fSinus));

		sb.End();

		gfxDevice.Update();
	}


// ----- Quitting ----------------------
//	SDL_Quit(); // GfxDevice should call this
	return 0;	
}
