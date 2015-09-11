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

/// Empty constructor
Material::Material()
	: isTransparent_(false), shaderProgram_(NULL), texture_(NULL)
{

}

Material::Material(GLShaderProgram* program, GLTexture* texture)
	: isTransparent_(false), shaderProgram_(program), texture_(texture)
{
	shaderUniforms_.setProgram(shaderProgram_);
	shaderAttributes_.setProgram(shaderProgram_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void Material::setShaderProgram(ShaderProgramPresets preset)
{
	switch(preset)
	{
		case SPRITE_PROGRAM:
			setShaderProgram(const_cast<GLShaderProgram *>(RenderResources::spriteShaderProgram()));
			attribute("aPosition")->setVboParameters(sizeof(RenderResources::VertexFormatPos2Tex2), (void*)offsetof(RenderResources::VertexFormatPos2Tex2, position));
			attribute("aTexCoords")->setVboParameters(sizeof(RenderResources::VertexFormatPos2Tex2), (void*)offsetof(RenderResources::VertexFormatPos2Tex2, texcoords));
			break;
		case TEXTNODE_PROGRAM:
			setShaderProgram(const_cast<GLShaderProgram *>(RenderResources::textnodeShaderProgram()));
			attribute("aPosition")->setVboParameters(sizeof(RenderResources::VertexFormatPos2Tex2), (void*)offsetof(RenderResources::VertexFormatPos2Tex2, position));
			attribute("aTexCoords")->setVboParameters(sizeof(RenderResources::VertexFormatPos2Tex2), (void*)offsetof(RenderResources::VertexFormatPos2Tex2, texcoords));
			break;
		case COLOR_PROGRAM:
			setShaderProgram(const_cast<GLShaderProgram *>(RenderResources::colorShaderProgram()));
			attribute("aPosition")->setVboParameters(sizeof(RenderResources::VertexFormatPos2), (void*)offsetof(RenderResources::VertexFormatPos2, position));
			break;
	}
}

void Material::setShaderProgram(GLShaderProgram* program)
{
	shaderProgram_ = program;
	shaderUniforms_.setProgram(shaderProgram_);
	shaderAttributes_.setProgram(shaderProgram_);
}

void Material::setTexture(const Texture& texture)
{
	texture_ = texture.glTexture_;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void Material::bind()
{
	if (texture_)
	{
		texture_->bind();
	}

	if (shaderProgram_)
	{
		shaderProgram_->use();
	}
}

unsigned int Material::sortKey()
{
	unsigned char lower = 0;
	unsigned int upper = 0;

	if (texture_)
	{
		lower = texture_->glHandle();
	}

	if (shaderProgram_)
	{
		upper = shaderProgram_->glHandle() << 8;
	}

	return upper + lower;
}

}
