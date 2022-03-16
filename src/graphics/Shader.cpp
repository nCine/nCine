#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"
#include "common_macros.h"
#include <nctl/CString.h>
#include "Shader.h"
#include "GLShaderProgram.h"
#include "tracy.h"

#ifdef WITH_EMBEDDED_SHADERS
	#include "shader_strings.h"
#else
	#include "FileSystem.h" // for dataPath()
#endif

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

Shader::Shader(const char *shaderName, LoadMode loadMode, DefaultVertex vertex, const char *fragment)
    : Shader()
{
	const bool hasLoaded = (loadMode == LoadMode::STRING)
	                           ? loadFromMemory(shaderName, vertex, fragment)
	                           : loadFromFile(shaderName, vertex, fragment);

	if (hasLoaded == false)
		LOGE_X("Shader \"%s\" cannot be loaded", shaderName);
}

Shader::Shader(LoadMode loadMode, DefaultVertex vertex, const char *fragment)
    : Shader(nullptr, loadMode, vertex, fragment)
{
}

Shader::Shader(const char *shaderName, LoadMode loadMode, const char *vertex, DefaultFragment fragment)
    : Shader()
{
	const bool hasLoaded = (loadMode == LoadMode::STRING)
	                           ? loadFromMemory(shaderName, vertex, fragment)
	                           : loadFromFile(shaderName, vertex, fragment);

	if (hasLoaded == false)
		LOGE_X("Shader \"%s\" cannot be loaded", shaderName);
}

Shader::Shader(LoadMode loadMode, const char *vertex, DefaultFragment fragment)
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

bool Shader::loadFromMemory(const char *shaderName, DefaultVertex vertex, const char *fragment)
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
	loadDefaultShader(vertex);
	glShaderProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, fragment);
	glShaderProgram_->link(GLShaderProgram::Introspection::ENABLED);

	return isLinked();
}

bool Shader::loadFromMemory(DefaultVertex vertex, const char *fragment)
{
	return loadFromMemory(nullptr, vertex, fragment);
}

bool Shader::loadFromMemory(const char *shaderName, const char *vertex, DefaultFragment fragment)
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
	loadDefaultShader(fragment);
	glShaderProgram_->link(GLShaderProgram::Introspection::ENABLED);

	return isLinked();
}

bool Shader::loadFromMemory(const char *vertex, DefaultFragment fragment)
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

bool Shader::loadFromFile(const char *shaderName, DefaultVertex vertex, const char *fragment)
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
	loadDefaultShader(vertex);
	glShaderProgram_->attachShader(GL_FRAGMENT_SHADER, fragment);
	glShaderProgram_->link(GLShaderProgram::Introspection::ENABLED);

	return isLinked();
}

bool Shader::loadFromFile(DefaultVertex vertex, const char *fragment)
{
	return loadFromFile(nullptr, vertex, fragment);
}

bool Shader::loadFromFile(const char *shaderName, const char *vertex, DefaultFragment fragment)
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
	loadDefaultShader(fragment);
	glShaderProgram_->link(GLShaderProgram::Introspection::ENABLED);

	return isLinked();
}

bool Shader::loadFromFile(const char *vertex, DefaultFragment fragment)
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

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool Shader::loadDefaultShader(DefaultVertex vertex)
{
	const char *vertexShader = nullptr;
#ifndef WITH_EMBEDDED_SHADERS
	switch (vertex)
	{
		case DefaultVertex::SPRITE:
			vertexShader = "sprite_vs.glsl";
			break;
		case DefaultVertex::SPRITE_NOTEXTURE:
			vertexShader = "sprite_notexture_vs.glsl";
			break;
		case DefaultVertex::MESHSPRITE:
			vertexShader = "meshsprite_vs.glsl";
			break;
		case DefaultVertex::MESHSPRITE_NOTEXTURE:
			vertexShader = "meshsprite_notexture_vs.glsl";
			break;
		case DefaultVertex::TEXTNODE:
			vertexShader = "textnode_vs.glsl";
			break;
	}
	const bool hasCompiled = glShaderProgram_->attachShader(GL_VERTEX_SHADER, (fs::dataPath() + "shaders/" + vertexShader).data());
#else
	// Skipping the initial new line character of the raw string literal
	switch (vertex)
	{
		case DefaultVertex::SPRITE:
			vertexShader = ShaderStrings::sprite_vs + 1;
			break;
		case DefaultVertex::SPRITE_NOTEXTURE:
			vertexShader = ShaderStrings::sprite_notexture_vs + 1;
			break;
		case DefaultVertex::MESHSPRITE:
			vertexShader = ShaderStrings::meshsprite_vs + 1;
			break;
		case DefaultVertex::MESHSPRITE_NOTEXTURE:
			vertexShader = ShaderStrings::meshsprite_notexture_vs + 1;
			break;
		case DefaultVertex::TEXTNODE:
			vertexShader = ShaderStrings::textnode_vs + 1;
			break;
	}
	const bool hasCompiled = glShaderProgram_->attachShaderFromString(GL_VERTEX_SHADER, vertexShader);
#endif
	return hasCompiled;
}

bool Shader::loadDefaultShader(DefaultFragment fragment)
{
	const char *fragmentShader = nullptr;
#ifndef WITH_EMBEDDED_SHADERS
	switch (fragment)
	{
		case DefaultFragment::SPRITE:
			fragmentShader = "sprite_fs.glsl";
			break;
		case DefaultFragment::SPRITE_GRAY:
			fragmentShader = "sprite_gray_fs.glsl";
			break;
		case DefaultFragment::SPRITE_NOTEXTURE:
			fragmentShader = "sprite_notexture_fs.glsl";
			break;
		case DefaultFragment::TEXTNODE_ALPHA:
			fragmentShader = "textnode_alpha_fs.glsl";
			break;
		case DefaultFragment::TEXTNODE_RED:
			fragmentShader = "textnode_red_fs.glsl";
	}
	const bool hasCompiled = glShaderProgram_->attachShader(GL_FRAGMENT_SHADER, (fs::dataPath() + "shaders/" + fragmentShader).data());
#else
	// Skipping the initial new line character of the raw string literal
	switch (fragment)
	{
		case DefaultFragment::SPRITE:
			fragmentShader = ShaderStrings::sprite_fs + 1;
			break;
		case DefaultFragment::SPRITE_GRAY:
			fragmentShader = ShaderStrings::sprite_gray_fs + 1;
			break;
		case DefaultFragment::SPRITE_NOTEXTURE:
			fragmentShader = ShaderStrings::sprite_notexture_fs + 1;
			break;
		case DefaultFragment::TEXTNODE_ALPHA:
			fragmentShader = ShaderStrings::textnode_alpha_fs + 1;
			break;
		case DefaultFragment::TEXTNODE_RED:
			fragmentShader = ShaderStrings::textnode_red_fs + 1;
	}
	const bool hasCompiled = glShaderProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, fragmentShader);
#endif
	return hasCompiled;
}

}
