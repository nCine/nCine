#ifndef CLASS_LUAEVENTHANDLER
#define CLASS_LUAEVENTHANDLER

#include "IAppEventHandler.h"
#include "IInputEventHandler.h"

namespace ncine {

class LuaStateManager;

/// The Lua event handler
class LuaEventHandler :
    public IAppEventHandler,
    public IInputEventHandler
{
  public:
	void onPreInit(AppConfiguration &config) override;
	void onInit() override;
	void onFrameStart() override;
	void onPostUpdate() override;
	void onDrawViewport(Viewport &viewport) override;
	void onFrameEnd() override;
	void onShutdown() override;
	void onSuspend() override;
	void onResume() override;

	void onKeyPressed(const KeyboardEvent &event) override;
	void onKeyReleased(const KeyboardEvent &event) override;

	void onMouseButtonPressed(const MouseEvent &event) override;
	void onMouseButtonReleased(const MouseEvent &event) override;
	void onMouseMoved(const MouseState &state) override;
	void onScrollInput(const ScrollEvent &event) override;

	void onJoyButtonPressed(const JoyButtonEvent &event) override;
	void onJoyButtonReleased(const JoyButtonEvent &event) override;
	void onJoyHatMoved(const JoyHatEvent &event) override;
	void onJoyAxisMoved(const JoyAxisEvent &event) override;

	void onJoyMappedButtonPressed(const JoyMappedButtonEvent &event) override;
	void onJoyMappedButtonReleased(const JoyMappedButtonEvent &event) override;
	void onJoyMappedAxisMoved(const JoyMappedAxisEvent &event) override;

	void onJoyConnected(const JoyConnectionEvent &event) override;
	void onJoyDisconnected(const JoyConnectionEvent &event) override;

	static LuaStateManager *luaState_;
};

}

#endif
