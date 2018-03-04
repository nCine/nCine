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

nctl::UniquePtr<GLVertexArrayObject> RenderResources::vao_;
nctl::UniquePtr<RenderBuffersManager> RenderResources::buffersManager_;
nctl::UniquePtr<GLShaderProgram> RenderResources::spriteShaderProgram_;
nctl::UniquePtr<GLShaderProgram> RenderResources::textnodeGrayShaderProgram_;
nctl::UniquePtr<GLShaderProgram> RenderResources::textnodeColorShaderProgram_;
nctl::UniquePtr<GLShaderProgram> RenderResources::colorShaderProgram_;
nctl::UniquePtr<GLShaderProgram> RenderResources::instancedSpritesShaderProgram_;
Matrix4x4f RenderResources::projectionMatrix_;

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void RenderResources::create()
{
	LOGI("Creating rendering resources...");

	vao_ = nctl::makeUnique<GLVertexArrayObject>();
	vao_->bind();

	buffersManager_ = nctl::makeUnique<RenderBuffersManager>();

	spriteShaderProgram_ = nctl::makeUnique<GLShaderProgram>();
#ifndef WITH_EMBEDDED_SHADERS
	spriteShaderProgram_->attachShader(GL_VERTEX_SHADER, (IFile::dataPath() + "shaders/sprite_vs.glsl").data());
	spriteShaderProgram_->attachShader(GL_FRAGMENT_SHADER, (IFile::dataPath() + "shaders/sprite_fs.glsl").data());
#else
	spriteShaderProgram_->attachShaderFromString(GL_VERTEX_SHADER, ShaderStrings::sprite_vs);
	spriteShaderProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, ShaderStrings::sprite_fs);
#endif
	spriteShaderProgram_->link();

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

	instancedSpritesShaderProgram_ = nctl::makeUnique<GLShaderProgram>();
#ifndef WITH_EMBEDDED_SHADERS
	instancedSpritesShaderProgram_->attachShader(GL_VERTEX_SHADER, (IFile::dataPath() + "shaders/instanced_sprites_vs.glsl").data());
	instancedSpritesShaderProgram_->attachShader(GL_FRAGMENT_SHADER, (IFile::dataPath() + "shaders/sprite_fs.glsl").data());
#else
	instancedSpritesShaderProgram_->attachShaderFromString(GL_VERTEX_SHADER, ShaderStrings::instanced_sprites_vs);
	instancedSpritesShaderProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, ShaderStrings::sprite_fs);
#endif
	instancedSpritesShaderProgram_->link(GLShaderProgram::Introspection::NO_UNIFORMS_IN_BLOCKS);

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
	instancedSpritesShaderProgram_.reset(nullptr);
	colorShaderProgram_.reset(nullptr);
	textnodeColorShaderProgram_.reset(nullptr);
	textnodeGrayShaderProgram_.reset(nullptr);
	spriteShaderProgram_.reset(nullptr);
	buffersManager_.reset(nullptr);
	vao_.reset(nullptr);

	LOGI("Rendering resources disposed");
}

}
