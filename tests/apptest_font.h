#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include <ncine/IAppEventHandler.h>
#include <ncine/IInputEventHandler.h>
#include <nctl/StaticArray.h>

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
	void onPreInit(nc::AppConfiguration &config) override;
	void onInit() override;
	void onFrameStart() override;

	void onKeyReleased(const nc::KeyboardEvent &event) override;

  private:
	static const unsigned int NumFonts = 3;
	static const unsigned int NumTexts = 6;

	nctl::StaticArray<nctl::UniquePtr<nc::Font>, NumFonts> fonts_;
	nctl::StaticArray<nctl::UniquePtr<nc::TextNode>, NumTexts> texts_;
};

#endif
