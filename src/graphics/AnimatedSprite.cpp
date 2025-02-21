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
	type_ = ObjectType::ANIMATED_SPRITE;
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
	bool isPaused = true;
	if (anims_.isEmpty() == false)
		isPaused = anims_[currentAnimIndex_].isPaused();
	return isPaused;
}

void AnimatedSprite::setPaused(bool isPaused)
{
	if (anims_.isEmpty() == false)
		anims_[currentAnimIndex_].setPaused(isPaused);
}

void AnimatedSprite::update(float frameTime)
{
	if (anims_.isEmpty() == false)
	{
		const unsigned int previousFrame = anims_[currentAnimIndex_].frame();
		anims_[currentAnimIndex_].updateFrame(frameTime);

		// Updating sprite texture rectangle only on change
		if (previousFrame != anims_[currentAnimIndex_].frame())
			setTexRect(anims_[currentAnimIndex_].rect());
	}

	Sprite::update(frameTime);
}

void AnimatedSprite::addAnimation(const RectAnimation &anim)
{
	if (anim.rectangles().isEmpty() == false)
	{
		anims_.pushBack(anim);
		currentAnimIndex_ = anims_.size() - 1;
		setTexRect(anims_[currentAnimIndex_].rect());
	}
}

void AnimatedSprite::addAnimation(RectAnimation &&anim)
{
	if (anim.rectangles().isEmpty() == false)
	{
		anims_.pushBack(nctl::move(anim));
		currentAnimIndex_ = anims_.size() - 1;
		setTexRect(anims_[currentAnimIndex_].rect());
	}
}

void AnimatedSprite::clearAnimations()
{
	anims_.clear();
}

void AnimatedSprite::setAnimationIndex(unsigned int animIndex)
{
	if (anims_.isEmpty() == false)
	{
		ASSERT(animIndex < anims_.size());
		currentAnimIndex_ = animIndex;
		setTexRect(anims_[currentAnimIndex_].rect());
	}
}

RectAnimation *AnimatedSprite::currentAnimation()
{
	RectAnimation *currentAnim = nullptr;
	if (anims_.isEmpty() == false)
		currentAnim = &anims_[currentAnimIndex_];
	return currentAnim;
}

const RectAnimation *AnimatedSprite::currentAnimation() const
{
	const RectAnimation *currentAnim = nullptr;
	if (anims_.isEmpty() == false)
		currentAnim = &anims_[currentAnimIndex_];
	return currentAnim;
}

unsigned int AnimatedSprite::numFrames() const
{
	unsigned int numframes = 0;
	if (anims_.isEmpty() == false)
		numframes = anims_[currentAnimIndex_].numFrames();
	return numframes;
}

unsigned int AnimatedSprite::frame() const
{
	unsigned int frame = 0;
	if (anims_.isEmpty() == false)
		frame = anims_[currentAnimIndex_].frame();
	return frame;
}

void AnimatedSprite::setFrame(unsigned int frameNum)
{
	if (anims_.isEmpty() == false)
	{
		anims_[currentAnimIndex_].setFrame(frameNum);
		setTexRect(anims_[currentAnimIndex_].rect());
	}
}

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

AnimatedSprite::AnimatedSprite(const AnimatedSprite &other)
    : Sprite(other), anims_(other.anims_), currentAnimIndex_(other.currentAnimIndex_)
{
	type_ = ObjectType::ANIMATED_SPRITE;
	setFrame(other.frame());
}

}
