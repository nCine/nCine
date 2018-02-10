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
	: isTransparent_(false), shaderProgram_(nullptr), texture_(nullptr)
{

}

Material::Material(GLShaderProgram *program, GLTexture *texture)
	: isTransparent_(false), shaderProgram_(program), texture_(texture)
{
	shaderUniforms_.setProgram(shaderProgram_);
	shaderAttributes_.setProgram(shaderProgram_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void Material::setShaderProgram(ShaderProgram shaderProgram)
{
	switch (shaderProgram)
	{
		case ShaderProgram::SPRITE:
			setShaderProgram(const_cast<GLShaderProgram *>(RenderResources::spriteShaderProgram()));
			uniform("texture")->setIntValue(0); // GL_TEXTURE0
			attribute("aPosition")->setVboParameters(sizeof(RenderResources::VertexFormatPos2Tex2), reinterpret_cast<void *>(offsetof(RenderResources::VertexFormatPos2Tex2, position)));
			attribute("aTexCoords")->setVboParameters(sizeof(RenderResources::VertexFormatPos2Tex2), reinterpret_cast<void *>(offsetof(RenderResources::VertexFormatPos2Tex2, texcoords)));
			break;
		case ShaderProgram::TEXTNODE_GRAY:
			setShaderProgram(const_cast<GLShaderProgram *>(RenderResources::textnodeGrayShaderProgram()));
			uniform("texture")->setIntValue(0); // GL_TEXTURE0
			attribute("aPosition")->setVboParameters(sizeof(RenderResources::VertexFormatPos2Tex2), reinterpret_cast<void *>(offsetof(RenderResources::VertexFormatPos2Tex2, position)));
			attribute("aTexCoords")->setVboParameters(sizeof(RenderResources::VertexFormatPos2Tex2), reinterpret_cast<void *>(offsetof(RenderResources::VertexFormatPos2Tex2, texcoords)));
			break;
		case ShaderProgram::TEXTNODE_COLOR:
			setShaderProgram(const_cast<GLShaderProgram *>(RenderResources::textnodeColorShaderProgram()));
			uniform("texture")->setIntValue(0); // GL_TEXTURE0
			attribute("aPosition")->setVboParameters(sizeof(RenderResources::VertexFormatPos2Tex2), reinterpret_cast<void *>(offsetof(RenderResources::VertexFormatPos2Tex2, position)));
			attribute("aTexCoords")->setVboParameters(sizeof(RenderResources::VertexFormatPos2Tex2), reinterpret_cast<void *>(offsetof(RenderResources::VertexFormatPos2Tex2, texcoords)));
			break;
		case ShaderProgram::COLOR:
			setShaderProgram(const_cast<GLShaderProgram *>(RenderResources::colorShaderProgram()));
			attribute("aPosition")->setVboParameters(sizeof(RenderResources::VertexFormatPos2), reinterpret_cast<void *>(offsetof(RenderResources::VertexFormatPos2, position)));
			break;
	}
}

void Material::setShaderProgram(GLShaderProgram *program)
{
	shaderProgram_ = program;
	shaderUniforms_.setProgram(shaderProgram_);
	shaderAttributes_.setProgram(shaderProgram_);
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
		shaderProgram_->use();
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
