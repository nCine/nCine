#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "IAppEventHandler.h"
#include "IInputEventHandler.h"
#include "StaticArray.h"
#include "Vector2.h"

namespace ncine {

class AppConfiguration;
class Texture;
class Sprite;
class SceneNode;
class Font;
class TextNode;
class String;

}

namespace nc = ncine;

/// My nCine event handler
class MyEventHandler
	: public nc::IAppEventHandler,
	  public nc::IInputEventHandler
{
  public:
	virtual void onPreInit(nc::AppConfiguration &config);
	virtual void onInit();
	virtual void onFrameStart();
	virtual void onShutdown();

#ifdef __ANDROID__
	virtual void onTouchDown(const nc::TouchEvent &event);
	virtual void onTouchUp(const nc::TouchEvent &event);
	virtual void onTouchMove(const nc::TouchEvent &event);
	virtual void onPointerDown(const nc::TouchEvent &event);
#endif
	virtual void onKeyReleased(const nc::KeyboardEvent &event);
	virtual void onMouseButtonPressed(const nc::MouseEvent &event);
	virtual void onMouseButtonReleased(const nc::MouseEvent &event);
	virtual void onMouseMoved(const nc::MouseState &state);
	virtual void onScrollInput(const nc::ScrollEvent &event);

	virtual void onJoyMappedAxisMoved(const nc::JoyMappedAxisEvent &event);
	virtual void onJoyMappedButtonReleased(const nc::JoyMappedButtonEvent &event);
	virtual void onJoyDisconnected(const nc::JoyConnectionEvent &event);

  private:
	static const unsigned int NumTextures = 4;
	static const unsigned int NumSprites = 500;
	static const unsigned int NumTexts = 5;

	bool pause_;
	float angle_;
	nc::Vector2f camPos_;
	float camRot_;
	float camScale_;

	nc::Vector2f scrollOrigin_;
	nc::Vector2f scrollMove_;
	nc::Vector2f scrollOrigin2_;
	nc::Vector2f scrollMove2_;
	float lastClick_;
	nc::Vector2f joyVectorLeft_;
	nc::Vector2f joyVectorRight_;

	nc::SceneNode *cameraNode_;
	nc::StaticArray<nc::Texture *, NumTextures> textures_;
	nc::StaticArray<nc::Sprite *, NumSprites> sprites_;
	nc::StaticArray<nc::Vector2f, NumSprites> spritePos_;
	nc::Font *font_;
	nc::StaticArray<nc::TextNode *, NumTexts> texts_;
	nc::TextNode *debugtext_;
	nc::String *debugString_;

	void resetCamera();
};

#endif
