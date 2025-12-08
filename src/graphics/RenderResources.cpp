#include <cstddef> // for `offsetof()`
#include <nctl/HashMapIterator.h>
#include <nctl/StaticString.h>
#include "RenderResources.h"
#include "BinaryShaderCache.h"
#include "RenderBuffersManager.h"
#include "RenderVaoPool.h"
#include "Application.h"
#include "Hash64.h"
#include "tracy.h"

#ifdef WITH_SCENEGRAPH
	#include "RenderCommandPool.h"
	#include "RenderBatcher.h"
	#include "Camera.h"
#endif

#ifdef WITH_EMBEDDED_SHADERS
	#include "shader_strings.h"
#else
	#include "FileSystem.h" // for dataPath()
#endif

namespace ncine {

namespace {
	char const * const BatchSizeFormatString = "#ifndef BATCH_SIZE\n\t#define BATCH_SIZE (%d)\n#endif\n#line 0\n";
}

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

char const * const RenderResources::ShadersDir = "shaders";

nctl::UniquePtr<BinaryShaderCache> RenderResources::binaryShaderCache_;
nctl::UniquePtr<RenderBuffersManager> RenderResources::buffersManager_;
nctl::UniquePtr<RenderVaoPool> RenderResources::vaoPool_;
nctl::UniquePtr<Hash64> RenderResources::hash64_;
#ifdef WITH_SCENEGRAPH
nctl::UniquePtr<RenderCommandPool> RenderResources::renderCommandPool_;
nctl::UniquePtr<RenderBatcher> RenderResources::renderBatcher_;

RenderResources::ShaderProgramCompileInfo::ShaderCompileInfo RenderResources::defaultVertexShaderInfos_[NumDefaultVertexShaders];
RenderResources::ShaderProgramCompileInfo::ShaderCompileInfo RenderResources::defaultFragmentShaderInfos_[NumDefaultFragmentShaders];
nctl::UniquePtr<GLShaderProgram> RenderResources::defaultShaderPrograms_[NumDefaultShaderPrograms];
nctl::HashMap<const GLShaderProgram *, GLShaderProgram *> RenderResources::batchedShaders_(32);

unsigned char RenderResources::cameraUniformsBuffer_[UniformsBufferSize];
nctl::HashMap<GLShaderProgram *, RenderResources::CameraUniformData> RenderResources::cameraUniformDataMap_(32);

Camera *RenderResources::currentCamera_ = nullptr;
nctl::UniquePtr<Camera> RenderResources::defaultCamera_;
Viewport *RenderResources::currentViewport_ = nullptr;
#endif

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/*! \note If not initially provided, the new computed hashes will be written back in the `ShaderCompileInfo` structures */
void RenderResources::compileShader(ShaderProgramCompileInfo &shaderToCompile)
{
	ZoneScoped;
	if (shaderToCompile.objectLabel)
	{
		// When Tracy is disabled the statement body is empty and braces are needed
		ZoneText(shaderToCompile.objectLabel, nctl::strnlen(shaderToCompile.objectLabel, nctl::String::MaxCStringLength));
	}

	if (binaryShaderCache_->isEnabled())
	{
		// Either there is a shader string or a shader file, never together
		FATAL_ASSERT((shaderToCompile.vertexInfo.shaderString != nullptr && shaderToCompile.vertexInfo.shaderFile == nullptr) ||
		             (shaderToCompile.vertexInfo.shaderString == nullptr && shaderToCompile.vertexInfo.shaderFile != nullptr));
		FATAL_ASSERT((shaderToCompile.fragmentInfo.shaderString != nullptr && shaderToCompile.fragmentInfo.shaderFile == nullptr) ||
		             (shaderToCompile.fragmentInfo.shaderString == nullptr && shaderToCompile.fragmentInfo.shaderFile != nullptr));
		// Either the shader is loaded from a file or a hash is provided
		FATAL_ASSERT(shaderToCompile.vertexInfo.shaderFile != nullptr || (shaderToCompile.vertexInfo.hash != 0 || shaderToCompile.vertexInfo.hashString != nullptr));
		FATAL_ASSERT(shaderToCompile.fragmentInfo.shaderFile != nullptr || (shaderToCompile.fragmentInfo.hash != 0 || shaderToCompile.fragmentInfo.hashString != nullptr));

		// Calculating a hash only if it has not been provided
		if (shaderToCompile.vertexInfo.hash == 0)
		{
			if (shaderToCompile.vertexInfo.shaderFile)
				shaderToCompile.vertexInfo.hash = hash64_->hashFileStat(shaderToCompile.vertexInfo.shaderFile);
			else if (shaderToCompile.vertexInfo.shaderString && shaderToCompile.vertexInfo.hashString)
				shaderToCompile.vertexInfo.hash = hash64_->scanHashString(shaderToCompile.vertexInfo.hashString);
		}

		// Calculating a hash only if it has not been provided
		if (shaderToCompile.fragmentInfo.hash == 0)
		{
			if (shaderToCompile.fragmentInfo.shaderFile)
				shaderToCompile.fragmentInfo.hash = hash64_->hashFileStat(shaderToCompile.fragmentInfo.shaderFile);
			else if (shaderToCompile.fragmentInfo.shaderString && shaderToCompile.fragmentInfo.hashString)
				shaderToCompile.fragmentInfo.hash = hash64_->scanHashString(shaderToCompile.fragmentInfo.hashString);
		}
	}

	// A similar sum is performd in `GLShaderProgram::link()` for attached shaders
	const uint64_t shaderHashSum = binaryShaderCache_->isEnabled() ? shaderToCompile.vertexInfo.hash + shaderToCompile.fragmentInfo.hash : 0;

	const AppConfiguration &appCfg = theApplication().appConfiguration();
	const GLShaderProgram::QueryPhase cfgQueryPhase = appCfg.deferShaderQueries ? GLShaderProgram::QueryPhase::DEFERRED : GLShaderProgram::QueryPhase::IMMEDIATE;

	BinaryShaderCache::ShaderInfo shaderInfo;
	const bool isCached = binaryShaderCache_->retrieveShaderInfo(shaderHashSum, shaderInfo);
	if (isCached)
	{
		shaderToCompile.shaderProgram->reset(cfgQueryPhase);
		const bool hasLoaded = shaderToCompile.shaderProgram->initFromBinary(shaderInfo.binaryFilename.data(), shaderToCompile.introspection);
		ASSERT(hasLoaded);
		shaderToCompile.shaderProgram->setObjectLabel(shaderInfo.objectLabel.data());
		return;
	}

	// ----------------------------------------------------------------------------------------------
	// ------- The code executes beyond this point only if the shader has not been cached yet -------
	// ----------------------------------------------------------------------------------------------

	const IGfxCapabilities &gfxCaps = theServiceLocator().gfxCapabilities();
	// Clamped between 16 KB and 64 KB
	const int maxUniformBlockSize = gfxCaps.value(IGfxCapabilities::GLIntValues::MAX_UNIFORM_BLOCK_SIZE);

	// If the UBO is smaller than 64 KB, then batched shaders need to be compiled twice. The first time determines the `BATCH_SIZE` define value.
	const bool compileTwice = (maxUniformBlockSize < 64 * 1024 &&
	                           shaderToCompile.introspection == GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS &&
#if defined(__EMSCRIPTEN__) || defined(WITH_ANGLE)
	                           theApplication().appConfiguration().fixedBatchSize == 0 &&
#endif
	                           theApplication().appConfiguration().compileBatchedShadersTwice);

	// The first compilation of a batched shader that needs a double compilation should be queried immediately
	const GLShaderProgram::QueryPhase queryPhase = compileTwice ? GLShaderProgram::QueryPhase::IMMEDIATE : cfgQueryPhase;
	shaderToCompile.shaderProgram->reset(queryPhase);

	nctl::StaticString<64> sourceString;
	const char *vertexStrings[3] = { nullptr, nullptr, nullptr };
	if (compileTwice)
	{
		// The first compilation of a batched shader needs a `BATCH_SIZE` defined as 1
		sourceString.format(BatchSizeFormatString, 1);
		vertexStrings[0] = sourceString.data();
	}

	bool vertexHasLoaded = false;
	bool fragmentHasLoaded = false;

	// Overriding all hash calculation performed in the `GLShader` class by the `attachShader*` wrapper methods
	if (shaderToCompile.vertexInfo.shaderString)
	{
		// The vertex shader source string can be either the first one or the second one, if the first defines the `BATCH_SIZE`
		vertexStrings[compileTwice ? 1 : 0] = shaderToCompile.vertexInfo.shaderString;
		vertexHasLoaded = shaderToCompile.shaderProgram->attachShaderFromStrings(GL_VERTEX_SHADER, vertexStrings, shaderToCompile.vertexInfo.hash);
	}
	else if (shaderToCompile.vertexInfo.shaderFile)
		vertexHasLoaded = shaderToCompile.shaderProgram->attachShaderFromStringsAndFile(GL_VERTEX_SHADER, vertexStrings, shaderToCompile.vertexInfo.shaderFile, shaderToCompile.vertexInfo.hash);

	if (shaderToCompile.fragmentInfo.shaderString)
		fragmentHasLoaded = shaderToCompile.shaderProgram->attachShaderFromString(GL_FRAGMENT_SHADER, shaderToCompile.fragmentInfo.shaderString, shaderToCompile.fragmentInfo.hash);
	else if (shaderToCompile.fragmentInfo.shaderFile)
		fragmentHasLoaded = shaderToCompile.shaderProgram->attachShaderFromFile(GL_FRAGMENT_SHADER, shaderToCompile.fragmentInfo.shaderFile, shaderToCompile.fragmentInfo.hash);

	ASSERT(vertexHasLoaded == true);
	ASSERT(fragmentHasLoaded == true);

	shaderToCompile.shaderProgram->setObjectLabel(shaderToCompile.objectLabel);
	// The first compilation of a batched shader does not need to be saved as a binary because
	// the `shaderInfo.txt` file will save the max batch value for the next application run
	const bool cacheWasEnabled = binaryShaderCache_->isEnabled();
	binaryShaderCache_->setEnabled(cacheWasEnabled && !compileTwice);
	// The first compilation of a batched shader needs the introspection
	const bool programHasLinked = shaderToCompile.shaderProgram->link(compileTwice ? GLShaderProgram::Introspection::ENABLED : shaderToCompile.introspection);
	FATAL_ASSERT_MSG_X(programHasLinked, "Failed to compile shader program \"%s\"", shaderToCompile.objectLabel);
	binaryShaderCache_->setEnabled(cacheWasEnabled);

	unsigned int maxBatchSize = 0; // default value for non batched shaders
#ifdef WITH_SCENEGRAPH
	if (compileTwice)
	{
		ZoneScopedN("compileTwice");

		GLShaderUniformBlocks blocks(shaderToCompile.shaderProgram.get(), Material::InstancesBlockName, nullptr);
		GLUniformBlockCache *block = blocks.uniformBlock(Material::InstancesBlockName);
		ASSERT(block != nullptr);
		if (block)
		{
			// The align amount should not be subtracted from the block total size
			maxBatchSize = maxUniformBlockSize / block->size();
			LOGD_X("Shader \"%s\" - block size: %d bytes (%d align), max batch size: %u", shaderToCompile.objectLabel, block->size(), block->alignAmount(), maxBatchSize);

			shaderToCompile.shaderProgram->reset(cfgQueryPhase);
			sourceString.format(BatchSizeFormatString, maxBatchSize);

			// Overriding all hash calculation performed in the `GLShader` class by the `attachShader*` wrapper methods.
			// Even if its source is going to be patched, shader hash will not change and it will be correctly loaded as binary on the next run.
			bool finalVertexHasLoaded = false;
			bool finalFragmentHasLoaded = false;
			if (shaderToCompile.vertexInfo.shaderString)
				finalVertexHasLoaded = shaderToCompile.shaderProgram->attachShaderFromStrings(GL_VERTEX_SHADER, vertexStrings, shaderToCompile.vertexInfo.hash);
			else if (shaderToCompile.vertexInfo.shaderFile)
				finalVertexHasLoaded = shaderToCompile.shaderProgram->attachShaderFromStringsAndFile(GL_VERTEX_SHADER, vertexStrings, shaderToCompile.vertexInfo.shaderFile, shaderToCompile.vertexInfo.hash);

			if (shaderToCompile.fragmentInfo.shaderString)
				finalFragmentHasLoaded = shaderToCompile.shaderProgram->attachShaderFromString(GL_FRAGMENT_SHADER, shaderToCompile.fragmentInfo.shaderString, shaderToCompile.fragmentInfo.hash);
			else if (shaderToCompile.fragmentInfo.shaderFile)
				finalFragmentHasLoaded = shaderToCompile.shaderProgram->attachShaderFromFile(GL_FRAGMENT_SHADER, shaderToCompile.fragmentInfo.shaderFile, shaderToCompile.fragmentInfo.hash);

			ASSERT(finalVertexHasLoaded == true);
			ASSERT(finalFragmentHasLoaded == true);

			// If link is sucessful, the binary shader is saved in the cache
			const bool finalProgramHasLinked = shaderToCompile.shaderProgram->link(shaderToCompile.introspection);
			FATAL_ASSERT_MSG_X(finalProgramHasLinked, "Failed to compile shader program \"%s\"", shaderToCompile.objectLabel);
		}
	}
#endif

#if defined(__EMSCRIPTEN__) || defined(WITH_ANGLE)
	if (shaderToCompile.introspection == GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS &&
	    theApplication().appConfiguration().fixedBatchSize != 0)
	{
		// If the batch size is fixed then it is also the maximum value
		maxBatchSize = theApplication().appConfiguration().fixedBatchSize;
	}
#endif

	const uint64_t shaderHashName = shaderToCompile.shaderProgram->hashName();
	ASSERT(shaderHashName == shaderHashSum); // The two hash sums should match
	binaryShaderCache_->registerShaderInfo(shaderHashName, shaderToCompile.objectLabel, maxBatchSize);
}

#ifdef WITH_SCENEGRAPH
GLShaderProgram *RenderResources::shaderProgram(Material::ShaderProgramType shaderProgramType)
{
	if (shaderProgramType != Material::ShaderProgramType::CUSTOM)
		return defaultShaderPrograms_[static_cast<int>(shaderProgramType)].get();
	else
		return nullptr;
}

const RenderResources::ShaderProgramCompileInfo::ShaderCompileInfo &RenderResources::defaultVertexShaderInfo(DefaultVertexShader shader)
{
	const unsigned int index = static_cast<unsigned int>(shader);
	FATAL_ASSERT(index < NumDefaultVertexShaders);
	return defaultVertexShaderInfos_[index];
}

const RenderResources::ShaderProgramCompileInfo::ShaderCompileInfo &RenderResources::defaultFragmentShaderInfo(DefaultFragmentShader shader)
{
	const unsigned int index = static_cast<unsigned int>(shader);
	FATAL_ASSERT(index < NumDefaultFragmentShaders);
	return defaultFragmentShaderInfos_[index];
}

const RenderResources::ShaderProgramCompileInfo::ShaderCompileInfo &RenderResources::defaultVertexShaderInfo(unsigned int index)
{
	FATAL_ASSERT(index < NumDefaultVertexShaders);
	return defaultVertexShaderInfos_[index];
}

const RenderResources::ShaderProgramCompileInfo::ShaderCompileInfo &RenderResources::defaultFragmentShaderInfo(unsigned int index)
{
	FATAL_ASSERT(index < NumDefaultFragmentShaders);
	return defaultFragmentShaderInfos_[index];
}

GLShaderProgram *RenderResources::batchedShader(const GLShaderProgram *shader)
{
	GLShaderProgram *batchedShader = nullptr;

	GLShaderProgram **findResult = batchedShaders_.find(shader);
	if (findResult != nullptr)
		batchedShader = *findResult;

	return batchedShader;
}

bool RenderResources::registerBatchedShader(const GLShaderProgram *shader, GLShaderProgram *batchedShader)
{
	FATAL_ASSERT(shader != nullptr);
	FATAL_ASSERT(batchedShader != nullptr);
	FATAL_ASSERT(shader != batchedShader);

	if (batchedShaders_.loadFactor() >= 0.8f)
		batchedShaders_.rehash(batchedShaders_.capacity() * 2);
	const bool inserted = batchedShaders_.insert(shader, batchedShader);

	return inserted;
}

bool RenderResources::unregisterBatchedShader(const GLShaderProgram *shader)
{
	ASSERT(shader != nullptr);
	const bool removed = batchedShaders_.remove(shader);
	return removed;
}

RenderResources::CameraUniformData *RenderResources::findCameraUniformData(GLShaderProgram *shaderProgram)
{
	return cameraUniformDataMap_.find(shaderProgram);
}

void RenderResources::insertCameraUniformData(GLShaderProgram *shaderProgram, CameraUniformData &&cameraUniformData)
{
	FATAL_ASSERT(shaderProgram != nullptr);

	if (cameraUniformDataMap_.loadFactor() >= 0.8f)
		cameraUniformDataMap_.rehash(cameraUniformDataMap_.capacity() * 2);

	cameraUniformDataMap_.insert(shaderProgram, nctl::move(cameraUniformData));
}

bool RenderResources::removeCameraUniformData(GLShaderProgram *shaderProgram)
{
	bool hasRemoved = false;

	if (cameraUniformDataMap_.isEmpty() == false)
		hasRemoved = cameraUniformDataMap_.remove(shaderProgram);

	return hasRemoved;
}

void RenderResources::setDefaultAttributesParameters(GLShaderProgram &shaderProgram)
{
	if (shaderProgram.numAttributes() > 0)
	{
		GLVertexFormat::Attribute *positionAttribute = shaderProgram.attribute(Material::PositionAttributeName);
		GLVertexFormat::Attribute *texCoordsAttribute = shaderProgram.attribute(Material::TexCoordsAttributeName);
		GLVertexFormat::Attribute *meshIndexAttribute = shaderProgram.attribute(Material::MeshIndexAttributeName);

		// The stride check avoid overwriting VBO parameters for custom mesh shaders attributes
		if (positionAttribute != nullptr && texCoordsAttribute != nullptr && meshIndexAttribute != nullptr)
		{
			if (positionAttribute->stride() == 0)
				positionAttribute->setVboParameters(sizeof(VertexFormatPos2Tex2Index), reinterpret_cast<void *>(offsetof(VertexFormatPos2Tex2Index, position)));

			if (texCoordsAttribute->stride() == 0)
				texCoordsAttribute->setVboParameters(sizeof(VertexFormatPos2Tex2Index), reinterpret_cast<void *>(offsetof(VertexFormatPos2Tex2Index, texcoords)));

			if (meshIndexAttribute->stride() == 0)
				meshIndexAttribute->setVboParameters(sizeof(VertexFormatPos2Tex2Index), reinterpret_cast<void *>(offsetof(VertexFormatPos2Tex2Index, drawindex)));
		}
		else if (positionAttribute != nullptr && texCoordsAttribute == nullptr && meshIndexAttribute != nullptr)
		{
			if (positionAttribute->stride() == 0)
				positionAttribute->setVboParameters(sizeof(VertexFormatPos2Index), reinterpret_cast<void *>(offsetof(VertexFormatPos2Index, position)));

			if (meshIndexAttribute->stride() == 0)
				meshIndexAttribute->setVboParameters(sizeof(VertexFormatPos2Index), reinterpret_cast<void *>(offsetof(VertexFormatPos2Index, drawindex)));
		}
		else if (positionAttribute != nullptr && texCoordsAttribute != nullptr && meshIndexAttribute == nullptr)
		{
			if (positionAttribute->stride() == 0)
				positionAttribute->setVboParameters(sizeof(VertexFormatPos2Tex2), reinterpret_cast<void *>(offsetof(VertexFormatPos2Tex2, position)));

			if (texCoordsAttribute->stride() == 0)
				texCoordsAttribute->setVboParameters(sizeof(VertexFormatPos2Tex2), reinterpret_cast<void *>(offsetof(VertexFormatPos2Tex2, texcoords)));
		}
		else if (positionAttribute != nullptr && texCoordsAttribute == nullptr && meshIndexAttribute == nullptr)
		{
			if (positionAttribute->stride() == 0)
				positionAttribute->setVboParameters(sizeof(VertexFormatPos2), reinterpret_cast<void *>(offsetof(VertexFormatPos2, position)));
		}
	}
}

#endif

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

#ifdef WITH_SCENEGRAPH
void RenderResources::setCurrentCamera(Camera *camera)
{
	if (camera != nullptr)
		currentCamera_ = camera;
	else
		currentCamera_ = defaultCamera_.get();
}

void RenderResources::updateCameraUniforms()
{
	// The buffer is shared among every shader program. There is no need to call `setFloatVector()` as `setDirty()` is enough.
	memcpy(cameraUniformsBuffer_, currentCamera_->projection().data(), 64);
	memcpy(cameraUniformsBuffer_ + 64, currentCamera_->view().data(), 64);
	for (nctl::HashMap<GLShaderProgram *, CameraUniformData>::Iterator i = cameraUniformDataMap_.begin(); i != cameraUniformDataMap_.end(); ++i)
	{
		CameraUniformData &cameraUniformData = *i;

		if (cameraUniformData.camera != currentCamera_)
			(*i).shaderUniforms.setDirty(true);
		else
		{
			if (cameraUniformData.updateFrameProjectionMatrix < currentCamera_->updateFrameProjectionMatrix())
				(*i).shaderUniforms.uniform(Material::ProjectionMatrixUniformName)->setDirty(true);
			if (cameraUniformData.updateFrameViewMatrix < currentCamera_->updateFrameViewMatrix())
				(*i).shaderUniforms.uniform(Material::ViewMatrixUniformName)->setDirty(true);
		}

		cameraUniformData.camera = currentCamera_;
		cameraUniformData.updateFrameProjectionMatrix = currentCamera_->updateFrameProjectionMatrix();
		cameraUniformData.updateFrameViewMatrix = currentCamera_->updateFrameViewMatrix();
	}
}

void RenderResources::setCurrentViewport(Viewport *viewport)
{
	FATAL_ASSERT(viewport != nullptr);
	currentViewport_ = viewport;
}

void RenderResources::fillDefaultShaderInfos()
{
	#ifdef WITH_EMBEDDED_SHADERS
	// Skipping the initial new line character of the raw string literal
	defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::SPRITE)] = ShaderProgramCompileInfo::ShaderCompileInfo(ShaderStrings::sprite_vs + 1, ShaderHashes::sprite_vs);
	defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::SPRITE_NOTEXTURE)] = ShaderProgramCompileInfo::ShaderCompileInfo(ShaderStrings::sprite_notexture_vs + 1, ShaderHashes::sprite_notexture_vs);
	defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::MESHSPRITE)] = ShaderProgramCompileInfo::ShaderCompileInfo(ShaderStrings::meshsprite_vs + 1, ShaderHashes::meshsprite_vs);
	defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::MESHSPRITE_NOTEXTURE)] = ShaderProgramCompileInfo::ShaderCompileInfo(ShaderStrings::meshsprite_notexture_vs + 1, ShaderHashes::meshsprite_notexture_vs);
	defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::TEXTNODE)] = ShaderProgramCompileInfo::ShaderCompileInfo(ShaderStrings::textnode_vs + 1, ShaderHashes::textnode_vs);
	defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::BATCHED_SPRITES)] = ShaderProgramCompileInfo::ShaderCompileInfo(ShaderStrings::batched_sprites_vs + 1, ShaderHashes::batched_sprites_vs);
	defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::BATCHED_SPRITES_NOTEXTURE)] = ShaderProgramCompileInfo::ShaderCompileInfo(ShaderStrings::batched_sprites_notexture_vs + 1, ShaderHashes::batched_sprites_notexture_vs);
	defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::BATCHED_MESHSPRITES)] = ShaderProgramCompileInfo::ShaderCompileInfo(ShaderStrings::batched_meshsprites_vs + 1, ShaderHashes::batched_meshsprites_vs);
	defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::BATCHED_MESHSPRITES_NOTEXTURE)] = ShaderProgramCompileInfo::ShaderCompileInfo(ShaderStrings::batched_meshsprites_notexture_vs + 1, ShaderHashes::batched_meshsprites_notexture_vs);
	defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::BATCHED_TEXTNODES)] = ShaderProgramCompileInfo::ShaderCompileInfo(ShaderStrings::batched_textnodes_vs + 1, ShaderHashes::batched_textnodes_vs);

	defaultFragmentShaderInfos_[static_cast<int>(DefaultFragmentShader::SPRITE)] = ShaderProgramCompileInfo::ShaderCompileInfo(ShaderStrings::sprite_fs + 1, ShaderHashes::sprite_fs);
	defaultFragmentShaderInfos_[static_cast<int>(DefaultFragmentShader::SPRITE_GRAY)] = ShaderProgramCompileInfo::ShaderCompileInfo(ShaderStrings::sprite_gray_fs + 1, ShaderHashes::sprite_gray_fs);
	defaultFragmentShaderInfos_[static_cast<int>(DefaultFragmentShader::SPRITE_NOTEXTURE)] = ShaderProgramCompileInfo::ShaderCompileInfo(ShaderStrings::sprite_notexture_fs + 1, ShaderHashes::sprite_notexture_fs);
	defaultFragmentShaderInfos_[static_cast<int>(DefaultFragmentShader::TEXTNODE_ALPHA)] = ShaderProgramCompileInfo::ShaderCompileInfo(ShaderStrings::textnode_alpha_fs + 1, ShaderHashes::textnode_alpha_fs);
	defaultFragmentShaderInfos_[static_cast<int>(DefaultFragmentShader::TEXTNODE_RED)] = ShaderProgramCompileInfo::ShaderCompileInfo(ShaderStrings::textnode_red_fs + 1, ShaderHashes::textnode_red_fs);
	#else
	defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::SPRITE)] = ShaderProgramCompileInfo::ShaderCompileInfo("sprite_vs.glsl");
	defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::SPRITE_NOTEXTURE)] = ShaderProgramCompileInfo::ShaderCompileInfo("sprite_notexture_vs.glsl");
	defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::MESHSPRITE)] = ShaderProgramCompileInfo::ShaderCompileInfo("meshsprite_vs.glsl");
	defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::MESHSPRITE_NOTEXTURE)] = ShaderProgramCompileInfo::ShaderCompileInfo("meshsprite_notexture_vs.glsl");
	defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::TEXTNODE)] = ShaderProgramCompileInfo::ShaderCompileInfo("textnode_vs.glsl");
	defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::BATCHED_SPRITES)] = ShaderProgramCompileInfo::ShaderCompileInfo("batched_sprites_vs.glsl");
	defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::BATCHED_SPRITES_NOTEXTURE)] = ShaderProgramCompileInfo::ShaderCompileInfo("batched_sprites_notexture_vs.glsl");
	defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::BATCHED_MESHSPRITES)] = ShaderProgramCompileInfo::ShaderCompileInfo("batched_meshsprites_vs.glsl");
	defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::BATCHED_MESHSPRITES_NOTEXTURE)] = ShaderProgramCompileInfo::ShaderCompileInfo("batched_meshsprites_notexture_vs.glsl");
	defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::BATCHED_TEXTNODES)] = ShaderProgramCompileInfo::ShaderCompileInfo("batched_textnodes_vs.glsl");

	defaultFragmentShaderInfos_[static_cast<int>(DefaultFragmentShader::SPRITE)] = ShaderProgramCompileInfo::ShaderCompileInfo("sprite_fs.glsl");
	defaultFragmentShaderInfos_[static_cast<int>(DefaultFragmentShader::SPRITE_GRAY)] = ShaderProgramCompileInfo::ShaderCompileInfo("sprite_gray_fs.glsl");
	defaultFragmentShaderInfos_[static_cast<int>(DefaultFragmentShader::SPRITE_NOTEXTURE)] = ShaderProgramCompileInfo::ShaderCompileInfo("sprite_notexture_fs.glsl");
	defaultFragmentShaderInfos_[static_cast<int>(DefaultFragmentShader::TEXTNODE_ALPHA)] = ShaderProgramCompileInfo::ShaderCompileInfo("textnode_alpha_fs.glsl");
	defaultFragmentShaderInfos_[static_cast<int>(DefaultFragmentShader::TEXTNODE_RED)] = ShaderProgramCompileInfo::ShaderCompileInfo("textnode_red_fs.glsl");
	#endif
}

