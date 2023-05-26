#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include <ncine/IAppEventHandler.h>
#include <ncine/IInputEventHandler.h>
#include <nctl/Array.h>
#include <ncine/Vector2.h>

namespace ncine {

class AppConfiguration;
class Texture;
class Sprite;

}

namespace nc = ncine;

struct Transform
{
	Transform(nc::Vector2f pos, nc::Vector2f vel)
	    : position(pos), velocity(vel) {}
	nc::Vector2f position;
	nc::Vector2f velocity;
};

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
#endif
	void onKeyReleased(const nc::KeyboardEvent &event) override;
	void onMouseButtonPressed(const nc::MouseEvent &event) override;
	void onJoyMappedButtonReleased(const nc::JoyMappedButtonEvent &event) override;

  private:
	struct Transform
	{
		Transform() {}
		Transform(nc::Vector2f pos, nc::Vector2f vel)
		    : position(pos), velocity(vel) {}
		nc::Vector2f position;
		nc::Vector2f velocity;
	};

	bool withVSync_;
	bool pause_;

	nctl::UniquePtr<nc::Texture> texture_;
	nctl::Array<nctl::UniquePtr<nc::Sprite>> sprites_;
	nctl::Array<Transform> transforms_;

	void addBunnies(unsigned int amount);
	bool setNumBunnies(unsigned int count);
};

#endif
