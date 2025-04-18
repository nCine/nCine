#ifndef CLASS_NCINE_SHADER
#define CLASS_NCINE_SHADER

#include <stdint.h>
#include "Object.h"

namespace nctl {
class String;
}

namespace ncine {

class GLShaderProgram;

/// Shader class
class DLL_PUBLIC Shader : public Object
{
  public:
	enum class LoadMode
	{
		STRING,
		FILE
	};

	enum class Introspection
	{
		ENABLED,
		NO_UNIFORMS_IN_BLOCKS,
		DISABLED
	};

	enum class DefaultVertex
	{
		SPRITE,
		SPRITE_NOTEXTURE,
		MESHSPRITE,
		MESHSPRITE_NOTEXTURE,
		TEXTNODE,
		BATCHED_SPRITES,
		BATCHED_SPRITES_NOTEXTURE,
		BATCHED_MESHSPRITES,
		BATCHED_MESHSPRITES_NOTEXTURE,
		BATCHED_TEXTNODES
	};

	enum class DefaultFragment
	{
		SPRITE,
		SPRITE_GRAY,
		SPRITE_NOTEXTURE,
		TEXTNODE_ALPHA,
		TEXTNODE_RED
	};

	/// Creates an OpenGL shader program name
	Shader();

	Shader(const char *shaderName, LoadMode loadMode, Introspection introspection, const char *vertex, const char *fragment, uint64_t vertexHash, uint64_t fragmentHash);
	Shader(const char *shaderName, LoadMode loadMode, Introspection introspection, const char *vertex, const char *fragment);
	Shader(const char *shaderName, LoadMode loadMode, const char *vertex, const char *fragment);
	Shader(LoadMode loadMode, const char *vertex, const char *fragment);

	Shader(const char *shaderName, LoadMode loadMode, Introspection introspection, DefaultVertex defaultVertex, const char *fragment, uint64_t fragmentHash);
	Shader(const char *shaderName, LoadMode loadMode, Introspection introspection, DefaultVertex defaultVertex, const char *fragment);
	Shader(const char *shaderName, LoadMode loadMode, DefaultVertex defaultVertex, const char *fragment);
	Shader(LoadMode loadMode, DefaultVertex defaultVertex, const char *fragment);
	Shader(const char *shaderName, LoadMode loadMode, Introspection introspection, const char *vertex, DefaultFragment defaultFragment, uint64_t vertexHash);
	Shader(const char *shaderName, LoadMode loadMode, Introspection introspection, const char *vertex, DefaultFragment defaultFragment);
	Shader(const char *shaderName, LoadMode loadMode, const char *vertex, DefaultFragment defaultFragment);
	Shader(LoadMode loadMode, const char *vertex, DefaultFragment defaultFragment);

	~Shader() override;

	bool loadFromMemory(const char *shaderName, Introspection introspection, const char *vertex, const char *fragment, uint64_t vertexHash, uint64_t fragmentHash);
	bool loadFromMemory(const char *shaderName, Introspection introspection, const char *vertex, const char *fragment);
	bool loadFromMemory(const char *shaderName, const char *vertex, const char *fragment);
	bool loadFromMemory(const char *vertex, const char *fragment);

	bool loadFromMemory(const char *shaderName, Introspection introspection, DefaultVertex defaultVertex, const char *fragment, uint64_t fragmentHash);
	bool loadFromMemory(const char *shaderName, Introspection introspection, DefaultVertex defaultVertex, const char *fragment);
	bool loadFromMemory(const char *shaderName, DefaultVertex defaultVertex, const char *fragment);
	bool loadFromMemory(DefaultVertex defaultVertex, const char *fragment);
	bool loadFromMemory(const char *shaderName, Introspection introspection, const char *vertex, DefaultFragment defaultFragment, uint64_t vertexHash);
	bool loadFromMemory(const char *shaderName, Introspection introspection, const char *vertex, DefaultFragment defaultFragment);
	bool loadFromMemory(const char *shaderName, const char *vertex, DefaultFragment defaultFragment);
	bool loadFromMemory(const char *vertex, DefaultFragment defaultFragment);

	bool loadFromFile(const char *shaderName, Introspection introspection, const char *vertex, const char *fragment);
	bool loadFromFile(const char *shaderName, const char *vertex, const char *fragment);
	bool loadFromFile(const char *vertex, const char *fragment);

	bool loadFromFile(const char *shaderName, Introspection introspection, DefaultVertex defaultVertex, const char *fragment);
	bool loadFromFile(const char *shaderName, DefaultVertex defaultVertex, const char *fragment);
	bool loadFromFile(DefaultVertex defaultVertex, const char *fragment);
	bool loadFromFile(const char *shaderName, Introspection introspection, const char *vertex, DefaultFragment defaultFragment);
	bool loadFromFile(const char *shaderName, const char *vertex, DefaultFragment defaultFragment);
	bool loadFromFile(const char *vertex, DefaultFragment defaultFragment);

	/// Sets the VBO stride and pointer for the specified vertex attribute
	bool setAttribute(const char *name, int stride, unsigned long int pointer);

	/// Returns true if the shader is linked and can therefore be used
	bool isLinked() const;

	/// Returns the length of the information log including the null termination character
	unsigned int retrieveInfoLogLength() const;
	/// Retrieves the information log and copies it in the provided string object
	void retrieveInfoLog(nctl::String &infoLog) const;

	/// Returns the automatic log on errors flag
	bool logOnErrors() const;
	/// Sets the automatic log on errors flag
	/*! If the flag is set to true, the shader will automatically log compilation and linking errors. */
	void setLogOnErrors(bool shouldLogOnErrors);

	/// Sets the OpenGL object label for the shader program
	void setGLShaderProgramLabel(const char *label);

	/// Registers a shader to be used for batches of render commands
	void registerBatchedShader(Shader &batchedShader);

	/// Returns true if the binary shader cache is enabled
	static bool isBinaryCacheEnabled();
	/// Enables or disables the binary shader cache
	static void setBinaryCacheEnabled(bool enable);

	inline static ObjectType sType() { return ObjectType::SHADER; }

  private:
	/// The OpenGL shader program
	nctl::UniquePtr<GLShaderProgram> glShaderProgram_;

	/// The method that all wrappers use to load a shader. It supports all arguments and combinations.
	bool load(LoadMode loadMode, const char *shaderName, Introspection introspection, const char *vertex, const char *fragment,
	          DefaultVertex defaultVertex, DefaultFragment defaultFragment, uint64_t vertexHash, uint64_t fragmentHash);

	/// Deleted copy constructor
	Shader(const Shader &) = delete;
	/// Deleted assignment operator
	Shader &operator=(const Shader &) = delete;

	friend class ShaderState;
};

}

#endif
