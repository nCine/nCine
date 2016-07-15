#ifndef CLASS_NCTEXTURELOADERSDL
#define CLASS_NCTEXTURELOADERSDL

#include <SDL/SDL.h>
#if defined(__APPLE__)
	#include <SDL_image/SDL_image.h>
#else
	#include <SDL/SDL_image.h>
#endif
#include "ITextureLoader.h"

namespace ncine {

/// SDL_image texture loader
class TextureLoaderSdl : public ITextureLoader
{
  public:
	explicit TextureLoaderSdl(const char *filename);
	explicit TextureLoaderSdl(IFile *fileHandle);
	~TextureLoaderSdl();

  private:
	SDL_Surface *sdlSurface_;

	/// Private copy constructor
	TextureLoaderSdl(const TextureLoaderSdl &);
	/// Private assignment operator
	TextureLoaderSdl &operator=(const TextureLoaderSdl &);

	void init();
};

}

#endif
