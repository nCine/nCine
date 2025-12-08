#ifndef CLASS_NCINE_RENDERRESOURCES
#define CLASS_NCINE_RENDERRESOURCES

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"

#include <nctl/UniquePtr.h>
#include <nctl/HashMap.h>
#include "GLShaderProgram.h" // For the UniquePtr to invoke the destructor
#include "GLShaderUniforms.h"

#ifdef WITH_SCENEGRAPH
	#include "Material.h"
#endif

namespace ncine {

class BinaryShaderCache;
class RenderBuffersManager;
class RenderVaoPool;
class RenderCommandPool;
class RenderBatcher;
class Hash64;
class Camera;
class Viewport;

/// The class that creates and handles application common OpenGL rendering resources
class RenderResources
{
  public:
#ifdef WITH_SCENEGRAPH
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
#endif

	/// A structure used by the `compileShader()` method to load and compile a shader program
	struct ShaderProgramCompileInfo
	{
		/// A structure containing the shader source or file path, and its hash
		struct ShaderCompileInfo
		{
			ShaderCompileInfo()
			    : shaderString(nullptr), shaderFile(nullptr), hashString(nullptr), hash(0) {}
			explicit ShaderCompileInfo(const char *filename)
			    : shaderString(nullptr), shaderFile(filename), hashString(nullptr), hash(0) {}
			ShaderCompileInfo(const char *source, const char *hash)
			    : shaderString(source), shaderFile(nullptr), hashString(hash), hash(0) {}

			/// The shader source as a string (for default shaders, when `WITH_EMBEDDED_SHADERS` is true)
			const char *shaderString;
			/// The shader file path (for default shaders, when `WITH_EMBEDDED_SHADERS` is false)
			const char *shaderFile;
			/// The hash as a string of characters (MD5 sum, 128-bit number)
			const char *hashString;
			/// The hash as a 64-bit number
			uint64_t hash;
		};

		/// Constructor that initializes the `ShaderCompileInfo` references from internal structures
		explicit ShaderProgramCompileInfo(nctl::UniquePtr<GLShaderProgram> &sp)
		    : shaderProgram(sp), vertexInfo(vertexInfoBlob), fragmentInfo(fragmentInfoBlob),
		      introspection(GLShaderProgram::Introspection::ENABLED), objectLabel(nullptr) {}

		/// Constructor that initializes the `ShaderCompileInfo` references from internal structures
		ShaderProgramCompileInfo(nctl::UniquePtr<GLShaderProgram> &sp,
		                         GLShaderProgram::Introspection introspection, const char *label)
		    : shaderProgram(sp), vertexInfo(vertexInfoBlob), fragmentInfo(fragmentInfoBlob),
		      introspection(introspection), objectLabel(label) {}

		/// Constructor that initializes the `ShaderCompileInfo` references from external structures
		/*! \note Used by `create()` to initialize the `defaultVertexShaderInfos_` and `defaultFragmentShaderInfos_` arrays */
		ShaderProgramCompileInfo(nctl::UniquePtr<GLShaderProgram> &sp,
		                         ShaderCompileInfo &vs, ShaderCompileInfo &fs,
		                         GLShaderProgram::Introspection introspection, const char *label)
		    : shaderProgram(sp), vertexInfo(vs), fragmentInfo(fs),
		      introspection(introspection), objectLabel(label) {}

		nctl::UniquePtr<GLShaderProgram> &shaderProgram;
		/// Internal structure normally pointed by the `vertexInfo` reference
		ShaderCompileInfo vertexInfoBlob;
		/// Internal structure normally pointed by the `fragmentInfo` reference
		ShaderCompileInfo fragmentInfoBlob;
		ShaderCompileInfo &vertexInfo;
		ShaderCompileInfo &fragmentInfo;
		GLShaderProgram::Introspection introspection;
		const char *objectLabel;
	};

#ifdef WITH_SCENEGRAPH
	struct CameraUniformData
	{
		CameraUniformData()
		    : camera(nullptr), updateFrameProjectionMatrix(0), updateFrameViewMatrix(0) {}

		GLShaderUniforms shaderUniforms;
		Camera *camera;
		unsigned long int updateFrameProjectionMatrix;
		unsigned long int updateFrameViewMatrix;
	};

	enum class DefaultVertexShader
	{
		SPRITE,
		SPRITE_NOTEXTURE,
		MESHSPRITE,
		MESHSPRITE_NOTEXTURE,
		TEXTNODE,
		BATCHED_SPRITES,
		BATCHED_SPRITES_NOTEXTURE,
		BATCHED_MESHSPRITES,
		BATCHED_MESHSPRITES_NOTEXTURE,
		BATCHED_TEXTNODES,

