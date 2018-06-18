#ifndef CLASS_NCINE_GLSHADERUNIFORMS
#define CLASS_NCINE_GLSHADERUNIFORMS

#include "nctl/HashMap.h"
#include "nctl/String.h"
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
	void setUniformsDataPointer(GLubyte *dataPointer);

	GLUniformCache *uniform(const char *name);
	void commitUniforms();

  private:
	GLShaderProgram *shaderProgram_;

	static const int UniformCachesHashSize = 16;
	nctl::StringHashMap<GLUniformCache> uniformCaches_;
	/// A dummy uniform cache returned when a uniform is not found in the hashmap
	static GLUniformCache uniformNotFound_;

	void importUniforms();
};

}

#endif
