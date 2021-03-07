#ifndef CLASS_NCINE_GLSHADERUNIFORMBLOCKS
#define CLASS_NCINE_GLSHADERUNIFORMBLOCKS

#include <nctl/StaticHashMap.h>
#include <nctl/String.h>
#include "GLUniformBlockCache.h"
#include "RenderBuffersManager.h"

namespace ncine {

class GLShaderProgram;
class GLBufferObject;

/// A class to handle all the uniform blocks of a shader program using a hashmap
class GLShaderUniformBlocks
{
  public:
	GLShaderUniformBlocks();
	explicit GLShaderUniformBlocks(GLShaderProgram *shaderProgram);
	void setProgram(GLShaderProgram *shaderProgram);
	void setUniformsDataPointer(GLubyte *dataPointer);

	GLUniformBlockCache *uniformBlock(const char *name);
	void commitUniformBlocks();

	void bind();

  private:
	GLShaderProgram *shaderProgram_;
	/// Pointer to the data of the first uniform block
	GLubyte *dataPointer_;

	/// Uniform buffer parameters for binding
	RenderBuffersManager::Parameters uboParams_;

	static const int UniformBlockCachesHashSize = 4;
	nctl::StaticHashMap<nctl::String, GLUniformBlockCache, UniformBlockCachesHashSize> uniformBlockCaches_;
	/// A dummy uniform block cache returned when a uniform block is not found in the hashmap
	static GLUniformBlockCache uniformBlockNotFound_;

	void importUniformBlocks();
};

}

#endif
