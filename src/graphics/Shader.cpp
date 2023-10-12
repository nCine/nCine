#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"
#include "common_macros.h"
#include <nctl/CString.h>
#include "Shader.h"
#include "GLShaderProgram.h"
#include "RenderResources.h"
#include "BinaryShaderCache.h"
#include "tracy.h"

#ifdef WITH_EMBEDDED_SHADERS
	#include "shader_strings.h"
#else
	#include "FileSystem.h" // for dataPath()
#endif

namespace ncine {

namespace {

	GLShaderProgram::Introspection shaderToShaderProgramIntrospection(Shader::Introspection introspection)
	{
		switch (introspection)
		{
			case Shader::Introspection::ENABLED:
				return GLShaderProgram::Introspection::ENABLED;
			case Shader::Introspection::NO_UNIFORMS_IN_BLOCKS:
				return GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS;
			case Shader::Introspection::DISABLED:
				return GLShaderProgram::Introspection::DISABLED;
		}
		return GLShaderProgram::Introspection::ENABLED;
	}

	bool isBatchedVertex(Shader::DefaultVertex defaultVertex)
	{
		switch (defaultVertex)
		{
			case Shader::DefaultVertex::BATCHED_SPRITES:
			case Shader::DefaultVertex::BATCHED_SPRITES_NOTEXTURE:
			case Shader::DefaultVertex::BATCHED_MESHSPRITES:
			case Shader::DefaultVertex::BATCHED_MESHSPRITES_NOTEXTURE:
			case Shader::DefaultVertex::BATCHED_TEXTNODES:
				return true;
			default:
				return false;
		}
	}

}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Shader::Shader()
    : Object(ObjectType::SHADER),
      glShaderProgram_(nctl::makeUnique<GLShaderProgram>(GLShaderProgram::QueryPhase::IMMEDIATE))
{
}

Shader::Shader(const char *shaderName, LoadMode loadMode, Introspection introspection, const char *vertex, const char *fragment)
    : Shader()
{
	const bool hasLoaded = (loadMode == LoadMode::STRING)
	                           ? loadFromMemory(shaderName, introspection, vertex, fragment)
	                           : loadFromFile(shaderName, introspection, vertex, fragment);

	if (hasLoaded == false)
		LOGE_X("Shader \"%s\" cannot be loaded", shaderName);
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

Shader::Shader(const char *shaderName, LoadMode loadMode, Introspection introspection, DefaultVertex defaultVertex, const char *fragment)
    : Shader()
{
	const bool hasLoaded = (loadMode == LoadMode::STRING)
	                           ? loadFromMemory(shaderName, introspection, defaultVertex, fragment)
	                           : loadFromFile(shaderName, introspection, defaultVertex, fragment);

	if (hasLoaded == false)
		LOGE_X("Shader \"%s\" cannot be loaded", shaderName);
}

Shader::Shader(const char *shaderName, LoadMode loadMode, DefaultVertex defaultVertex, const char *fragment)
    : Shader()
{
	const bool hasLoaded = (loadMode == LoadMode::STRING)
	                           ? loadFromMemory(shaderName, defaultVertex, fragment)
	                           : loadFromFile(shaderName, defaultVertex, fragment);

	if (hasLoaded == false)
		LOGE_X("Shader \"%s\" cannot be loaded", shaderName);
}

Shader::Shader(LoadMode loadMode, DefaultVertex defaultVertex, const char *fragment)
    : Shader(nullptr, loadMode, defaultVertex, fragment)
{
}

Shader::Shader(const char *shaderName, LoadMode loadMode, Introspection introspection, const char *vertex, DefaultFragment defaultFragment)
    : Shader()
{
	const bool hasLoaded = (loadMode == LoadMode::STRING)
	                           ? loadFromMemory(shaderName, introspection, vertex, defaultFragment)
	                           : loadFromFile(shaderName, introspection, vertex, defaultFragment);

	if (hasLoaded == false)
		LOGE_X("Shader \"%s\" cannot be loaded", shaderName);
}

Shader::Shader(const char *shaderName, LoadMode loadMode, const char *vertex, DefaultFragment defaultFragment)
    : Shader()
{
	const bool hasLoaded = (loadMode == LoadMode::STRING)
	                           ? loadFromMemory(shaderName, vertex, defaultFragment)
	                           : loadFromFile(shaderName, vertex, defaultFragment);

	if (hasLoaded == false)
		LOGE_X("Shader \"%s\" cannot be loaded", shaderName);
}

Shader::Shader(LoadMode loadMode, const char *vertex, DefaultFragment defaultFragment)
    : Shader(nullptr, loadMode, vertex, defaultFragment)
{
}

Shader::~Shader()
{
	// In case there is a batched version of this shader
	RenderResources::unregisterBatchedShader(glShaderProgram_.get());
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool Shader::loadFromMemory(const char *shaderName, Introspection introspection, const char *vertex, const char *fragment)
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
	glShaderProgram_->link(shaderToShaderProgramIntrospection(introspection));

	return isLinked();
}

bool Shader::loadFromMemory(const char *shaderName, const char *vertex, const char *fragment)
{
	return loadFromMemory(shaderName, Introspection::ENABLED, vertex, fragment);
}

bool Shader::loadFromMemory(const char *vertex, const char *fragment)
{
	return loadFromMemory(nullptr, vertex, fragment);
}

bool Shader::loadFromMemory(const char *shaderName, Introspection introspection, DefaultVertex defaultVertex, const char *fragment)
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
	loadDefaultShader(defaultVertex);
	glShaderProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, fragment);
	glShaderProgram_->link(shaderToShaderProgramIntrospection(introspection));

