#ifndef CLASS_NCINE_MATERIAL
#define CLASS_NCINE_MATERIAL

#include "GLShaderUniforms.h"
#include "GLShaderUniformBlocks.h"

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
	/// One of the predefined shader programs
	enum class ShaderProgramType
	{
		/// Shader program for Sprite classes
		SPRITE = 0,
		/// Shader program for Sprite classes with grayscale font texture
		SPRITE_GRAY,
		/// Shader program for Sprite classes with a solid color and no texture
		SPRITE_NO_TEXTURE,
		/// Shader program for MeshSprite classes
		MESH_SPRITE,
		/// Shader program for MeshSprite classes with grayscale font texture
		MESH_SPRITE_GRAY,
		/// Shader program for MeshSprite classes with a solid color and no texture
		MESH_SPRITE_NO_TEXTURE,
		/// Shader program for TextNode classes with glyph data in alpha channel
		TEXTNODE_ALPHA,
		/// Shader program for TextNode classes with glyph data in red channel
		TEXTNODE_RED,
		/// Shader program for a batch of Sprite classes
		BATCHED_SPRITES,
		/// Shader program for a batch of Sprite classes with grayscale font texture
		BATCHED_SPRITES_GRAY,
		/// Shader program for a batch of Sprite classes with solid colors and no texture
		BATCHED_SPRITES_NO_TEXTURE,
		/// Shader program for a batch of MeshSprite classes
		BATCHED_MESH_SPRITES,
		/// Shader program for a batch of MeshSprite classes with grayscale font texture
		BATCHED_MESH_SPRITES_GRAY,
		/// Shader program for a batch of MeshSprite classes with solid colors and no texture
		BATCHED_MESH_SPRITES_NO_TEXTURE,
		/// Shader program for a batch of TextNode classes with color font texture
		BATCHED_TEXTNODES_ALPHA,
		/// Shader program for a batch of TextNode classes with grayscale font texture
		BATCHED_TEXTNODES_RED,
		/// A custom shader program
		CUSTOM
	};

	// Shader uniform block and model matrix uniform names
	static const char *InstanceBlockName;
	static const char *InstancesBlockName; // for batched shaders
	static const char *ModelMatrixUniformName;

	// Camera related shader uniform names
	static const char *GuiProjectionMatrixUniformName;
	static const char *DepthUniformName;
	static const char *ProjectionMatrixUniformName;
	static const char *ViewMatrixUniformName;
	static const char *ProjectionViewMatrixExcludeString;

	// Shader uniform and attribute names
	static const char *TextureUniformName;
	static const char *ColorUniformName;
	static const char *SpriteSizeUniformName;
	static const char *TexRectUniformName;
	static const char *PositionAttributeName;
	static const char *TexCoordsAttributeName;
	static const char *MeshIndexAttributeName;
	static const char *ColorAttributeName;

	/// Default constructor
	Material();
	Material(GLShaderProgram *program, GLTexture *texture);

	inline bool isBlendingEnabled() const { return isBlendingEnabled_; }
	inline void setBlendingEnabled(bool blendingEnabled) { isBlendingEnabled_ = blendingEnabled; }

	inline GLenum srcBlendingFactor() const { return srcBlendingFactor_; }
	inline GLenum destBlendingFactor() const { return destBlendingFactor_; }
	void setBlendingFactors(GLenum srcBlendingFactor, GLenum destBlendingFactor);

	inline ShaderProgramType shaderProgramType() const { return shaderProgramType_; }
	bool setShaderProgramType(ShaderProgramType shaderProgramType);
	inline const GLShaderProgram *shaderProgram() const { return shaderProgram_; }
	inline GLShaderProgram *shaderProgram() { return shaderProgram_; }
	void setShaderProgram(GLShaderProgram *program);

	void setDefaultAttributesParameters();
	void reserveUniformsDataMemory();
	void setUniformsDataPointer(GLubyte *dataPointer);

	/// Wrapper around `GLShaderUniforms::hasUniform()`
	inline bool hasUniform(const char *name) const { return shaderUniforms_.hasUniform(name); }
	/// Wrapper around `GLShaderUniformBlocks::hasUniformBlock()`
	inline bool hasUniformBlock(const char *name) const { return shaderUniformBlocks_.hasUniformBlock(name); }

	/// Wrapper around `GLShaderUniforms::uniform()`
	inline GLUniformCache *uniform(const char *name) { return shaderUniforms_.uniform(name); }
	/// Wrapper around `GLShaderUniformBlocks::uniformBlock()`
	inline GLUniformBlockCache *uniformBlock(const char *name) { return shaderUniformBlocks_.uniformBlock(name); }

	inline const GLTexture *texture() const { return texture_; }
	inline void setTexture(const GLTexture *texture) { texture_ = texture; }
	void setTexture(const Texture &texture);

  private:
	bool isBlendingEnabled_;
	GLenum srcBlendingFactor_;
	GLenum destBlendingFactor_;

	ShaderProgramType shaderProgramType_;
	GLShaderProgram *shaderProgram_;
	GLShaderUniforms shaderUniforms_;
	GLShaderUniformBlocks shaderUniformBlocks_;
	const GLTexture *texture_;

	/// The size of the memory buffer containing uniform values
	unsigned int uniformsHostBufferSize_;
	/// Memory buffer with uniform values to be sent to the GPU
	nctl::UniquePtr<GLubyte[]> uniformsHostBuffer_;

	void bind();
	/// Wrapper around `GLShaderUniforms::commitUniforms()`
	inline void commitUniforms() { shaderUniforms_.commitUniforms(); }
	/// Wrapper around `GLShaderUniformBlocks::commitUniformBlocks()`
	inline void commitUniformBlocks() { shaderUniformBlocks_.commitUniformBlocks(); }
	/// Wrapper around `GLShaderProgram::defineVertexFormat()`
	void defineVertexFormat(const GLBufferObject *vbo, const GLBufferObject *ibo, unsigned int vboOffset);
	uint32_t sortKey();

	friend class RenderCommand;
};

}

#endif
