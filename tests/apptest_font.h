#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "IAppEventHandler.h"
#include "IInputEventHandler.h"

namespace ncine {

class AppConfiguration;
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
	virtual void onPreInit(nc::AppConfiguration &config);
	virtual void onInit();
	virtual void onShutdown();

	virtual void onKeyReleased(const nc::KeyboardEvent &event);

  private:
	nc::Font *font1_;
	nc::Font *font2_;
	nc::Font *font3_;
	nc::TextNode *text1_;
	nc::TextNode *text2_;
	nc::TextNode *text3_;
	nc::TextNode *text4_;
	nc::TextNode *text5_;
	nc::TextNode *text6_;
};

#endif
