#ifndef CLASS_NCINE_RENDERRESOURCES
#define CLASS_NCINE_RENDERRESOURCES

#include "GLBufferObject.h"
#include "GLShaderProgram.h"
#include "GLVertexArrayObject.h"
#include "RenderBuffersManager.h"
#include "nctl/StaticArray.h"
#include "nctl/UniquePtr.h"
#include "Matrix4x4.h"

namespace ncine {

/// The class that creates and handles application common OpenGL rendering resources
class RenderResources
{
  public:
	/// A vertex format structure for vertices with positions only
	struct VertexFormatPos2
	{
		GLfloat position[2];
	};

	/// A vertex format structure for vertices with positions and texture coordinates
	struct VertexFormatPos2Tex2
	{
		GLfloat position[2];
		GLfloat texcoords[2];
	};

	static inline const GLVertexArrayObject *vao() { return vao_.get(); }
	static inline GLShaderProgram *spriteShaderProgram() { return spriteShaderProgram_.get(); }
	static inline GLShaderProgram *textnodeGrayShaderProgram() { return textnodeGrayShaderProgram_.get(); }
	static inline GLShaderProgram *textnodeColorShaderProgram() { return textnodeColorShaderProgram_.get(); }
	static inline GLShaderProgram *colorShaderProgram() { return colorShaderProgram_.get(); }
	static inline GLShaderProgram *instancedSpritesShaderProgram() { return instancedSpritesShaderProgram_.get(); }
	static RenderBuffersManager &buffersManager() { return *buffersManager_; }
	static const Matrix4x4f &projectionMatrix() { return projectionMatrix_; }

  private:
	static nctl::UniquePtr<GLVertexArrayObject> vao_;
	static nctl::UniquePtr<RenderBuffersManager> buffersManager_;
	static nctl::UniquePtr<GLShaderProgram> spriteShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> textnodeGrayShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> textnodeColorShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> colorShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> instancedSpritesShaderProgram_;

	static Matrix4x4f projectionMatrix_;

	static void create();
	static void dispose();

	/// Static class, deleted constructor
	RenderResources() = delete;
	/// Static class, deleted copy constructor
	RenderResources(const RenderResources &other) = delete;
	/// Static class, deleted assignement operator
	RenderResources &operator=(const RenderResources &other) = delete;

	/// The `Application` class needs to create and dispose the resources
	friend class Application;
};

}

#endif
