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
	/// Returns the number of float elements that composes the vertex format
	inline unsigned int numElementsPerVertex() const { return numElementsPerVertex_; }

	/// Sets all the drawing parameters
	void setDrawParameters(GLenum primitiveType, GLint firstVertex, GLsizei numVertices);
	/// Sets the number of float elements that composes the vertex format
	inline void setNumElementsPerVertex(unsigned int numElements) { numElementsPerVertex_ = numElements; }
	/// Creates a custom VBO that is unique to this `Geometry` object
	void createCustomVbo(unsigned int numFloats, GLenum usage);
	/// Retrieves a pointer that can be used to write vertex data from a custom VBO owned by this object
	/*! This overloaded version allows a custom alignment specification */
	GLfloat *acquireVertexPointer(unsigned int numFloats, unsigned int numFloatsAlignment);
	/// Retrieves a pointer that can be used to write vertex data from a custom VBO owned by this object
	inline GLfloat *acquireVertexPointer(unsigned int numFloats) { return acquireVertexPointer(numFloats, 1); }
	/// Retrieves a pointer that can be used to write vertex data from a VBO owned by the buffers manager
	GLfloat *acquireVertexPointer();
	/// Releases the pointer used to write vertex data
	void releaseVertexPointer();

	/// Returns a pointer into host memory containing vertex data to be copied into a VBO
	inline const float *hostVertexPointer() const { return hostVertexPointer_; }
	/// Sets a pointer into host memory containing vertex data to be copied into a VBO
	inline void setHostVertexPointer(const float *vertexPointer) { hostVertexPointer_ = vertexPointer; }

	/// Shares the VBO of another `Geometry` object
	void shareVbo(const Geometry &geometry);

  private:
	GLenum primitiveType_;
	GLint firstVertex_;
	GLsizei numVertices_;
	unsigned int numElementsPerVertex_;
	const float *hostVertexPointer_;

	nctl::UniquePtr<GLBufferObject> vbo_;
	nctl::UniquePtr<GLBufferObject> ibo_;

	RenderBuffersManager::Parameters vboParams_;
	const RenderBuffersManager::Parameters *sharedVboParams_;

	void bind();
	void draw(GLsizei numInstances);
	void commitVertices();

	/// Deleted copy constructor
	Geometry(const Geometry &) = delete;
	/// Deleted assignment operator
	Geometry &operator=(const Geometry &) = delete;

	friend class RenderCommand;
};

}

#endif
