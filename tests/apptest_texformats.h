#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "IAppEventHandler.h"
#include "IInputEventHandler.h"
#include "StaticArray.h"
#include "ncString.h"

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
	void onShutdown() override;

#ifdef __ANDROID__
	void onTouchUp(const nc::TouchEvent &event) override;
#endif
	void onKeyReleased(const nc::KeyboardEvent &event) override;
	void onMouseButtonReleased(const nc::MouseEvent &event) override;

	void onJoyMappedAxisMoved(const nc::JoyMappedAxisEvent &event) override;
	void onJoyDisconnected(const nc::JoyConnectionEvent &event) override;

  private:
	static const int MaxTexFormats = 32;

	enum Direction
	{
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	static int selected_;
	static int newSelection_;
	static float scale_;
	static float newScale_;

	nc::SceneNode *dummy_;
	nc::StaticArray<nc::String, MaxTexFormats> filenames_;
	nc::StaticArray<nc::Texture *, MaxTexFormats> textures_;
	nc::StaticArray<nc::Sprite *, MaxTexFormats> sprites_;
	nc::Font *font_;
	nc::TextNode *textNode_;

	void handleInput(Direction direction);
	void handleCoordInput(float x, float y);
};

#endif
