#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "IAppEventHandler.h"
#include "IInputEventHandler.h"
#include "StaticArray.h"
#include "ncString.h"

namespace ncine {

class SceneNode;
class Texture;
class Sprite;
class Font;
class TextNode;

}

namespace nc = ncine;

/// My nCine event handler
class MyEventHandler
	: public nc::IAppEventHandler,
	  public nc::IInputEventHandler
{
  public:
	virtual void onInit();
	virtual void onShutdown();

#ifdef __ANDROID__
	virtual void onTouchUp(const nc::TouchEvent &event);
#else
	virtual void onKeyReleased(const nc::KeyboardEvent &event);
	virtual void onMouseButtonReleased(const nc::MouseEvent &event);
#endif

  private:
	static const int MaxTexFormats = 32;

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

	void handleInput(float x, float y);
};

#endif
