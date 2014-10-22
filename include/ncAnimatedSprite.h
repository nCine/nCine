#ifndef CLASS_NCANIMATEDSPRITE
#define CLASS_NCANIMATEDSPRITE

#include "ncSprite.h"
#include "ncRectAnimation.h"

/// A class for an animated sprite
class ncAnimatedSprite : public ncSprite
{
  public:
	ncAnimatedSprite(ncSceneNode* parent, ncTexture *texture);
	explicit ncAnimatedSprite(ncTexture *texture);
	ncAnimatedSprite(ncSceneNode* parent, ncTexture *texture, int x, int y);
	ncAnimatedSprite(ncTexture *texture, int x, int y);
	virtual ~ncAnimatedSprite();

	/// Returns true if the current animation is paused
	inline bool isPaused() const { return anims_[currentAnim]->isPaused(); }
	/// Sets the pause state for the animation
	inline void setPaused(bool isPaused) { anims_[currentAnim]->setPaused(isPaused); }

	virtual void update(float interval);

	// Adds a new animation
	void addAnimation(ncRectAnimation* anim);
	// Sets the current animation and its frame number
	void setAnimation(int animNum);
	/// Sets the current animation to a specified frame number
	inline void setFrame(int frameNum) { anims_[currentAnim]->SetFrame(frameNum); }

	inline static ObjectType sType() { return ANIMATEDSPRITE_TYPE; }

  private:
	ncArray<ncRectAnimation *> anims_;
	int currentAnim;

	void init();
};

#endif
