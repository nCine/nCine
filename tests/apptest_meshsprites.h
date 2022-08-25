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
class Font;
class SceneNode;
class Viewport;
class Camera;
class TextNode;
class Sprite;
class MeshSprite;

}

namespace nc = ncine;

/// My nCine event handler
class MyEventHandler :
    public nc::IAppEventHandler,
    public nc::IInputEventHandler
{
  public:
	static const unsigned int NumTextures = 4;
	static const unsigned int NumSprites = 500;

	void onPreInit(nc::AppConfiguration &config) override;
	void onInit() override;
	void onFrameStart() override;

#ifdef __ANDROID__
	void onTouchDown(const nc::TouchEvent &event) override;
	void onTouchUp(const nc::TouchEvent &event) override;
	void onTouchMove(const nc::TouchEvent &event) override;
#endif
	void onKeyReleased(const nc::KeyboardEvent &event) override;
	void onMouseButtonPressed(const nc::MouseEvent &event) override;
	void onMouseButtonReleased(const nc::MouseEvent &event) override;
	void onScrollInput(const nc::ScrollEvent &event) override;

	void onJoyMappedAxisMoved(const nc::JoyMappedAxisEvent &event) override;
	void onJoyMappedButtonReleased(const nc::JoyMappedButtonEvent &event) override;
	void onJoyDisconnected(const nc::JoyConnectionEvent &event) override;

  private:
	enum Direction
	{
		RIGHT = 0,
		DOWN,
		LEFT,
		UP
	};

	bool withAtlas_;
	bool withViewport_;
	bool pause_;
	int animDivider_;
	float angle_;
	float camScale_;

	nc::Vector2f scrollOrigin_;
	nc::Vector2f scrollMove_;
	nc::TimeStamp lastClickTime_;
	nc::Vector2f joyVectorLeft_;
	nc::Vector2f joyVectorRight_;

	nctl::StaticArray<nctl::UniquePtr<nc::Texture>, NumTextures> textures_;
	nctl::UniquePtr<nc::Texture> megaTexture_;
	nctl::UniquePtr<nc::Font> font_;
	nctl::UniquePtr<nc::SceneNode> cameraNode_;
	nctl::UniquePtr<nc::Viewport> viewport_;
	nctl::UniquePtr<nc::Camera> camera_;

	nctl::UniquePtr<nctl::String> debugString_;
	nctl::UniquePtr<nc::TextNode> debugText_;

	bool updateActiveSprites_;
	float activeSpritesFloat_;
	bool meshSpritesEnabled_;
	nctl::StaticArray<nctl::UniquePtr<nc::Sprite>, NumSprites> sprites_;
	nctl::StaticArray<nctl::UniquePtr<nc::MeshSprite>, NumSprites> meshSprites_;
	nctl::StaticArray<nctl::UniquePtr<nc::MeshSprite>, NumSprites> transpMeshSprites_;

	void resetCamera();
	void setupAtlas();
	void setupTextures();
	void setupViewport();
	void toggleSpritesType();
	void updateEnabledSprites();
	void checkClick(float x, float y);
};

#endif
