#include "AnimatedSprite.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AnimatedSprite::AnimatedSprite(SceneNode* parent, Texture *texture)
	: Sprite(parent, texture), anims_(4), currentAnim(-1)
{

}

AnimatedSprite::AnimatedSprite(Texture *texture)
	: Sprite(texture), anims_(4), currentAnim(-1)
{

}

AnimatedSprite::AnimatedSprite(SceneNode* parent, Texture *texture, int x, int y)
	: Sprite(parent, texture, x, y), anims_(4), currentAnim(-1)
{

}

AnimatedSprite::AnimatedSprite(Texture *texture, int x, int y)
	: Sprite(texture, x, y), anims_(4), currentAnim(-1)
{

}

AnimatedSprite::~AnimatedSprite()
{
	for (unsigned int i = 0; i < anims_.size(); i++)
	{
		delete anims_[i];
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Returns true if the current animation is paused
bool AnimatedSprite::isPaused() const
{
	bool isPaused = true;

	if (anims_.isEmpty() == false)
	{
		isPaused = anims_[currentAnim]->isPaused();
	}

	return isPaused;
}

/// Sets the pause state for the animation
void AnimatedSprite::setPaused(bool isPaused)
{
	if (anims_.isEmpty() == false)
	{
		anims_[currentAnim]->setPaused(isPaused);
	}
}

void AnimatedSprite::update(float interval)
{
	if (anims_.isEmpty() == false)
	{
		unsigned int previousFrame = anims_[currentAnim]->frame();
		anims_[currentAnim]->updateFrame(interval);

		// Updating sprite texture rectangle only on change
		if (previousFrame != anims_[currentAnim]->frame())
		{
			setTexRect(anims_[currentAnim]->rect());
		}
	}

	Sprite::update(interval);
}

/// Adds a new animation
void AnimatedSprite::addAnimation(RectAnimation* anim)
{
	if (anim)
	{
		anims_.insertBack(anim);
		currentAnim = anims_.size() - 1;
		setTexRect(anims_[currentAnim]->rect());
	}
}

/// Sets the current animation and its frame number
void AnimatedSprite::setAnimation(int animNum)
{
	// early-out if no animations available
	if (anims_.isEmpty())
	{
		return;
	}


	if (static_cast<unsigned int>(animNum) >= anims_.size())
	{
		currentAnim = anims_.size() - 1;
	}
	else if (animNum < 0)
	{
		currentAnim = 0;
	}
	else
	{
		currentAnim = animNum;
	}

	setTexRect(anims_[currentAnim]->rect());
}

/// Sets the current animation to a specified frame number
void AnimatedSprite::setFrame(int frameNum)
{
	if (anims_.isEmpty() == false)
	{
		anims_[currentAnim]->SetFrame(frameNum);
	}
}

}
