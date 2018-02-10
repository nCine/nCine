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
	enum class ShaderProgram
	{
		/// Shader program for Sprite classes
		SPRITE,
		/// Shader program for TextNode classes with grayscale font texture
		TEXTNODE_GRAY,
		/// Shader program for TextNode classes with color font texture
		TEXTNODE_COLOR,
		/// Shader program for ProfilePlotter
		COLOR
	};

	/// Default constructor
	Material();
	Material(GLShaderProgram *program, GLTexture *texture);

	bool isTransparent() const { return isTransparent_; }
	void setTransparent(bool isTransparent) { isTransparent_ = isTransparent; }

	void setShaderProgram(ShaderProgram shaderProgram);
	/// Wrapper around `GLShaderUniforms::uniform()`
	inline GLUniformCache *uniform(const char *name) { return shaderUniforms_.uniform(name); }
	/// Wrapper around `GLShaderAttributes::attribute()`
	inline GLVertexAttribute *attribute(const char *name) { return shaderAttributes_.attribute(name); }
	inline void setTexture(GLTexture *texture) { texture_ = texture; }
	void setTexture(const Texture &texture);

  private:
	bool isTransparent_;

	GLShaderProgram *shaderProgram_;
	GLShaderUniforms shaderUniforms_;
	GLShaderAttributes shaderAttributes_;
	const GLTexture *texture_;

	void bind();
	void setShaderProgram(GLShaderProgram *program);
	/// Wrapper around `GLShaderUniforms::commitUniforms()`
	void commitUniforms() { shaderUniforms_.commitUniforms(); }
	/// Wrapper around `GLShaderAttributes::defineVertexPointers()`
	void defineVertexPointers(const GLBufferObject *vbo) { shaderAttributes_.defineVertexPointers(vbo); }
	unsigned int sortKey();

	friend class RenderCommand;
};

}

#endif
