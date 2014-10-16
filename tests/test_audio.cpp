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
	const int Width = 960;
	const int Height = 640;
	bool shouldQuit = false;

	const float DefaultGain = 1.0f;
	float gain = DefaultGain;
	const float DefaultPitch = 1.0f;
	float pitch = DefaultPitch;
	const float DefaultXPos = 0.0f;
	float xPos = 0.0f;
	bool isLooping = true;

// ----- Init ----------------------
	ncFrameTimer t(5.0f, 0.0f);
	ncServiceLocator::registerLogger(new ncFileLogger("log.txt", ncILogger::LOG_VERBOSE, ncILogger::LOG_OFF));
	ncSDLGfxDevice gfxDevice(Width, Height);
	gfxDevice.setWindowTitle("Test");

	ncServiceLocator::registerAudioDevice(new ncALAudioDevice());
//	ncAudioBuffer audioBuffer("sounds/bomb.wav");
//	ncAudioBufferPlayer player(&audioBuffer);
	ncAudioStreamPlayer player("sounds/bomb.ogg");
	player.play();


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
						case SDLK_SPACE:
							if (player.isPaused() || player.isStopped())
							{
								player.play();
							}
							else if (player.isPlaying())
							{
								player.pause();
							}
							break;
						case SDLK_a:
							player.play();
							break;
						case SDLK_s:
							player.stop();
							break;
						case SDLK_d:
							player.pause();
							break;
						case SDLK_l:
							isLooping = !isLooping;
							break;
						case SDLK_KP0:
							gain = DefaultGain;
							pitch = DefaultPitch;
							xPos = DefaultXPos;
							break;
						case SDLK_KP7:
							gain -= 0.1f;
							if (gain < 0.0f)
							{
								gain = 0.0f;
							}
							break;
						case SDLK_KP8:
							gain = DefaultGain;
							break;
						case SDLK_KP9:
							gain += 0.1f;
							if (gain > 1.0f)
							{
								gain = 1.0f;
							}
							break;
						case SDLK_KP4:
							pitch -= 0.1f;
							break;
						case SDLK_KP5:
							pitch = DefaultPitch;
							break;
						case SDLK_KP6:
							pitch += 0.1f;
							break;
						case SDLK_KP1:
							xPos -= 0.1f;
							break;
						case SDLK_KP2:
							xPos = DefaultXPos;
							break;
						case SDLK_KP3:
							xPos += 0.1f;
							break;
						default:
							break;
					}
					break;
			}
		}


// ----- Blitting on the screen --------
		t.addFrame();

		player.setGain(gain);
		player.setPitch(pitch);
		player.setPosition(xPos, 0.0f, 0.0f);
		player.setLooping(isLooping);

		ncServiceLocator::audioDevice().updatePlayers();
		gfxDevice.clear();
		gfxDevice.update();
	}


// ----- Quitting ----------------------
	// Unregistering all services to close the audio device
	ncServiceLocator::unregisterAll();
	return 0;
}
