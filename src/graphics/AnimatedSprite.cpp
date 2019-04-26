#include "AnimatedSprite.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AnimatedSprite::AnimatedSprite(SceneNode *parent, Texture *texture)
    : AnimatedSprite(parent, texture, 0.0f, 0.0f)
{
}

AnimatedSprite::AnimatedSprite(Texture *texture)
    : AnimatedSprite(nullptr, texture, 0.0f, 0.0f)
{
}

AnimatedSprite::AnimatedSprite(SceneNode *parent, Texture *texture, float xx, float yy)
    : Sprite(parent, texture, xx, yy), anims_(4), currentAnim_(-1)
{
}

AnimatedSprite::AnimatedSprite(SceneNode *parent, Texture *texture, const Vector2f &position)
    : AnimatedSprite(parent, texture, position.x, position.y)
{
}

AnimatedSprite::AnimatedSprite(Texture *texture, float xx, float yy)
    : AnimatedSprite(nullptr, texture, xx, yy)
{
}

AnimatedSprite::AnimatedSprite(Texture *texture, const Vector2f &position)
    : AnimatedSprite(nullptr, texture, position.x, position.y)
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool AnimatedSprite::isPaused() const
{
	ASSERT(!anims_.isEmpty());
	const bool isPaused = anims_[currentAnim_]->isPaused();
	return isPaused;
}

void AnimatedSprite::setPaused(bool isPaused)
{
	ASSERT(!anims_.isEmpty());
	anims_[currentAnim_]->setPaused(isPaused);
}

void AnimatedSprite::update(float interval)
{
	ASSERT(!anims_.isEmpty());

	const unsigned int previousFrame = anims_[currentAnim_]->frame();
	anims_[currentAnim_]->updateFrame(interval);

	// Updating sprite texture rectangle only on change
	if (previousFrame != anims_[currentAnim_]->frame())
		setTexRect(anims_[currentAnim_]->rect());

	Sprite::update(interval);
}

void AnimatedSprite::addAnimation(nctl::UniquePtr<RectAnimation> anim)
{
	ASSERT(anim);

	anims_.pushBack(nctl::move(anim));
	currentAnim_ = anims_.size() - 1;
	setTexRect(anims_[currentAnim_]->rect());
}

void AnimatedSprite::clearAnimations()
{
	for (nctl::UniquePtr<RectAnimation> &anim : anims_)
		anim.reset(nullptr);
	anims_.clear();
}

void AnimatedSprite::setAnimation(int animNum)
{
	ASSERT(!anims_.isEmpty());
	ASSERT(animNum >= 0);
	ASSERT(static_cast<unsigned int>(animNum) < anims_.size());

	currentAnim_ = animNum;
	setTexRect(anims_[currentAnim_]->rect());
}

void AnimatedSprite::setFrame(unsigned int frameNum)
{
	ASSERT(!anims_.isEmpty());
	anims_[currentAnim_]->setFrame(frameNum);
	setTexRect(anims_[currentAnim_]->rect());
}

}
