#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "IAppEventHandler.h"
#include "IInputEventHandler.h"

#include "LuaStateManager.h"

namespace ncine {

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
	MyEventHandler();
	void onPreInit(nc::AppConfiguration &config) override;
	void onInit() override;
	void onFrameStart() override;

#ifdef __ANDROID__
	void onTouchDown(const nc::TouchEvent &event) override;
	void onTouchMove(const nc::TouchEvent &event) override;
#endif

	void onKeyReleased(const nc::KeyboardEvent &event) override;
	void onMouseButtonPressed(const nc::MouseEvent &event) override;
	void onMouseMoved(const nc::MouseState &state) override;

  private:
	nc::LuaStateManager luaState_;
	unsigned int variationIndex_;

	nctl::UniquePtr<nc::Texture> texture_;
	nctl::UniquePtr<nc::ParticleSystem> particleSystem_;
	nctl::UniquePtr<nc::Timer> emitTimer_;

	void runScript();
};

#endif
