#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"
#include "common_macros.h"
#include <nctl/CString.h>
#include "Shader.h"
#include "GLShaderProgram.h"
#include "RenderResources.h"
#include "BinaryShaderCache.h"
#include "Hash64.h"
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

	RenderResources::DefaultVertexShader shaderToRenderResourcesDefaultVertex(Shader::DefaultVertex defaultVertex)
	{
		switch (defaultVertex)
		{
			case Shader::DefaultVertex::SPRITE: return RenderResources::DefaultVertexShader::SPRITE;
			case Shader::DefaultVertex::SPRITE_NOTEXTURE: return RenderResources::DefaultVertexShader::SPRITE_NOTEXTURE;
			case Shader::DefaultVertex::MESHSPRITE: return RenderResources::DefaultVertexShader::MESHSPRITE;
			case Shader::DefaultVertex::MESHSPRITE_NOTEXTURE: return RenderResources::DefaultVertexShader::MESHSPRITE_NOTEXTURE;
			case Shader::DefaultVertex::TEXTNODE: return RenderResources::DefaultVertexShader::TEXTNODE;
			case Shader::DefaultVertex::BATCHED_SPRITES: return RenderResources::DefaultVertexShader::BATCHED_SPRITES;
			case Shader::DefaultVertex::BATCHED_SPRITES_NOTEXTURE: return RenderResources::DefaultVertexShader::BATCHED_SPRITES_NOTEXTURE;
			case Shader::DefaultVertex::BATCHED_MESHSPRITES: return RenderResources::DefaultVertexShader::BATCHED_MESHSPRITES;
			case Shader::DefaultVertex::BATCHED_MESHSPRITES_NOTEXTURE: return RenderResources::DefaultVertexShader::BATCHED_MESHSPRITES_NOTEXTURE;
			case Shader::DefaultVertex::BATCHED_TEXTNODES: return RenderResources::DefaultVertexShader::BATCHED_TEXTNODES;
		}
		return RenderResources::DefaultVertexShader::SPRITE;
	}

	RenderResources::DefaultFragmentShader shaderToRenderResourcesDefaultFragment(Shader::DefaultFragment defaultFragment)
	{
		switch (defaultFragment)
		{
			case Shader::DefaultFragment::SPRITE: return RenderResources::DefaultFragmentShader::SPRITE;
			case Shader::DefaultFragment::SPRITE_GRAY: return RenderResources::DefaultFragmentShader::SPRITE_GRAY;
			case Shader::DefaultFragment::SPRITE_NOTEXTURE: return RenderResources::DefaultFragmentShader::SPRITE_NOTEXTURE;
			case Shader::DefaultFragment::TEXTNODE_ALPHA: return RenderResources::DefaultFragmentShader::TEXTNODE_ALPHA;
			case Shader::DefaultFragment::TEXTNODE_RED: return RenderResources::DefaultFragmentShader::TEXTNODE_RED;
		}
		return RenderResources::DefaultFragmentShader::SPRITE;
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

Shader::Shader(const char *shaderName, LoadMode loadMode, Introspection introspection, const char *vertex, const char *fragment, uint64_t vertexHash, uint64_t fragmentHash)
    : Shader()
{
	const bool hasLoaded = (loadMode == LoadMode::STRING)
	                           ? loadFromMemory(shaderName, introspection, vertex, fragment, vertexHash, fragmentHash)
	                           : loadFromFile(shaderName, introspection, vertex, fragment);

	if (hasLoaded == false)
		LOGE_X("Shader \"%s\" cannot be loaded", shaderName);
}

Shader::Shader(const char *shaderName, LoadMode loadMode, Introspection introspection, const char *vertex, const char *fragment)
    : Shader(shaderName, loadMode, introspection, vertex, fragment, 0, 0)
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

Shader::Shader(const char *shaderName, LoadMode loadMode, Introspection introspection, DefaultVertex defaultVertex, const char *fragment, uint64_t fragmentHash)
    : Shader()
{
	const bool hasLoaded = (loadMode == LoadMode::STRING)
	                           ? loadFromMemory(shaderName, introspection, defaultVertex, fragment, fragmentHash)
	                           : loadFromFile(shaderName, introspection, defaultVertex, fragment);

	if (hasLoaded == false)
		LOGE_X("Shader \"%s\" cannot be loaded", shaderName);
}

Shader::Shader(const char *shaderName, LoadMode loadMode, Introspection introspection, DefaultVertex defaultVertex, const char *fragment)
    : Shader(shaderName, loadMode, introspection, defaultVertex, fragment, 0)
{
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

Shader::Shader(const char *shaderName, LoadMode loadMode, Introspection introspection, const char *vertex, DefaultFragment defaultFragment, uint64_t vertexHash)
    : Shader()
{
	const bool hasLoaded = (loadMode == LoadMode::STRING)
	                           ? loadFromMemory(shaderName, introspection, vertex, defaultFragment, vertexHash)
	                           : loadFromFile(shaderName, introspection, vertex, defaultFragment);

	if (hasLoaded == false)
		LOGE_X("Shader \"%s\" cannot be loaded", shaderName);
}

Shader::Shader(const char *shaderName, LoadMode loadMode, Introspection introspection, const char *vertex, DefaultFragment defaultFragment)
    : Shader(shaderName, loadMode, introspection, vertex, defaultFragment, 0)
{
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

bool Shader::loadFromMemory(const char *shaderName, Introspection introspection, const char *vertex, const char *fragment, uint64_t vertexHash, uint64_t fragmentHash)
{
	return load(LoadMode::STRING, shaderName, introspection, vertex, fragment, DefaultVertex::SPRITE, DefaultFragment::SPRITE, vertexHash, fragmentHash);
}

bool Shader::loadFromMemory(const char *shaderName, Introspection introspection, const char *vertex, const char *fragment)
{
	return load(LoadMode::STRING, shaderName, introspection, vertex, fragment, DefaultVertex::SPRITE, DefaultFragment::SPRITE, 0, 0);
}

bool Shader::loadFromMemory(const char *shaderName, const char *vertex, const char *fragment)
{
	return load(LoadMode::STRING, shaderName, Introspection::ENABLED, vertex, fragment, DefaultVertex::SPRITE, DefaultFragment::SPRITE, 0, 0);
}

bool Shader::loadFromMemory(const char *vertex, const char *fragment)
{
	return load(LoadMode::STRING, nullptr, Introspection::ENABLED, vertex, fragment, DefaultVertex::SPRITE, DefaultFragment::SPRITE, 0, 0);
}

bool Shader::loadFromMemory(const char *shaderName, Introspection introspection, DefaultVertex defaultVertex, const char *fragment, uint64_t fragmentHash)
{
	return load(LoadMode::STRING, shaderName, introspection, nullptr, fragment, defaultVertex, DefaultFragment::SPRITE, 0, fragmentHash);
}

bool Shader::loadFromMemory(const char *shaderName, Introspection introspection, DefaultVertex defaultVertex, const char *fragment)
{
	return load(LoadMode::STRING, shaderName, introspection, nullptr, fragment, defaultVertex, DefaultFragment::SPRITE, 0, 0);
}

bool Shader::loadFromMemory(const char *shaderName, DefaultVertex defaultVertex, const char *fragment)
{
	const Introspection introspection = isBatchedVertex(defaultVertex) ? Introspection::NO_UNIFORMS_IN_BLOCKS : Introspection::ENABLED;
	return load(LoadMode::STRING, shaderName, introspection, nullptr, fragment, defaultVertex, DefaultFragment::SPRITE, 0, 0);
}

bool Shader::loadFromMemory(DefaultVertex defaultVertex, const char *fragment)
{
	const Introspection introspection = isBatchedVertex(defaultVertex) ? Introspection::NO_UNIFORMS_IN_BLOCKS : Introspection::ENABLED;
	return load(LoadMode::STRING, nullptr, introspection, nullptr, fragment, defaultVertex, DefaultFragment::SPRITE, 0, 0);
}

bool Shader::loadFromMemory(const char *shaderName, Introspection introspection, const char *vertex, DefaultFragment defaultFragment, uint64_t vertexHash)
{
	return load(LoadMode::STRING, shaderName, introspection, vertex, nullptr, DefaultVertex::SPRITE, defaultFragment, vertexHash, 0);
}

bool Shader::loadFromMemory(const char *shaderName, Introspection introspection, const char *vertex, DefaultFragment defaultFragment)
{
	return load(LoadMode::STRING, shaderName, introspection, vertex, nullptr, DefaultVertex::SPRITE, defaultFragment, 0, 0);
}

bool Shader::loadFromMemory(const char *shaderName, const char *vertex, DefaultFragment defaultFragment)
{
	return load(LoadMode::STRING, shaderName, Introspection::ENABLED, vertex, nullptr, DefaultVertex::SPRITE, defaultFragment, 0, 0);
}

bool Shader::loadFromMemory(const char *vertex, DefaultFragment defaultFragment)
{
	return load(LoadMode::STRING, nullptr, Introspection::ENABLED, vertex, nullptr, DefaultVertex::SPRITE, defaultFragment, 0, 0);
}

bool Shader::loadFromFile(const char *shaderName, Introspection introspection, const char *vertex, const char *fragment)
{
	return load(LoadMode::FILE, shaderName, introspection, vertex, fragment, DefaultVertex::SPRITE, DefaultFragment::SPRITE, 0, 0);
}

bool Shader::loadFromFile(const char *shaderName, const char *vertex, const char *fragment)
{
	return load(LoadMode::FILE, shaderName, Introspection::ENABLED, vertex, fragment, DefaultVertex::SPRITE, DefaultFragment::SPRITE, 0, 0);
}

bool Shader::loadFromFile(const char *vertex, const char *fragment)
{
	return load(LoadMode::FILE, nullptr, Introspection::ENABLED, vertex, fragment, DefaultVertex::SPRITE, DefaultFragment::SPRITE, 0, 0);
}

bool Shader::loadFromFile(const char *shaderName, Introspection introspection, DefaultVertex defaultVertex, const char *fragment)
{
	return load(LoadMode::FILE, shaderName, introspection, nullptr, fragment, defaultVertex, DefaultFragment::SPRITE, 0, 0);
}

bool Shader::loadFromFile(const char *shaderName, DefaultVertex defaultVertex, const char *fragment)
{
	const Introspection introspection = isBatchedVertex(defaultVertex) ? Introspection::NO_UNIFORMS_IN_BLOCKS : Introspection::ENABLED;
	return load(LoadMode::FILE, shaderName, introspection, nullptr, fragment, defaultVertex, DefaultFragment::SPRITE, 0, 0);
}

bool Shader::loadFromFile(DefaultVertex defaultVertex, const char *fragment)
{
	const Introspection introspection = isBatchedVertex(defaultVertex) ? Introspection::NO_UNIFORMS_IN_BLOCKS : Introspection::ENABLED;
	return load(LoadMode::FILE, nullptr, introspection, nullptr, fragment, defaultVertex, DefaultFragment::SPRITE, 0, 0);
}

bool Shader::loadFromFile(const char *shaderName, Introspection introspection, const char *vertex, DefaultFragment defaultFragment)
{
	return load(LoadMode::FILE, shaderName, introspection, vertex, nullptr, DefaultVertex::SPRITE, defaultFragment, 0, 0);
}

bool Shader::loadFromFile(const char *shaderName, const char *vertex, DefaultFragment defaultFragment)
{
	return load(LoadMode::FILE, shaderName, Introspection::ENABLED, vertex, nullptr, DefaultVertex::SPRITE, defaultFragment, 0, 0);
}

bool Shader::loadFromFile(const char *vertex, DefaultFragment defaultFragment)
{
	return load(LoadMode::FILE, nullptr, Introspection::ENABLED, vertex, nullptr, DefaultVertex::SPRITE, defaultFragment, 0, 0);
}

/*! \return True if the vertex attribute has been found */
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

/*! \return True if the shader program has been successfully linked */
bool Shader::load(LoadMode loadMode, const char *shaderName, Introspection introspection, const char *vertex, const char *fragment,
                  DefaultVertex defaultVertex, DefaultFragment defaultFragment, uint64_t vertexHash, uint64_t fragmentHash)
{
	ZoneScoped;
	if (shaderName)
	{
		// When Tracy is disabled the statement body is empty and braces are needed
		ZoneText(shaderName, nctl::strnlen(shaderName, nctl::String::MaxCStringLength));
	}

	// Do not provide a hash if loading a default shader
	ASSERT(vertex || (vertex == nullptr && vertexHash == 0));
	ASSERT(fragment || (fragment == nullptr && fragmentHash == 0));

	if (loadMode == LoadMode::FILE)
	{
		// Do not provide a hash if loading from files
		ASSERT(vertexHash == 0 && fragmentHash == 0);
	}

	// If the binary shader cache is available, then a hash needs to be calculated for shader strings
	BinaryShaderCache &bsc = RenderResources::binaryShaderCache();
	const bool withHash = (bsc.isEnabled() && loadMode == LoadMode::STRING);

	RenderResources::ShaderProgramCompileInfo shaderProgramInfo(glShaderProgram_, shaderToShaderProgramIntrospection(introspection), shaderName);

#ifndef WITH_EMBEDDED_SHADERS
	nctl::String vertexShaderPath(256);
	nctl::String fragmentShaderPath(256);
#endif

	// Requesting a default vertex shader
	if (vertex == nullptr)
	{
		// If default shaders are embedded, then the default shader info structure point to sources from `ShaderStrings`
		shaderProgramInfo.vertexInfo = RenderResources::defaultVertexShaderInfo(shaderToRenderResourcesDefaultVertex(defaultVertex));
#ifndef WITH_EMBEDDED_SHADERS
		// Prepending the shaders path to the shader filename
		vertexShaderPath = fs::dataPath() + RenderResources::ShadersDir + "/" + shaderProgramInfo.vertexInfo.shaderFile;
		shaderProgramInfo.vertexInfo.shaderFile = vertexShaderPath.data();
#endif
	}
	else
	{
		shaderProgramInfo.vertexInfo.hash = vertexHash; // assign the provided hash first
		if (loadMode == LoadMode::STRING)
		{
			shaderProgramInfo.vertexInfo.shaderString = vertex;
			// If the hash is not provided, it is calculated from the source string
			if (vertexHash == 0 && withHash)
				shaderProgramInfo.vertexInfo.hash = RenderResources::hash64().hashString(vertex, strlen(vertex));
		}
		else
			shaderProgramInfo.vertexInfo.shaderFile = vertex;
	}

	// Requesting a default fragment shader
	if (fragment == nullptr)
	{
		// If default shaders are embedded, then the default shader info structures point to sources from `ShaderStrings`
		shaderProgramInfo.fragmentInfo = RenderResources::defaultFragmentShaderInfo(shaderToRenderResourcesDefaultFragment(defaultFragment));
#ifndef WITH_EMBEDDED_SHADERS
		// Prepending the shaders path to the shader filename
		fragmentShaderPath = fs::dataPath() + RenderResources::ShadersDir + "/" + shaderProgramInfo.fragmentInfo.shaderFile;
		shaderProgramInfo.fragmentInfo.shaderFile = fragmentShaderPath.data();
#endif
	}
	else
	{
		shaderProgramInfo.fragmentInfo.hash = fragmentHash; // assign the provided hash first
		if (loadMode == LoadMode::STRING)
		{
			shaderProgramInfo.fragmentInfo.shaderString = fragment;
			// If the hash is not provided, it is calculated from the source string
			if (fragmentHash == 0 && withHash)
				shaderProgramInfo.fragmentInfo.hash = RenderResources::hash64().hashString(fragment, strlen(fragment));
		}
		else
			shaderProgramInfo.fragmentInfo.shaderFile = fragment;
	}

	RenderResources::compileShader(shaderProgramInfo);
	// The shader info hashmap with the custom shader hashes will be saved at application shutdown

	return isLinked();
}

}
