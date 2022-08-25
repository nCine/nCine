#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include <ncine/IAppEventHandler.h>
#include <ncine/IInputEventHandler.h>
#include "ncine/TextNode.h"
#include <nctl/StaticString.h>
#include <nctl/StaticArray.h>

namespace ncine {

class AppConfiguration;
class SceneNode;
class Texture;
class Sprite;
class Font;
class TexNode;

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
	static const unsigned int NumTextures = 4;
	static const int NumRowSprites = 8;
	static const int NumColSprites = 8;

	bool pauseSprites_;
	bool pauseGroup_;
	float angleSprites_;
	float angleGroup_;

	nctl::StaticString<128> debugString_;
	nctl::UniquePtr<nc::Font> font_;
	nctl::UniquePtr<nc::TextNode> debugText_;

	nctl::UniquePtr<nc::Texture> megaTexture_;
	nctl::UniquePtr<nc::SceneNode> dummy_;
	nctl::UniquePtr<nc::SceneNode> parent_;
	nctl::StaticArray<nctl::UniquePtr<nc::Sprite>, NumColSprites * NumRowSprites> sprites_;

	void checkClick(float x, float y);
};

#endif