void RenderResources::registerDefaultBatchedShaders()
{
	batchedShaders_.insert(defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::SPRITE)].get(), defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::BATCHED_SPRITES)].get());
	batchedShaders_.insert(defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::SPRITE_GRAY)].get(), defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::BATCHED_SPRITES_GRAY)].get());
	batchedShaders_.insert(defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::SPRITE_NO_TEXTURE)].get(), defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::BATCHED_SPRITES_NO_TEXTURE)].get());
	batchedShaders_.insert(defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::MESH_SPRITE)].get(), defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::BATCHED_MESH_SPRITES)].get());
	batchedShaders_.insert(defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::MESH_SPRITE_GRAY)].get(), defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::BATCHED_MESH_SPRITES_GRAY)].get());
	batchedShaders_.insert(defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::MESH_SPRITE_NO_TEXTURE)].get(), defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::BATCHED_MESH_SPRITES_NO_TEXTURE)].get());
	batchedShaders_.insert(defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::TEXTNODE_ALPHA)].get(), defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::BATCHED_TEXTNODES_ALPHA)].get());
	batchedShaders_.insert(defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::TEXTNODE_RED)].get(), defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::BATCHED_TEXTNODES_RED)].get());
	batchedShaders_.insert(defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::TEXTNODE_SPRITE)].get(), defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::BATCHED_TEXTNODES_SPRITE)].get());
}

