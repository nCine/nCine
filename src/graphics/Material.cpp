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

uint32_t Material::sortKey()
{
	static const uint32_t Seed = 1697381921;

	uint32_t hash = Seed;
	for (unsigned int i = 0; i < GLTexture::MaxTextureUnits; i++)
	{
		const GLuint textureHandle = (textures_[i] != nullptr) ? textures_[i]->glHandle() : 0;
		hash = nctl::fasthash32(&textureHandle, sizeof(textureHandle), hash);
	}

	const GLuint shaderProgramHandle = shaderProgram_->glHandle();
	hash = nctl::fasthash32(&shaderProgramHandle, sizeof(shaderProgramHandle), hash);

	hash = nctl::fasthash32(&srcBlendingFactor_, sizeof(srcBlendingFactor_), hash);
	hash = nctl::fasthash32(&destBlendingFactor_, sizeof(destBlendingFactor_), hash);

	return hash;
}

}
