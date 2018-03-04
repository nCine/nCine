#ifndef CLASS_NCINE_GEOMETRY
#define CLASS_NCINE_GEOMETRY

#include "GLBufferObject.h"
#include "nctl/UniquePtr.h"
#include "RenderBuffersManager.h"

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
	/// Retrieves a pointer that can be used to write vertex data from a custom VBO owned by this object
	GLfloat *acquireVertexPointer(unsigned int numFloats);
	/// Retrieves a pointer that can be used to write vertex data from a VBO owned by the buffers manager
	GLfloat *acquireVertexPointer();
	/// Releases the pointer used to write vertex data
	void releaseVertexPointer();

	/// Shares the VBO of another `Geometry` object
	void shareVbo(const Geometry &geometry);

  private:
	GLenum primitiveType_;
	GLint firstVertex_;
	GLsizei numVertices_;

	nctl::UniquePtr<GLBufferObject> vbo_;
	nctl::UniquePtr<GLBufferObject> ibo_;

	RenderBuffersManager::Parameters vboParams_;
	const RenderBuffersManager::Parameters *sharedVboParams_;

	void bind();
	void draw(GLsizei numInstances);

	/// Deleted copy constructor
	Geometry(const Geometry &) = delete;
	/// Deleted assignment operator
	Geometry &operator=(const Geometry &) = delete;

	friend class RenderCommand;
};

}

#endif
