#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include <ncine/IAppEventHandler.h>
#include <ncine/IInputEventHandler.h>
#include <nctl/StaticArray.h>
#include <ncine/Vector2.h>
#include <ncine/Colorf.h>
#include <ncine/TimeStamp.h>

namespace nctl {

class String;

}

namespace ncine {

class AppConfiguration;
class Texture;
class Sprite;
class SceneNode;
class Font;
class TextNode;
class Viewport;
class Camera;

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
	void onMouseButtonPressed(const nc::MouseEvent &event) override;
	void onMouseButtonReleased(const nc::MouseEvent &event) override;
	void onMouseMoved(const nc::MouseState &state) override;
	void onScrollInput(const nc::ScrollEvent &event) override;

  private:
	struct ViewportData
	{
		nctl::UniquePtr<nc::Viewport> viewport;
		nctl::UniquePtr<nc::Camera> camera;
		nctl::UniquePtr<nc::SceneNode> rootNode;
		nctl::UniquePtr<nc::Sprite> sprite;
	};

	struct ViewportSetupData
	{
		bool assignCamera = true;
		bool assignRootNode = true;
		nc::Vector2i size = nc::Vector2i::Zero;
		nc::Colorf clearColor = nc::Colorf::Black;
		unsigned int spritePositionIndex = 4;
	};

	static const unsigned int NumTextures = 4;
	static const unsigned int NumSprites = 500;
	static const unsigned int NumViewports = 4;

	bool pause_;
	bool inputEnabled_;
	float angle_;
	nc::Vector2f camPos_;
	float camRot_;
	float camScale_;

	nc::Vector2f scrollOrigin_;
	nc::Vector2f scrollMove_;
	nc::Vector2f scrollOrigin2_;
	nc::Vector2f scrollMove2_;
	nc::TimeStamp lastClickTime_;

	nctl::StaticArray<nctl::UniquePtr<nc::Texture>, NumTextures> textures_;
	nctl::UniquePtr<nc::Font> font_;

	nctl::UniquePtr<nctl::String> debugString_;
	nctl::UniquePtr<nc::TextNode> debugText_;

	nctl::UniquePtr<nc::Camera> screenCamera_;
	ViewportData viewportData[NumViewports];
	ViewportSetupData viewportCreationData[NumViewports];

	nctl::StaticArray<nctl::UniquePtr<nc::Sprite>, NumSprites> sprites_;
	nctl::StaticArray<nc::Vector2f, NumSprites> spritePos_;

	void resetCamera();
};

#endif
