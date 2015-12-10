#include <stddef.h> // for offsetof()
#include "RenderResources.h"
#include "IFile.h" // for dataPath()
#include "ServiceLocator.h"

#ifdef WITH_EMBEDDED_SHADERS
	#include "shader_strings.h"
#endif

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

GLBufferObject *RenderResources::quadVbo_ = NULL;
GLShaderProgram *RenderResources::spriteShaderProgram_ = NULL;
GLShaderProgram *RenderResources::textnodeShaderProgram_ = NULL;
GLShaderProgram *RenderResources::colorShaderProgram_ = NULL;

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void RenderResources::create()
{
	const VertexFormatPos2Tex2 quadVertices[] =
	{
		{ 0.5f, -0.5f,  1.0f, 1.0f},
		{ 0.5f,  0.5f,  1.0f, 0.0f},
		{-0.5f, -0.5f,  0.0f, 1.0f},

		{-0.5f,  0.5f,  0.0f, 0.0f}
	};

	LOGI("Creating rendering resources...");

	quadVbo_ = new GLBufferObject(GL_ARRAY_BUFFER);
	quadVbo_->bufferData(sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	spriteShaderProgram_ = new GLShaderProgram();
#ifndef WITH_EMBEDDED_SHADERS
	spriteShaderProgram_->attachShader(GL_VERTEX_SHADER, (IFile::dataPath() + "shaders/sprite_vs.glsl").data());
	spriteShaderProgram_->attachShader(GL_FRAGMENT_SHADER, (IFile::dataPath() + "shaders/sprite_fs.glsl").data());
#else
	spriteShaderProgram_->attachShaderFromString(GL_VERTEX_SHADER, ShaderStrings::sprite_vs);
	spriteShaderProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, ShaderStrings::sprite_fs);
#endif
	spriteShaderProgram_->link();
	spriteShaderProgram_->use();

	textnodeShaderProgram_ = new GLShaderProgram();
#ifndef WITH_EMBEDDED_SHADERS
	textnodeShaderProgram_->attachShader(GL_VERTEX_SHADER, (IFile::dataPath() + "shaders/textnode_vs.glsl").data());
	textnodeShaderProgram_->attachShader(GL_FRAGMENT_SHADER, (IFile::dataPath() + "shaders/textnode_fs.glsl").data());
#else
	textnodeShaderProgram_->attachShaderFromString(GL_VERTEX_SHADER, ShaderStrings::textnode_vs);
	textnodeShaderProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, ShaderStrings::textnode_fs);
#endif
	textnodeShaderProgram_->link();
	textnodeShaderProgram_->use();

	colorShaderProgram_ = new GLShaderProgram();
#ifndef WITH_EMBEDDED_SHADERS
	colorShaderProgram_->attachShader(GL_VERTEX_SHADER, (IFile::dataPath() + "shaders/color_vs.glsl").data());
	colorShaderProgram_->attachShader(GL_FRAGMENT_SHADER, (IFile::dataPath() + "shaders/color_fs.glsl").data());
#else
	colorShaderProgram_->attachShaderFromString(GL_VERTEX_SHADER, ShaderStrings::color_vs);
	colorShaderProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, ShaderStrings::color_fs);
#endif
	colorShaderProgram_->link();
	colorShaderProgram_->use();

	LOGI("Rendering resources created");
}

void RenderResources::dispose()
{
	delete colorShaderProgram_;
	delete textnodeShaderProgram_;
	delete spriteShaderProgram_;
	delete quadVbo_;

	colorShaderProgram_ = NULL;
	textnodeShaderProgram_ = NULL;
	spriteShaderProgram_ = NULL;
	quadVbo_ = NULL;

	LOGI("Rendering resources disposed");
}

}
