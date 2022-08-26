#ifndef CLASS_NCINE_GLUNIFORMBLOCKCACHE
#define CLASS_NCINE_GLUNIFORMBLOCKCACHE

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"
#include "GLUniformCache.h"
#include <nctl/StaticHashMap.h>
#include <nctl/String.h>

namespace ncine {

class GLUniformBlock;
class GLBufferObject;

/// A class to cache a uniform block buffer and then update it in the shader
class GLUniformBlockCache
{
  public:
	GLUniformBlockCache();
	explicit GLUniformBlockCache(GLUniformBlock *uniformBlock);

	inline const GLUniformBlock *uniformBlock() const { return uniformBlock_; }
	/// Wrapper around `GLUniformBlock::index()`
	GLuint index() const;
	/// Wrapper around `GLUniformBlock::bindingIndex()`
	GLuint bindingIndex() const;
	/// Wrapper around `GLUniformBlock::size()`
	GLint size() const;
	/// Wrapper around `GLUniformBlock::alignAmount()`
	unsigned char alignAmount() const;

	inline GLubyte *dataPointer() { return dataPointer_; }
	inline const GLubyte *dataPointer() const { return dataPointer_; }
	void setDataPointer(GLubyte *dataPointer);

	inline GLint usedSize() const { return usedSize_; }
	void setUsedSize(GLint usedSize);

	bool copyData(unsigned int destIndex, const GLubyte *src, unsigned int numBytes);
	inline bool copyData(const GLubyte *src) { return copyData(0, src, usedSize_); }

	GLUniformCache *uniform(const char *name);
	/// Wrapper around `GLUniformBlock::setBlockBinding()`
	void setBlockBinding(GLuint blockBinding);

  private:
	GLUniformBlock *uniformBlock_;
	GLubyte *dataPointer_;
	/// Keeps tracks of how much of the cache needs to be uploaded to the UBO
	GLint usedSize_;

	static const int UniformHashSize = 8;
	nctl::StaticHashMap<nctl::String, GLUniformCache, UniformHashSize> uniformCaches_;
};

}

#endif
