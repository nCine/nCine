#include "common_macros.h"
#include "GLShader.h"
#include "IFile.h"
#include <nctl/String.h>

namespace ncine {

static nctl::String patchLines(64);

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLShader::GLShader(GLenum type)
    : glHandle_(0), status_(Status::NOT_COMPILED)
{
	if (patchLines.isEmpty())
	{
#if (defined(__ANDROID__) && GL_ES_VERSION_3_0) || defined(__EMSCRIPTEN__)
		patchLines.append("#version 300 es\n");
#else
		patchLines.append("#version 330\n");
#endif

#if defined(__EMSCRIPTEN__)
		// Adding the definition on Emscripten even if batching is disabled.
		// It will shrink arrays and make compilation on ANGLE a lot faster.
		patchLines.append("#define OUT_OF_BOUNDS_ACCESS\n");
#endif
	}

	glHandle_ = glCreateShader(type);
}

GLShader::GLShader(GLenum type, const char *filename)
    : GLShader(type)
{
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

	const GLchar *source_lines[2] = { patchLines.data(), string };
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

		const GLchar *source_lines[2] = { patchLines.data(), source.data() };
		const GLint lengths[2] = { static_cast<GLint>(patchLines.length()), length };
		glShaderSource(glHandle_, 2, source_lines, lengths);
	}
}

void GLShader::compile(ErrorChecking errorChecking)
{
	glCompileShader(glHandle_);

	if (errorChecking == ErrorChecking::IMMEDIATE)
		checkCompilation();
	else
		status_ = Status::COMPILED_WITH_DEFERRED_CHECKS;
}

bool GLShader::checkCompilation()
{
	if (status_ == Status::COMPILED)
		return true;

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

		status_ = Status::COMPILATION_FAILED;
		return false;
	}

	status_ = Status::COMPILED;
	return true;
}

}
