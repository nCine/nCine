#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include <ncine/IAppEventHandler.h>
#include <ncine/IInputEventHandler.h>
#include <nctl/StaticArray.h>

namespace ncine {

class AppConfiguration;
class SceneNode;
class Texture;
class Sprite;

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
#endif
	void onKeyReleased(const nc::KeyboardEvent &event) override;
	void onMouseButtonPressed(const nc::MouseEvent &event) override;
	void onMouseButtonReleased(const nc::MouseEvent &event) override;

  private:
	static const unsigned int NumTextures = 4;
	static const int NumRowSprites = 8;
	static const int NumColSprites = 8;

	bool pause_;
	float angle_;
	nctl::UniquePtr<nc::Texture> megaTexture_;
	nctl::UniquePtr<nc::SceneNode> dummy_;
	nctl::StaticArray<nctl::UniquePtr<nc::Sprite>, NumColSprites * NumRowSprites> sprites_;
};

#endif
