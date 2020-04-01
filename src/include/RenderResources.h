#ifndef CLASS_NCINE_RENDERRESOURCES
#define CLASS_NCINE_RENDERRESOURCES

#include "GLBufferObject.h"
#include "GLShaderProgram.h"
#include "RenderBuffersManager.h"
#include "RenderVaoPool.h"
#include <nctl/UniquePtr.h>
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

	/// A vertex format structure for vertices with positions, texture coordinates and draw indices
	struct VertexFormatPos2Tex2Index
	{
		GLfloat position[2];
		GLfloat texcoords[2];
		int drawindex;
	};

	static inline RenderBuffersManager &buffersManager() { return *buffersManager_; }
	static inline RenderVaoPool &vaoPool() { return *vaoPool_; }
	static inline GLShaderProgram *spriteShaderProgram() { return spriteShaderProgram_.get(); }
	static inline GLShaderProgram *spriteGrayShaderProgram() { return spriteGrayShaderProgram_.get(); }
	static inline GLShaderProgram *meshSpriteShaderProgram() { return meshSpriteShaderProgram_.get(); }
	static inline GLShaderProgram *meshSpriteGrayShaderProgram() { return meshSpriteGrayShaderProgram_.get(); }
	static inline GLShaderProgram *textnodeAlphaShaderProgram() { return textnodeAlphaShaderProgram_.get(); }
	static inline GLShaderProgram *textnodeRedShaderProgram() { return textnodeRedShaderProgram_.get(); }
	static inline GLShaderProgram *batchedSpritesShaderProgram() { return batchedSpritesShaderProgram_.get(); }
	static inline GLShaderProgram *batchedSpritesGrayShaderProgram() { return batchedSpritesGrayShaderProgram_.get(); }
	static inline GLShaderProgram *batchedMeshSpritesShaderProgram() { return batchedMeshSpritesShaderProgram_.get(); }
	static inline GLShaderProgram *batchedMeshSpritesGrayShaderProgram() { return batchedMeshSpritesGrayShaderProgram_.get(); }
	static inline GLShaderProgram *batchedTextnodesAlphaShaderProgram() { return batchedTextnodesAlphaShaderProgram_.get(); }
	static inline GLShaderProgram *batchedTextnodesRedShaderProgram() { return batchedTextnodesRedShaderProgram_.get(); }
	static inline const Matrix4x4f &projectionMatrix() { return projectionMatrix_; }
	static inline bool hasProjectionChanged(bool batchingEnabled) { return (batchingEnabled) ? projectionHasChangedBatching_ : projectionHasChanged_; }
	static void clearDirtyProjectionFlag(bool batchingEnabled);

	static void createMinimal();

  private:
	static nctl::UniquePtr<RenderBuffersManager> buffersManager_;
	static nctl::UniquePtr<RenderVaoPool> vaoPool_;
	static nctl::UniquePtr<GLShaderProgram> spriteShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> spriteGrayShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> meshSpriteShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> meshSpriteGrayShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> textnodeAlphaShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> textnodeRedShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> batchedSpritesShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> batchedSpritesGrayShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> batchedMeshSpritesShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> batchedMeshSpritesGrayShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> batchedTextnodesAlphaShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> batchedTextnodesRedShaderProgram_;

	static Matrix4x4f projectionMatrix_;
	static bool projectionHasChanged_;
	static bool projectionHasChangedBatching_;

	static void setProjectionMatrix(const Matrix4x4f &projectionMatrix);
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
	/// The `IGfxDevice` class needs to update the projection matrix
	friend class IGfxDevice;
};

}

#endif
