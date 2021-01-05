#include "AnimatedSprite.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AnimatedSprite::AnimatedSprite()
    : AnimatedSprite(nullptr, nullptr, 0.0f, 0.0f)
{
}

AnimatedSprite::AnimatedSprite(SceneNode *parent, Texture *texture)
    : AnimatedSprite(parent, texture, 0.0f, 0.0f)
{
}

AnimatedSprite::AnimatedSprite(Texture *texture)
    : AnimatedSprite(nullptr, texture, 0.0f, 0.0f)
{
}

AnimatedSprite::AnimatedSprite(SceneNode *parent, Texture *texture, float xx, float yy)
    : Sprite(parent, texture, xx, yy), anims_(4), currentAnimIndex_(0)
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

AnimatedSprite AnimatedSprite::clone(SceneNode *parent) const
{
	AnimatedSprite newSprite(parent, texture_, x, y);
	BaseSprite::cloneInto(newSprite);
	for (const nctl::UniquePtr<RectAnimation> &anim : anims_)
		newSprite.addAnimation(nctl::makeUnique<RectAnimation>(*anim));

	newSprite.setAnimationIndex(animationIndex());
	newSprite.setFrame(frame());

	return newSprite;
}

bool AnimatedSprite::isPaused() const
{
	ASSERT(!anims_.isEmpty());
	const bool isPaused = anims_[currentAnimIndex_]->isPaused();
	return isPaused;
}

void AnimatedSprite::setPaused(bool isPaused)
{
	ASSERT(!anims_.isEmpty());
	anims_[currentAnimIndex_]->setPaused(isPaused);
}

void AnimatedSprite::update(float interval)
{
	ASSERT(!anims_.isEmpty());

	const unsigned int previousFrame = anims_[currentAnimIndex_]->frame();
	anims_[currentAnimIndex_]->updateFrame(interval);

	// Updating sprite texture rectangle only on change
	if (previousFrame != anims_[currentAnimIndex_]->frame())
		setTexRect(anims_[currentAnimIndex_]->rect());

	Sprite::update(interval);
}

void AnimatedSprite::addAnimation(nctl::UniquePtr<RectAnimation> anim)
{
	ASSERT(anim);

	anims_.pushBack(nctl::move(anim));
	currentAnimIndex_ = anims_.size() - 1;
	setTexRect(anims_[currentAnimIndex_]->rect());
}

void AnimatedSprite::clearAnimations()
{
	for (nctl::UniquePtr<RectAnimation> &anim : anims_)
		anim.reset(nullptr);
	anims_.clear();
}

void AnimatedSprite::setAnimationIndex(unsigned int animIndex)
{
	ASSERT(!anims_.isEmpty());
	ASSERT(animIndex < anims_.size());

	currentAnimIndex_ = animIndex;
	setTexRect(anims_[currentAnimIndex_]->rect());
}

unsigned int AnimatedSprite::frame() const
{
	ASSERT(!anims_.isEmpty());
	return anims_[currentAnimIndex_]->frame();
}

void AnimatedSprite::setFrame(unsigned int frameNum)
{
	ASSERT(!anims_.isEmpty());
	anims_[currentAnimIndex_]->setFrame(frameNum);
	setTexRect(anims_[currentAnimIndex_]->rect());
}

}
