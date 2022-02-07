#ifndef CLASS_NCINE_SHADER
#define CLASS_NCINE_SHADER

#include "Object.h"

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

	Shader(const char *shaderName, LoadMode loadMode, const char *vertex, const char *fragment);
	Shader(LoadMode loadMode, const char *vertex, const char *fragment);

	bool loadFromMemory(const char *shaderName, const char *vertex, const char *fragment);
	bool loadFromMemory(const char *vertex, const char *fragment);
	bool loadFromFile(const char *shaderName, const char *vertex, const char *fragment);
	bool loadFromFile(const char *vertex, const char *fragment);

	/// Returns true if the shader is linked and can therefore be used
	bool isLinked() const;

	void setGLShaderProgramLabel(const char *label);

	inline static ObjectType sType() { return ObjectType::SHADER; }

  private:
	/// The OpenGL shader program
	nctl::UniquePtr<GLShaderProgram> glShaderProgram_;

	/// Deleted copy constructor
	Shader(const Shader &) = delete;
	/// Deleted assignment operator
	Shader &operator=(const Shader &) = delete;

	friend class ShaderState;
};

}

#endif
