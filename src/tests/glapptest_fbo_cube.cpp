#include <stddef.h> // for offsetof()
#include "glapptest_fbo_cube.h"
#include <ncine/Application.h>
#include <ncine/AppConfiguration.h>
#include <ncine/GLShaderProgram.h>
#include <ncine/GLShaderUniforms.h>
#include <ncine/GLTexture.h>
#include <ncine/GLFramebufferObject.h>
#include <ncine/GLVertexArrayObject.h>
#include <ncine/GLBufferObject.h>
#include <ncine/GLDepthTest.h>
#include <ncine/GLClearColor.h>
#include <ncine/GLViewport.h>
#include "../../tests/apptest_datapath.h"

#ifdef WITH_EMBEDDED_SHADERS
	#include "shader_strings.h"
#else
	#include "FileSystem.h" // for dataPath()
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

const VertexFormatCol triVertices[] = {
	// clang-format off
	{{ 0.0f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
	{{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
	{{ 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
	// clang-format on
};

const VertexFormatTex cubeVertices[] = {
	// clang-format off
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
	// clang-format on
};

const GLushort cubeIndices[] = {
	// clang-format off
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
	// clang-format on
};

}

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
	return nctl::makeUnique<MyEventHandler>();
}

void MyEventHandler::onPreInit(nc::AppConfiguration &config)
{
	config.withScenegraph = false;
	config.withAudio = false;
	config.withThreads = false;
	config.vaoPoolSize = 2;

	setDataPath(config);
}

void MyEventHandler::onInit()
{
	colorProgram_ = nctl::makeUnique<nc::GLShaderProgram>(nc::GLShaderProgram::QueryPhase::DEFERRED);
#ifndef WITH_EMBEDDED_SHADERS
	colorProgram_->attachShader(GL_VERTEX_SHADER, (nc::fs::dataPath() + "shaders/vcolor_vs.glsl").data());
	colorProgram_->attachShader(GL_FRAGMENT_SHADER, (nc::fs::dataPath() + "shaders/vcolor_fs.glsl").data());
#else
	colorProgram_->attachShaderFromString(GL_VERTEX_SHADER, nc::ShaderStrings::vcolor_vs);
	colorProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, nc::ShaderStrings::vcolor_fs);
#endif
	colorProgram_->link(nc::GLShaderProgram::Introspection::ENABLED);
	colorUniforms_ = nctl::makeUnique<nc::GLShaderUniforms>(colorProgram_.get());
	colorUniforms_->setUniformsDataPointer(uniformsBuffer_);

	texProgram_ = nctl::makeUnique<nc::GLShaderProgram>(nc::GLShaderProgram::QueryPhase::DEFERRED);
#ifndef WITH_EMBEDDED_SHADERS
	texProgram_->attachShader(GL_VERTEX_SHADER, (nc::IFile::fs() + "shaders/texture_vs.glsl").data());
	texProgram_->attachShader(GL_FRAGMENT_SHADER, (nc::IFile::fs() + "shaders/texture_fs.glsl").data());
#else
	texProgram_->attachShaderFromString(GL_VERTEX_SHADER, nc::ShaderStrings::texture_vs);
	texProgram_->attachShaderFromString(GL_FRAGMENT_SHADER, nc::ShaderStrings::texture_fs);
#endif
	texProgram_->link(nc::GLShaderProgram::Introspection::ENABLED);
	texUniforms_ = nctl::makeUnique<nc::GLShaderUniforms>(texProgram_.get());
	texUniforms_->setUniformsDataPointer(&uniformsBuffer_[colorProgram_->uniformsSize()]);
	texUniforms_->uniform("uTexture")->setIntValue(0);
	texUniforms_->uniform("uColor")->setFloatValue(1.0f, 1.0f, 1.0f, 1.0f);

	FATAL_ASSERT(UniformsBufferSize >= colorProgram_->uniformsSize() + texProgram_->uniformsSize());

	texture_ = nctl::makeUnique<nc::GLTexture>(GL_TEXTURE_2D);
	texture_->texImage2D(0, GL_RGB, FboSize, FboSize, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	texture_->texParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	fbo_ = nctl::makeUnique<nc::GLFramebufferObject>();
	fbo_->attachTexture(*texture_, GL_COLOR_ATTACHMENT0);
	fbo_->attachRenderbuffer(GL_DEPTH_COMPONENT16, FboSize, FboSize, GL_DEPTH_ATTACHMENT);
	if (fbo_->isStatusComplete() == false)
		LOGE("Framebuffer object status is not complete\n");

	vboTri_ = nctl::makeUnique<nc::GLBufferObject>(GL_ARRAY_BUFFER);
	vboTri_->bufferData(sizeof(triVertices), triVertices, GL_STATIC_DRAW);
	colorProgram_->attribute("aPosition")->setVboParameters(sizeof(VertexFormatCol), reinterpret_cast<void *>(offsetof(VertexFormatCol, position)));
	colorProgram_->attribute("aColor")->setVboParameters(sizeof(VertexFormatCol), reinterpret_cast<void *>(offsetof(VertexFormatCol, color)));

	vboCube_ = nctl::makeUnique<nc::GLBufferObject>(GL_ARRAY_BUFFER);
	vboCube_->bufferData(sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	texProgram_->attribute("aPosition")->setVboParameters(sizeof(VertexFormatTex), reinterpret_cast<void *>(offsetof(VertexFormatTex, position)));
	texProgram_->attribute("aTexCoords")->setVboParameters(sizeof(VertexFormatTex), reinterpret_cast<void *>(offsetof(VertexFormatTex, texcoords)));

	iboCube_ = nctl::makeUnique<nc::GLBufferObject>(GL_ELEMENT_ARRAY_BUFFER);
	iboCube_->bufferData(sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

	width_ = nc::theApplication().widthInt();
	height_ = nc::theApplication().heightInt();
	nc::GLViewport::setRect(0, 0, width_, height_);
	nc::GLDepthTest::enable();

	pauseTri_ = false;
	pauseCube_ = false;
	angleTri_ = 0.0f;
	angleCube_ = 0.0f;
}

void MyEventHandler::onFrameStart()
{
	// Triangle
	nc::GLViewport::setRect(0, 0, FboSize, FboSize);
	colorProgram_->use();

	projection_ = nc::Matrix4x4f::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
	colorUniforms_->uniform("uProjectionMatrix")->setFloatVector(projection_.data());
	modelView_ = nc::Matrix4x4f::rotationZ(angleTri_);
	colorUniforms_->uniform("uModelViewMatrix")->setFloatVector(modelView_.data());
	colorUniforms_->commitUniforms();

	fbo_->bind(GL_FRAMEBUFFER);
	nc::GLClearColor::setColor(0.5f, 0.5f, 0.5f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	colorProgram_->defineVertexFormat(vboTri_.get());
	vboTri_->bind();
	iboCube_->unbind();
	texture_->unbind();
	glDrawArrays(GL_TRIANGLES, 0, 3);
	const GLenum invalidAttachment = GL_DEPTH_ATTACHMENT;
	fbo_->invalidate(1, &invalidAttachment);

	// Cube
	nc::GLViewport::setRect(0, 0, width_, height_);
	texProgram_->use();

	projection_ = nc::Matrix4x4f::perspective(60.0f, width_ / static_cast<float>(height_), 1.0f, 20.0f);
	texUniforms_->uniform("uProjectionMatrix")->setFloatVector(projection_.data());
	modelView_ = nc::Matrix4x4f::translation(0.0f, 0.0f, -5.0f);
	modelView_ *= nc::Matrix4x4f::rotationY(angleCube_);
	modelView_ *= nc::Matrix4x4f::rotationZ(angleCube_);
	texUniforms_->uniform("uModelViewMatrix")->setFloatVector(modelView_.data());
	texUniforms_->commitUniforms();

	fbo_->unbind();
	nc::GLClearColor::setColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	texProgram_->defineVertexFormat(vboCube_.get());
	vboCube_->bind();
	iboCube_->bind();
	texture_->bind();
	glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_SHORT, nullptr);

	if (pauseTri_ == false)
		angleTri_ += 20.0f * nc::theApplication().interval();
	if (pauseCube_ == false)
		angleCube_ += 20.0f * nc::theApplication().interval();
}

#ifdef __ANDROID__
void MyEventHandler::onTouchDown(const nc::TouchEvent &event)
{
	pauseTri_ = true;
}

void MyEventHandler::onTouchUp(const nc::TouchEvent &event)
{
	pauseTri_ = false;
}

void MyEventHandler::onPointerDown(const nc::TouchEvent &event)
{
	if (event.count == 2)
	{
		pauseTri_ = false;
		pauseCube_ = true;
	}
}

void MyEventHandler::onPointerUp(const nc::TouchEvent &event)
{
	if (event.count == 2)
	{
		pauseTri_ = true;
		pauseCube_ = false;
	}
}
#endif

void MyEventHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
	if (event.sym == nc::KeySym::ESCAPE)
		nc::theApplication().quit();
}

void MyEventHandler::onMouseButtonPressed(const nc::MouseEvent &event)
{
	if (event.isLeftButton())
		pauseTri_ = true;
	else if (event.isRightButton())
		pauseCube_ = true;
}

void MyEventHandler::onMouseButtonReleased(const nc::MouseEvent &event)
{
	if (event.isLeftButton())
		pauseTri_ = false;
	else if (event.isRightButton())
		pauseCube_ = false;
}

void MyEventHandler::onJoyMappedButtonReleased(const nc::JoyMappedButtonEvent &event)
{
	if (event.buttonName == nc::ButtonName::LBUMPER)
		pauseTri_ = !pauseTri_;
	else if (event.buttonName == nc::ButtonName::RBUMPER)
		pauseCube_ = !pauseCube_;
	else if (event.buttonName == nc::ButtonName::START)
	{
		pauseTri_ = !pauseTri_;
		pauseCube_ = !pauseCube_;
	}
	else if (event.buttonName == nc::ButtonName::GUIDE)
		nc::theApplication().quit();
}
