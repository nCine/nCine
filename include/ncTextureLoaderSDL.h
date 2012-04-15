#ifndef CLASS_NCTEXTURELOADERSDL
#define CLASS_NCTEXTURELOADERSDL

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "ncITextureLoader.h"

/// SDL_image texture loader
class ncTextureLoaderSDL : public ncITextureLoader
{
private:
	SDL_Surface *m_pSDLSurface;

	void Init();

public:
	ncTextureLoaderSDL(const char *pFilename);
	ncTextureLoaderSDL(ncIFile *pFileHandle);
	~ncTextureLoaderSDL();
};

#endif
