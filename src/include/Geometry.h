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

	/// Returns the primitive type (`GL_TRIANGLES`, `GL_TRIANGLE_STRIP`, ...)
	inline GLenum primitiveType() const { return primitiveType_; }
	/// Returns the index of the first vertex to draw
	inline GLint firstVertex() const { return firstVertex_; }
	/// Returns the number of vertices
	inline GLsizei numVertices() const { return numVertices_; }
	/// Returns the number of float elements that composes the vertex format
	inline unsigned int numElementsPerVertex() const { return numElementsPerVertex_; }

	/// Sets all three drawing parameters
	void setDrawParameters(GLenum primitiveType, GLint firstVertex, GLsizei numVertices);
	/// Sets the primitive type (`GL_TRIANGLES`, `GL_TRIANGLE_STRIP`, ...)
	inline void setPrimitiveType(GLenum primitiveType) { primitiveType_ = primitiveType; }
	/// Sets the index number of the first vertex to draw
	inline void setFirstVertex(GLint firstVertex) { firstVertex_ = firstVertex; }
	/// Sets the number of vertices
	inline void setNumVertices(GLsizei numVertices) { numVertices_ = numVertices; }
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
	void shareVbo(const Geometry *geometry);

	/// Returns the number of indices used to render the geometry
	inline unsigned int numIndices() const { return numIndices_; }
	/// Sets the index number of the first index to draw
	inline void setFirstIndex(GLushort firstIndex) { firstIndex_ = firstIndex; }
	/// Sets the number of indices used to render the geometry
	inline void setNumIndices(unsigned int numIndices) { numIndices_ = numIndices; }
	/// Creates a custom IBO that is unique to this `Geometry` object
	void createCustomIbo(unsigned int numIndices, GLenum usage);
	/// Retrieves a pointer that can be used to write index data from a custom IBO owned by this object
	GLushort *acquireIndexPointer(unsigned int numIndices);
	/// Retrieves a pointer that can be used to write index data from a IBO owned by the buffers manager
	GLushort *acquireIndexPointer();
	/// Releases the pointer used to write index data
	void releaseIndexPointer();

	/// Returns a pointer into host memory containing index data to be copied into a IBO
	inline const GLushort *hostIndexPointer() const { return hostIndexPointer_; }
	/// Sets a pointer into host memory containing index data to be copied into a IBO
	inline void setHostIndexPointer(const GLushort *indexPointer) { hostIndexPointer_ = indexPointer; }

	/// Shares the IBO of another `Geometry` object
	void shareIbo(const Geometry *geometry);

  private:
	GLenum primitiveType_;
	GLint firstVertex_;
	GLsizei numVertices_;
	unsigned int numElementsPerVertex_;
	GLushort firstIndex_;
	unsigned int numIndices_;
	const float *hostVertexPointer_;
	const GLushort *hostIndexPointer_;

	nctl::UniquePtr<GLBufferObject> vbo_;
	RenderBuffersManager::Parameters vboParams_;
	const RenderBuffersManager::Parameters *sharedVboParams_;

	nctl::UniquePtr<GLBufferObject> ibo_;
	RenderBuffersManager::Parameters iboParams_;
	const RenderBuffersManager::Parameters *sharedIboParams_;

	void bind();
	void draw(GLsizei numInstances);
	void commitVertices();
	void commitIndices();

	inline const RenderBuffersManager::Parameters &vboParams() const { return sharedVboParams_ ? *sharedVboParams_ : vboParams_; }
	inline const RenderBuffersManager::Parameters &iboParams() const { return sharedIboParams_ ? *sharedIboParams_ : iboParams_; }

	/// Deleted copy constructor
	Geometry(const Geometry &) = delete;
	/// Deleted assignment operator
	Geometry &operator=(const Geometry &) = delete;

	friend class RenderCommand;
};

}

#endif
