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

Shader::Shader()
    : Object(ObjectType::SHADER),
      glShaderProgram_(nctl::makeUnique<GLShaderProgram>(GLShaderProgram::QueryPhase::IMMEDIATE))
{
}

Shader::Shader(const char *shaderName, LoadMode loadMode, const char *vertex, const char *fragment)
    : Shader()
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

	glShaderProgram_->reset(); // reset before attaching new shaders
	setName(shaderName);
	glShaderProgram_->setObjectLabel(shaderName);
	glShaderProgram_->attachShaderFromString(GL_VERTEX_SHADER, vertex);
	glShaderProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, fragment);
	glShaderProgram_->link(GLShaderProgram::Introspection::ENABLED);

	return isLinked();
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

	glShaderProgram_->reset(); // reset before attaching new shaders
	setName(shaderName);
	glShaderProgram_->setObjectLabel(shaderName);
	glShaderProgram_->attachShader(GL_VERTEX_SHADER, vertex);
	glShaderProgram_->attachShader(GL_FRAGMENT_SHADER, fragment);
	glShaderProgram_->link(GLShaderProgram::Introspection::ENABLED);

	return isLinked();
}

bool Shader::loadFromFile(const char *vertex, const char *fragment)
{
	return loadFromFile(nullptr, vertex, fragment);
}

bool Shader::isLinked() const
{
	return glShaderProgram_->isLinked();
}

unsigned int Shader::retrieveInfoLogLength() const
{
	return glShaderProgram_->retrieveInfoLogLength();
}

void Shader::retrieveInfoLog(nctl::String &infoLog) const
{
	glShaderProgram_->retrieveInfoLog(infoLog);
}

bool Shader::logOnErrors() const
{
	return glShaderProgram_->logOnErrors();
}

void Shader::setLogOnErrors(bool shouldLogOnErrors)
{
	glShaderProgram_->setLogOnErrors(shouldLogOnErrors);
}

void Shader::setGLShaderProgramLabel(const char *label)
{
	glShaderProgram_->setObjectLabel(label);
}

}
