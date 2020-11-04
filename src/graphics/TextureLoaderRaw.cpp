#include "TextureLoaderRaw.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

TextureLoaderRaw::TextureLoaderRaw(int width, int height, int mipMapCount, GLenum internalFormat)
    : ITextureLoader()
{
	width_ = width;
	height_ = height;
	mipMapCount_ = mipMapCount;
	texFormat_ = TextureFormat(internalFormat);
	bpp_ = texFormat_.numChannels();

	unsigned int numPixels = width * height;
	for (int i = 0; i < mipMapCount_; i++)
	{
		dataSize_ += numPixels * bpp_;
		numPixels /= 2;
	}

	hasLoaded_ = true;
}

}
