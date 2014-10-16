#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "ncIAppEventHandler.h"
#include "ncIInputEventHandler.h"
#include "ncVector2f.h"
class ncTexture;
class ncParticleSystem;
class ncTimer;

/// My nCine application
class MyEventHandler
	: public ncIAppEventHandler,
	  public ncIInputEventHandler
{
  public:
	virtual void onInit();
	virtual void onFrameStart();
	virtual void onFrameEnd();
	virtual void onShutdown();

#ifdef __ANDROID__
	virtual void onTouchDown(const ncTouchEvent &event);
	virtual void onTouchMove(const ncTouchEvent &event);
	virtual void onAcceleration(const ncAccelerometerEvent &event);
#else
	virtual void onKeyReleased(const ncKeyboardEvent &event);
	virtual void onMouseButtonPressed(const ncMouseEvent &event);
	virtual void onMouseMoved(const ncMouseState &state);
#endif

  private:
	static const int NumParticles = 50;

	ncTexture *texture_;
	ncParticleSystem *particleSystem_;
	ncTimer *emitTimer_;
	ncVector2f emitVector_;
};

#endif
