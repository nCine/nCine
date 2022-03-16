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
	GLShaderUniformBlocks(GLShaderProgram *shaderProgram, const char *includeOnly, const char *exclude);
	inline void setProgram(GLShaderProgram *shaderProgram) { setProgram(shaderProgram, nullptr, nullptr); }
	void setProgram(GLShaderProgram *shaderProgram, const char *includeOnly, const char *exclude);
	void setUniformsDataPointer(GLubyte *dataPointer);

	inline unsigned int numUniformBlocks() const { return uniformBlockCaches_.size(); }
	inline bool hasUniformBlock(const char *name) const { return (uniformBlockCaches_.find(name) != nullptr); }
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

	/// Imports the uniform blocks with the option of including only some or excluing others
	void importUniformBlocks(const char *includeOnly, const char *exclude);
};

}

#endif
