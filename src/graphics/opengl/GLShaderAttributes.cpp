#include "GLShaderAttributes.h"
#include "GLShaderProgram.h"
#include "GLBufferObject.h"
#include "nctl/HashMapIterator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

nctl::StaticArray<GLShaderAttributes::GLVertexAttribPointerState, GLShaderAttributes::MaxDefinedVertexAttribPointers> GLShaderAttributes::definedPointers_(nctl::StaticArrayMode::EXTEND_SIZE);
GLVertexAttribute GLShaderAttributes::attributeNotFound_;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLShaderAttributes::GLShaderAttributes()
	: shaderProgram_(nullptr), vertexAttributes_(VertexAttributesHashSize)
{
}

GLShaderAttributes::GLShaderAttributes(GLShaderProgram *shaderProgram)
	: shaderProgram_(nullptr), vertexAttributes_(VertexAttributesHashSize)
{
	setProgram(shaderProgram);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void GLShaderAttributes::setProgram(GLShaderProgram *shaderProgram)
{
	shaderProgram_ = shaderProgram;
	vertexAttributes_.clear();
	importAttributes();
}

GLVertexAttribute *GLShaderAttributes::attribute(const char *name)
{
	ASSERT(name);
	GLVertexAttribute *vertexAttribute = nullptr;

	if (shaderProgram_)
	{
		vertexAttribute = vertexAttributes_.find(name);

		if (vertexAttribute == nullptr)
		{
			// Returning the dummy vertex attribute to prevent the application from crashing
			vertexAttribute = &attributeNotFound_;
			LOGW_X("Attribute \"%s\" not found in shader program %u", name, shaderProgram_->glHandle());
		}
	}
	else
		LOGE_X("Cannot find attribute \"%s\", no shader program associated", name);

	return vertexAttribute;
}

void GLShaderAttributes::defineVertexPointers(const GLBufferObject *vbo)
{
	if (shaderProgram_)
	{
		GLuint boundVboHandle = 0;
		if (vbo)
		{
			vbo->bind(); // VBO has to be bound before setting attribute pointers
			boundVboHandle = vbo->glHandle();
		}

		for (GLVertexAttribute &attribute : vertexAttributes_)
		{
			const unsigned int location = static_cast<unsigned int>(attribute.shaderAttribute()->location());
			GLVertexAttribPointerState &pointerState = definedPointers_[location];

			if (pointerState != attribute || pointerState.boundVbo != boundVboHandle)
			{
				attribute.vertexAttribPointer();
				pointerState.boundVbo = boundVboHandle;

				pointerState.size = attribute.shaderAttribute()->numComponents();
				pointerState.type = attribute.shaderAttribute()->basicType();
				pointerState.vboStride = attribute.vboStride();
				pointerState.vboPointer = attribute.vboPointer();
			}
			if (pointerState.enabled == false)
			{
				attribute.enable();
				pointerState.enabled = true;
			}
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
	if (count > VertexAttributesHashSize)
		LOGW_X("More active attributes (%d) than hashmap buckets (%d)", count, VertexAttributesHashSize);

	for (const GLAttribute &attribute : shaderProgram_->attributes_)
	{
		GLVertexAttribute vertexAttribute(&attribute);
		vertexAttributes_[attribute.name()] = vertexAttribute;
	}
}

bool GLShaderAttributes::GLVertexAttribPointerState::operator==(const GLVertexAttribute &attribute) const
{
	if (attribute.shaderAttribute()->numComponents() == size &&
	    attribute.shaderAttribute()->basicType() == type &&
	    attribute.vboStride() == vboStride && attribute.vboPointer() == vboPointer)
	{
		return true;
	}
	else
		return false;
}

bool GLShaderAttributes::GLVertexAttribPointerState::operator!=(const GLVertexAttribute &attribute) const
{
	return !operator==(attribute);
}

}
