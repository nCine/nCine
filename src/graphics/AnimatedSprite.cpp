#include "AnimatedSprite.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AnimatedSprite::AnimatedSprite(SceneNode *parent, Texture *texture)
	: Sprite(parent, texture), anims_(4), currentAnim_(-1)
{

}

AnimatedSprite::AnimatedSprite(Texture *texture)
	: Sprite(texture), anims_(4), currentAnim_(-1)
{

}

AnimatedSprite::AnimatedSprite(SceneNode *parent, Texture *texture, float x, float y)
	: Sprite(parent, texture, x, y), anims_(4), currentAnim_(-1)
{

}

AnimatedSprite::AnimatedSprite(Texture *texture, float x, float y)
	: Sprite(texture, x, y), anims_(4), currentAnim_(-1)
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

bool AnimatedSprite::isPaused() const
{
	bool isPaused = true;

	if (anims_.isEmpty() == false)
	{
		isPaused = anims_[currentAnim_]->isPaused();
	}

	return isPaused;
}

void AnimatedSprite::setPaused(bool isPaused)
{
	if (anims_.isEmpty() == false)
	{
		anims_[currentAnim_]->setPaused(isPaused);
	}
}

void AnimatedSprite::update(float interval)
{
	if (anims_.isEmpty() == false)
	{
		unsigned int previousFrame = anims_[currentAnim_]->frame();
		anims_[currentAnim_]->updateFrame(interval);

		// Updating sprite texture rectangle only on change
		if (previousFrame != anims_[currentAnim_]->frame())
		{
			setTexRect(anims_[currentAnim_]->rect());
		}
	}

	Sprite::update(interval);
}

void AnimatedSprite::addAnimation(RectAnimation *anim)
{
	if (anim)
	{
		anims_.pushBack(anim);
		currentAnim_ = anims_.size() - 1;
		setTexRect(anims_[currentAnim_]->rect());
	}
}

void AnimatedSprite::setAnimation(int animNum)
{
	// early-out if no animations available
	if (anims_.isEmpty())
	{
		return;
	}


	if (static_cast<unsigned int>(animNum) >= anims_.size())
	{
		currentAnim_ = anims_.size() - 1;
	}
	else if (animNum < 0)
	{
		currentAnim_ = 0;
	}
	else
	{
		currentAnim_ = animNum;
	}

	setTexRect(anims_[currentAnim_]->rect());
}

void AnimatedSprite::setFrame(unsigned int frameNum)
{
	if (anims_.isEmpty() == false)
	{
		anims_[currentAnim_]->setFrame(frameNum);
	}
}

}
