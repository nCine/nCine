#include "ncParticle.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncParticle::ncParticle(ncSceneNode* parent, ncTexture *texture)
	: ncSprite(parent, texture), life_(0.0f), startingLife(0.0f), inLocalSpace_(false)
{
	renderCommand_.setType(ncRenderCommand::PARTICLE_TYPE);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Initializes a particle with initial life, position, velocity and rotation
void ncParticle::init(float life, ncVector2f pos, ncVector2f vel, float rot, bool inLocalSpace)
{
	life_ = life;
	startingLife = life;
	setPosition(pos);
	velocity_ = vel;
	setRotation(rot);
	inLocalSpace_ = inLocalSpace;
}

/// Updates particle data after the specified amount of seconds has passed
void ncParticle::update(float interval)
{
	if (interval > life_)
	{
		life_ = 0.0f;    // dead particle
	}
	else
	{
		life_ -= interval;

		x += velocity_.x * interval;
		y += velocity_.y * interval;
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Custom transform method to allow independent position from parent
void ncParticle::transform()
{
	// If in local space transform as any other scene node
	if (inLocalSpace_)
	{
		ncSceneNode::transform();
	}
	else
	{
		if (parent_)
		{
			absScaleFactor_ = parent_->absScale() * scaleFactor_;
			absColor_ = parent_->absColor() * color_;
		}
		else
		{
			absScaleFactor_ = scaleFactor_;
			absColor_ = color_;
		}

		// Always independent movement
		absRotation_ = rotation_;
		absX_ = x;
		absY_ = y;
	}
}