	return isLinked();
}

bool Shader::loadFromMemory(const char *shaderName, DefaultVertex defaultVertex, const char *fragment)
{
	const Introspection introspection = isBatchedVertex(defaultVertex) ? Introspection::NO_UNIFORMS_IN_BLOCKS : Introspection::ENABLED;
	return loadFromMemory(shaderName, introspection, defaultVertex, fragment);
}

bool Shader::loadFromMemory(DefaultVertex defaultVertex, const char *fragment)
{
	return loadFromMemory(nullptr, defaultVertex, fragment);
}

bool Shader::loadFromMemory(const char *shaderName, Introspection introspection, const char *vertex, DefaultFragment defaultFragment)
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
	loadDefaultShader(defaultFragment);
	glShaderProgram_->link(shaderToShaderProgramIntrospection(introspection));

	return isLinked();
}

bool Shader::loadFromMemory(const char *shaderName, const char *vertex, DefaultFragment defaultFragment)
{
	return loadFromMemory(shaderName, Introspection::ENABLED, vertex, defaultFragment);
}

bool Shader::loadFromMemory(const char *vertex, DefaultFragment defaultFragment)
{
	return loadFromMemory(nullptr, vertex, defaultFragment);
}

bool Shader::loadFromFile(const char *shaderName, Introspection introspection, const char *vertex, const char *fragment)
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
	glShaderProgram_->attachShaderFromFile(GL_VERTEX_SHADER, vertex);
	glShaderProgram_->attachShaderFromFile(GL_FRAGMENT_SHADER, fragment);
	glShaderProgram_->link(shaderToShaderProgramIntrospection(introspection));

	return isLinked();
}

bool Shader::loadFromFile(const char *shaderName, const char *vertex, const char *fragment)
{
	return loadFromFile(shaderName, Introspection::ENABLED, vertex, fragment);
}

bool Shader::loadFromFile(const char *vertex, const char *fragment)
{
	return loadFromFile(nullptr, vertex, fragment);
}

bool Shader::loadFromFile(const char *shaderName, Introspection introspection, DefaultVertex defaultVertex, const char *fragment)
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
	loadDefaultShader(defaultVertex);
	glShaderProgram_->attachShaderFromFile(GL_FRAGMENT_SHADER, fragment);
	glShaderProgram_->link(shaderToShaderProgramIntrospection(introspection));

	return isLinked();
}

bool Shader::loadFromFile(const char *shaderName, DefaultVertex defaultVertex, const char *fragment)
{
	const Introspection introspection = isBatchedVertex(defaultVertex) ? Introspection::NO_UNIFORMS_IN_BLOCKS : Introspection::ENABLED;
	return loadFromFile(shaderName, introspection, defaultVertex, fragment);
}

bool Shader::loadFromFile(DefaultVertex defaultVertex, const char *fragment)
{
	return loadFromFile(nullptr, defaultVertex, fragment);
}

