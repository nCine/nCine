#include <cstdlib> // for exit()
#include "ncTexture.h"
#include "ncITextureLoader.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncTexture::ncTexture()
	: gLId_(0), width_(0), height_(0),
	  mipMapLevels_(1), isCompressed_(false), hasAlphaChannel_(false)
{
	type_ = TEXTURE_TYPE;
	glGenTextures(1, &gLId_);
}


ncTexture::ncTexture(const char *filename)
	: gLId_(0), width_(0), height_(0),
	  mipMapLevels_(1), isCompressed_(false), hasAlphaChannel_(false)
{
	type_ = TEXTURE_TYPE;
	setName(filename);

	glGenTextures(1, &gLId_);
	bind();

	ncITextureLoader *pTexLoader = ncITextureLoader::createFromFile(filename);
	load(*pTexLoader);
	delete pTexLoader;
}

ncTexture::ncTexture(const char *filename, int width, int height)
	: gLId_(0), width_(0), height_(0), isCompressed_(false), hasAlphaChannel_(false)
{
	type_ = TEXTURE_TYPE;
	setName(filename);

	glGenTextures(1, &gLId_);
	bind();

	ncITextureLoader *pTexLoader = ncITextureLoader::createFromFile(filename);
	load(*pTexLoader, width, height);
	delete pTexLoader;
}

ncTexture::ncTexture(const char *filename, ncPoint size)
	: gLId_(0), width_(0), height_(0), isCompressed_(false), hasAlphaChannel_(false)
{
	type_ = TEXTURE_TYPE;
	setName(filename);

	glGenTextures(1, &gLId_);
	bind();

	ncITextureLoader *pTexLoader = ncITextureLoader::createFromFile(filename);
	load(*pTexLoader, size.x, size.y);
	delete pTexLoader;
}

ncTexture::~ncTexture()
{
	glDeleteTextures(1, &gLId_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Sets texture filtering for both magnification and minification
void ncTexture::setFiltering(GLenum filter)
{
	bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Loads a texture based on information from the texture format and loader
void ncTexture::load(const ncITextureLoader& texLoader)
{
	load(texLoader, texLoader.width(), texLoader.height());
}

/// Loads a texture overriding the size detected by the texture loader
void ncTexture::load(const ncITextureLoader& texLoader, int width, int height)
{
	const ncGfxCapabilities& gfxCaps = ncServiceLocator::gfxCapabilities();
	if (width > gfxCaps.maxTextureSize() || height > gfxCaps.maxTextureSize())
	{
		LOGF("Texture size is bigger than device maximum");
		exit(EXIT_FAILURE);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if (texLoader.mipMapCount() > 1)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
#ifndef __ANDROID__
		// To prevent artifacts if the MIP map chain is not complete
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, texLoader.mipMapCount());
#endif
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	const ncTextureFormat &texFormat = texLoader.texFormat();

	int levelWidth = width;
	int levelHeight = height;
	for (int i = 0; i < texLoader.mipMapCount(); i++)
	{
		if (texFormat.isCompressed())
		{
			glCompressedTexImage2D(GL_TEXTURE_2D, i, texFormat.internalFormat(), levelWidth, levelHeight, 0,
				texLoader.dataSize(i), texLoader.pixels(i));
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, i, texFormat.internalFormat(), levelWidth, levelHeight, 0,
				texFormat.format(), texFormat.type(), texLoader.pixels(i));
		}

		levelWidth /= 2;
		levelHeight /= 2;
	}

	width_ = width;
	height_ = height;
	mipMapLevels_ = texLoader.mipMapCount();
	isCompressed_ = texFormat.isCompressed();
	hasAlphaChannel_ = texFormat.hasAlpha();
}
