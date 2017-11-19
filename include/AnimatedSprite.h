#ifndef CLASS_NCINE_ANIMATEDSPRITE
#define CLASS_NCINE_ANIMATEDSPRITE

#include "Sprite.h"
#include "RectAnimation.h"

namespace ncine {

/// A class for an animated sprite
class DLL_PUBLIC AnimatedSprite : public Sprite
{
  public:
	AnimatedSprite(SceneNode *parent, Texture *texture);
	explicit AnimatedSprite(Texture *texture);
	AnimatedSprite(SceneNode *parent, Texture *texture, float x, float y);
	AnimatedSprite(Texture *texture, float x, float y);
	virtual ~AnimatedSprite();

	/// Returns true if the current animation is paused
	bool isPaused() const;
	/// Sets the pause state for the animation
	void setPaused(bool isPaused);

	virtual void update(float interval);

	/// Adds a new animation
	void addAnimation(RectAnimation *anim);
	/// Sets the current animation and its frame number
	void setAnimation(int animNum);
	/// Sets the current animation to a specified frame number
	void setFrame(unsigned int frameNum);

	inline static ObjectType sType() { return ANIMATEDSPRITE_TYPE; }

  private:
	Array<RectAnimation *> anims_;
	int currentAnim_;

	/// Deleted copy constructor
	AnimatedSprite(const AnimatedSprite &) = delete;
	/// Deleted assignment operator
	AnimatedSprite &operator=(const AnimatedSprite &) = delete;

	void init();
};

}

#endif
