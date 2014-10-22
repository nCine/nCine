#include "ncAnimatedSprite.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncAnimatedSprite::ncAnimatedSprite(ncSceneNode* parent, ncTexture *texture)
	: ncSprite(parent, texture), anims_(4), currentAnim(-1)
{

}

ncAnimatedSprite::ncAnimatedSprite(ncTexture *texture)
	: ncSprite(texture), anims_(4), currentAnim(-1)
{

}

ncAnimatedSprite::ncAnimatedSprite(ncSceneNode* parent, ncTexture *texture, int x, int y)
	: ncSprite(parent, texture, x, y), anims_(4), currentAnim(-1)
{

}

ncAnimatedSprite::ncAnimatedSprite(ncTexture *texture, int x, int y)
	: ncSprite(texture, x, y), anims_(4), currentAnim(-1)
{

}

ncAnimatedSprite::~ncAnimatedSprite()
{
	for (unsigned int i = 0; i < anims_.size(); i++)
	{
		delete anims_[i];
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncAnimatedSprite::update(float interval)
{
	unsigned int previousFrame = anims_[currentAnim]->frame();
	anims_[currentAnim]->updateFrame(interval);

	// Updating sprite texture rectangle only on change
	if (previousFrame != anims_[currentAnim]->frame())
	{
		setTexRect(anims_[currentAnim]->rect());
	}

	ncSprite::update(interval);
}

/// Adds a new animation
void ncAnimatedSprite::addAnimation(ncRectAnimation* anim)
{
	if (anim)
	{
		anims_.insertBack(anim);
		currentAnim = anims_.size() - 1;
		setTexRect(anims_[currentAnim]->rect());
	}
}

/// Sets the current animation and its frame number
void ncAnimatedSprite::setAnimation(int animNum)
{
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
