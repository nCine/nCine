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

	const GLfloat *floatVector() const;
	GLfloat floatValue(unsigned int index) const;
	const GLint *intVector() const;
	GLint intValue(unsigned int index) const;

	bool setFloatVector(const GLfloat *vec);
	bool setFloatValue(GLfloat v0);
	bool setFloatValue(GLfloat v0, GLfloat v1);
	bool setFloatValue(GLfloat v0, GLfloat v1, GLfloat v2);
	bool setFloatValue(GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
	bool setIntVector(const GLint *vec);
	bool setIntValue(GLint v0);
	bool setIntValue(GLint v0, GLint v1);
	bool setIntValue(GLint v0, GLint v1, GLint v2);
	bool setIntValue(GLint v0, GLint v1, GLint v2, GLint v3);

	inline bool isDirty() const { return isDirty_; }
	inline void setDirty(bool isDirty) { isDirty_ = isDirty; }
	bool commitValue();

  private:
	const GLUniform *uniform_;
	GLubyte *dataPointer_;
	/// A flag to signal if the uniform needs to be committed
	bool isDirty_;

	bool checkFloat() const;
	bool checkInt() const;
	bool checkComponents(unsigned int requiredComponents) const;
};

}

#endif
