#ifndef CLASS_NCINE_ANIMATEDSPRITE
#define CLASS_NCINE_ANIMATEDSPRITE

#include "Sprite.h"
#include "RectAnimation.h"

namespace ncine {

/// A class for an animated sprite
class DLL_PUBLIC AnimatedSprite : public Sprite
{
  public:
	AnimatedSprite(SceneNode* parent, Texture *texture);
	explicit AnimatedSprite(Texture *texture);
	AnimatedSprite(SceneNode* parent, Texture *texture, int x, int y);
	AnimatedSprite(Texture *texture, int x, int y);
	virtual ~AnimatedSprite();

	/// Returns true if the current animation is paused
	inline bool isPaused() const { return anims_[currentAnim]->isPaused(); }
	/// Sets the pause state for the animation
	inline void setPaused(bool isPaused) { anims_[currentAnim]->setPaused(isPaused); }

	virtual void update(float interval);

	// Adds a new animation
	void addAnimation(RectAnimation* anim);
	// Sets the current animation and its frame number
	void setAnimation(int animNum);
	/// Sets the current animation to a specified frame number
	inline void setFrame(int frameNum) { anims_[currentAnim]->SetFrame(frameNum); }

	inline static ObjectType sType() { return ANIMATEDSPRITE_TYPE; }

  private:
	Array<RectAnimation *> anims_;
	int currentAnim;

	/// Private copy constructor
	AnimatedSprite(const AnimatedSprite&);
	/// Private assignment operator
	AnimatedSprite& operator=(const AnimatedSprite&);

	void init();
};

}

#endif
