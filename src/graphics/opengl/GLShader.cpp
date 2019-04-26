#include "common_macros.h"
#include "GLShader.h"
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

void GLShader::loadFromString(const char *string)
{
	ASSERT(string);

#if defined(__ANDROID__) && GL_ES_VERSION_3_0
	const char *versionLine = "#version 300 es\n";
#else
	const char *versionLine = "#version 330\n";
#endif
	const GLchar *source_lines[2] = { versionLine, string };
	glShaderSource(glHandle_, 2, source_lines, nullptr);
}

void GLShader::loadFromFile(const char *filename)
{
	nctl::UniquePtr<IFile> fileHandle = IFile::createFileHandle(filename);

	fileHandle->open(IFile::OpenMode::READ);
	if (fileHandle->isOpened())
	{
		const GLint length = static_cast<int>(fileHandle->size());
		nctl::String source(length);
		fileHandle->read(source.data(), length);

#if defined(__ANDROID__) && GL_ES_VERSION_3_0
		const char *versionLine = "#version 300 es\n";
		const GLint lengths[2] = { 16, length };
#else
		const char *versionLine = "#version 330\n";
		const GLint lengths[2] = { 13, length };
#endif
		const GLchar *source_lines[2] = { versionLine, source.data() };
		glShaderSource(glHandle_, 2, source_lines, lengths);
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
			nctl::String infoLog(length);
			glGetShaderInfoLog(glHandle_, length, &length, infoLog.data());
			LOGW_X("%s", infoLog.data());
		}

		return false;
	}

	return true;
}

}
