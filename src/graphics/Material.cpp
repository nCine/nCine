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
      shaderProgramType_(ShaderProgramType::CUSTOM), shaderProgram_(program), texture_(texture), uniformsHostBufferSize_(0)
{
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

void Material::setTexture(const Texture &texture)
{
	texture_ = texture.glTexture_.get();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void Material::bind()
{
	if (texture_)
		texture_->bind();

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

}
uint32_t Material::sortKey()
{
	uint16_t lower = 0;
	uint32_t middle = 0;
	uint32_t upper = 0;

	if (texture_)
		lower = static_cast<uint16_t>(texture_->glHandle());

	if (shaderProgram_)
		middle = shaderProgram_->glHandle() << 16;

	if (isBlendingEnabled_)
	{
		upper = glBlendingFactorToInt(srcBlendingFactor_) * 16 + glBlendingFactorToInt(destBlendingFactor_);
		upper = upper << 24;
	}

	return upper + middle + lower;
}

}