void RenderResources::create()
{
	ZoneScoped;
	LOGI("Creating rendering resources...");

	// `create()` can be called after `createMinimal()`

	const AppConfiguration &appCfg = theApplication().appConfiguration();
	if (binaryShaderCache_ == nullptr)
		binaryShaderCache_ = nctl::makeUnique<BinaryShaderCache>(appCfg.useBinaryShaderCache, appCfg.shaderCacheDirname.data());
	if (buffersManager_ == nullptr)
		buffersManager_ = nctl::makeUnique<RenderBuffersManager>(appCfg.useBufferMapping, appCfg.vboSize, appCfg.iboSize);
	if (vaoPool_ == nullptr)
		vaoPool_ = nctl::makeUnique<RenderVaoPool>(appCfg.vaoPoolSize);
	if (hash64_ == nullptr)
		hash64_ = nctl::makeUnique<Hash64>();
	renderCommandPool_ = nctl::makeUnique<RenderCommandPool>(appCfg.renderCommandPoolSize);
	renderBatcher_ = nctl::makeUnique<RenderBatcher>();
	defaultCamera_ = nctl::makeUnique<Camera>();
	currentCamera_ = defaultCamera_.get();

	fillDefaultShaderInfos();

	// Define some references to shorten shader program names
	nctl::UniquePtr<GLShaderProgram> &spriteProg = defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::SPRITE)];
	nctl::UniquePtr<GLShaderProgram> &spriteGrayProg = defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::SPRITE_GRAY)];
	nctl::UniquePtr<GLShaderProgram> &spriteNoTextureProg = defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::SPRITE_NO_TEXTURE)];
	nctl::UniquePtr<GLShaderProgram> &meshSpriteProg = defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::MESH_SPRITE)];
	nctl::UniquePtr<GLShaderProgram> &meshSpriteGrayProg = defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::MESH_SPRITE_GRAY)];
	nctl::UniquePtr<GLShaderProgram> &meshSpriteNoTextureProg = defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::MESH_SPRITE_NO_TEXTURE)];
	nctl::UniquePtr<GLShaderProgram> &textnodeAlphaProg = defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::TEXTNODE_ALPHA)];
	nctl::UniquePtr<GLShaderProgram> &textnodeRedProg = defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::TEXTNODE_RED)];
	nctl::UniquePtr<GLShaderProgram> &textnodeSpriteProg = defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::TEXTNODE_SPRITE)];
	nctl::UniquePtr<GLShaderProgram> &batchedSpritesProg = defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::BATCHED_SPRITES)];
	nctl::UniquePtr<GLShaderProgram> &batchedSpritesGrayProg = defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::BATCHED_SPRITES_GRAY)];
	nctl::UniquePtr<GLShaderProgram> &batchedSpritesNoTextureProg = defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::BATCHED_SPRITES_NO_TEXTURE)];
	nctl::UniquePtr<GLShaderProgram> &batchedMeshSpritesProg = defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::BATCHED_MESH_SPRITES)];
	nctl::UniquePtr<GLShaderProgram> &batchedMeshSpritesGrayProg = defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::BATCHED_MESH_SPRITES_GRAY)];
	nctl::UniquePtr<GLShaderProgram> &batchedMeshSpritesNoTextureProg = defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::BATCHED_MESH_SPRITES_NO_TEXTURE)];
	nctl::UniquePtr<GLShaderProgram> &batchedTextnodesAlphaProg = defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::BATCHED_TEXTNODES_ALPHA)];
	nctl::UniquePtr<GLShaderProgram> &batchedTextnodesRedProg = defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::BATCHED_TEXTNODES_RED)];
	nctl::UniquePtr<GLShaderProgram> &batchedTextnodesSpriteProg = defaultShaderPrograms_[static_cast<int>(Material::ShaderProgramType::BATCHED_TEXTNODES_SPRITE)];
	// Define some references to shorten default vertex shader names
	ShaderProgramCompileInfo::ShaderCompileInfo &spriteVs = defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::SPRITE)];
	ShaderProgramCompileInfo::ShaderCompileInfo &spriteNoTextureVs = defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::SPRITE_NOTEXTURE)];
	ShaderProgramCompileInfo::ShaderCompileInfo &meshSpriteVs = defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::MESHSPRITE)];
	ShaderProgramCompileInfo::ShaderCompileInfo &meshSpriteNoTextureVs = defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::MESHSPRITE_NOTEXTURE)];
	ShaderProgramCompileInfo::ShaderCompileInfo &textnodeVs = defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::TEXTNODE)];
	ShaderProgramCompileInfo::ShaderCompileInfo &batchedSpritesVs = defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::BATCHED_SPRITES)];
	ShaderProgramCompileInfo::ShaderCompileInfo &batchedSpritesNoTextureVs = defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::BATCHED_SPRITES_NOTEXTURE)];
	ShaderProgramCompileInfo::ShaderCompileInfo &batchedMeshSpritesVs = defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::BATCHED_MESHSPRITES)];
	ShaderProgramCompileInfo::ShaderCompileInfo &batchedMeshSpritesNoTextureVs = defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::BATCHED_MESHSPRITES_NOTEXTURE)];
	ShaderProgramCompileInfo::ShaderCompileInfo &batchedTextnodesVs = defaultVertexShaderInfos_[static_cast<int>(DefaultVertexShader::BATCHED_TEXTNODES)];
	// Define some references to shorten default fragment shader names
	ShaderProgramCompileInfo::ShaderCompileInfo &spriteFs = defaultFragmentShaderInfos_[static_cast<int>(DefaultFragmentShader::SPRITE)];
	ShaderProgramCompileInfo::ShaderCompileInfo &spriteGrayFs = defaultFragmentShaderInfos_[static_cast<int>(DefaultFragmentShader::SPRITE_GRAY)];
	ShaderProgramCompileInfo::ShaderCompileInfo &spriteNoTextureFs = defaultFragmentShaderInfos_[static_cast<int>(DefaultFragmentShader::SPRITE_NOTEXTURE)];
	ShaderProgramCompileInfo::ShaderCompileInfo &textnodeAlphaFs = defaultFragmentShaderInfos_[static_cast<int>(DefaultFragmentShader::TEXTNODE_ALPHA)];
	ShaderProgramCompileInfo::ShaderCompileInfo &textnodeRedFs = defaultFragmentShaderInfos_[static_cast<int>(DefaultFragmentShader::TEXTNODE_RED)];

	ShaderProgramCompileInfo shadersToCompile[] = {
		{ spriteProg, spriteVs, spriteFs, GLShaderProgram::Introspection::ENABLED, "Sprite" },
		{ spriteGrayProg, spriteVs, spriteGrayFs, GLShaderProgram::Introspection::ENABLED, "Sprite_Gray" },
		{ spriteNoTextureProg, spriteNoTextureVs, spriteNoTextureFs, GLShaderProgram::Introspection::ENABLED, "Sprite_NoTexture" },
		{ meshSpriteProg, meshSpriteVs, spriteFs, GLShaderProgram::Introspection::ENABLED, "MeshSprite" },
		{ meshSpriteGrayProg, meshSpriteVs, spriteGrayFs, GLShaderProgram::Introspection::ENABLED, "MeshSprite_Gray" },
		{ meshSpriteNoTextureProg, meshSpriteNoTextureVs, spriteNoTextureFs, GLShaderProgram::Introspection::ENABLED, "MeshSprite_NoTexture" },
		{ textnodeAlphaProg, textnodeVs, textnodeAlphaFs, GLShaderProgram::Introspection::ENABLED, "TextNode_Alpha" },
		{ textnodeRedProg, textnodeVs, textnodeRedFs, GLShaderProgram::Introspection::ENABLED, "TextNode_Red" },
		{ textnodeSpriteProg, textnodeVs, spriteFs, GLShaderProgram::Introspection::ENABLED, "TextNode_Sprite" },
		{ batchedSpritesProg, batchedSpritesVs, spriteFs, GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS, "Batched_Sprites" },
		{ batchedSpritesGrayProg, batchedSpritesVs, spriteGrayFs, GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS, "Batched_Sprites_Gray" },
		{ batchedSpritesNoTextureProg, batchedSpritesNoTextureVs, spriteNoTextureFs, GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS, "Batched_Sprites_NoTexture" },
		{ batchedMeshSpritesProg, batchedMeshSpritesVs, spriteFs, GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS, "Batched_MeshSprites" },
		{ batchedMeshSpritesGrayProg, batchedMeshSpritesVs, spriteGrayFs, GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS, "Batched_MeshSprites_Gray" },
		{ batchedMeshSpritesNoTextureProg, batchedMeshSpritesNoTextureVs, spriteNoTextureFs, GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS, "Batched_MeshSprites_NoTexture" },
		{ batchedTextnodesAlphaProg, batchedTextnodesVs, textnodeAlphaFs, GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS, "Batched_TextNodes_Alpha" },
		{ batchedTextnodesRedProg, batchedTextnodesVs, textnodeRedFs, GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS, "Batched_TextNodes_Red" },
		{ batchedTextnodesSpriteProg, batchedTextnodesVs, spriteFs, GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS, "Batched_TextNodes_Sprite" }
	};

	const unsigned int numShaderToCompile = (sizeof(shadersToCompile) / sizeof(*shadersToCompile));
	FATAL_ASSERT(numShaderToCompile <= NumDefaultShaderPrograms);
	for (unsigned int i = 0; i < numShaderToCompile; i++)
	{
		ShaderProgramCompileInfo &shaderToCompile = shadersToCompile[i];
		shaderToCompile.shaderProgram = nctl::makeUnique<GLShaderProgram>();
	#ifndef WITH_EMBEDDED_SHADERS
		const char *vertexFilename = shaderToCompile.vertexInfo.shaderFile;
		const char *fragmentFilename = shaderToCompile.fragmentInfo.shaderFile;
		const nctl::String vertexShaderPath = fs::dataPath() + ShadersDir + "/" + shaderToCompile.vertexInfo.shaderFile;
		const nctl::String fragmentShaderPath = fs::dataPath() + ShadersDir + "/" + shaderToCompile.fragmentInfo.shaderFile;
		shaderToCompile.vertexInfo.shaderFile = vertexShaderPath.data();
		shaderToCompile.fragmentInfo.shaderFile = fragmentShaderPath.data();
	#endif
		compileShader(shaderToCompile);
	#ifndef WITH_EMBEDDED_SHADERS
		// Restore the original shader filename without the data path (the path strings will go out of scope soon)
		shaderToCompile.vertexInfo.shaderFile = vertexFilename;
		shaderToCompile.fragmentInfo.shaderFile = fragmentFilename;
	#endif
	}

	registerDefaultBatchedShaders();

	// Calculating a default projection matrix for all shader programs
	const float width = theApplication().width();
	const float height = theApplication().height();
	defaultCamera_->setOrthoProjection(0.0f, width, 0.0f, height);

	LOGI("Rendering resources created");
}
#endif

