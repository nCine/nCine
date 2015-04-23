#include "GLShader.h"
#include "ServiceLocator.h"
#include "IFile.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLShader::GLShader(GLenum type)
	: glHandle_(0)
{
	glHandle_ = glCreateShader(type);
}

GLShader::GLShader(GLenum type, const char *filename)
	: glHandle_(0)
{
	glHandle_ = glCreateShader(type);
	loadFromFile(filename);
}

GLShader::~GLShader()
{
	glDeleteShader(glHandle_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////


void GLShader::loadFromFile(const char *filename)
{
	IFile *fileHandle = IFile::createFileHandle(filename);

	fileHandle->open(IFile::MODE_READ);
	if (fileHandle->isOpened())
	{
		GLint length = static_cast<int>(fileHandle->size());
		String source(length);
		fileHandle->read(source.data(), length);
		const GLchar* source_lines[1] = { source.data() };
		glShaderSource(glHandle_, 1, source_lines, &length);
	}
}

bool GLShader::compile()
{
	glCompileShader(glHandle_);

	GLint status = 0;
	glGetShaderiv(glHandle_, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length = 0;
		glGetShaderiv(glHandle_, GL_INFO_LOG_LENGTH, &length);

		if (length > 0)
		{
			String infoLog(length);
			glGetShaderInfoLog(glHandle_, length, &length, infoLog.data());
			LOGW_X("%s", infoLog.data());
		}

		return false;
	}

	return true;
}

}
