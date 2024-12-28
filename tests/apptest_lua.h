#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include <ncine/IAppEventHandler.h>
#include <ncine/IInputEventHandler.h>
#include <ncine/LuaStateManager.h>

#define SUPPRESS_ON_SCREEN_ERRORS (0)

#if !SUPPRESS_ON_SCREEN_ERRORS
namespace ncine {

class Font;
class TextNode;

}
#endif

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
	void onPostUpdate() override;
	void onDrawViewport(nc::Viewport &viewport) override;
	void onFrameEnd() override;
	void onResizeWindow(int width, int height) override;
	void onChangeScalingFactor(float factor) override;
	void onShutdown() override;
	void onSuspend() override;
	void onResume() override;

	void onKeyPressed(const nc::KeyboardEvent &event) override;
	void onKeyReleased(const nc::KeyboardEvent &event) override;
	void onTextInput(const nc::TextInputEvent &event) override;

	void onTouchDown(const nc::TouchEvent &event) override;
	void onTouchUp(const nc::TouchEvent &event) override;
	void onTouchMove(const nc::TouchEvent &event) override;
	void onPointerDown(const nc::TouchEvent &event) override;
	void onPointerUp(const nc::TouchEvent &event) override;
#ifdef __ANDROID__
	void onAcceleration(const nc::AccelerometerEvent &event) override;
#endif

	void onMouseButtonPressed(const nc::MouseEvent &event) override;
	void onMouseButtonReleased(const nc::MouseEvent &event) override;
	void onMouseMoved(const nc::MouseState &state) override;
	void onScrollInput(const nc::ScrollEvent &event) override;

	void onJoyButtonPressed(const nc::JoyButtonEvent &event) override;
	void onJoyButtonReleased(const nc::JoyButtonEvent &event) override;
	void onJoyHatMoved(const nc::JoyHatEvent &event) override;
	void onJoyAxisMoved(const nc::JoyAxisEvent &event) override;

	void onJoyMappedButtonPressed(const nc::JoyMappedButtonEvent &event) override;
	void onJoyMappedButtonReleased(const nc::JoyMappedButtonEvent &event) override;
	void onJoyMappedAxisMoved(const nc::JoyMappedAxisEvent &event) override;

	void onJoyConnected(const nc::JoyConnectionEvent &event) override;
	void onJoyDisconnected(const nc::JoyConnectionEvent &event) override;

	bool onQuitRequest() override;

  private:
	nc::LuaStateManager luaState_;
#if !SUPPRESS_ON_SCREEN_ERRORS
	nctl::UniquePtr<nc::Font> font_;
	nctl::UniquePtr<nc::TextNode> text_;
#endif

	enum class LoadMode
	{
		NORMAL,
		FAST
	};

	void expose(lua_State *L);
	static int reloadScript(lua_State *L);
	static int showGui(lua_State *L);
	static int isGuiHidden(lua_State *L);

	bool runScript(LoadMode mode);
};

#endif
