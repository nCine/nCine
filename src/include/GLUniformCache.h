#ifndef CLASS_NCINE_GLUNIFORMCACHE
#define CLASS_NCINE_GLUNIFORMCACHE

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"

namespace ncine {

class GLUniform;

/// A class to cache a uniform value and then update it in the shader
class GLUniformCache
{
  public:
	GLUniformCache();
	explicit GLUniformCache(const GLUniform *uniform);

	inline const GLUniform *uniform() const { return uniform_; }
	inline const GLubyte *dataPointer() const { return dataPointer_; }
	inline void setDataPointer(GLubyte *dataPointer) { dataPointer_ = dataPointer; }

	void setFloatVector(const GLfloat *vec);
	void setFloatValue(GLfloat v0);
	void setFloatValue(GLfloat v0, GLfloat v1);
	void setFloatValue(GLfloat v0, GLfloat v1, GLfloat v2);
	void setFloatValue(GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
	void setIntVector(const GLint *vec);
	void setIntValue(GLint v0);
	void setIntValue(GLint v0, GLint v1);
	void setIntValue(GLint v0, GLint v1, GLint v2);
	void setIntValue(GLint v0, GLint v1, GLint v2, GLint v3);

	bool isDirty() const { return isDirty_; }
	void commitValue();

  private:
	const GLUniform *uniform_;
	GLubyte *dataPointer_;
	/// A flag to signal if the uniform needs to be committed
	bool isDirty_;

	void checkFloat();
	void checkInt();
	void checkComponents(unsigned int requiredComponents);
};

}

#endif
