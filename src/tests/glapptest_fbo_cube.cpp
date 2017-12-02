#include <stddef.h> // for offsetof()
#include "glapptest_fbo_cube.h"
#include "Application.h"
#include "AppConfiguration.h"
#include "GLShaderProgram.h"
#include "GLShaderUniforms.h"
#include "GLShaderAttributes.h"
#include "GLTexture.h"
#include "GLFramebufferObject.h"
#include "GLBufferObject.h"
#include "IFile.h" // for dataPath()
#include "../../tests/apptest_datapath.h"

#ifdef WITH_EMBEDDED_SHADERS
	#include "shader_strings.h"
#endif

namespace {

struct VertexFormatCol
{
	GLfloat position[3];
	GLfloat color[3];
};

struct VertexFormatTex
{
	GLfloat position[3];
	GLfloat texcoords[2];
};

enum
{
	ATTRIB_POSITION,
	ATTRIB_COLOR,
	ATTRIB_TEXCOORDS
};

const VertexFormatCol triVertices[] =
{
	{{ 0.0f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
	{{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
	{{ 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
};

const VertexFormatTex cubeVertices[] =
{
	{{ 1.0f,  1.0f, -1.0f}, {0.0f, 0.0f}},
	{{ 1.0f, -1.0f, -1.0f}, {1.0f, 0.0f}},
	{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f}},

	{{ 1.0f,  1.0f,  1.0f}, {0.0f, 0.0f}},
	{{-1.0f,  1.0f,  1.0f}, {1.0f, 0.0f}},
	{{ 1.0f, -1.0f,  1.0f}, {0.0f, 1.0f}},

	{{ 1.0f,  1.0f,  1.0f}, {1.0f, 0.0f}},
	{{ 1.0f, -1.0f, -1.0f}, {0.0f, 1.0f}},
	{{ 1.0f, -1.0f, -1.0f}, {1.0f, 0.0f}},

	{{ 1.0f, -1.0f,  1.0f}, {1.0f, 1.0f}},
	{{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f}},
	{{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f}},

	{{-1.0f, -1.0f,  1.0f}, {1.0f, 0.0f}},
	{{-1.0f,  1.0f,  1.0f}, {1.0f, 1.0f}},
	{{ 1.0f,  1.0f, -1.0f}, {1.0f, 0.0f}},

	{{-1.0f,  1.0f,  1.0f}, {0.0f, 1.0f}},
	{{-1.0f,  1.0f, -1.0f}, {0.0f, 1.0f}},
	{{ 1.0f, -1.0f,  1.0f}, {1.0f, 1.0f}},

	{{-1.0f,  1.0f, -1.0f}, {1.0f, 1.0f}},
	{{-1.0f, -1.0f,  1.0f}, {1.0f, 1.0f}},
	{{-1.0f, -1.0f,  1.0f}, {0.0f, 1.0f}}
};

const GLushort cubeIndices[] =
{
	 0,  1,  2,
	 3,  4,  5,
	 0,  6,  7,
	 8,  9, 10,
	11, 12, 13,
	 3, 14, 15,
	16,  0,  2,
	 6, 17,  7,
	14, 18, 15,
	 4, 19,  5,
	16, 11, 13,
	 9, 20, 10
};

}

nc::IAppEventHandler *createAppEventHandler()
{
	return new MyEventHandler;
}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	config.enableScenegraph(false);
	config.enableAudio(false);
	config.enableThreads(false);

	setDataPath(config);
}

void MyEventHandler::onInit()
{
	colorProgram_ = new nc::GLShaderProgram();
#ifndef WITH_EMBEDDED_SHADERS
	colorProgram_->attachShader(GL_VERTEX_SHADER, (nc::IFile::dataPath() + "shaders/vcolor_vs.glsl").data());
	colorProgram_->attachShader(GL_FRAGMENT_SHADER, (nc::IFile::dataPath() + "shaders/vcolor_fs.glsl").data());
#else
	colorProgram_->attachShaderFromString(GL_VERTEX_SHADER, nc::ShaderStrings::vcolor_vs);
	colorProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, nc::ShaderStrings::vcolor_fs);
#endif
	colorProgram_->bindAttribLocation(ATTRIB_POSITION, "aPosition");
	colorProgram_->bindAttribLocation(ATTRIB_COLOR, "aColor");
	colorProgram_->link();
	colorProgram_->use();
	colorUniforms_ = new nc::GLShaderUniforms(colorProgram_);
	colorAttributes_ = new nc::GLShaderAttributes(colorProgram_);

	texProgram_ = new nc::GLShaderProgram();
#ifndef WITH_EMBEDDED_SHADERS
	texProgram_->attachShader(GL_VERTEX_SHADER, (nc::IFile::dataPath() + "shaders/texture_vs.glsl").data());
	texProgram_->attachShader(GL_FRAGMENT_SHADER, (nc::IFile::dataPath() + "shaders/texture_fs.glsl").data());
#else
	texProgram_->attachShaderFromString(GL_VERTEX_SHADER, nc::ShaderStrings::texture_vs);
	texProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, nc::ShaderStrings::texture_fs);
#endif
	texProgram_->bindAttribLocation(ATTRIB_POSITION, "aPosition");
	texProgram_->bindAttribLocation(ATTRIB_TEXCOORDS, "aTexCoords");
	texProgram_->link();
	texProgram_->use();
	texUniforms_ = new nc::GLShaderUniforms(texProgram_);
	texUniforms_->uniform("texture")->setIntValue(0);
	texUniforms_->uniform("color")->setFloatValue(1.0f, 1.0f, 1.0f, 1.0f);
	texAttributes_ = new nc::GLShaderAttributes(texProgram_);


	texture_ = new nc::GLTexture(GL_TEXTURE_2D);
	texture_->texImage2D(0, GL_RGB, FboSize, FboSize, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	texture_->texParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	fbo_ = new nc::GLFramebufferObject();
	fbo_->attachTexture(*texture_, GL_COLOR_ATTACHMENT0);
	fbo_->attachRenderbuffer(GL_DEPTH_COMPONENT16, FboSize, FboSize, GL_DEPTH_ATTACHMENT);
	if (fbo_->isStatusComplete() == false)
		LOGE("Framebuffer object status is not complete\n");

	vboTri_ = new nc::GLBufferObject(GL_ARRAY_BUFFER);
	vboTri_->bufferData(sizeof(triVertices), triVertices, GL_STATIC_DRAW);
	colorAttributes_->attribute("aPosition")->setVboParameters(sizeof(VertexFormatCol), reinterpret_cast<void *>(offsetof(VertexFormatCol, position)));
	colorAttributes_->attribute("aColor")->setVboParameters(sizeof(VertexFormatCol), reinterpret_cast<void *>(offsetof(VertexFormatCol, color)));
	colorAttributes_->defineVertexPointers(vboTri_);

	vboCube_ = new nc::GLBufferObject(GL_ARRAY_BUFFER);
	vboCube_->bufferData(sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	texAttributes_->attribute("aPosition")->setVboParameters(sizeof(VertexFormatTex), reinterpret_cast<void *>(offsetof(VertexFormatTex, position)));
	texAttributes_->attribute("aTexCoords")->setVboParameters(sizeof(VertexFormatTex), reinterpret_cast<void *>(offsetof(VertexFormatTex, texcoords)));
	texAttributes_->defineVertexPointers(vboCube_);

	iboCube_ = new nc::GLBufferObject(GL_ELEMENT_ARRAY_BUFFER);
	iboCube_->bufferData(sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

	width_ = nc::theApplication().widthInt();
	height_ = nc::theApplication().heightInt();
	glViewport(0, 0, width_, height_);
	glEnable(GL_DEPTH_TEST);

	angle_ = 0.0f;
}

void MyEventHandler::onFrameStart()
{
	// Triangle
	glViewport(0, 0, FboSize, FboSize);
	colorProgram_->use();

	projection_ = nc::Matrix4x4f::ortho(-1.5f, 1.5f, -1.5f, 1.5f, 1.0f, 4.0f);
	colorUniforms_->uniform("projection")->setFloatVector(projection_.data());
	modelView_ = nc::Matrix4x4f::rotationZ(angle_);
	colorUniforms_->uniform("modelView")->setFloatVector(modelView_.data());
	colorUniforms_->commitUniforms();

	fbo_->bind(GL_FRAMEBUFFER);
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	vboTri_->bind();
	iboCube_->unbind();
	texture_->unbind();
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// Cube
	glViewport(0, 0, width_, height_);
	texProgram_->use();

	projection_ = nc::Matrix4x4f::perspective(60.0f, width_ / static_cast<float>(height_), 1.0f, 20.0f);
	texUniforms_->uniform("projection")->setFloatVector(projection_.data());
	modelView_ = nc::Matrix4x4f::translation(0.0f, 0.0f, -5.0f);
	modelView_ *= nc::Matrix4x4f::rotationY(angle_);
	modelView_ *= nc::Matrix4x4f::rotationZ(angle_);
	texUniforms_->uniform("modelView")->setFloatVector(modelView_.data());
	texUniforms_->commitUniforms();

	fbo_->unbind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	vboCube_->bind();
	iboCube_->bind();
	texture_->bind();
	glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_SHORT, 0);

	angle_ += 20.0f * nc::theApplication().interval();
}

void MyEventHandler::onShutdown()
{
	delete iboCube_;
	delete vboCube_;
	delete vboTri_;
	delete fbo_;
	delete texture_;
	delete texAttributes_;
	delete texUniforms_;
	delete texProgram_;
	delete colorAttributes_;
	delete colorUniforms_;
	delete colorProgram_;
}

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::ESCAPE || event.sym == nc::KeySym::Q)
		nc::theApplication().quit();
	else if (event.sym == nc::KeySym::SPACE)
		nc::theApplication().togglePause();
}
