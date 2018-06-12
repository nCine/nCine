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
nctl::UniquePtr<GLShaderProgram> RenderResources::meshspriteShaderProgram_;
nctl::UniquePtr<GLShaderProgram> RenderResources::textnodeGrayShaderProgram_;
nctl::UniquePtr<GLShaderProgram> RenderResources::textnodeColorShaderProgram_;
nctl::UniquePtr<GLShaderProgram> RenderResources::colorShaderProgram_;
nctl::UniquePtr<GLShaderProgram> RenderResources::batchedSpritesShaderProgram_;
nctl::UniquePtr<GLShaderProgram> RenderResources::batchedMeshSpritesShaderProgram_;
nctl::UniquePtr<GLShaderProgram> RenderResources::batchedTextnodesGrayShaderProgram_;
nctl::UniquePtr<GLShaderProgram> RenderResources::batchedTextnodesColorShaderProgram_;
Matrix4x4f RenderResources::projectionMatrix_;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void RenderResources::createMinimal()
{
	LOGI("Creating a minimal set of rendering resources...");

	const AppConfiguration &appCfg = theApplication().appConfiguration();
	buffersManager_ = nctl::makeUnique<RenderBuffersManager>(appCfg.vboSize(), appCfg.iboSize());
	vaoPool_ = nctl::makeUnique<RenderVaoPool>(appCfg.vaoPoolSize());

	LOGI("Minimal rendering resources created");
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void RenderResources::create()
{
	LOGI("Creating rendering resources...");

	const AppConfiguration &appCfg = theApplication().appConfiguration();
	buffersManager_ = nctl::makeUnique<RenderBuffersManager>(appCfg.vboSize(), appCfg.iboSize());
	vaoPool_ = nctl::makeUnique<RenderVaoPool>(appCfg.vaoPoolSize());

	spriteShaderProgram_ = nctl::makeUnique<GLShaderProgram>();
#ifndef WITH_EMBEDDED_SHADERS
	spriteShaderProgram_->attachShader(GL_VERTEX_SHADER, (IFile::dataPath() + "shaders/sprite_vs.glsl").data());
	spriteShaderProgram_->attachShader(GL_FRAGMENT_SHADER, (IFile::dataPath() + "shaders/sprite_fs.glsl").data());
#else
	spriteShaderProgram_->attachShaderFromString(GL_VERTEX_SHADER, ShaderStrings::sprite_vs);
	spriteShaderProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, ShaderStrings::sprite_fs);
#endif
	spriteShaderProgram_->link();

	meshspriteShaderProgram_ = nctl::makeUnique<GLShaderProgram>();
#ifndef WITH_EMBEDDED_SHADERS
	meshspriteShaderProgram_->attachShader(GL_VERTEX_SHADER, (IFile::dataPath() + "shaders/meshsprite_vs.glsl").data());
	meshspriteShaderProgram_->attachShader(GL_FRAGMENT_SHADER, (IFile::dataPath() + "shaders/sprite_fs.glsl").data());
#else
	meshspriteShaderProgram_->attachShaderFromString(GL_VERTEX_SHADER, ShaderStrings::meshsprite_vs);
	meshspriteShaderProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, ShaderStrings::sprite_fs);
#endif
	meshspriteShaderProgram_->link();

	textnodeGrayShaderProgram_ = nctl::makeUnique<GLShaderProgram>();
#ifndef WITH_EMBEDDED_SHADERS
	textnodeGrayShaderProgram_->attachShader(GL_VERTEX_SHADER, (IFile::dataPath() + "shaders/textnode_vs.glsl").data());
	textnodeGrayShaderProgram_->attachShader(GL_FRAGMENT_SHADER, (IFile::dataPath() + "shaders/textnode_gray_fs.glsl").data());
#else
	textnodeGrayShaderProgram_->attachShaderFromString(GL_VERTEX_SHADER, ShaderStrings::textnode_vs);
	textnodeGrayShaderProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, ShaderStrings::textnode_gray_fs);
#endif
	textnodeGrayShaderProgram_->link();

	textnodeColorShaderProgram_ = nctl::makeUnique<GLShaderProgram>();
#ifndef WITH_EMBEDDED_SHADERS
	textnodeColorShaderProgram_->attachShader(GL_VERTEX_SHADER, (IFile::dataPath() + "shaders/textnode_vs.glsl").data());
	textnodeColorShaderProgram_->attachShader(GL_FRAGMENT_SHADER, (IFile::dataPath() + "shaders/textnode_color_fs.glsl").data());
#else
	textnodeColorShaderProgram_->attachShaderFromString(GL_VERTEX_SHADER, ShaderStrings::textnode_vs);
	textnodeColorShaderProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, ShaderStrings::textnode_color_fs);
