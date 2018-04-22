#include "common_macros.h"
#include "GLVertexFormat.h"
#include "GLBufferObject.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLVertexFormat::GLVertexFormat()
	: attributes_(nctl::StaticArrayMode::EXTEND_SIZE)
{

}

GLVertexFormat::GLVertexFormat(const GLVertexFormat &other)
	: GLVertexFormat()
{
	for (unsigned int i = 0; i < MaxAttributes; i++)
	{
		attributes_[i].enabled_ = other.attributes_[i].enabled_;
		if (attributes_[i].enabled_)
		{
			attributes_[i].vbo_ = other.attributes_[i].vbo_;
			attributes_[i].index_ = other.attributes_[i].index_;
			attributes_[i].size_ = other.attributes_[i].size_;
			attributes_[i].type_ = other.attributes_[i].type_;
			attributes_[i].stride_ = other.attributes_[i].stride_;
			attributes_[i].pointer_ = other.attributes_[i].pointer_;
		}
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool GLVertexFormat::Attribute::operator==(const Attribute &other) const
{
	return ((other.enabled_ == false && enabled_ == false) ||
	        ((other.enabled_ == true && enabled_ == true) &&
	         (other.vbo_ && vbo_ && other.vbo_->glHandle() == vbo_->glHandle()) &&
	          other.index_ == index_ &&
	          other.size_ == size_ &&
	          other.type_ == type_ &&
	          other.stride_ == stride_ &&
	          other.pointer_ == pointer_));
}

bool GLVertexFormat::Attribute::operator!=(const Attribute &other) const
{
	return !operator==(other);
}

void GLVertexFormat::Attribute::init(unsigned int index, GLint size, GLenum type)
{
	enabled_ = true;
	vbo_ = nullptr;
	index_ = index;
	size_ = size;
	type_ = type;
	stride_ = 0;
	pointer_ = nullptr;
}

void GLVertexFormat::define()
{
	for (unsigned int i = 0; i < MaxAttributes; i++)
	{
		if (attributes_[i].enabled_)
		{
			attributes_[i].vbo_->bind();
			glEnableVertexAttribArray(attributes_[i].index_);
			glVertexAttribPointer(attributes_[i].index_, attributes_[i].size_, attributes_[i].type_, GL_FALSE, attributes_[i].stride_, attributes_[i].pointer_);
		}
	}
}

void GLVertexFormat::reset()
{
	for (unsigned int i = 0; i < MaxAttributes; i++)
		attributes_[i].enabled_ = false;
}

bool GLVertexFormat::operator==(const GLVertexFormat &other) const
{
	bool areEqual = true;
	for (unsigned int i = 0; i < MaxAttributes; i++)
	{
		if (other.attributes_[i] != attributes_[i])
		{
			areEqual = false;
			break;
		}
	}

	return areEqual;
}

bool GLVertexFormat::operator!=(const GLVertexFormat &other) const
{
	return !operator==(other);
}

}
