#ifndef CLASS_NCINE_GEOMETRY
#define CLASS_NCINE_GEOMETRY

#include "GLBufferObject.h"
#include "Array.h"

namespace ncine {

/// The class containing geometric data for a drawable node
class Geometry
{
  public:
	// Empty constructor
	Geometry();
	~Geometry();

	/// Returns the drawing type (GL_TRIANGLE, GL_LINES, ...)
	inline GLenum drawType() const { return drawType_; }
	/// Returns the first vertex to draw from the arrays
	inline GLint firstVertex() const { return firstVertex_; }
	/// Returns the number of vertices
	inline GLsizei numVertices() const { return numVertices_; }

	void setDrawParameters(GLenum drawType, GLint firstVertex, GLsizei numVertices);
	void createCustomVbo(unsigned int numFloats, GLenum usage);
	void updateVboData(unsigned int floatOffset, unsigned int floatSize, const GLfloat *data);

	void shareVbo(const Geometry& geometry);
	void makeSharedQuad();

  private:
	/// Sharing type for the buffers
	enum SHARING_TYPE
	{
		/// It must not be modified, the buffer is a resource shared across the application
		COMMON_RESOURCE,
		/// It can be modified, the buffer is shared between more objects
		SHARED,
		/// It can be modified, the buffer belongs only to this object
		/*! In this case the destructor needs to delete the buffer */
		UNIQUE
	};

	/// The sharing type for the vertex buffer object
	SHARING_TYPE vboSharingType_;

	GLenum drawType_;
	GLint firstVertex_;
	GLsizei numVertices_;

	GLBufferObject *vbo_;
	GLBufferObject *ibo_;

	void bind();
	// TODO: Handle multiple VBOs?
	GLuint vboHandle();

	/// Private copy constructor
	Geometry(const Geometry&);
	/// Private assignment operator
	Geometry& operator=(const Geometry&);

	friend class RenderCommand;
};

}

#endif
