#include "Particle.h"
#include "RenderCommand.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Particle::Particle(SceneNode *parent, Texture *texture)
	: Sprite(parent, texture), life_(0.0f), startingLife(0.0f), inLocalSpace_(false)
{
	renderCommand_->setType(RenderCommand::PARTICLE_TYPE);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void Particle::init(float life, Vector2f pos, Vector2f vel, float rot, bool inLocalSpace)
{
	life_ = life;
	startingLife = life;
	setPosition(pos);
	velocity_ = vel;
	setRotation(rot);
	inLocalSpace_ = inLocalSpace;
}

void Particle::update(float interval)
{
	if (interval > life_)
	{
		life_ = 0.0f; // dead particle
	}
	else
	{
		life_ -= interval;

		x += velocity_.x * interval;
		y += velocity_.y * interval;
	}
}

void Particle::transform()
{
	SceneNode::transform();

	if (inLocalSpace_ == false)
	{
		worldMatrix_ = localMatrix_;

		// Always independent movement
		absScaleFactor_ = scaleFactor_;
		absRotation_ = rotation_;
		absX_ = x;
		absY_ = y;
	}
}

}
