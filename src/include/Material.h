#ifndef CLASS_NCINE_MATERIAL
#define CLASS_NCINE_MATERIAL

#include "GLShaderUniforms.h"
#include "GLShaderAttributes.h"

namespace ncine {

class GLShaderProgram;
class GLTexture;
class Texture;
class GLUniformCache;
class GLAttribute;

/// The class containing material data for a drawable node
class Material
{
  public:
	enum ShaderProgramPresets
	{
		/// Shader program for Sprite classes
		SPRITE_PROGRAM,
		/// Shader program for TextNode classes
		TEXTNODE_PROGRAM,
		/// Shader program for ProfilePlotter
		COLOR_PROGRAM
	};

	// Empty constructor
	Material();
	Material(GLShaderProgram* program, GLTexture* texture);

	bool isTransparent() const { return isTransparent_; }
	void setTransparent(bool isTransparent) { isTransparent_ = isTransparent; }

	void setShaderProgram(ShaderProgramPresets preset);
	/// Wrapper around `GLShaderUniforms::uniform()`
	inline GLUniformCache* uniform(const char *name) { return shaderUniforms_.uniform(name); }
	/// Wrapper around `GLShaderAttributes::attribute()`
	inline GLVertexAttribute* attribute(const char *name) { return shaderAttributes_.attribute(name); }
	inline void setTexture(GLTexture* texture) { texture_ = texture; }
	void setTexture(const Texture& texture);

  private:
	bool isTransparent_;

	GLShaderProgram* shaderProgram_;
	GLShaderUniforms shaderUniforms_;
	GLShaderAttributes shaderAttributes_;
	GLTexture* texture_;

	void bind();
	void setShaderProgram(GLShaderProgram* program);
	/// Wrapper around `GLShaderUniforms::commitUniforms()`
	void commitUniforms() { shaderUniforms_.commitUniforms(); }
	/// Wrapper around `GLShaderAttributes::defineVertexPointers()`
	void defineVertexPointers(GLuint boundVboHandle) { shaderAttributes_.defineVertexPointers(boundVboHandle); }
	unsigned int sortKey();

	friend class RenderCommand;
};

}

#endif
