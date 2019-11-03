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
    : isTransparent_(false), shaderProgramType_(ShaderProgramType::CUSTOM),
      shaderProgram_(nullptr), texture_(nullptr)
{
}

Material::Material(GLShaderProgram *program, GLTexture *texture)
    : isTransparent_(false), shaderProgramType_(ShaderProgramType::CUSTOM),
      shaderProgram_(program), texture_(texture)
{
	setShaderProgram(program);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void Material::setShaderProgramType(ShaderProgramType shaderProgramType)
{
	switch (shaderProgramType)
	{
		case ShaderProgramType::SPRITE:
			setShaderProgram(RenderResources::spriteShaderProgram());
			break;
		case ShaderProgramType::SPRITE_GRAY:
			setShaderProgram(RenderResources::spriteGrayShaderProgram());
			break;
		case ShaderProgramType::MESH_SPRITE:
			setShaderProgram(RenderResources::meshSpriteShaderProgram());
			break;
		case ShaderProgramType::MESH_SPRITE_GRAY:
			setShaderProgram(RenderResources::meshSpriteGrayShaderProgram());
			break;
		case ShaderProgramType::TEXTNODE_ALPHA:
			setShaderProgram(RenderResources::textnodeAlphaShaderProgram());
			break;
		case ShaderProgramType::TEXTNODE_RED:
			setShaderProgram(RenderResources::textnodeRedShaderProgram());
			break;
		case ShaderProgramType::BATCHED_SPRITES:
			setShaderProgram(RenderResources::batchedSpritesShaderProgram());
			break;
		case ShaderProgramType::BATCHED_SPRITES_GRAY:
			setShaderProgram(RenderResources::batchedSpritesGrayShaderProgram());
			break;
		case ShaderProgramType::BATCHED_MESH_SPRITES:
			setShaderProgram(RenderResources::batchedMeshSpritesShaderProgram());
			break;
		case ShaderProgramType::BATCHED_MESH_SPRITES_GRAY:
			setShaderProgram(RenderResources::batchedMeshSpritesGrayShaderProgram());
			break;
		case ShaderProgramType::BATCHED_TEXTNODES_ALPHA:
			setShaderProgram(RenderResources::batchedTextnodesAlphaShaderProgram());
			break;
		case ShaderProgramType::BATCHED_TEXTNODES_RED:
			setShaderProgram(RenderResources::batchedTextnodesRedShaderProgram());
			break;
		case ShaderProgramType::CUSTOM:
			break;
	}

	switch (shaderProgramType)
	{
		case ShaderProgramType::SPRITE:
		case ShaderProgramType::SPRITE_GRAY:
			setUniformsDataPointer(nullptr);
			uniform("uTexture")->setIntValue(0); // GL_TEXTURE0
			break;
		case ShaderProgramType::MESH_SPRITE:
		case ShaderProgramType::MESH_SPRITE_GRAY:
			setUniformsDataPointer(nullptr);
			uniform("uTexture")->setIntValue(0); // GL_TEXTURE0
			attribute("aPosition")->setVboParameters(sizeof(RenderResources::VertexFormatPos2Tex2), reinterpret_cast<void *>(offsetof(RenderResources::VertexFormatPos2Tex2, position)));
			attribute("aTexCoords")->setVboParameters(sizeof(RenderResources::VertexFormatPos2Tex2), reinterpret_cast<void *>(offsetof(RenderResources::VertexFormatPos2Tex2, texcoords)));
			break;
		case ShaderProgramType::TEXTNODE_ALPHA:
		case ShaderProgramType::TEXTNODE_RED:
			setUniformsDataPointer(nullptr);
			uniform("uTexture")->setIntValue(0); // GL_TEXTURE0
			attribute("aPosition")->setVboParameters(sizeof(RenderResources::VertexFormatPos2Tex2), reinterpret_cast<void *>(offsetof(RenderResources::VertexFormatPos2Tex2, position)));
			attribute("aTexCoords")->setVboParameters(sizeof(RenderResources::VertexFormatPos2Tex2), reinterpret_cast<void *>(offsetof(RenderResources::VertexFormatPos2Tex2, texcoords)));
			break;
		case ShaderProgramType::BATCHED_SPRITES:
		case ShaderProgramType::BATCHED_SPRITES_GRAY:
			// Uniforms data pointer not set at this time
			break;
		case ShaderProgramType::BATCHED_MESH_SPRITES:
		case ShaderProgramType::BATCHED_MESH_SPRITES_GRAY:
			attribute("aPosition")->setVboParameters(sizeof(RenderResources::VertexFormatPos2Tex2Index), reinterpret_cast<void *>(offsetof(RenderResources::VertexFormatPos2Tex2Index, position)));
			attribute("aTexCoords")->setVboParameters(sizeof(RenderResources::VertexFormatPos2Tex2Index), reinterpret_cast<void *>(offsetof(RenderResources::VertexFormatPos2Tex2Index, texcoords)));
			attribute("aMeshIndex")->setVboParameters(sizeof(RenderResources::VertexFormatPos2Tex2Index), reinterpret_cast<void *>(offsetof(RenderResources::VertexFormatPos2Tex2Index, drawindex)));
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
			break;
	}

	// Should be assigned after calling `setShaderProgram()`
	shaderProgramType_ = shaderProgramType;

	if (uniform("projection")->dataPointer() != nullptr && shaderProgramType != Material::ShaderProgramType::CUSTOM)
		uniform("projection")->setFloatVector(RenderResources::projectionMatrix().data());
}

void Material::setShaderProgram(GLShaderProgram *program)
{
	shaderProgramType_ = ShaderProgramType::CUSTOM;
	shaderProgram_ = program;
	shaderUniforms_.setProgram(shaderProgram_);
	shaderUniformBlocks_.setProgram(shaderProgram_);

	shaderAttributes_.setProgram(shaderProgram_);
}

void Material::setUniformsDataPointer(GLubyte *dataPointer)
{
	ASSERT(shaderProgram_);

	if (dataPointer == nullptr)
	{
		// Total memory size for all uniforms and uniform blocks
		const unsigned int uniformsSize = shaderProgram_->uniformsSize() + shaderProgram_->uniformBlocksSize();
		uniformsHostBuffer_ = nctl::makeUnique<GLubyte[]>(uniformsSize);
		dataPointer = uniformsHostBuffer_.get();
	}

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

unsigned int Material::sortKey()
{
	unsigned char lower = 0;
	unsigned int upper = 0;

	if (texture_)
		lower = static_cast<unsigned char>(texture_->glHandle());

	if (shaderProgram_)
		upper = shaderProgram_->glHandle() << 8;

	return upper + lower;
}

}
