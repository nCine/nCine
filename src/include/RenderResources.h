#ifndef CLASS_NCINE_RENDERRESOURCES
#define CLASS_NCINE_RENDERRESOURCES

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"

#include <nctl/UniquePtr.h>
#include <nctl/HashMap.h>
#include "Material.h"
#include "Matrix4x4.h"
#include "GLShaderProgram.h" // For the UniquePtr to invoke the destructor
#include "GLShaderUniforms.h"

namespace ncine {

class RenderBuffersManager;
class RenderVaoPool;
class RenderCommandPool;
class RenderBatcher;
class Camera;
class Viewport;

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

	static inline RenderBuffersManager &buffersManager() { return *buffersManager_; }
	static inline RenderVaoPool &vaoPool() { return *vaoPool_; }
	static inline RenderCommandPool &renderCommandPool() { return *renderCommandPool_; }
	static inline RenderBatcher &renderBatcher() { return *renderBatcher_; }

	static GLShaderProgram *shaderProgram(Material::ShaderProgramType shaderProgramType);

	static GLShaderProgram *batchedShader(const GLShaderProgram *shader);
	static bool registerBatchedShader(const GLShaderProgram *shader, ncine::GLShaderProgram *batchedShader);
	static bool unregisterBatchedShader(const GLShaderProgram *shader);

	static inline unsigned char *cameraUniformsBuffer() { return cameraUniformsBuffer_; }
	static CameraUniformData *findCameraUniformData(GLShaderProgram *shaderProgram);
	static void insertCameraUniformData(GLShaderProgram *shaderProgram, CameraUniformData &&cameraUniformData);
	static bool removeCameraUniformData(GLShaderProgram *shaderProgram);

	static inline const Camera *currentCamera() { return currentCamera_; }
	static inline const Viewport *currentViewport() { return currentViewport_; }

	static void setDefaultAttributesParameters(GLShaderProgram &shaderProgram);

  private:
	static nctl::UniquePtr<RenderBuffersManager> buffersManager_;
	static nctl::UniquePtr<RenderVaoPool> vaoPool_;
	static nctl::UniquePtr<RenderCommandPool> renderCommandPool_;
	static nctl::UniquePtr<RenderBatcher> renderBatcher_;

	static nctl::UniquePtr<GLShaderProgram> defaultShaderPrograms_[16];
	static nctl::HashMap<const GLShaderProgram *, GLShaderProgram *> batchedShaders_;

	static const int UniformsBufferSize = 128; // two 4x4 float matrices
	static unsigned char cameraUniformsBuffer_[UniformsBufferSize];
	static nctl::HashMap<GLShaderProgram *, CameraUniformData> cameraUniformDataMap_;

	static Camera *currentCamera_;
	static nctl::UniquePtr<Camera> defaultCamera_;
	static Viewport *currentViewport_;

	static void setCurrentCamera(Camera *camera);
	static void updateCameraUniforms();
	static void setCurrentViewport(Viewport *viewport);

	static void create();
	static void createMinimal();
	static void dispose();

	static void registerDefaultBatchedShaders();

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
