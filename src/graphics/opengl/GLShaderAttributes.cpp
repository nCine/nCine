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

		for (nctl::StringHashMap<GLVertexAttribute>::Iterator i = vertexAttributes_.begin(); i != vertexAttributes_.end(); ++i)
		{
			GLVertexAttribute &attribute = *i;
			int location = attribute.shaderAttribute()->location();
			if (definedPointers_[location] != attribute || definedPointers_[location].boundVbo() != boundVboHandle)
			{
				(*i).vertexAttribPointer();
				definedPointers_[location] = attribute;
				definedPointers_[location].setBoundVbo(boundVboHandle);
			}
			if (definedPointers_[location].isEnabled() == false)
			{
				(*i).enable();
				definedPointers_[location].setEnabled(true);
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

	for (unsigned int i = 0; i < count; i++)
	{
		const GLAttribute &attribute = shaderProgram_->attributes_[i];
		GLVertexAttribute vertexAttribute(&attribute);

		vertexAttributes_[attribute.name()] = vertexAttribute;
	}
}

GLShaderAttributes::GLVertexAttribPointerState::GLVertexAttribPointerState()
	: enabled_(false), size_(-1), type_(GL_FLOAT), vboStride_(0), vboPointer_(nullptr)
{

}

bool GLShaderAttributes::GLVertexAttribPointerState::operator==(const GLVertexAttribute &attribute) const
{
	if (attribute.shaderAttribute()->numComponents() == size_ &&
	    attribute.shaderAttribute()->basicType() == type_ &&
	    attribute.vboStride() == vboStride_ && attribute.vboPointer() == vboPointer_)
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

GLShaderAttributes::GLVertexAttribPointerState &GLShaderAttributes::GLVertexAttribPointerState::operator=(const GLVertexAttribute &attribute)
{
	size_ = attribute.shaderAttribute()->numComponents();
	type_ = attribute.shaderAttribute()->basicType();
	vboStride_ = attribute.vboStride();
	vboPointer_ = attribute.vboPointer();

	return *this;
}

}
