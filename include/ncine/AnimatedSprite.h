#ifndef CLASS_NCINE_ANIMATEDSPRITE
#define CLASS_NCINE_ANIMATEDSPRITE

#include "Sprite.h"
#include "RectAnimation.h"

namespace ncine {

/// A class for an animated sprite
class DLL_PUBLIC AnimatedSprite : public Sprite
{
  public:
	/// Constructor for an animated sprite with a parent and texture, positioned in the relative origin
	AnimatedSprite(SceneNode *parent, Texture *texture);
	/// Constructor for an animated sprite with a texture but no parent, positioned in the origin
	explicit AnimatedSprite(Texture *texture);
	/// Constructor for an animated sprite with a parent, a texture and a specified relative position
	AnimatedSprite(SceneNode *parent, Texture *texture, float xx, float yy);
	/// Constructor for an animated sprite with a parent, a texture and a specified relative position as a vector
	AnimatedSprite(SceneNode *parent, Texture *texture, const Vector2f &position);
	/// Constructor for an animated sprite with a texture and a specified position but no parent
	AnimatedSprite(Texture *texture, float xx, float yy);
	/// Constructor for an animated sprite with a texture and a specified position as a vector but no parent
	AnimatedSprite(Texture *texture, const Vector2f &position);

	/// Returns true if the current animation is paused
	bool isPaused() const;
	/// Sets the pause state for the animation
	void setPaused(bool isPaused);

	void update(float interval) override;

	/// Adds a new animation
	void addAnimation(nctl::UniquePtr<RectAnimation> anim);
	/// Deletes all animations
	void clearAnimations();
	/// Sets the current animation and its frame number
	void setAnimation(int animNum);
	/// Sets the current animation to a specified frame number
	void setFrame(unsigned int frameNum);

	inline static ObjectType sType() { return ObjectType::ANIMATED_SPRITE; }

  private:
	nctl::Array<nctl::UniquePtr<RectAnimation>> anims_;
	int currentAnim_;

	/// Deleted copy constructor
	AnimatedSprite(const AnimatedSprite &) = delete;
	/// Deleted assignment operator
	AnimatedSprite &operator=(const AnimatedSprite &) = delete;
};

}

#endif
