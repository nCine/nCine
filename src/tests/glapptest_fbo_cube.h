#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "IAppEventHandler.h"
#include "IInputEventHandler.h"
#include "Matrix4x4.h"

namespace ncine {

class AppConfiguration;
class GLShaderProgram;
class GLShaderUniforms;
class GLTexture;
class GLFramebufferObject;
class GLBufferObject;

}

namespace nc = ncine;

/// My nCine event handler
class MyEventHandler :
    public nc::IAppEventHandler,
    public nc::IInputEventHandler
{
  public:
	void onPreInit(nc::AppConfiguration &config) override;
	void onInit() override;
	void onFrameStart() override;

#ifdef __ANDROID__
	void onTouchDown(const nc::TouchEvent &event) override;
	void onTouchUp(const nc::TouchEvent &event) override;
	void onPointerDown(const nc::TouchEvent &event) override;
	void onPointerUp(const nc::TouchEvent &event) override;
#endif
	void onKeyReleased(const nc::KeyboardEvent &event) override;
	void onMouseButtonPressed(const nc::MouseEvent &event) override;
	void onMouseButtonReleased(const nc::MouseEvent &event) override;

	void onJoyMappedButtonReleased(const nc::JoyMappedButtonEvent &event) override;

  private:
	static const int FboSize = 256;

	bool pauseTri_;
	bool pauseCube_;
	float angleTri_;
	float angleCube_;
	int width_, height_;
	static const int UniformsBufferSize = 276;
	unsigned char uniformsBuffer_[UniformsBufferSize];

	nctl::UniquePtr<nc::GLShaderProgram> colorProgram_;
	nctl::UniquePtr<nc::GLShaderUniforms> colorUniforms_;
	nctl::UniquePtr<nc::GLShaderProgram> texProgram_;
	nctl::UniquePtr<nc::GLShaderUniforms> texUniforms_;
	nctl::UniquePtr<nc::GLTexture> texture_;
	nctl::UniquePtr<nc::GLFramebufferObject> fbo_;
	nctl::UniquePtr<nc::GLBufferObject> vboTri_;
	nctl::UniquePtr<nc::GLBufferObject> vboCube_;
	nctl::UniquePtr<nc::GLBufferObject> iboCube_;

	nc::Matrix4x4f projection_;
	nc::Matrix4x4f modelView_;
};

#endif
