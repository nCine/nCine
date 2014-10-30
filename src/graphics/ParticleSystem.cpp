#include "ParticleSystem.h"
#include "Vector2f.h"
#include "Particle.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Constructs a particle system made of the specified maximum amount of particles
ParticleSystem::ParticleSystem(SceneNode* parent, unsigned int count, Texture *texture, Rect texRect)
	: DrawableNode(parent, 0, 0), poolSize_(count), poolTop_(count - 1), affectors_(4), inLocalSpace_(false)
{
	type_ = PARTICLESYSTEM_TYPE;
	setPriority(DrawableNode::SCENE_PRIORITY);

	particlePool_ = new Particle*[poolSize_];
	particleList_ = new Particle*[poolSize_];
	for (unsigned int i = 0; i < poolSize_; i++)
	{
		particlePool_[i] = new Particle(NULL, texture);
		particlePool_[i]->setTexRect(texRect);
		particleList_[i] = particlePool_[i];
	}
}

ParticleSystem::~ParticleSystem()
{
	// Empty the children list before the mass deletion
	children_.clear();

	unsigned int i;

	for (i = 0; i < affectors_.size(); i++)
	{
		delete affectors_[i];
	}

	for (i = 0; i < poolSize_; i++)
	{
		delete particleList_[i];
	}

	delete[] particlePool_;
	delete[] particleList_;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ParticleSystem::emitParticles(unsigned int amount, float life, const Vector2f &vel)
{
	Vector2f rndPosition;
	Vector2f rndVelocity;

	// Particles are rotated towards the emission vector
	float rotation = -(atan2(vel.y, vel.x) - atan2(1.0f, 0.0f)) * 180.0f / M_PI;
	if (rotation < 0.0f)
	{
		rotation += 360.0f;
	}

	for (unsigned int i = 0; i < amount; i++)
	{
		// No more unused particles in the pool
		if (poolTop_ == 0)
		{
			break;
		}

		float rndLife = life * randBetween(0.85f, 1.0f);
		// FIXME: arbitrary random position amount
		rndPosition.x = 10.0f * randBetween(-1.0f, 1.0f); // 25
		rndPosition.y = 10.0f * randBetween(-1.0f, 1.0f);
		rndVelocity.x = vel.x * randBetween(0.8f, 1.0f);
		rndVelocity.y = vel.y * randBetween(0.8f, 1.0f);

		if (inLocalSpace_ == false)
		{
			rndPosition += absPosition();
		}

		// acquiring a particle from the pool
		particlePool_[poolTop_]->init(rndLife, rndPosition, rndVelocity, rotation, inLocalSpace_);
		addChildNode(particlePool_[poolTop_]);
		poolTop_--;
	}
}

void ParticleSystem::update(float interval)
{
	// early return if the node has not to be updated
	if (!shouldUpdate_)
	{
		return;
	}

	for (List<SceneNode *>::Const_Iterator i = children_.begin(); i != children_.end(); ++i)
	{
		Particle *particle = static_cast<Particle *>(*i);

		// Update the particle if it's alive
		if (particle->isAlive())
		{
			for (unsigned int j = 0; j < affectors_.size(); j++)
			{
				affectors_[j]->affect(particle);
			}

			particle->update(interval);

			// Releasing the particle if it has just died
			if (particle->isAlive() == false)
			{
				// releasing a particle
				poolTop_++;
				particlePool_[poolTop_] = particle;
				removeChildNode(i++);
			}
		}
	}
}

}
