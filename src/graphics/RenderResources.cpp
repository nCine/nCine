#include <stddef.h> // for offsetof()
#include "RenderResources.h"
#include "IFile.h" // for dataPath()

#ifdef WITH_EMBEDDED_SHADERS
	#include "shader_strings.h"
#endif

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

GLBufferObject *RenderResources::quadVbo_ = NULL;
GLShaderProgram *RenderResources::spriteShaderProgram_ = NULL;
GLShaderProgram *RenderResources::textnodeGrayShaderProgram_ = NULL;
GLShaderProgram *RenderResources::textnodeColorShaderProgram_ = NULL;
GLShaderProgram *RenderResources::colorShaderProgram_ = NULL;

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void RenderResources::create()
{
	const VertexFormatPos2Tex2 quadVertices[] =
	{
		{{ 0.5f, -0.5f}, {1.0f, 1.0f}},
		{{ 0.5f,  0.5f}, {1.0f, 0.0f}},
		{{-0.5f, -0.5f}, {0.0f, 1.0f}},

		{{-0.5f,  0.5f}, {0.0f, 0.0f}}
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

	textnodeGrayShaderProgram_ = new GLShaderProgram();
#ifndef WITH_EMBEDDED_SHADERS
	textnodeGrayShaderProgram_->attachShader(GL_VERTEX_SHADER, (IFile::dataPath() + "shaders/textnode_vs.glsl").data());
	textnodeGrayShaderProgram_->attachShader(GL_FRAGMENT_SHADER, (IFile::dataPath() + "shaders/textnode_gray_fs.glsl").data());
#else
	textnodeGrayShaderProgram_->attachShaderFromString(GL_VERTEX_SHADER, ShaderStrings::textnode_vs);
	textnodeGrayShaderProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, ShaderStrings::textnode_gray_fs);
#endif
	textnodeGrayShaderProgram_->link();
	textnodeGrayShaderProgram_->use();

	textnodeColorShaderProgram_ = new GLShaderProgram();
#ifndef WITH_EMBEDDED_SHADERS
	textnodeColorShaderProgram_->attachShader(GL_VERTEX_SHADER, (IFile::dataPath() + "shaders/textnode_vs.glsl").data());
	textnodeColorShaderProgram_->attachShader(GL_FRAGMENT_SHADER, (IFile::dataPath() + "shaders/textnode_color_fs.glsl").data());
#else
	textnodeColorShaderProgram_->attachShaderFromString(GL_VERTEX_SHADER, ShaderStrings::textnode_vs);
	textnodeColorShaderProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, ShaderStrings::textnode_color_fs);
#endif
	textnodeColorShaderProgram_->link();
	textnodeColorShaderProgram_->use();

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
	delete textnodeColorShaderProgram_;
	delete textnodeGrayShaderProgram_;
	delete spriteShaderProgram_;
	delete quadVbo_;

	colorShaderProgram_ = NULL;
	textnodeColorShaderProgram_ = NULL;
	textnodeGrayShaderProgram_ = NULL;
	spriteShaderProgram_ = NULL;
	quadVbo_ = NULL;

	LOGI("Rendering resources disposed");
}

}
