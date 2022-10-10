#include <cstddef> // for offsetof()
#include "Material.h"
#include "RenderResources.h"
#include "GLShaderProgram.h"
#include "GLUniform.h"
#include "GLTexture.h"
#include "Texture.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

const char *Material::InstanceBlockName = "InstanceBlock";
const char *Material::InstancesBlockName = "InstancesBlock";
const char *Material::ModelMatrixUniformName = "modelMatrix";

const char *Material::GuiProjectionMatrixUniformName = "uGuiProjection";
const char *Material::DepthUniformName = "uDepth";
const char *Material::ProjectionMatrixUniformName = "uProjectionMatrix";
const char *Material::ViewMatrixUniformName = "uViewMatrix";
const char *Material::ProjectionViewMatrixExcludeString = "uProjectionMatrix\0uViewMatrix\0";

const char *Material::TextureUniformName = "uTexture";
const char *Material::ColorUniformName = "color";
const char *Material::SpriteSizeUniformName = "spriteSize";
const char *Material::TexRectUniformName = "texRect";
const char *Material::PositionAttributeName = "aPosition";
const char *Material::TexCoordsAttributeName = "aTexCoords";
const char *Material::MeshIndexAttributeName = "aMeshIndex";
const char *Material::ColorAttributeName = "aColor";

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Material::Material()
    : Material(nullptr, nullptr)
{
}

