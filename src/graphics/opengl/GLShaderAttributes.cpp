#include "GLShaderAttributes.h"
#include "GLShaderProgram.h"
#include <nctl/StaticHashMapIterator.h>
#include "RenderVaoPool.h"
#include "RenderResources.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLShaderAttributes::GLShaderAttributes()
    : shaderProgram_(nullptr)
{
}

GLShaderAttributes::GLShaderAttributes(GLShaderProgram *shaderProgram)
    : shaderProgram_(nullptr)
{
	setProgram(shaderProgram);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void GLShaderAttributes::setProgram(GLShaderProgram *shaderProgram)
{
	shaderProgram_ = shaderProgram;
	attributeLocations_.clear();
	vertexFormat_.reset();
	importAttributes();
}

GLVertexFormat::Attribute *GLShaderAttributes::attribute(const char *name)
{
	ASSERT(name);
	GLVertexFormat::Attribute *vertexAttribute = nullptr;

	if (shaderProgram_)
	{
		int location = -1;
		const bool attributeFound = attributeLocations_.contains(name, location);

		if (attributeFound)
			vertexAttribute = &vertexFormat_[location];
	}
	else
		LOGE_X("Cannot find attribute \"%s\", no shader program associated", name);

	return vertexAttribute;
}

void GLShaderAttributes::defineVertexFormat(const GLBufferObject *vbo, const GLBufferObject *ibo, unsigned int vboOffset)
{
	if (shaderProgram_)
	{
		if (vbo)
		{
			for (int location : attributeLocations_)
			{
				vertexFormat_[location].setVbo(vbo);
				vertexFormat_[location].setBaseOffset(vboOffset);
			}
			vertexFormat_.setIbo(ibo);

			RenderResources::vaoPool().bindVao(vertexFormat_);
		}
	}
	else
		LOGE("No shader program associated");
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void GLShaderAttributes::importAttributes()
{
	unsigned int count = shaderProgram_->attributes_.size();
	if (count > GLVertexFormat::MaxAttributes)
		LOGW_X("More active attributes (%d) than supported by the vertex format class (%d)", count, GLVertexFormat::MaxAttributes);

	for (unsigned int i = 0; i < shaderProgram_->attributes_.size(); i++)
	{
		const GLAttribute &attribute = shaderProgram_->attributes_[i];
		const int location = attribute.location();
		if (location < 0)
			continue;

		attributeLocations_[attribute.name()] = location;
		vertexFormat_[location].init(attribute.location(), attribute.numComponents(), attribute.basicType());
	}
}

}
