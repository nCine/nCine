#include <cstdlib> // for exit()
#include "ncTextureFormat.h"
#include "ncServiceLocator.h"


///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncTextureFormat::ncTextureFormat(GLenum internalFormat)
	: internalFormat_(internalFormat), format_(-1),
	  type_(-1), isCompressed_(false)
{
	findExternalFmt();
}

ncTextureFormat::ncTextureFormat(GLenum internalFormat, GLenum type)
	: internalFormat_(internalFormat), format_(-1),
	  type_(-1), isCompressed_(false)
{
	findExternalFmt();
	// Overriding the type found by FindExternalFmt()
	type_ = type;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

#ifndef __ANDROID__
/// Converts the external format to the corresponding BGR one
void ncTextureFormat::bgrFormat()
{
	if (format_ == GL_RGBA)
	{
		format_ = GL_BGRA;
	}
	else if (format_ == GL_RGB)
	{
		format_ = GL_BGR;
	}
}
#endif

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Tries to find an external format corresponding to the internal one
void ncTextureFormat::findExternalFmt()
{
	bool found = false;

#ifndef __ANDROID__
	if (found == false)
	{
		found = integerFormat();
	}
	if (found == false)
	{
		found = floatFormat();
	}
	if (found == false)
	{
		found = compressedFormat();
	}
#else
	if (found == false)
	{
		found = oesFormat();
	}
	if (found == false)
	{
		found = oesCompressedFormat();
	}
#endif

	if (found == false)
	{
		LOGF_X("Unknown internal format: %d", internalFormat_);
		exit(EXIT_FAILURE);
	}
}

#ifndef __ANDROID__
/// Searches a match between an integer internal format and an external one
bool ncTextureFormat::integerFormat()
{
	bool found = true;

	switch (internalFormat_)
	{
		case GL_RGBA:
		case GL_RGBA8:
		case 4:
			format_ = GL_RGBA;
			break;
		case GL_RGB:
		case GL_RGB8:
		case 3:
			format_ = GL_RGB;
			break;
		case GL_LUMINANCE_ALPHA:
		case GL_LUMINANCE8_ALPHA8:
		case 2:
			format_ = GL_LUMINANCE_ALPHA;
			break;
		case GL_ALPHA:
		case GL_ALPHA8:
		case 1:
			format_ = GL_ALPHA;
			break;
		case GL_LUMINANCE:
		case GL_LUMINANCE8:
			format_ = GL_LUMINANCE;
			break;
		case GL_DEPTH_COMPONENT:
		case GL_DEPTH_COMPONENT16_ARB:
		case GL_DEPTH_COMPONENT24_ARB:
		case GL_DEPTH_COMPONENT32_ARB:
			format_ = GL_DEPTH_COMPONENT;
			break;
		default:
			found = false;
			break;
	}

	if (found)
	{
		type_ = GL_UNSIGNED_BYTE;
	}

	return found;
}

/// Searches a match between a floating point internal format and an external one
bool ncTextureFormat::floatFormat()
{
	bool found = true;

	switch (internalFormat_)
	{
		case GL_RGBA16F_ARB:
		case GL_RGBA32F_ARB:
			format_ = GL_RGBA;
			break;
		case GL_RGB16F_ARB:
		case GL_RGB32F_ARB:
			format_ = GL_RGB;
			break;
		default:
			found = false;
			break;
	}

	if (found)
	{
		type_ = GL_FLOAT;
	}

	return found;
}

/// Searches a match between a compressed internal format and an external one
bool ncTextureFormat::compressedFormat()
{
	bool found = true;

	switch (internalFormat_)
	{
		case GL_COMPRESSED_RGBA:
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			format_ = GL_RGBA;
			break;
		case GL_COMPRESSED_RGB:
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
			format_ = GL_RGB;
			break;
		default:
			found = false;
			break;
	}

	if (found)
	{
		type_ = GL_UNSIGNED_BYTE;
		isCompressed_ = true;
	}

	return found;
}
#else
/// Searches a match between an OpenGL ES internal format and an external one
bool ncTextureFormat::oesFormat()
{
	bool found = true;

	switch (internalFormat_)
	{
		case GL_RGBA:
			format_ = GL_RGBA;
			break;
		case GL_RGB:
			format_ = GL_RGB;
			break;
		case GL_LUMINANCE_ALPHA:
			format_ = GL_LUMINANCE_ALPHA;
			break;
		case GL_LUMINANCE:
			format_ = GL_LUMINANCE;
			break;
		case GL_ALPHA:
			format_ = GL_ALPHA;
			break;
		default:
			found = false;
			break;
	}

	if (found)
	{
		type_ = GL_UNSIGNED_BYTE;
	}

	return found;
}

/// Searches a match between a OpenGL ES compressed internal format and an external one
bool ncTextureFormat::oesCompressedFormat()
{
	bool found = true;

	switch (internalFormat_)
	{
		case GL_ATC_RGBA_EXPLICIT_ALPHA_AMD:
		case GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
			format_ = GL_RGBA;
			break;
		case GL_ETC1_RGB8_OES:
		case GL_ATC_RGB_AMD:
			format_ = GL_RGB;
			break;
		default:
			found = false;
			break;
	}

	if (found)
	{
		type_ = GL_UNSIGNED_BYTE;
		isCompressed_ = true;
	}

	return found;
}
#endif

/// Calculates the pixel data size for each MIP map level
long int ncTextureFormat::calculateMipSizes(GLenum internalFormat, int width, int height, int mipMapCount, long int *mipDataOffsets, long int *mipDataSizes)
{
	unsigned int blockWidth = 1; // Compression block width in pixels
	unsigned int blockHeight = 1; // Compression block height in pixels
	unsigned int bpp = 1; // Bits per pixel
	unsigned int minDataSize = 1; // Minimum data size in bytes

	switch (internalFormat)
	{
		case GL_RGBA:
			bpp = 32;
			break;
		case GL_RGB:
			bpp = 24;
			break;
		case GL_LUMINANCE_ALPHA:
		case GL_UNSIGNED_SHORT_5_6_5:
		case GL_UNSIGNED_SHORT_5_5_5_1:
		case GL_UNSIGNED_SHORT_4_4_4_4:
			bpp = 16;
			break;
		case GL_LUMINANCE:
		case GL_ALPHA:
			bpp = 8;
			break;
#ifndef __ANDROID__
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
			// max(1, width / 4) x max(1, height / 4) x 8(DXT1)
			blockWidth = 4;
			blockHeight = 4;
			bpp = 4;
			minDataSize = 8;
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			// max(1, width / 4) x max(1, height / 4) x 16(DXT2-5)
			blockWidth = 4;
			blockHeight = 4;
			bpp = 8;
			minDataSize = 16;
			break;
#else
		case GL_ETC1_RGB8_OES:
			blockWidth = 4;
			blockHeight = 4;
			bpp = 4;
			minDataSize = 8;
			break;

		case GL_ATC_RGBA_EXPLICIT_ALPHA_AMD:
		case GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
			// ((width_in_texels+3)/4) * ((height_in_texels+3)/4) * 16
			blockWidth = 4;
			blockHeight = 4;
			bpp = 8;
			minDataSize = 16;
			break;
		case GL_ATC_RGB_AMD:
			// ((width_in_texels+3)/4) * ((height_in_texels+3)/4) * 8
			blockWidth = 4;
			blockHeight = 4;
			bpp = 4;
			minDataSize = 8;
			break;

		case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
		case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG:
			blockWidth = 8;
			blockHeight = 4;
			bpp = 2;
			minDataSize = 2 * 2 * ((blockWidth * blockHeight * bpp) / 8);
			break;
		case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
		case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
			blockWidth = 4;
			blockHeight = 4;
			bpp = 4;
			minDataSize = 2 * 2 * ((blockWidth * blockHeight * bpp) / 8);
			break;
#endif
		default:
			LOGF_X("MIP maps not supported for internal format: %d", internalFormat);
			exit(EXIT_FAILURE);
			break;
	}

	int levelWidth = width;
	int levelHeight = height;
	long int dataSizesSum = 0;
	for (int i = 0; i < mipMapCount; i++)
	{
		mipDataOffsets[i] = dataSizesSum;
		mipDataSizes[i] = (levelWidth / blockWidth) * (levelHeight / blockHeight) * ((blockWidth * blockHeight  * bpp) / 8);
		// Clamping to the minimum valid size
		if (mipDataSizes[i] < int(minDataSize))
		{
			mipDataSizes[i] = minDataSize;
		}

		levelWidth /= 2;
		levelHeight /= 2;
		dataSizesSum += mipDataSizes[i];
	}

	return dataSizesSum;
}
