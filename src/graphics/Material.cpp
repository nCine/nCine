#include <cstddef> // for offsetof()
#include "Material.h"
#include "RenderResources.h"
#include "GLShaderProgram.h"
#include "GLUniform.h"
#include "GLTexture.h"
#include "Texture.h"

namespace ncine {

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
	if (shaderProgramType_ == shaderProgramType)
		return false;

	switch (shaderProgramType)
	{
		case ShaderProgramType::SPRITE:
			setShaderProgram(RenderResources::spriteShaderProgram());
			shaderProgram_->setProgramLabel("Sprite");
			break;
		case ShaderProgramType::SPRITE_GRAY:
			setShaderProgram(RenderResources::spriteGrayShaderProgram());
			shaderProgram_->setProgramLabel("Sprite_Gray");
			break;
		case ShaderProgramType::SPRITE_NO_TEXTURE:
			setShaderProgram(RenderResources::spriteNoTextureShaderProgram());
			shaderProgram_->setProgramLabel("Sprite_NoTexture");
			break;
		case ShaderProgramType::MESH_SPRITE:
			setShaderProgram(RenderResources::meshSpriteShaderProgram());
			shaderProgram_->setProgramLabel("MeshSprite");
			break;
		case ShaderProgramType::MESH_SPRITE_GRAY:
			setShaderProgram(RenderResources::meshSpriteGrayShaderProgram());
			shaderProgram_->setProgramLabel("MeshSprite_Gray");
			break;
		case ShaderProgramType::MESH_SPRITE_NO_TEXTURE:
			setShaderProgram(RenderResources::meshSpriteNoTextureShaderProgram());
			shaderProgram_->setProgramLabel("MeshSprite_NoTexture");
			break;
		case ShaderProgramType::TEXTNODE_ALPHA:
			setShaderProgram(RenderResources::textnodeAlphaShaderProgram());
			shaderProgram_->setProgramLabel("TextNode_Alpha");
			break;
		case ShaderProgramType::TEXTNODE_RED:
			setShaderProgram(RenderResources::textnodeRedShaderProgram());
			shaderProgram_->setProgramLabel("TextNode_Red");
			break;
		case ShaderProgramType::BATCHED_SPRITES:
			setShaderProgram(RenderResources::batchedSpritesShaderProgram());
			shaderProgram_->setProgramLabel("Batched_Sprites");
			break;
		case ShaderProgramType::BATCHED_SPRITES_GRAY:
			setShaderProgram(RenderResources::batchedSpritesGrayShaderProgram());
			shaderProgram_->setProgramLabel("Batched_Sprites_Gray");
			break;
		case ShaderProgramType::BATCHED_SPRITES_NO_TEXTURE:
			setShaderProgram(RenderResources::batchedSpritesNoTextureShaderProgram());
			shaderProgram_->setProgramLabel("Batched_Sprites_NoTexture");
			break;
		case ShaderProgramType::BATCHED_MESH_SPRITES:
			setShaderProgram(RenderResources::batchedMeshSpritesShaderProgram());
			shaderProgram_->setProgramLabel("Batched_MeshSprites");
			break;
		case ShaderProgramType::BATCHED_MESH_SPRITES_GRAY:
			setShaderProgram(RenderResources::batchedMeshSpritesGrayShaderProgram());
			shaderProgram_->setProgramLabel("Batched_MeshSprites_Gray");
			break;
		case ShaderProgramType::BATCHED_MESH_SPRITES_NO_TEXTURE:
			setShaderProgram(RenderResources::batchedMeshSpritesNoTextureShaderProgram());
			shaderProgram_->setProgramLabel("Batched_MeshSprites_NoTexture");
			break;
		case ShaderProgramType::BATCHED_TEXTNODES_ALPHA:
			setShaderProgram(RenderResources::batchedTextnodesAlphaShaderProgram());
			shaderProgram_->setProgramLabel("Batched_TextNodes_Alpha");
			break;
		case ShaderProgramType::BATCHED_TEXTNODES_RED:
			setShaderProgram(RenderResources::batchedTextnodesRedShaderProgram());
			shaderProgram_->setProgramLabel("Batched_TextNodes_Red");
			break;
		case ShaderProgramType::CUSTOM:
			break;
	}

