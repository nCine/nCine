#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include <ncine/IAppEventHandler.h>
#include <ncine/IInputEventHandler.h>
#include <nctl/StaticArray.h>
#include <ncine/Vector2.h>

namespace ncine {

class AppConfiguration;
class Texture;
class Sprite;
class AudioBuffer;
class AudioBufferPlayer;
class AudioStreamPlayer;
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
	static const int NumTextures = 4;
	static const int NumSounds = 5;
	static const int NumFonts = 5;

	void onPreInit(nc::AppConfiguration &config) override;
	void onInit() override;
	void onFrameStart() override;
	void onKeyReleased(const nc::KeyboardEvent &event) override;

  private:
	static const unsigned int NumSprites = 4;

	nctl::StaticArray<nctl::UniquePtr<nc::Texture>, NumTextures> textures_;
	nctl::StaticArray<nctl::UniquePtr<nc::Sprite>, NumSprites> sprites_;

	nctl::UniquePtr<nc::AudioBuffer> audioBuffer_;
	nctl::UniquePtr<nc::AudioBufferPlayer> bufferPlayer_;
	nctl::UniquePtr<nc::AudioStreamPlayer> streamPlayer_;

	nctl::UniquePtr<nc::Font> font_;
	nctl::UniquePtr<nc::TextNode> textNode_;
};

#endif
