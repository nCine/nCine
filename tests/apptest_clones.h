#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include <ncine/IAppEventHandler.h>
#include <ncine/IInputEventHandler.h>
#include <ncine/TimeStamp.h>
#include <nctl/UniquePtr.h>

namespace ncine {

class AppConfiguration;
class Texture;
class Sprite;
class AnimatedSprite;
class MeshSprite;
class Font;
class TextNode;
class ParticleSystem;

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
	static const unsigned int NumParticles = 50;

	nctl::UniquePtr<nc::Texture> spriteTexture_;
	nctl::UniquePtr<nc::Sprite> sprite_;
	nctl::UniquePtr<nc::Sprite> clonedSprite_;

	nctl::UniquePtr<nc::Texture> animSpriteTexture_;
	nctl::UniquePtr<nc::AnimatedSprite> animSprite_;
	nctl::UniquePtr<nc::AnimatedSprite> clonedAnimSprite_;

	nctl::UniquePtr<nc::Texture> meshSpriteTexture_;
	nctl::UniquePtr<nc::MeshSprite> meshSprite_;
	nctl::UniquePtr<nc::MeshSprite> clonedMeshSprite_;

	nctl::UniquePtr<nc::Font> font_;
	nctl::UniquePtr<nc::TextNode> textNode_;
	nctl::UniquePtr<nc::TextNode> clonedTextNode_;

	nctl::UniquePtr<nc::Texture> particleTexture_;
	nctl::UniquePtr<nc::ParticleSystem> particleSystem_;
	nctl::UniquePtr<nc::ParticleSystem> clonedParticleSystem_;
	nc::TimeStamp lastEmissionTime_;
};

#endif
