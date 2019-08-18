#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"
#include "common_macros.h"
#include "Texture.h"
#include "ITextureLoader.h"
#include "GLTexture.h"
#include "RenderStatistics.h"
#include "tracy.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Texture::Texture(const char *filename)
    : Texture(filename, 0, 0)
{
}

Texture::Texture(const char *filename, int width, int height)
    : Object(ObjectType::TEXTURE, filename), glTexture_(nctl::makeUnique<GLTexture>(GL_TEXTURE_2D)),
      width_(0), height_(0), mipMapLevels_(1), isCompressed_(false), numChannels_(0), dataSize_(0),
      minFiltering_(Filtering::NEAREST), magFiltering_(Filtering::NEAREST), wrapMode_(Wrap::CLAMP_TO_EDGE)
{
	ZoneScoped;
	ZoneText(filename, strnlen(filename, nctl::String::MaxCStringLength));
	glTexture_->bind();
	setGLTextureLabel(filename);

	nctl::UniquePtr<ITextureLoader> texLoader = ITextureLoader::createFromFile(filename);
	load(*texLoader.get(), width, height);

	RenderStatistics::addTexture(dataSize_);
}

Texture::Texture(const char *filename, Vector2i size)
    : Texture(filename, size.x, size.y)
{
}

Texture::~Texture()
{
	RenderStatistics::removeTexture(dataSize_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void Texture::setMinFiltering(Filtering filter)
{
	GLenum glFilter = GL_NEAREST;
	// clang-format off
	switch (filter)
	{
		case Filtering::NEAREST:				glFilter = GL_NEAREST; break;
		case Filtering::LINEAR:					glFilter = GL_LINEAR; break;
		case Filtering::NEAREST_MIPMAP_NEAREST:	glFilter = GL_NEAREST_MIPMAP_NEAREST; break;
		case Filtering::LINEAR_MIPMAP_NEAREST:	glFilter = GL_LINEAR_MIPMAP_NEAREST; break;
		case Filtering::NEAREST_MIPMAP_LINEAR:	glFilter = GL_NEAREST_MIPMAP_LINEAR; break;
		case Filtering::LINEAR_MIPMAP_LINEAR:	glFilter = GL_LINEAR_MIPMAP_LINEAR; break;
		default:								glFilter = GL_NEAREST; break;
	}
	// clang-format on

	glTexture_->bind();
	glTexture_->texParameteri(GL_TEXTURE_MIN_FILTER, glFilter);
	minFiltering_ = filter;
}

void Texture::setMagFiltering(Filtering filter)
{
	GLenum glFilter = GL_NEAREST;
	// clang-format off
	switch (filter)
	{
		case Filtering::NEAREST:				glFilter = GL_NEAREST; break;
		case Filtering::LINEAR:					glFilter = GL_LINEAR; break;
		default:								glFilter = GL_NEAREST; break;
	}
	// clang-format on

	glTexture_->bind();
	glTexture_->texParameteri(GL_TEXTURE_MAG_FILTER, glFilter);
	magFiltering_ = filter;
}

void Texture::setWrap(Wrap wrapMode)
{
	GLenum glWrap = GL_CLAMP_TO_EDGE;
	// clang-format off
	switch (wrapMode)
	{
		case Wrap::CLAMP_TO_EDGE:				glWrap = GL_CLAMP_TO_EDGE; break;
		case Wrap::MIRRORED_REPEAT:				glWrap = GL_MIRRORED_REPEAT; break;
		case Wrap::REPEAT:						glWrap = GL_REPEAT; break;
		default:								glWrap = GL_CLAMP_TO_EDGE; break;
	}
	// clang-format on

	glTexture_->bind();
	glTexture_->texParameteri(GL_TEXTURE_WRAP_S, glWrap);
	glTexture_->texParameteri(GL_TEXTURE_WRAP_T, glWrap);
	wrapMode_ = wrapMode;
}

void *Texture::imguiTexId()
{
	return reinterpret_cast<void *>(glTexture_.get());
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void Texture::load(const ITextureLoader &texLoader, int width, int height)
{
	// Loading a texture without overriding the size detected by the loader
	if (width == 0 || height == 0)
	{
		width = texLoader.width();
		height = texLoader.height();
	}

	const IGfxCapabilities &gfxCaps = theServiceLocator().gfxCapabilities();
	const int maxTextureSize = gfxCaps.value(IGfxCapabilities::GLIntValues::MAX_TEXTURE_SIZE);
	FATAL_ASSERT_MSG_X(width <= maxTextureSize, "Texture width %d is bigger than device maximum %d", width, maxTextureSize);
	FATAL_ASSERT_MSG_X(height <= maxTextureSize, "Texture height %d is bigger than device maximum %d", height, maxTextureSize);

	glTexture_->texParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexture_->texParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	wrapMode_ = Wrap::CLAMP_TO_EDGE;

	if (texLoader.mipMapCount() > 1)
	{
		glTexture_->texParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexture_->texParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		magFiltering_ = Filtering::LINEAR;
		minFiltering_ = Filtering::LINEAR_MIPMAP_LINEAR;
		// To prevent artifacts if the MIP map chain is not complete
		glTexture_->texParameteri(GL_TEXTURE_MAX_LEVEL, texLoader.mipMapCount());
	}
	else
	{
		glTexture_->texParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexture_->texParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		magFiltering_ = Filtering::LINEAR;
		minFiltering_ = Filtering::LINEAR;
	}

	const TextureFormat &texFormat = texLoader.texFormat();

	int levelWidth = width;
	int levelHeight = height;

#if (defined(__ANDROID__) && GL_ES_VERSION_3_0) || defined(__EMSCRIPTEN__)
	const bool withTexStorage = true;
#else
	const bool withTexStorage = gfxCaps.hasExtension(IGfxCapabilities::GLExtensions::ARB_TEXTURE_STORAGE);
#endif

	if (withTexStorage)
		glTexture_->texStorage2D(texLoader.mipMapCount(), texFormat.internalFormat(), width, height);
	for (int i = 0; i < texLoader.mipMapCount(); i++)
	{
		if (texFormat.isCompressed())
		{
			if (withTexStorage)
				glTexture_->compressedTexSubImage2D(i, 0, 0, levelWidth, levelHeight, texFormat.internalFormat(), texLoader.dataSize(i), texLoader.pixels(i));
			else
				glTexture_->compressedTexImage2D(i, texFormat.internalFormat(), levelWidth, levelHeight, texLoader.dataSize(i), texLoader.pixels(i));
		}
		else
		{
			if (withTexStorage)
				glTexture_->texSubImage2D(i, 0, 0, levelWidth, levelHeight, texFormat.format(), texFormat.type(), texLoader.pixels(i));
			else
				glTexture_->texImage2D(i, texFormat.internalFormat(), levelWidth, levelHeight, texFormat.format(), texFormat.type(), texLoader.pixels(i));
		}

		levelWidth /= 2;
		levelHeight /= 2;
	}

	width_ = width;
	height_ = height;
	mipMapLevels_ = texLoader.mipMapCount();
	isCompressed_ = texFormat.isCompressed();
	numChannels_ = texFormat.numChannels();
	dataSize_ = texLoader.dataSize();
}

void Texture::setGLTextureLabel(const char *filename)
{
	glTexture_->setObjectLabel(filename);
}

}