#endif
	textnodeColorShaderProgram_->link();

	colorShaderProgram_ = nctl::makeUnique<GLShaderProgram>();
#ifndef WITH_EMBEDDED_SHADERS
	colorShaderProgram_->attachShader(GL_VERTEX_SHADER, (IFile::dataPath() + "shaders/color_vs.glsl").data());
	colorShaderProgram_->attachShader(GL_FRAGMENT_SHADER, (IFile::dataPath() + "shaders/color_fs.glsl").data());
#else
	colorShaderProgram_->attachShaderFromString(GL_VERTEX_SHADER, ShaderStrings::color_vs);
	colorShaderProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, ShaderStrings::color_fs);
#endif
	colorShaderProgram_->link();

	batchedSpritesShaderProgram_ = nctl::makeUnique<GLShaderProgram>();
#ifndef WITH_EMBEDDED_SHADERS
	batchedSpritesShaderProgram_->attachShader(GL_VERTEX_SHADER, (IFile::dataPath() + "shaders/batched_sprites_vs.glsl").data());
	batchedSpritesShaderProgram_->attachShader(GL_FRAGMENT_SHADER, (IFile::dataPath() + "shaders/sprite_fs.glsl").data());
#else
	batchedSpritesShaderProgram_->attachShaderFromString(GL_VERTEX_SHADER, ShaderStrings::batched_sprites_vs);
	batchedSpritesShaderProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, ShaderStrings::sprite_fs);
#endif
	batchedSpritesShaderProgram_->link(GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS);

	batchedMeshSpritesShaderProgram_ = nctl::makeUnique<GLShaderProgram>();
#ifndef WITH_EMBEDDED_SHADERS
	batchedMeshSpritesShaderProgram_->attachShader(GL_VERTEX_SHADER, (IFile::dataPath() + "shaders/batched_meshsprites_vs.glsl").data());
	batchedMeshSpritesShaderProgram_->attachShader(GL_FRAGMENT_SHADER, (IFile::dataPath() + "shaders/sprite_fs.glsl").data());
#else
	batchedMeshSpritesShaderProgram_->attachShaderFromString(GL_VERTEX_SHADER, ShaderStrings::batched_meshsprites_vs);
	batchedMeshSpritesShaderProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, ShaderStrings::sprite_fs);
#endif
	batchedMeshSpritesShaderProgram_->link(GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS);

	batchedTextnodesGrayShaderProgram_ = nctl::makeUnique<GLShaderProgram>();
#ifndef WITH_EMBEDDED_SHADERS
	batchedTextnodesGrayShaderProgram_->attachShader(GL_VERTEX_SHADER, (IFile::dataPath() + "shaders/batched_textnodes_vs.glsl").data());
	batchedTextnodesGrayShaderProgram_->attachShader(GL_FRAGMENT_SHADER, (IFile::dataPath() + "shaders/textnode_gray_fs.glsl").data());
#else
	batchedTextnodesGrayShaderProgram_->attachShaderFromString(GL_VERTEX_SHADER, ShaderStrings::batched_textnodes_vs);
	batchedTextnodesGrayShaderProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, ShaderStrings::textnode_gray_fs);
#endif
	batchedTextnodesGrayShaderProgram_->link(GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS);

	batchedTextnodesColorShaderProgram_ = nctl::makeUnique<GLShaderProgram>();
#ifndef WITH_EMBEDDED_SHADERS
	batchedTextnodesColorShaderProgram_->attachShader(GL_VERTEX_SHADER, (IFile::dataPath() + "shaders/batched_textnodes_vs.glsl").data());
	batchedTextnodesColorShaderProgram_->attachShader(GL_FRAGMENT_SHADER, (IFile::dataPath() + "shaders/textnode_color_fs.glsl").data());
#else
	batchedTextnodesColorShaderProgram_->attachShaderFromString(GL_VERTEX_SHADER, ShaderStrings::batched_textnodes_vs);
	batchedTextnodesColorShaderProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, ShaderStrings::textnode_color_fs);
#endif
	batchedTextnodesColorShaderProgram_->link(GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS);

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
	batchedTextnodesColorShaderProgram_.reset(nullptr);
	batchedTextnodesGrayShaderProgram_.reset(nullptr);
	batchedMeshSpritesShaderProgram_.reset(nullptr);
	batchedSpritesShaderProgram_.reset(nullptr);
	colorShaderProgram_.reset(nullptr);
	textnodeColorShaderProgram_.reset(nullptr);
	textnodeGrayShaderProgram_.reset(nullptr);
	meshspriteShaderProgram_.reset(nullptr);
	spriteShaderProgram_.reset(nullptr);
	vaoPool_.reset(nullptr);
	buffersManager_.reset(nullptr);

	LOGI("Rendering resources disposed");
}

}