	switch (shaderProgramType)
	{
		case ShaderProgramType::BATCHED_SPRITES:
		case ShaderProgramType::BATCHED_SPRITES_GRAY:
		case ShaderProgramType::BATCHED_SPRITES_NO_TEXTURE:
			// Uniforms data pointer not set at this time
			break;
		case ShaderProgramType::BATCHED_MESH_SPRITES:
		case ShaderProgramType::BATCHED_MESH_SPRITES_GRAY:
			attribute("aPosition")->setVboParameters(sizeof(RenderResources::VertexFormatPos2Tex2Index), reinterpret_cast<void *>(offsetof(RenderResources::VertexFormatPos2Tex2Index, position)));
			attribute("aTexCoords")->setVboParameters(sizeof(RenderResources::VertexFormatPos2Tex2Index), reinterpret_cast<void *>(offsetof(RenderResources::VertexFormatPos2Tex2Index, texcoords)));
			attribute("aMeshIndex")->setVboParameters(sizeof(RenderResources::VertexFormatPos2Tex2Index), reinterpret_cast<void *>(offsetof(RenderResources::VertexFormatPos2Tex2Index, drawindex)));
			// Uniforms data pointer not set at this time
			break;
		case ShaderProgramType::BATCHED_MESH_SPRITES_NO_TEXTURE:
			attribute("aPosition")->setVboParameters(sizeof(RenderResources::VertexFormatPos2Index), reinterpret_cast<void *>(offsetof(RenderResources::VertexFormatPos2Index, position)));
			attribute("aMeshIndex")->setVboParameters(sizeof(RenderResources::VertexFormatPos2Index), reinterpret_cast<void *>(offsetof(RenderResources::VertexFormatPos2Index, drawindex)));
			// Uniforms data pointer not set at this time
			break;
		case ShaderProgramType::BATCHED_TEXTNODES_ALPHA:
		case ShaderProgramType::BATCHED_TEXTNODES_RED:
			attribute("aPosition")->setVboParameters(sizeof(RenderResources::VertexFormatPos2Tex2Index), reinterpret_cast<void *>(offsetof(RenderResources::VertexFormatPos2Tex2Index, position)));
			attribute("aTexCoords")->setVboParameters(sizeof(RenderResources::VertexFormatPos2Tex2Index), reinterpret_cast<void *>(offsetof(RenderResources::VertexFormatPos2Tex2Index, texcoords)));
			attribute("aMeshIndex")->setVboParameters(sizeof(RenderResources::VertexFormatPos2Tex2Index), reinterpret_cast<void *>(offsetof(RenderResources::VertexFormatPos2Tex2Index, drawindex)));
			// Uniforms data pointer not set at this time
			break;
		case ShaderProgramType::CUSTOM:
		default:
			break;
	}

	// Should be assigned after calling `setShaderProgram()`
	shaderProgramType_ = shaderProgramType;
	return true;
}

bool Material::setShaderProgram(GLShaderProgram *program)
{
	if (shaderProgram_ == program)
		return false;

	shaderProgramType_ = ShaderProgramType::CUSTOM;
	shaderProgram_ = program;
	// The camera uniforms are handled separately as they have a different update frequency
	shaderUniforms_.setProgram(shaderProgram_, nullptr, RenderResources::ProjectionViewMatrixExcludeString);
	shaderUniformBlocks_.setProgram(shaderProgram_);
	shaderAttributes_.setProgram(shaderProgram_);

	return true;
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
	shaderAttributes_.defineVertexFormat(vbo, ibo, vboOffset);
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
