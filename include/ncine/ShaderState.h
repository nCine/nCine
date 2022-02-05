#ifndef CLASS_NCINE_SHADERSTATE
#define CLASS_NCINE_SHADERSTATE

#include "common_defines.h"
#include "Vector4.h"
#include "Colorf.h"

namespace ncine {

class DrawableNode;
class Shader;

/// Shader state class for the user to use custom shaders
class DLL_PUBLIC ShaderState
{
  public:
	ShaderState();
	ShaderState(DrawableNode *node, Shader *shader);
	~ShaderState();

	inline const DrawableNode *node() const { return node_; }
	bool setNode(DrawableNode *node);

	inline const Shader *shader() const { return shader_; }
	bool setShader(Shader *shader);

	bool setAttribute(const char *name, int stride, unsigned long int pointer);

	bool setUniformInt(const char *blockName, const char *name, const int *vector);
	bool setUniformInt(const char *blockName, const char *name, int value0);
	bool setUniformInt(const char *blockName, const char *name, int value0, int value1);
	bool setUniformInt(const char *blockName, const char *name, int value0, int value1, int value2);
	bool setUniformInt(const char *blockName, const char *name, int value0, int value1, int value2, int value3);

	bool setUniformInt(const char *blockName, const char *name, const Vector2i &vector);
	bool setUniformInt(const char *blockName, const char *name, const Vector3i &vector);
	bool setUniformInt(const char *blockName, const char *name, const Vector4i &vector);

	bool setUniformFloat(const char *blockName, const char *name, const float *vector);
	bool setUniformFloat(const char *blockName, const char *name, float value0);
	bool setUniformFloat(const char *blockName, const char *name, float value0, float value1);
	bool setUniformFloat(const char *blockName, const char *name, float value0, float value1, float value2);
	bool setUniformFloat(const char *blockName, const char *name, float value0, float value1, float value2, float value3);

	bool setUniformFloat(const char *blockName, const char *name, const Vector2f &vector);
	bool setUniformFloat(const char *blockName, const char *name, const Vector3f &vector);
	bool setUniformFloat(const char *blockName, const char *name, const Vector4f &vector);
	bool setUniformFloat(const char *blockName, const char *name, const Colorf &color);

  private:
	DrawableNode *node_;
	Shader *shader_;
	int previousShaderType_;

	/// Deleted copy constructor
	ShaderState(const ShaderState &) = delete;
	/// Deleted assignment operator
	ShaderState &operator=(const ShaderState &) = delete;
};

}

#endif
