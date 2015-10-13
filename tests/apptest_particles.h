#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "IAppEventHandler.h"
#include "IInputEventHandler.h"
#include "Vector2.h"

namespace ncine {

class Texture;
class ParticleSystem;
class Timer;

}

namespace nc = ncine;

/// My nCine application
class MyEventHandler
	: public nc::IAppEventHandler,
	  public nc::IInputEventHandler
{
  public:
	virtual void onInit();
	virtual void onFrameStart();
	virtual void onFrameEnd();
	virtual void onShutdown();

#ifdef __ANDROID__
	virtual void onTouchDown(const nc::TouchEvent &event);
	virtual void onTouchMove(const nc::TouchEvent &event);
	virtual void onAcceleration(const nc::AccelerometerEvent &event);
#else
	virtual void onKeyReleased(const nc::KeyboardEvent &event);
	virtual void onMouseButtonPressed(const nc::MouseEvent &event);
	virtual void onMouseMoved(const nc::MouseState &state);
#endif

  private:
	static const unsigned int NumParticles = 50;

	nc::Texture *texture_;
	nc::ParticleSystem *particleSystem_;
	nc::Timer *emitTimer_;
	nc::Vector2f emitVector_;
};

#endif