		COUNT
	};

	enum class DefaultFragmentShader
	{
		SPRITE,
		SPRITE_GRAY,
		SPRITE_NOTEXTURE,
		TEXTNODE_ALPHA,
		TEXTNODE_RED,

		COUNT
	};
#endif

	/// The name of the directory, inside the data path, that contains the default shader files
	static char const * const ShadersDir;

	static inline BinaryShaderCache &binaryShaderCache() { return *binaryShaderCache_; }
	static inline RenderBuffersManager &buffersManager() { return *buffersManager_; }
	static inline RenderVaoPool &vaoPool() { return *vaoPool_; }
	static inline const Hash64 &hash64() { return *hash64_; }
#ifdef WITH_SCENEGRAPH
	static inline RenderCommandPool &renderCommandPool() { return *renderCommandPool_; }
	static inline RenderBatcher &renderBatcher() { return *renderBatcher_; }
#endif

	/// The function compiles a shader ex-novo or load it from the binary cache
	static void compileShader(ShaderProgramCompileInfo &shaderToCompile);
#ifdef WITH_SCENEGRAPH
	static GLShaderProgram *shaderProgram(Material::ShaderProgramType shaderProgramType);

	/// Retrieves the compilation information for a default vertex shader
	static const ShaderProgramCompileInfo::ShaderCompileInfo &defaultVertexShaderInfo(DefaultVertexShader shader);
	/// Retrieves the compilation information for a default fragment shader
	static const ShaderProgramCompileInfo::ShaderCompileInfo &defaultFragmentShaderInfo(DefaultFragmentShader shader);
	/// Retrieves the compilation information for a default vertex shader (given its enum index)
	static const ShaderProgramCompileInfo::ShaderCompileInfo &defaultVertexShaderInfo(unsigned int index);
	/// Retrieves the compilation information for a default fragment shader (given its enum index)
	static const ShaderProgramCompileInfo::ShaderCompileInfo &defaultFragmentShaderInfo(unsigned int index);

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
#endif

  private:
	static nctl::UniquePtr<BinaryShaderCache> binaryShaderCache_;
	static nctl::UniquePtr<RenderBuffersManager> buffersManager_;
	static nctl::UniquePtr<RenderVaoPool> vaoPool_;
	/// Hashing class used to hash shaders sources and keep their statistics separated
	static nctl::UniquePtr<Hash64> hash64_;
#ifdef WITH_SCENEGRAPH
	static nctl::UniquePtr<RenderCommandPool> renderCommandPool_;
	static nctl::UniquePtr<RenderBatcher> renderBatcher_;

	static const unsigned int NumDefaultVertexShaders = static_cast<unsigned int>(DefaultVertexShader::COUNT);
	static ShaderProgramCompileInfo::ShaderCompileInfo defaultVertexShaderInfos_[NumDefaultVertexShaders];
	static const unsigned int NumDefaultFragmentShaders = static_cast<unsigned int>(DefaultFragmentShader::COUNT);
	static ShaderProgramCompileInfo::ShaderCompileInfo defaultFragmentShaderInfos_[NumDefaultFragmentShaders];

	static const unsigned int NumDefaultShaderPrograms = 18;
	static nctl::UniquePtr<GLShaderProgram> defaultShaderPrograms_[NumDefaultShaderPrograms];
	/// Hash map from a shader program pointer to the pointer of its batched version
	static nctl::HashMap<const GLShaderProgram *, GLShaderProgram *> batchedShaders_;

	static const unsigned int UniformsBufferSize = 128; // two 4x4 float matrices
	static unsigned char cameraUniformsBuffer_[UniformsBufferSize];
	static nctl::HashMap<GLShaderProgram *, CameraUniformData> cameraUniformDataMap_;

	static Camera *currentCamera_;
	static nctl::UniquePtr<Camera> defaultCamera_;
	static Viewport *currentViewport_;

	static void setCurrentCamera(Camera *camera);
	static void updateCameraUniforms();
	static void setCurrentViewport(Viewport *viewport);

	static void fillDefaultShaderInfos();
	static void registerDefaultBatchedShaders();

	static void create();
#endif

	static void createMinimal();
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
	/// The `ScreenViewport` class needs to change the projection of the default camera
	friend class ScreenViewport;
};

}

#endif