void RenderResources::createMinimal()
{
	ZoneScoped;
	LOGI("Creating a minimal set of rendering resources...");

	// `createMinimal()` cannot be called after `create()`
	ASSERT(binaryShaderCache_ == nullptr);
	ASSERT(buffersManager_ == nullptr);
	ASSERT(vaoPool_ == nullptr);
	ASSERT(hash64_ == nullptr);

	const AppConfiguration &appCfg = theApplication().appConfiguration();
	binaryShaderCache_ = nctl::makeUnique<BinaryShaderCache>(appCfg.useBinaryShaderCache, appCfg.shaderCacheDirname.data());
	buffersManager_ = nctl::makeUnique<RenderBuffersManager>(appCfg.useBufferMapping, appCfg.vboSize, appCfg.iboSize);
	vaoPool_ = nctl::makeUnique<RenderVaoPool>(appCfg.vaoPoolSize);
	hash64_ = nctl::makeUnique<Hash64>();

	LOGI("Minimal rendering resources created");
}

void RenderResources::dispose()
{
	const bool resourcesCreated = (hash64_ != nullptr);

#if WITH_SCENEGRAPH
	for (nctl::UniquePtr<GLShaderProgram> &shaderProgram : defaultShaderPrograms_)
		shaderProgram.reset(nullptr);

	ASSERT(cameraUniformDataMap_.isEmpty());

	defaultCamera_.reset(nullptr);
	renderBatcher_.reset(nullptr);
	renderCommandPool_.reset(nullptr);
#endif
	hash64_.reset(nullptr);
	vaoPool_.reset(nullptr);
	buffersManager_.reset(nullptr);
	binaryShaderCache_.reset(nullptr);

	if (resourcesCreated)
		LOGI("Rendering resources disposed");
}

}