Material::Material(GLShaderProgram *program, GLTexture *texture)
    : isBlendingEnabled_(false), srcBlendingFactor_(GL_SRC_ALPHA), destBlendingFactor_(GL_ONE_MINUS_SRC_ALPHA),
      shaderProgramType_(ShaderProgramType::CUSTOM), shaderProgram_(program), uniformsHostBufferSize_(0)
{
	for (unsigned int i = 0; i < GLTexture::MaxTextureUnits; i++)
		textures_[i] = nullptr;
	textures_[0] = texture;

	if (program)
		setShaderProgram(program);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void Material::setBlendingFactors(GLenum srcBlendingFactor, GLenum destBlendingFactor)
{
	srcBlendingFactor_ = srcBlendingFactor;
	destBlendingFactor_ = destBlendingFactor;
}

bool Material::setShaderProgramType(ShaderProgramType shaderProgramType)
{
	GLShaderProgram *shaderProgram = RenderResources::shaderProgram(shaderProgramType);
	if (shaderProgram == nullptr || shaderProgram == shaderProgram_)
		return false;

	setShaderProgram(shaderProgram);

	// Should be assigned after calling `setShaderProgram()`
	shaderProgramType_ = shaderProgramType;
	return true;
}

void Material::setShaderProgram(GLShaderProgram *program)
{
	// Allow self-assignment to take into account the case where the shader program loads new shaders

	shaderProgramType_ = ShaderProgramType::CUSTOM;
	shaderProgram_ = program;
	// The camera uniforms are handled separately as they have a different update frequency
	shaderUniforms_.setProgram(shaderProgram_, nullptr, ProjectionViewMatrixExcludeString);
	shaderUniformBlocks_.setProgram(shaderProgram_);

	RenderResources::setDefaultAttributesParameters(*shaderProgram_);
}

void Material::setDefaultAttributesParameters()
{
	RenderResources::setDefaultAttributesParameters(*shaderProgram_);
}

void Material::reserveUniformsDataMemory()
{
	ASSERT(shaderProgram_);

	// Total memory size for all uniforms and uniform blocks
	const unsigned int uniformsSize = shaderProgram_->uniformsSize() + shaderProgram_->uniformBlocksSize();
	if (uniformsSize > uniformsHostBufferSize_)
	{
		uniformsHostBuffer_ = nctl::makeUnique<GLubyte[]>(uniformsSize);
		uniformsHostBufferSize_ = uniformsSize;
	}
	GLubyte *dataPointer = uniformsHostBuffer_.get();
	shaderUniforms_.setUniformsDataPointer(dataPointer);
	shaderUniformBlocks_.setUniformsDataPointer(&dataPointer[shaderProgram_->uniformsSize()]);
}

void Material::setUniformsDataPointer(GLubyte *dataPointer)
{
	ASSERT(shaderProgram_);
	ASSERT(dataPointer);

	uniformsHostBuffer_.reset(nullptr);
	uniformsHostBufferSize_ = 0;
	shaderUniforms_.setUniformsDataPointer(dataPointer);
	shaderUniformBlocks_.setUniformsDataPointer(&dataPointer[shaderProgram_->uniformsSize()]);
}

const GLTexture *Material::texture(unsigned int unit) const
{
	const GLTexture *texture = nullptr;
	if (unit < GLTexture::MaxTextureUnits)
		texture = textures_[unit];
	return texture;
}

bool Material::setTexture(unsigned int unit, const GLTexture *texture)
{
	bool result = false;
	if (unit < GLTexture::MaxTextureUnits)
	{
		textures_[unit] = texture;
		result = true;
	}
	return result;
}

bool Material::setTexture(unsigned int unit, const Texture &texture)
{
	return setTexture(unit, texture.glTexture_.get());
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void Material::bind()
{
	for (unsigned int i = 0; i < GLTexture::MaxTextureUnits; i++)
	{
		if (textures_[i] != nullptr)
			textures_[i]->bind(i);
		else
			GLTexture::unbind(i);
	}

	if (shaderProgram_)
	{
		shaderProgram_->use();
		shaderUniformBlocks_.bind();
	}
}

void Material::defineVertexFormat(const GLBufferObject *vbo, const GLBufferObject *ibo, unsigned int vboOffset)
{
	shaderProgram_->defineVertexFormat(vbo, ibo, vboOffset);
}

namespace {

	uint8_t glBlendingFactorToInt(GLenum blendingFactor)
	{
		switch (blendingFactor)
		{
			case GL_ZERO: return 0;
			case GL_ONE: return 1;
			case GL_SRC_COLOR: return 2;
			case GL_ONE_MINUS_SRC_COLOR: return 3;
			case GL_DST_COLOR: return 4;
			case GL_ONE_MINUS_DST_COLOR: return 5;
			case GL_SRC_ALPHA: return 6;
			case GL_ONE_MINUS_SRC_ALPHA: return 7;
			case GL_DST_ALPHA: return 8;
			case GL_ONE_MINUS_DST_ALPHA: return 9;
			case GL_CONSTANT_COLOR: return 10;
			case GL_ONE_MINUS_CONSTANT_COLOR: return 11;
			case GL_CONSTANT_ALPHA: return 12;
			case GL_ONE_MINUS_CONSTANT_ALPHA: return 13;
			case GL_SRC_ALPHA_SATURATE: return 14;
		}
		return 0;
	}

	struct SortHashData
	{
		GLuint textures[GLTexture::MaxTextureUnits];
		GLuint shaderProgram;
		uint8_t srcBlendingFactor;
		uint8_t destBlendingFactor;
	};

}

uint32_t Material::sortKey()
{
	static const uint32_t Seed = 1697381921;
	// Align to 64 bits for `fasthash64()` to properly work on Emscripten without alignment faults
	static SortHashData hashData alignas(8);

	for (unsigned int i = 0; i < GLTexture::MaxTextureUnits; i++)
		hashData.textures[i] = (textures_[i] != nullptr) ? textures_[i]->glHandle() : 0;
	hashData.shaderProgram = shaderProgram_->glHandle();
	hashData.srcBlendingFactor = glBlendingFactorToInt(srcBlendingFactor_);
	hashData.destBlendingFactor = glBlendingFactorToInt(destBlendingFactor_);

	return nctl::fasthash32(reinterpret_cast<const void *>(&hashData), sizeof(SortHashData), Seed);
}

}
