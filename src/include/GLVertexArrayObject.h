#ifndef CLASS_NCINE_GLVERTEXARRAYOBJECT
#define CLASS_NCINE_GLVERTEXARRAYOBJECT

#include "GLHashMap.h"

namespace ncine {

/// A class to handle OpenGL Vertex Array Object
class GLVertexArrayObject
{
  public:
	GLVertexArrayObject();
	~GLVertexArrayObject();

	inline GLuint glHandle() const { return glHandle_; }

	bool bind() const;
	static bool unbind();

	void setObjectLabel(const char *label);

  private:
	static GLuint boundVAO_;

	GLuint glHandle_;

	/// Deleted copy constructor
	GLVertexArrayObject(const GLVertexArrayObject &) = delete;
	/// Deleted assignment operator
	GLVertexArrayObject &operator=(const GLVertexArrayObject &) = delete;
};

}

#endif