bool Shader::loadFromFile(const char *shaderName, Introspection introspection, const char *vertex, DefaultFragment defaultFragment)
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
	glShaderProgram_->attachShaderFromFile(GL_VERTEX_SHADER, vertex);
	loadDefaultShader(defaultFragment);
	glShaderProgram_->link(shaderToShaderProgramIntrospection(introspection));

	return isLinked();
}

bool Shader::loadFromFile(const char *shaderName, const char *vertex, DefaultFragment defaultFragment)
{
	return loadFromFile(shaderName, Introspection::ENABLED, vertex, defaultFragment);
}

bool Shader::loadFromFile(const char *vertex, DefaultFragment defaultFragment)
{
	return loadFromFile(nullptr, vertex, defaultFragment);
}

bool Shader::setAttribute(const char *name, int stride, unsigned long int pointer)
{
	GLVertexFormat::Attribute *attribute = glShaderProgram_->attribute(name);
	if (attribute != nullptr)
		attribute->setVboParameters(stride, reinterpret_cast<void *>(pointer));

	return (attribute != nullptr);
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

void Shader::registerBatchedShader(Shader &batchedShader)
{
	RenderResources::registerBatchedShader(glShaderProgram_.get(), batchedShader.glShaderProgram_.get());
}

bool Shader::isBinaryCacheEnabled()
{
	return RenderResources::binaryShaderCache().isEnabled();
}

void Shader::setBinaryCacheEnabled(bool enable)
{
	RenderResources::binaryShaderCache().setEnabled(enable);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool Shader::loadDefaultShader(DefaultVertex defaultVertex)
{
	const char *vertexShader = nullptr;
#ifndef WITH_EMBEDDED_SHADERS
	switch (defaultVertex)
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
		case DefaultVertex::BATCHED_SPRITES:
			vertexShader = "batched_sprites_vs.glsl";
			break;
		case DefaultVertex::BATCHED_SPRITES_NOTEXTURE:
			vertexShader = "batched_sprites_notexture_vs.glsl";
			break;
		case DefaultVertex::BATCHED_MESHSPRITES:
			vertexShader = "batched_meshsprites_vs.glsl";
			break;
		case DefaultVertex::BATCHED_MESHSPRITES_NOTEXTURE:
			vertexShader = "batched_meshsprites_notexture_vs.glsl";
			break;
		case DefaultVertex::BATCHED_TEXTNODES:
			vertexShader = "batched_textnodes_vs.glsl";
			break;
	}
	const bool hasCompiled = glShaderProgram_->attachShaderFromFile(GL_VERTEX_SHADER, (fs::dataPath() + RenderResources::ShadersDir + "/" + vertexShader).data());
#else
	// Skipping the initial new line character of the raw string literal
	switch (defaultVertex)
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
		case DefaultVertex::BATCHED_SPRITES:
			vertexShader = ShaderStrings::batched_sprites_vs + 1;
			break;
		case DefaultVertex::BATCHED_SPRITES_NOTEXTURE:
			vertexShader = ShaderStrings::batched_sprites_notexture_vs + 1;
			break;
		case DefaultVertex::BATCHED_MESHSPRITES:
			vertexShader = ShaderStrings::batched_meshsprites_vs + 1;
			break;
		case DefaultVertex::BATCHED_MESHSPRITES_NOTEXTURE:
			vertexShader = ShaderStrings::batched_meshsprites_notexture_vs + 1;
			break;
		case DefaultVertex::BATCHED_TEXTNODES:
			vertexShader = ShaderStrings::batched_textnodes_vs + 1;
			break;
	}
	const bool hasCompiled = glShaderProgram_->attachShaderFromString(GL_VERTEX_SHADER, vertexShader);
#endif
	return hasCompiled;
}

bool Shader::loadDefaultShader(DefaultFragment defaultFragment)
{
	const char *fragmentShader = nullptr;
#ifndef WITH_EMBEDDED_SHADERS
	switch (defaultFragment)
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
			break;
	}
	const bool hasCompiled = glShaderProgram_->attachShaderFromFile(GL_FRAGMENT_SHADER, (fs::dataPath() + RenderResources::ShadersDir + "/" + fragmentShader).data());
#else
	// Skipping the initial new line character of the raw string literal
	switch (defaultFragment)
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
			break;
	}
	const bool hasCompiled = glShaderProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, fragmentShader);
#endif
	return hasCompiled;
}

}
