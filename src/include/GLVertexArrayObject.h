#ifndef CLASS_NCINE_GLVERTEXARRAYOBJECT
#define CLASS_NCINE_GLVERTEXARRAYOBJECT

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"
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
	bool unbind() const;

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
