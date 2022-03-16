#ifndef CLASS_NCINE_GLUNIFORMBLOCK
#define CLASS_NCINE_GLUNIFORMBLOCK

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"
#include "GLUniform.h"
#include <nctl/StaticHashMap.h>
#include <nctl/String.h>

namespace ncine {

class GLShaderProgram;

/// A class to store information about an OpenGL uniform block
class GLUniformBlock
{
  public:
	enum class DiscoverUniforms
	{
		ENABLED,
		DISABLED
	};

	GLUniformBlock();
	GLUniformBlock(GLuint program, GLuint blockIndex, DiscoverUniforms discover);
	GLUniformBlock(GLuint program, GLuint blockIndex);

	inline GLuint index() const { return index_; }
	inline GLint bindingIndex() const { return bindingIndex_; }
	inline GLint size() const { return size_; }
	inline const char *name() const { return name_; }

	inline GLUniform *uniform(const char *name) { return blockUniforms_.find(name); }
	void setBlockBinding(GLuint blockBinding);

  private:
	/// Max number of discoverable uniforms per block
	static const int MaxNumBlockUniforms = 16;

	static const int BlockUniformHashSize = 8;
	nctl::StaticHashMap<nctl::String, GLUniform, BlockUniformHashSize> blockUniforms_;

	GLuint program_;
	GLuint index_;
	GLint size_;
	/// Current binding index for the uniform block. Negative if not bound.
	GLint bindingIndex_;
	static const int MaxNameLength = 32;
	char name_[MaxNameLength];

	friend class GLUniformBlockCache;
};

}

#endif
