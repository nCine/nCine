#ifndef CLASS_NCINE_RENDERRESOURCES
#define CLASS_NCINE_RENDERRESOURCES

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"

#include <nctl/UniquePtr.h>
#include <nctl/HashMap.h>
#include "Matrix4x4.h"
#include "GLShaderUniforms.h"

namespace ncine {

class RenderBuffersManager;
class RenderVaoPool;
class RenderCommandPool;
class RenderBatcher;
class GLShaderProgram;
class Camera;

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

	/// A vertex format structure for vertices with positions and draw indices
	struct VertexFormatPos2Index
	{
		GLfloat position[2];
		int drawindex;
	};

	/// A vertex format structure for vertices with positions, texture coordinates and draw indices
	struct VertexFormatPos2Tex2Index
	{
		GLfloat position[2];
		GLfloat texcoords[2];
		int drawindex;
	};

	struct CameraUniformData
	{
		CameraUniformData()
		    : camera(nullptr), updateFrameProjectionMatrix(0), updateFrameViewMatrix(0) {}

		GLShaderUniforms shaderUniforms;
		Camera *camera;
		unsigned long int updateFrameProjectionMatrix;
		unsigned long int updateFrameViewMatrix;
	};

	static const char *ProjectionMatrixUniformName;
	static const char *ViewMatrixUniformName;
	static const char *ProjectionViewMatrixExcludeString;

	static inline RenderBuffersManager &buffersManager() { return *buffersManager_; }
	static inline RenderVaoPool &vaoPool() { return *vaoPool_; }
	static inline RenderCommandPool &renderCommandPool() { return *renderCommandPool_; }
	static inline RenderBatcher &renderBatcher() { return *renderBatcher_; }

	static inline GLShaderProgram *spriteShaderProgram() { return spriteShaderProgram_.get(); }
	static inline GLShaderProgram *spriteGrayShaderProgram() { return spriteGrayShaderProgram_.get(); }
	static inline GLShaderProgram *spriteNoTextureShaderProgram() { return spriteNoTextureShaderProgram_.get(); }
	static inline GLShaderProgram *meshSpriteShaderProgram() { return meshSpriteShaderProgram_.get(); }
	static inline GLShaderProgram *meshSpriteGrayShaderProgram() { return meshSpriteGrayShaderProgram_.get(); }
	static inline GLShaderProgram *meshSpriteNoTextureShaderProgram() { return meshSpriteNoTextureShaderProgram_.get(); }
	static inline GLShaderProgram *textnodeAlphaShaderProgram() { return textnodeAlphaShaderProgram_.get(); }
	static inline GLShaderProgram *textnodeRedShaderProgram() { return textnodeRedShaderProgram_.get(); }
	static inline GLShaderProgram *batchedSpritesShaderProgram() { return batchedSpritesShaderProgram_.get(); }
	static inline GLShaderProgram *batchedSpritesGrayShaderProgram() { return batchedSpritesGrayShaderProgram_.get(); }
	static inline GLShaderProgram *batchedSpritesNoTextureShaderProgram() { return batchedSpritesNoTextureShaderProgram_.get(); }
	static inline GLShaderProgram *batchedMeshSpritesShaderProgram() { return batchedMeshSpritesShaderProgram_.get(); }
	static inline GLShaderProgram *batchedMeshSpritesGrayShaderProgram() { return batchedMeshSpritesGrayShaderProgram_.get(); }
	static inline GLShaderProgram *batchedMeshSpritesNoTextureShaderProgram() { return batchedMeshSpritesNoTextureShaderProgram_.get(); }
	static inline GLShaderProgram *batchedTextnodesAlphaShaderProgram() { return batchedTextnodesAlphaShaderProgram_.get(); }
	static inline GLShaderProgram *batchedTextnodesRedShaderProgram() { return batchedTextnodesRedShaderProgram_.get(); }

	static inline unsigned char *cameraUniformsBuffer() { return cameraUniformsBuffer_; }
	static inline nctl::HashMap<GLShaderProgram *, CameraUniformData> &cameraUniformDataMap() { return cameraUniformDataMap_; }
	static inline const Camera *currentCamera() { return currentCamera_; }

	static void createMinimal();

  private:
	static nctl::UniquePtr<RenderBuffersManager> buffersManager_;
	static nctl::UniquePtr<RenderVaoPool> vaoPool_;
	static nctl::UniquePtr<RenderCommandPool> renderCommandPool_;
	static nctl::UniquePtr<RenderBatcher> renderBatcher_;

	static nctl::UniquePtr<GLShaderProgram> spriteShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> spriteGrayShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> spriteNoTextureShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> meshSpriteShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> meshSpriteGrayShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> meshSpriteNoTextureShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> textnodeAlphaShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> textnodeRedShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> batchedSpritesShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> batchedSpritesGrayShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> batchedSpritesNoTextureShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> batchedMeshSpritesShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> batchedMeshSpritesGrayShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> batchedMeshSpritesNoTextureShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> batchedTextnodesAlphaShaderProgram_;
	static nctl::UniquePtr<GLShaderProgram> batchedTextnodesRedShaderProgram_;

	static const int UniformsBufferSize = 128; // two 4x4 float matrices
	static unsigned char cameraUniformsBuffer_[UniformsBufferSize];
	static nctl::HashMap<GLShaderProgram *, CameraUniformData> cameraUniformDataMap_;

	static Camera *currentCamera_;
	static nctl::UniquePtr<Camera> defaultCamera_;

	static void setCamera(Camera *camera);
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
	/// The `Viewport` class needs to set the current camera
	friend class Viewport;
};

}

#endif
