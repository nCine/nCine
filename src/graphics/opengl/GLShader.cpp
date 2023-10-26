#include "common_macros.h"
#include "GLShader.h"
#include "GLDebug.h"
#include "IFile.h"
#include <nctl/StaticString.h>
#include <nctl/Array.h>
#include "RenderResources.h"
#include "BinaryShaderCache.h"
#include "Hash64.h"

#if defined(__EMSCRIPTEN__) || defined(WITH_ANGLE)
	#include "Application.h"
#endif

namespace ncine {

namespace {

	nctl::StaticString<256> patchLines;
	nctl::Array<const char *> sourceStrings(4);
	nctl::Array<GLint> sourceLengths(4);
	nctl::String fileSource(1024);

	const char *typeToString(GLenum type)
	{
		switch (type)
		{
			case GL_VERTEX_SHADER: return "Vertex";
			case GL_FRAGMENT_SHADER: return "Fragment";
			default: return "Unknown";
		}
	}

}

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

char GLShader::infoLogString_[MaxInfoLogLength];

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLShader::GLShader(GLenum type)
    : type_(type), glHandle_(0), sourceHash_(0), status_(Status::NOT_COMPILED)
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
			patchLines.formatAppend("#define BATCH_SIZE (%u)\n", theApplication().appConfiguration().fixedBatchSize);
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

bool GLShader::loadFromString(const char *string)
{
	const char *strings[2] = { string, nullptr };
	return loadFromStringsAndFile(strings, nullptr);
}

bool GLShader::loadFromStringAndFile(const char *string, const char *filename)
{
	const char *strings[2] = { string, nullptr };
	return loadFromStringsAndFile(strings, filename);
}

bool GLShader::loadFromStrings(const char **strings)
{
	return loadFromStringsAndFile(strings, nullptr);
}

bool GLShader::loadFromStringsAndFile(const char **strings, const char *filename)
{
	return loadFromStringsAndFile(strings, filename, 0);
}

/*! \note If the provided hash is zero, it will be calculated from the given sources and file */
bool GLShader::loadFromStringsAndFile(const char **strings, const char *filename, uint64_t sourceHash)
{
	const bool noStrings = (strings == nullptr || strings[0] == nullptr);
	if (noStrings && filename == nullptr)
		return false;

	unsigned int numStrings = 0;
	if (strings != nullptr)
		for (const char **nextString = strings; *nextString != nullptr; numStrings++, nextString++);

	sourceStrings.clear();
	sourceLengths.clear();
	if (sourceStrings.capacity() < numStrings + 2)
	{
		sourceStrings.setCapacity(numStrings + 2);
		sourceLengths.setCapacity(numStrings + 2);
	}

	// Patch lines are normally taken into account for hashing, but calculation is skipped if `sourceHash` is different than zero.
	// In this case, changing the `fixedBatchSize` value will not trigger an automatic recompilation of the binary shader.
	GLsizei count = 1;
	sourceStrings.pushBack(patchLines.data());
	sourceLengths.pushBack(static_cast<GLint>(patchLines.length()));

	sourceHash_ = sourceHash;
	if (numStrings > 0)
	{
		for (unsigned int i = 0; i < numStrings; i++)
		{
			ASSERT(strings[i] != nullptr);
			sourceStrings.pushBack(strings[i]);
			const unsigned long sourceLength = nctl::strnlen(strings[i], MaxShaderSourceLength);
			sourceLengths.pushBack(sourceLength);
			count++;
		}
		if (sourceHash == 0 && RenderResources::binaryShaderCache().isEnabled())
			sourceHash_ += RenderResources::hash64().hashStrings(count, sourceStrings.data(), sourceLengths.data());
	}

	if (filename)
	{
		nctl::UniquePtr<IFile> fileHandle = IFile::createFileHandle(filename);

		fileHandle->open(IFile::OpenMode::READ);
		if (fileHandle->isOpened())
		{
			const unsigned int fileLength = fileHandle->size();
			if (fileLength + 1 > fileSource.capacity())
				fileSource.setCapacity(fileLength + 1);

			fileHandle->read(fileSource.data(), fileLength);
			fileSource.setLength(fileLength);

			sourceStrings.pushBack(fileSource.data());
			sourceLengths.pushBack(static_cast<int>(fileLength));
			count++;

			setObjectLabel(filename);
			if (sourceHash == 0 && RenderResources::binaryShaderCache().isEnabled())
				sourceHash_ += RenderResources::hash64().hashFileStat(filename);
		}
	}

	LOGD_X("%s Shader %u - hash: 0x%016llx", typeToString(type_), glHandle_, sourceHash_);
	glShaderSource(glHandle_, count, sourceStrings.data(), sourceLengths.data());

	return (count > 1);
}

bool GLShader::loadFromFile(const char *filename)
{
	return loadFromStringsAndFile(nullptr, filename);
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
				LOGW_X("%s Shader: %s", typeToString(type_), infoLogString_);
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
