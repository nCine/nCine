#include "RenderResources.h"
#include "Application.h"
#include "IFile.h" // for dataPath()

#ifdef WITH_EMBEDDED_SHADERS
	#include "shader_strings.h"
#endif

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

nctl::UniquePtr<RenderBuffersManager> RenderResources::buffersManager_;
nctl::UniquePtr<RenderVaoPool> RenderResources::vaoPool_;
nctl::UniquePtr<GLShaderProgram> RenderResources::spriteShaderProgram_;
nctl::UniquePtr<GLShaderProgram> RenderResources::spriteGrayShaderProgram_;
nctl::UniquePtr<GLShaderProgram> RenderResources::meshSpriteShaderProgram_;
nctl::UniquePtr<GLShaderProgram> RenderResources::meshSpriteGrayShaderProgram_;
nctl::UniquePtr<GLShaderProgram> RenderResources::textnodeShaderProgram_;
nctl::UniquePtr<GLShaderProgram> RenderResources::textnodeGrayShaderProgram_;
nctl::UniquePtr<GLShaderProgram> RenderResources::batchedSpritesShaderProgram_;
nctl::UniquePtr<GLShaderProgram> RenderResources::batchedSpritesGrayShaderProgram_;
nctl::UniquePtr<GLShaderProgram> RenderResources::batchedMeshSpritesShaderProgram_;
nctl::UniquePtr<GLShaderProgram> RenderResources::batchedMeshSpritesGrayShaderProgram_;
nctl::UniquePtr<GLShaderProgram> RenderResources::batchedTextnodesGrayShaderProgram_;
nctl::UniquePtr<GLShaderProgram> RenderResources::batchedTextnodesShaderProgram_;
Matrix4x4f RenderResources::projectionMatrix_;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void RenderResources::createMinimal()
{
	LOGI("Creating a minimal set of rendering resources...");

	const AppConfiguration &appCfg = theApplication().appConfiguration();
	buffersManager_ = nctl::makeUnique<RenderBuffersManager>(appCfg.useBufferMapping, appCfg.vboSize, appCfg.iboSize);
	vaoPool_ = nctl::makeUnique<RenderVaoPool>(appCfg.vaoPoolSize);

	LOGI("Minimal rendering resources created");
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

namespace {

	struct ShaderLoad
	{
		nctl::UniquePtr<GLShaderProgram> &shaderProgram;
		const char *vertexShader;
		const char *fragmentShader;
		GLShaderProgram::Introspection introspection;
	};

}

void RenderResources::create()
{
	LOGI("Creating rendering resources...");

	const AppConfiguration &appCfg = theApplication().appConfiguration();
	buffersManager_ = nctl::makeUnique<RenderBuffersManager>(appCfg.useBufferMapping, appCfg.vboSize, appCfg.iboSize);
	vaoPool_ = nctl::makeUnique<RenderVaoPool>(appCfg.vaoPoolSize);

	ShaderLoad shadersToLoad[] = {
#ifndef WITH_EMBEDDED_SHADERS
		{ RenderResources::spriteShaderProgram_, "sprite_vs.glsl", "sprite_fs.glsl", GLShaderProgram::Introspection::ENABLED },
		{ RenderResources::spriteGrayShaderProgram_, "sprite_vs.glsl", "sprite_gray_fs.glsl", GLShaderProgram::Introspection::ENABLED },
		{ RenderResources::meshSpriteShaderProgram_, "meshsprite_vs.glsl", "sprite_fs.glsl", GLShaderProgram::Introspection::ENABLED },
		{ RenderResources::meshSpriteGrayShaderProgram_, "meshsprite_vs.glsl", "sprite_gray_fs.glsl", GLShaderProgram::Introspection::ENABLED },
		{ RenderResources::textnodeShaderProgram_, "textnode_vs.glsl", "textnode_fs.glsl", GLShaderProgram::Introspection::ENABLED },
		{ RenderResources::textnodeGrayShaderProgram_, "textnode_vs.glsl", "textnode_gray_fs.glsl", GLShaderProgram::Introspection::ENABLED },
		{ RenderResources::batchedSpritesShaderProgram_, "batched_sprites_vs.glsl", "sprite_fs.glsl", GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS },
		{ RenderResources::batchedSpritesGrayShaderProgram_, "batched_sprites_vs.glsl", "sprite_gray_fs.glsl", GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS },
		{ RenderResources::batchedMeshSpritesShaderProgram_, "batched_meshsprites_vs.glsl", "sprite_fs.glsl", GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS },
		{ RenderResources::batchedMeshSpritesGrayShaderProgram_, "batched_meshsprites_vs.glsl", "sprite_gray_fs.glsl", GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS },
		{ RenderResources::batchedTextnodesShaderProgram_, "batched_textnodes_vs.glsl", "textnode_fs.glsl", GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS },
		{ RenderResources::batchedTextnodesGrayShaderProgram_, "batched_textnodes_vs.glsl", "textnode_gray_fs.glsl", GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS }
#else
		{ RenderResources::spriteShaderProgram_, ShaderStrings::sprite_vs, ShaderStrings::sprite_fs, GLShaderProgram::Introspection::ENABLED },
		{ RenderResources::spriteGrayShaderProgram_, ShaderStrings::sprite_vs, ShaderStrings::sprite_gray_fs, GLShaderProgram::Introspection::ENABLED },
		{ RenderResources::meshSpriteShaderProgram_, ShaderStrings::meshsprite_vs, ShaderStrings::sprite_fs, GLShaderProgram::Introspection::ENABLED },
		{ RenderResources::meshSpriteGrayShaderProgram_, ShaderStrings::meshsprite_vs, ShaderStrings::sprite_gray_fs, GLShaderProgram::Introspection::ENABLED },
		{ RenderResources::textnodeShaderProgram_, ShaderStrings::textnode_vs, ShaderStrings::textnode_fs, GLShaderProgram::Introspection::ENABLED },
		{ RenderResources::textnodeGrayShaderProgram_, ShaderStrings::textnode_vs, ShaderStrings::textnode_gray_fs, GLShaderProgram::Introspection::ENABLED },
		{ RenderResources::batchedSpritesShaderProgram_, ShaderStrings::batched_sprites_vs, ShaderStrings::sprite_fs, GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS },
		{ RenderResources::batchedSpritesGrayShaderProgram_, ShaderStrings::batched_sprites_vs, ShaderStrings::sprite_gray_fs, GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS },
		{ RenderResources::batchedMeshSpritesShaderProgram_, ShaderStrings::batched_meshsprites_vs, ShaderStrings::sprite_fs, GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS },
		{ RenderResources::batchedMeshSpritesGrayShaderProgram_, ShaderStrings::batched_meshsprites_vs, ShaderStrings::sprite_gray_fs, GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS },
		{ RenderResources::batchedTextnodesShaderProgram_, ShaderStrings::batched_textnodes_vs, ShaderStrings::textnode_fs, GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS },
		{ RenderResources::batchedTextnodesGrayShaderProgram_, ShaderStrings::batched_textnodes_vs, ShaderStrings::textnode_gray_fs, GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS }
#endif
	};

	const GLShaderProgram::QueryPhase queryPhase = appCfg.deferShaderQueries ? GLShaderProgram::QueryPhase::DEFERRED : GLShaderProgram::QueryPhase::IMMEDIATE;
	const unsigned int numShaderToLoad = (sizeof(shadersToLoad) / sizeof(*shadersToLoad));
	for (unsigned int i = 0; i < numShaderToLoad; i++)
	{
		const ShaderLoad &shaderToLoad = shadersToLoad[i];

		shaderToLoad.shaderProgram = nctl::makeUnique<GLShaderProgram>(queryPhase);
#ifndef WITH_EMBEDDED_SHADERS
		shaderToLoad.shaderProgram->attachShader(GL_VERTEX_SHADER, (IFile::dataPath() + "shaders/" + shaderToLoad.vertexShader).data());
		shaderToLoad.shaderProgram->attachShader(GL_FRAGMENT_SHADER, (IFile::dataPath() + "shaders/" + shaderToLoad.fragmentShader).data());
#else
		shaderToLoad.shaderProgram->attachShaderFromString(GL_VERTEX_SHADER, shaderToLoad.vertexShader);
		shaderToLoad.shaderProgram->attachShaderFromString(GL_FRAGMENT_SHADER, shaderToLoad.fragmentShader);
#endif
		shaderToLoad.shaderProgram->link(shaderToLoad.introspection);
		FATAL_ASSERT(shaderToLoad.shaderProgram->status() != GLShaderProgram::Status::LINKING_FAILED);
	}

	// Calculating a common projection matrix for all shader programs
	const float width = theApplication().width();
	const float height = theApplication().height();
	const float near = -1.0f;
	const float far = 1.0f;

	// TODO: Projection matrix is hard-coded, should it go in a camera class? (Y-axis points downward)
	projectionMatrix_ = Matrix4x4f::ortho(0.0f, width, 0.0f, height, near, far);

	LOGI("Rendering resources created");
}

void RenderResources::dispose()
{
	batchedTextnodesGrayShaderProgram_.reset(nullptr);
	batchedTextnodesShaderProgram_.reset(nullptr);
	batchedMeshSpritesGrayShaderProgram_.reset(nullptr);
	batchedMeshSpritesShaderProgram_.reset(nullptr);
	batchedSpritesGrayShaderProgram_.reset(nullptr);
	batchedSpritesShaderProgram_.reset(nullptr);
	textnodeGrayShaderProgram_.reset(nullptr);
	textnodeShaderProgram_.reset(nullptr);
	meshSpriteGrayShaderProgram_.reset(nullptr);
	meshSpriteShaderProgram_.reset(nullptr);
	spriteGrayShaderProgram_.reset(nullptr);
	spriteShaderProgram_.reset(nullptr);
	vaoPool_.reset(nullptr);
	buffersManager_.reset(nullptr);

	LOGI("Rendering resources disposed");
}

}
