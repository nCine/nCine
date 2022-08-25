#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include <ncine/IAppEventHandler.h>
#include <ncine/IInputEventHandler.h>
#include <nctl/StaticArray.h>
#include <nctl/String.h>

namespace ncine {

class AppConfiguration;
class SceneNode;
class Texture;
class Sprite;
class Font;
class TextNode;

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

#ifdef __ANDROID__
	void onTouchUp(const nc::TouchEvent &event) override;
#endif
	void onKeyReleased(const nc::KeyboardEvent &event) override;
	void onMouseButtonReleased(const nc::MouseEvent &event) override;

	void onJoyMappedAxisMoved(const nc::JoyMappedAxisEvent &event) override;
	void onJoyMappedButtonReleased(const nc::JoyMappedButtonEvent &event) override;
	void onJoyDisconnected(const nc::JoyConnectionEvent &event) override;

  private:
	static const int MaxTexFormats = 64;

	enum class Direction
	{
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	int selection_;
	int newSelection_;
	float scale_;
	float newScale_;

	nctl::UniquePtr<nc::Font> font_;
	nctl::UniquePtr<nc::SceneNode> dummy_;
	nctl::UniquePtr<nc::TextNode> textNode_;
	nctl::StaticArray<nctl::String, MaxTexFormats> filenames_;
	nctl::StaticArray<nctl::UniquePtr<nc::Texture>, MaxTexFormats> textures_;
	nctl::StaticArray<nctl::UniquePtr<nc::Sprite>, MaxTexFormats> sprites_;

	void handleInput(Direction direction);
	void handleCoordInput(float x, float y);
};

#endif
