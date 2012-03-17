#include "ncFrameTimer.h"
#include "ncServiceLocator.h"
#include "ncFileLogger.h"
#include "ncSDLGfxDevice.h"

#include "ncALAudioDevice.h"
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

	const float fDefaultGain = 1.0f;
	float fGain = fDefaultGain;
	const float fDefaultPitch = 1.0f;
	float fPitch = fDefaultPitch;
	const float fDefaultXPos = 0.0f;
	float fXPos = 0.0f;
	bool bLooping = true;

// ----- Init ----------------------
	ncFrameTimer t(5, 0);
	ncServiceLocator::RegisterLogger(new ncFileLogger("log.txt", ncILogger::LOG_VERBOSE, ncILogger::LOG_OFF));
	ncSDLGfxDevice gfxDevice(iWidth, iHeight);
	SDL_WM_SetCaption("Test", NULL);

	ncServiceLocator::RegisterAudioDevice(new ncALAudioDevice());
//	ncAudioBuffer audioBuffer("sounds/bomb.wav");
//	ncAudioBufferPlayer player(&audioBuffer);
	ncAudioStreamPlayer player("sounds/bomb.ogg");
	player.Play();

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
				case SDLK_a:
					player.Play();
					break;
				case SDLK_s:
					player.Stop();
					break;
				case SDLK_d:
					player.Pause();
					break;
				case SDLK_l:
					bLooping = !bLooping;
					break;
				case SDLK_KP0:
					fGain = fDefaultGain;
					fPitch = fDefaultPitch;
					fXPos = fDefaultXPos;
					break;
				case SDLK_KP7:
					fGain -= 0.1f;
					if (fGain < 0.0f)
						fGain = 0.0f;
					break;
				case SDLK_KP8:
					fGain = fDefaultGain;
					break;
				case SDLK_KP9:
					fGain += 0.1f;
					if (fGain > 1.0f)
						fGain = 1.0f;
					break;
				case SDLK_KP4:
					fPitch -= 0.1f;
					break;
				case SDLK_KP5:
					fPitch = fDefaultPitch;
					break;
				case SDLK_KP6:
					fPitch += 0.1f;
					break;
				case SDLK_KP1:
					fXPos -= 0.1f;
					break;
				case SDLK_KP2:
					fXPos = fDefaultXPos;
					break;
				case SDLK_KP3:
					fXPos += 0.1f;
					break;
				default:
					break;
				}
			}
		}


// ----- Blitting on the screen --------
		t.AddFrame();

		player.SetGain(fGain);
		player.SetPitch(fPitch);
		player.SetPosition(fXPos, 0.0f, 0.0f);
		player.SetLooping(bLooping);

		ncServiceLocator::AudioDevice().UpdatePlayers();
		gfxDevice.Clear();
		gfxDevice.Update();
	}


// ----- Quitting ----------------------
	return 0;	
}
