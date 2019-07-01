#include "common_macros.h"
#include "GLVertexFormat.h"
#include "GLBufferObject.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLVertexFormat::GLVertexFormat()
    : attributes_(nctl::StaticArrayMode::EXTEND_SIZE), ibo_(nullptr)
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
			attributes_[i].baseOffset_ = other.attributes_[i].baseOffset_;
		}
	}
	ibo_ = other.ibo_;
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
	         other.normalized_ == normalized_ &&
	         other.stride_ == stride_ &&
	         other.pointer_ == pointer_ &&
	         other.baseOffset_ == baseOffset_));
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
	normalized_ = GL_FALSE;
	stride_ = 0;
	pointer_ = nullptr;
	baseOffset_ = 0;
}

void GLVertexFormat::define()
{
	for (unsigned int i = 0; i < MaxAttributes; i++)
	{
		if (attributes_[i].enabled_)
		{
			attributes_[i].vbo_->bind();
			glEnableVertexAttribArray(attributes_[i].index_);

#if (defined(__ANDROID__) && !GL_ES_VERSION_3_2) || defined(__EMSCRIPTEN__)
			const GLubyte *initialPointer = reinterpret_cast<const GLubyte *>(attributes_[i].pointer_);
			const GLvoid *pointer = reinterpret_cast<const GLvoid *>(initialPointer + attributes_[i].baseOffset_);
#else
			const GLvoid *pointer = attributes_[i].pointer_;
#endif

			switch (attributes_[i].type_)
			{
				case GL_BYTE:
				case GL_UNSIGNED_BYTE:
				case GL_SHORT:
				case GL_UNSIGNED_SHORT:
				case GL_INT:
				case GL_UNSIGNED_INT:
					if (attributes_[i].normalized_)
						glVertexAttribPointer(attributes_[i].index_, attributes_[i].size_, attributes_[i].type_, GL_TRUE, attributes_[i].stride_, pointer);
					else
						glVertexAttribIPointer(attributes_[i].index_, attributes_[i].size_, attributes_[i].type_, attributes_[i].stride_, pointer);
					break;
				default:
					glVertexAttribPointer(attributes_[i].index_, attributes_[i].size_, attributes_[i].type_, attributes_[i].normalized_, attributes_[i].stride_, pointer);
					break;
			}
		}
	}

	if (ibo_)
		ibo_->bind();
}

void GLVertexFormat::reset()
{
	for (unsigned int i = 0; i < MaxAttributes; i++)
		attributes_[i].enabled_ = false;
	ibo_ = nullptr;
}

bool GLVertexFormat::operator==(const GLVertexFormat &other) const
{
	bool areEqual = (other.ibo_ == ibo_);

	// If indices are the same then check attributes too
	if (areEqual)
	{
		for (unsigned int i = 0; i < MaxAttributes; i++)
		{
			if (other.attributes_[i] != attributes_[i])
			{
				areEqual = false;
				break;
			}
		}
	}

	return areEqual;
}

bool GLVertexFormat::operator!=(const GLVertexFormat &other) const
{
	return !operator==(other);
}

}
