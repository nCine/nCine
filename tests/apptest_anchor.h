#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include <ncine/IAppEventHandler.h>
#include <ncine/IInputEventHandler.h>
#include <nctl/StaticArray.h>
#include <ncine/Vector2.h>
#include <ncine/TimeStamp.h>

namespace ncine {

class AppConfiguration;
class Font;
class Texture;
class Sprite;
class MeshSprite;
class TextNode;
class ParticleSystem;
class SizeAffector;
class RotationAffector;

}

namespace nc = ncine;

/// My nCine event handler
class MyEventHandler :
    public nc::IAppEventHandler,
    public nc::IInputEventHandler
{
  public:
	static const unsigned int NumTextures = 4;

	MyEventHandler();

	void onPreInit(nc::AppConfiguration &config) override;
	void onInit() override;
	void onFrameStart() override;
	void onKeyReleased(const nc::KeyboardEvent &event) override;

  private:
	static const unsigned int NumSprites = 4;

	enum Type
	{
		SPRITE,
		MESH_SPRITE,
		TEXT_NODE,
		PARTICLE_SYSTEM
	};

	int currentType_;
	nc::Vector2f anchorPoint_;
	nc::Vector2f position_;
	float angle_;
	float scale_;

	nctl::UniquePtr<nc::Font> font_;
	nctl::StaticArray<nctl::UniquePtr<nc::Texture>, NumTextures> textures_;
	nctl::StaticArray<nctl::UniquePtr<nc::Sprite>, NumSprites> sprites_;
	nctl::StaticArray<nctl::UniquePtr<nc::MeshSprite>, NumSprites> meshSprites_;
	nctl::StaticArray<nctl::UniquePtr<nc::TextNode>, NumSprites> textNodes_;
	nctl::StaticArray<nctl::UniquePtr<nc::ParticleSystem>, NumSprites> particleSystems_;
	nctl::StaticArray<nc::SizeAffector *, NumSprites> sizeAffectors_;
	nctl::StaticArray<nc::RotationAffector *, NumSprites> rotationAffectors_;
	nc::TimeStamp lastEmissionTime_;
};

#endif
