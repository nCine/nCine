#ifndef CLASS_NCINE_GLSHADERUNIFORMS
#define CLASS_NCINE_GLSHADERUNIFORMS

#include "Array.h"
#include "HashMap.h"
#include "ncString.h"
#include "GLUniformCache.h"

namespace ncine {

class GLShaderProgram;

/// A class to handle all the uniforms of a shader program using a hashmap
class GLShaderUniforms
{
  public:
	GLShaderUniforms();
	explicit GLShaderUniforms(GLShaderProgram *shaderProgram);
	void setProgram(GLShaderProgram *shaderProgram);

	GLUniformCache *uniform(const char *name);
	void commitUniforms();

  private:
	GLShaderProgram *shaderProgram_;

	static const int UniformCachesHashSize = 16;
	StringHashMap<GLUniformCache>::Type uniformCaches_;
	/// A dummy uniform cache returned when a uniform is not found in the hashmap
	static GLUniformCache uniformNotFound_;

	static const unsigned int UniformFloatBufferSize = 128;
	GLfloat uniformsFloatBuffer_[UniformFloatBufferSize];
	static const unsigned int UniformIntBufferSize = 16;
	GLfloat uniformsIntBuffer_[UniformIntBufferSize];

	void importUniforms();
};

}

#endif
