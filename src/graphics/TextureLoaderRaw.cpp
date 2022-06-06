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

	unsigned int numPixels = width * height;
	const unsigned int bytesPerPixel = texFormat_.numChannels();
	dataSize_ = numPixels * bytesPerPixel;
	for (int i = 0; i < mipMapCount_; i++)
	{
		dataSize_ += numPixels * bytesPerPixel;
		numPixels /= 2;
	}

	hasLoaded_ = true;
}

}
