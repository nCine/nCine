#include "common_macros.h"
#include "GLShader.h"
#include "GLDebug.h"
#include "IFile.h"
#include <nctl/StaticString.h>

#if defined(__EMSCRIPTEN__) || defined(WITH_ANGLE)
	#include "Application.h"
#endif

namespace ncine {

namespace {

	static nctl::StaticString<256> patchLines;

}

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

char GLShader::infoLogString_[MaxInfoLogLength];

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLShader::GLShader(GLenum type)
    : glHandle_(0), status_(Status::NOT_COMPILED)
{
	if (patchLines.isEmpty())
	{
#if (defined(WITH_OPENGLES) && GL_ES_VERSION_3_0) || defined(__EMSCRIPTEN__)
		patchLines.append("#version 300 es\n");
#else
		patchLines.append("#version 330\n");
#endif

#if defined(__EMSCRIPTEN__)
		patchLines.append("#define __EMSCRIPTEN__\n");
#elif defined(__ANDROID__)
		patchLines.append("#define __ANDROID__\n");
#elif defined(WITH_ANGLE)
		patchLines.append("#define WITH_ANGLE\n");
#endif

#if defined(__EMSCRIPTEN__) || defined(WITH_ANGLE)
		// ANGLE does not seem capable of handling large arrays that are not entirely filled.
		// A small array size will also make shader compilation a lot faster.
		if (theApplication().appConfiguration().fixedBatchSize > 0)
		{
			patchLines.append("#define WITH_FIXED_BATCH_SIZE\n");
			patchLines.formatAppend("#define BATCH_SIZE (%u)\n", theApplication().appConfiguration().fixedBatchSize);
		}
#endif
		// Exclude patch lines when counting line numbers in info logs
		patchLines.append("#line 0\n");
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
	fileHandle->setExitOnFailToOpen(false);

	fileHandle->open(IFile::OpenMode::READ);
	if (fileHandle->isOpened())
	{
		const GLint length = static_cast<int>(fileHandle->size());
		nctl::String source(length);
		fileHandle->read(source.data(), length);

		const GLchar *source_lines[2] = { patchLines.data(), source.data() };
		const GLint lengths[2] = { static_cast<GLint>(patchLines.length()), length };
		glShaderSource(glHandle_, 2, source_lines, lengths);

		setObjectLabel(filename);
	}
}

bool GLShader::compile(ErrorChecking errorChecking, bool logOnErrors)
{
	glCompileShader(glHandle_);

	if (errorChecking == ErrorChecking::IMMEDIATE)
		return checkCompilation(logOnErrors);
	else
	{
		status_ = Status::COMPILED_WITH_DEFERRED_CHECKS;
		return true;
	}
}

bool GLShader::checkCompilation(bool logOnErrors)
{
	if (status_ == Status::COMPILED)
		return true;

	GLint status = 0;
	glGetShaderiv(glHandle_, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		if (logOnErrors)
		{
			GLint length = 0;
			glGetShaderiv(glHandle_, GL_INFO_LOG_LENGTH, &length);

			if (length > 0)
			{
				glGetShaderInfoLog(glHandle_, MaxInfoLogLength, &length, infoLogString_);
				LOGW_X("%s", infoLogString_);
			}
		}

		status_ = Status::COMPILATION_FAILED;
		return false;
	}

	status_ = Status::COMPILED;
	return true;
}

void GLShader::setObjectLabel(const char *label)
{
	GLDebug::objectLabel(GLDebug::LabelTypes::SHADER, glHandle_, label);
}

}
