#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include <ncine/IAppEventHandler.h>
#include <ncine/IInputEventHandler.h>
#include <nctl/StaticArray.h>
#include <ncine/Vector2.h>
#include <ncine/TimeStamp.h>

namespace nctl {

class String;

}

namespace ncine {

class AppConfiguration;
class Texture;
class Sprite;
class SceneNode;
class Font;
class TextNode;
class Viewport;
class Camera;

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
	void onTouchMove(const nc::TouchEvent &event) override;
	void onPointerDown(const nc::TouchEvent &event) override;
#endif
	void onKeyReleased(const nc::KeyboardEvent &event) override;
	void onMouseButtonPressed(const nc::MouseEvent &event) override;
	void onMouseButtonReleased(const nc::MouseEvent &event) override;
	void onMouseMoved(const nc::MouseState &state) override;
	void onScrollInput(const nc::ScrollEvent &event) override;

	void onJoyMappedAxisMoved(const nc::JoyMappedAxisEvent &event) override;
	void onJoyMappedButtonReleased(const nc::JoyMappedButtonEvent &event) override;
	void onJoyDisconnected(const nc::JoyConnectionEvent &event) override;

  private:
	static const unsigned int NumTextures = 4;
	static const unsigned int NumSprites = 500;
	static const unsigned int NumViewports = 2;

	bool pause_;
	float angle_;
	nc::Vector2f camPos_;
	float camRot_;
	float camScale_;

	nc::Vector2f scrollOrigin_;
	nc::Vector2f scrollMove_;
	nc::Vector2f scrollOrigin2_;
	nc::Vector2f scrollMove2_;
	nc::TimeStamp lastClickTime_;
	nc::Vector2f joyVectorLeft_;
	nc::Vector2f joyVectorRight_;

	nctl::StaticArray<nctl::UniquePtr<nc::Texture>, NumTextures> textures_;
	nctl::UniquePtr<nc::Font> font_;
	nctl::UniquePtr<nc::SceneNode> cameraNode_;

	nctl::UniquePtr<nctl::String> debugString_;
	nctl::UniquePtr<nc::TextNode> debugText_;

	nctl::StaticArray<nctl::UniquePtr<nc::Viewport>, NumViewports> viewports_;
	nctl::StaticArray<nctl::UniquePtr<nc::Camera>, NumViewports + 1> cameras_;
	nctl::StaticArray<nctl::UniquePtr<nc::SceneNode>, NumViewports> vpNodes_;
	nctl::StaticArray<nctl::UniquePtr<nc::Sprite>, NumViewports> vpSprites_;

	nctl::StaticArray<nctl::UniquePtr<nc::Sprite>, NumSprites> sprites_;
	nctl::StaticArray<nc::Vector2f, NumSprites> spritePos_;

	void resetCamera();
	void checkClick(float x, float y);
};

#endif
