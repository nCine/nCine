#include "ncFrameTimer.h"
#include "ncServiceLocator.h"
#include "ncFileLogger.h"
#include "ncSDLGfxDevice.h"

#include "ncALAudioDevice.h"
#include "ncAudioLoader.h"
#include "ncAudioBuffer.h"
#include "ncAudioBufferPlayer.h"
#include "ncAudioStream.h"
#include "ncAudioStreamPlayer.h"

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

	ncServiceLocator::RegisterAudioDevice(new ncALAudioDevice());
//	ncAudioBuffer audioBuffer("sounds/bomb.ogg");
//	ncAudioBufferPlayer player(&audioBuffer);
	ncAudioStreamPlayer player("sounds/bomb.ogg");
//	player.SetLooping(true);

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
				case SDLK_SPACE:
					if (player.isPaused() || player.isStopped())
						player.Play();
					else if (player.isPlaying())
						player.Pause();
					break;
				case SDLK_p:
					player.Pause();
					break;
				case SDLK_l:
					player.Play();
					break;
				case SDLK_RETURN:
					player.Stop();
					break;
				default:
					break;
				}
			}
		}


// ----- Blitting on the screen --------
		t.AddFrame();

		ncServiceLocator::GetAudioDevice().UpdateStreams();
		gfxDevice.Clear();
		gfxDevice.Update();
	}


// ----- Quitting ----------------------
	return 0;	
}
