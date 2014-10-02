#ifndef CLASS_NCTEXTURELOADERSDL
#define CLASS_NCTEXTURELOADERSDL

#include <SDL/SDL.h>
#if defined(__APPLE__)
	#include <SDL_image/SDL_image.h>
#else
	#include <SDL/SDL_image.h>
#endif
#include "ncITextureLoader.h"

/// SDL_image texture loader
class ncTextureLoaderSDL : public ncITextureLoader
{
  public:
	ncTextureLoaderSDL(const char *pFilename);
	ncTextureLoaderSDL(ncIFile *pFileHandle);
	~ncTextureLoaderSDL();

  private:
	SDL_Surface *m_pSDLSurface;

	void Init();
};

#endif
