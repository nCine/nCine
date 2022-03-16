#ifndef CLASS_NCINE_SHADER
#define CLASS_NCINE_SHADER

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

	enum class DefaultVertex
	{
		SPRITE,
		SPRITE_NOTEXTURE,
		MESHSPRITE,
		MESHSPRITE_NOTEXTURE,
		TEXTNODE,
	};

	enum class DefaultFragment
	{
		SPRITE,
		SPRITE_GRAY,
		SPRITE_NOTEXTURE,
		TEXTNODE_ALPHA,
		TEXTNODE_RED,
	};

	/// Creates an OpenGL shader program name
	Shader();

	Shader(const char *shaderName, LoadMode loadMode, const char *vertex, const char *fragment);
	Shader(LoadMode loadMode, const char *vertex, const char *fragment);

	Shader(const char *shaderName, LoadMode loadMode, DefaultVertex vertex, const char *fragment);
	Shader(LoadMode loadMode, DefaultVertex vertex, const char *fragment);
	Shader(const char *shaderName, LoadMode loadMode, const char *vertex, DefaultFragment fragment);
	Shader(LoadMode loadMode, const char *vertex, DefaultFragment fragment);

	bool loadFromMemory(const char *shaderName, const char *vertex, const char *fragment);
	bool loadFromMemory(const char *vertex, const char *fragment);

	bool loadFromMemory(const char *shaderName, DefaultVertex vertex, const char *fragment);
	bool loadFromMemory(DefaultVertex vertex, const char *fragment);
	bool loadFromMemory(const char *shaderName, const char *vertex, DefaultFragment fragment);
	bool loadFromMemory(const char *vertex, DefaultFragment fragment);

	bool loadFromFile(const char *shaderName, const char *vertex, const char *fragment);
	bool loadFromFile(const char *vertex, const char *fragment);

	bool loadFromFile(const char *shaderName, DefaultVertex vertex, const char *fragment);
	bool loadFromFile(DefaultVertex vertex, const char *fragment);
	bool loadFromFile(const char *shaderName, const char *vertex, DefaultFragment fragment);
	bool loadFromFile(const char *vertex, DefaultFragment fragment);

	/// Returns true if the shader is linked and can therefore be used
	bool isLinked() const;

	/// Returns the length of the information log including the null termination character
	unsigned int retrieveInfoLogLength() const;
	/// Retrieves the information log and copies it in the provided string object
	void retrieveInfoLog(nctl::String &infoLog) const;

	/// Returns the automatic log on errors flag
	bool logOnErrors() const;
	/// Sets the automatic log on errors flag
	/*! If the flag is true the shader will automatically log compilation and linking errors. */
	void setLogOnErrors(bool shouldLogOnErrors);

	/// Sets the OpenGL object label for the shader program
	void setGLShaderProgramLabel(const char *label);

	inline static ObjectType sType() { return ObjectType::SHADER; }

  private:
	/// The OpenGL shader program
	nctl::UniquePtr<GLShaderProgram> glShaderProgram_;

	bool loadDefaultShader(DefaultVertex vertex);
	bool loadDefaultShader(DefaultFragment fragment);

	/// Deleted copy constructor
	Shader(const Shader &) = delete;
	/// Deleted assignment operator
	Shader &operator=(const Shader &) = delete;

	friend class ShaderState;
};

}

#endif
