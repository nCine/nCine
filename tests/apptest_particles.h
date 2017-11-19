#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "IAppEventHandler.h"
#include "IInputEventHandler.h"
#include "Vector2.h"

namespace ncine {

class AppConfiguration;
class Texture;
class ParticleSystem;
class Timer;

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
	void onShutdown() override;

#ifdef __ANDROID__
	void onTouchDown(const nc::TouchEvent &event) override;
	void onTouchMove(const nc::TouchEvent &event) override;
	void onAcceleration(const nc::AccelerometerEvent &event) override;
#endif
	void onKeyReleased(const nc::KeyboardEvent &event) override;
	void onMouseButtonPressed(const nc::MouseEvent &event) override;
	void onMouseMoved(const nc::MouseState &state) override;

	void onJoyMappedAxisMoved(const nc::JoyMappedAxisEvent &event) override;
	void onJoyDisconnected(const nc::JoyConnectionEvent &event) override;

  private:
	static const unsigned int NumParticles = 50;

	nc::Texture *texture_;
	nc::ParticleSystem *particleSystem_;
	nc::Timer *emitTimer_;
	nc::Vector2f emitVector_;

	nc::Vector2f joyVectorLeft_;
	nc::Vector2f joyVectorRight_;
};

#endif
