#include "Particle.h"
#include "RenderCommand.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Particle::Particle(SceneNode *parent, Texture *texture)
    : Sprite(parent, texture), life_(0.0f), startingLife(0.0f),
      startingRotation(0.0f), inLocalSpace_(false)
{
	type_ = ObjectType::PARTICLE;
	renderCommand_->setType(RenderCommand::CommandTypes::PARTICLE);
	setEnabled(false);
}

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

Particle::Particle(const Particle &other)
    : Sprite(other), life_(other.life_), startingLife(other.startingLife),
      startingRotation(other.startingRotation), inLocalSpace_(other.inLocalSpace_)
{
	type_ = ObjectType::PARTICLE;
	renderCommand_->setType(RenderCommand::CommandTypes::PARTICLE);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void Particle::init(float life, Vector2f pos, Vector2f vel, float rot, bool inLocalSpace)
{
	life_ = life;
	startingLife = life;
	startingRotation = rot;
	setPosition(pos);
	velocity_ = vel;
	setRotation(rot);
	inLocalSpace_ = inLocalSpace;
	setEnabled(true);
}

void Particle::update(float frameTime)
{
	if (frameTime >= life_)
	{
		life_ = 0.0f; // dead particle
		setEnabled(false);
	}
	else
	{
		life_ -= frameTime;
		move(velocity_ * frameTime);
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
		absPosition_ = position_;
	}
}

}
