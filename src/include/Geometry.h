#ifndef CLASS_NCINE_GEOMETRY
#define CLASS_NCINE_GEOMETRY

#include "GLBufferObject.h"
#include "Array.h"

namespace ncine {

/// The class containing geometric data for a drawable node
class Geometry
{
  public:
	/// Default constructor
	Geometry();
	~Geometry();

	/// Returns the primitive type (`GL_TRIANGLE`, `GL_LINES`, ...)
	inline GLenum primitiveType() const { return primitiveType_; }
	/// Returns the first vertex to draw from the arrays
	inline GLint firstVertex() const { return firstVertex_; }
	/// Returns the number of vertices
	inline GLsizei numVertices() const { return numVertices_; }

	/// Sets all the drawing parameters
	void setDrawParameters(GLenum primitiveType, GLint firstVertex, GLsizei numVertices);
	/// Creates a custom VBO that is unique to this `Geometry` object
	void createCustomVbo(unsigned int numFloats, GLenum usage);
	/// Creates a custom VBO that is unique to this object
	void updateVboData(unsigned int floatOffset, unsigned int floatSize, const GLfloat *data);

	/// Shares the VBO of another `Geometry` object
	void shareVbo(const Geometry &geometry);
	/// Makes this `Geometry` object use the common quad VBO from `RenderResources`
	void makeSharedQuad();

  private:
	/// Sharing type for the buffers
	enum class SharingType
	{
		/// It must not be modified, the buffer is a resource shared across the application
		COMMON_RESOURCE,
		/// It can be modified, the buffer is shared between more objects
		SHARED,
		/// It can be modified, the buffer belongs only to this object
		/*! In this case the destructor needs to delete the buffer. */
		UNIQUE
	};

	/// The sharing type for the vertex buffer object
	SharingType vboSharingType_;

	GLenum primitiveType_;
	GLint firstVertex_;
	GLsizei numVertices_;

	GLBufferObject *vbo_;
	GLBufferObject *ibo_;

	void bind();

	/// Deleted copy constructor
	Geometry(const Geometry &) = delete;
	/// Deleted assignment operator
	Geometry &operator=(const Geometry &) = delete;

	friend class RenderCommand;
};

}

#endif
