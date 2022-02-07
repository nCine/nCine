#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"
#include "common_macros.h"
#include <nctl/CString.h>
#include "Shader.h"
#include "GLShaderProgram.h"
#include "tracy.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Shader::Shader(const char *shaderName, LoadMode loadMode, const char *vertex, const char *fragment)
    : Object(ObjectType::SHADER)
{
	const bool hasLoaded = (loadMode == LoadMode::STRING)
	                           ? loadFromMemory(shaderName, vertex, fragment)
	                           : loadFromFile(shaderName, vertex, fragment);

	if (hasLoaded == false)
		LOGE_X("Shader \"%s\" cannot be loaded", shaderName);
}

Shader::Shader(LoadMode loadMode, const char *vertex, const char *fragment)
    : Shader(nullptr, loadMode, vertex, fragment)
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool Shader::loadFromMemory(const char *shaderName, const char *vertex, const char *fragment)
{
	ZoneScoped;
	if (shaderName)
	{
		// When Tracy is disabled the statement body is empty and braces are needed
		ZoneText(shaderName, nctl::strnlen(shaderName, nctl::String::MaxCStringLength));
	}

	nctl::UniquePtr<GLShaderProgram> shaderProgram = nctl::makeUnique<GLShaderProgram>(GLShaderProgram::QueryPhase::IMMEDIATE);
	shaderProgram->setFatalAssertOnErrors(false);
	setName(shaderName);
	shaderProgram->setObjectLabel(shaderName);
	shaderProgram->attachShaderFromString(GL_VERTEX_SHADER, vertex);
	shaderProgram->attachShaderFromString(GL_FRAGMENT_SHADER, fragment);
	shaderProgram->link(GLShaderProgram::Introspection::ENABLED);

	const bool hasLinked = shaderProgram->status() != GLShaderProgram::Status::NOT_LINKED &&
	                       shaderProgram->status() != GLShaderProgram::Status::LINKING_FAILED;
	if (hasLinked)
		glShaderProgram_ = nctl::move(shaderProgram);
	return hasLinked;
}

bool Shader::loadFromMemory(const char *vertex, const char *fragment)
{
	return loadFromMemory(nullptr, vertex, fragment);
}

bool Shader::loadFromFile(const char *shaderName, const char *vertex, const char *fragment)
{
	ZoneScoped;
	if (shaderName)
	{
		// When Tracy is disabled the statement body is empty and braces are needed
		ZoneText(shaderName, nctl::strnlen(shaderName, nctl::String::MaxCStringLength));
	}

	nctl::UniquePtr<GLShaderProgram> shaderProgram = nctl::makeUnique<GLShaderProgram>(GLShaderProgram::QueryPhase::IMMEDIATE);
	shaderProgram->setFatalAssertOnErrors(false);
	setName(shaderName);
	shaderProgram->setObjectLabel(shaderName);
	shaderProgram->attachShader(GL_VERTEX_SHADER, vertex);
	shaderProgram->attachShader(GL_FRAGMENT_SHADER, fragment);
	shaderProgram->link(GLShaderProgram::Introspection::ENABLED);

	const bool hasLinked = shaderProgram->status() != GLShaderProgram::Status::NOT_LINKED &&
	                       shaderProgram->status() != GLShaderProgram::Status::LINKING_FAILED;
	if (hasLinked)
		glShaderProgram_ = nctl::move(shaderProgram);
	return hasLinked;
}

bool Shader::loadFromFile(const char *vertex, const char *fragment)
{
	return loadFromFile(nullptr, vertex, fragment);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool Shader::isLinked() const
{
	return (glShaderProgram_->status() != GLShaderProgram::Status::NOT_LINKED &&
	        glShaderProgram_->status() != GLShaderProgram::Status::LINKING_FAILED);
}

void Shader::setGLShaderProgramLabel(const char *label)
{
	glShaderProgram_->setObjectLabel(label);
}

}
