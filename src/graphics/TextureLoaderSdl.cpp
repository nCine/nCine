#include <cstdlib> // for exit()
#include "TextureLoaderSdl.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

TextureLoaderSdl::TextureLoaderSdl(const char *filename)
	: ITextureLoader(filename), sdlSurface_(NULL)
{
	init();
}

TextureLoaderSdl::TextureLoaderSdl(IFile *fileHandle)
	: ITextureLoader(fileHandle), sdlSurface_(NULL)
{
	init();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void TextureLoaderSdl::init()
{
	LOGI_X("Loading \"%s\"", fileHandle_->filename());
	sdlSurface_ = IMG_Load(fileHandle_->filename());
	if (!sdlSurface_)
	{
		LOGF_X("Cannot load \"%s\"", fileHandle_->filename());
		exit(-1);
	}

	bpp_ = sdlSurface_->format->BitsPerPixel;
	if (bpp_ == 32)
	{
		texFormat_ = TextureFormat(GL_RGBA8);
	}
	else if (bpp_  == 24)
	{
		texFormat_ = TextureFormat(GL_RGB8);
	}
	else if (bpp_ == 8)
	{
		texFormat_ = TextureFormat(GL_ALPHA8);
	}
	else
	{
		LOGF_X("Not a true color or alpha image: %d", bpp_);
		exit(-1);
	}

	if (sdlSurface_->format->Rmask != 0x000000ff)
	{
		texFormat_.bgrFormat();
	}

	width_ = sdlSurface_->w;
	height_ = sdlSurface_->h;
	mipMapCount_ = 1; // No MIP Mapping
	pixels_ = static_cast<GLubyte*>(sdlSurface_->pixels);
}

TextureLoaderSdl::~TextureLoaderSdl()
{
	if (sdlSurface_)
	{
		SDL_FreeSurface(sdlSurface_);
		pixels_ = NULL;
	}
}

}
