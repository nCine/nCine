#include "ParticleSystem.h"
#include "Vector2.h"
#include "Particle.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ParticleSystem::ParticleSystem(SceneNode *parent, unsigned int count, Texture *texture, Recti texRect)
	: SceneNode(parent, 0, 0), poolSize_(count), poolTop_(count - 1), particlePool_(poolSize_, ArrayMode::FIXED_CAPACITY),
	  particleList_(poolSize_, ArrayMode::FIXED_CAPACITY), affectors_(4), inLocalSpace_(false)
{
	type_ = PARTICLESYSTEM_TYPE;

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
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ParticleSystem::emitParticles(unsigned int amount, float life, const Vector2f &vel)
{
	Vector2f rndPosition;
	Vector2f rndVelocity;

	// Particles are rotated towards the emission vector
	float rotation = -(atan2(vel.y, vel.x) - atan2(1.0f, 0.0f)) * 180.0f / static_cast<float>(M_PI);
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
		// HACK: hard-coded random position amount
		rndPosition.x = 10.0f * randBetween(-1.0f, 1.0f); // 25
		rndPosition.y = 10.0f * randBetween(-1.0f, 1.0f);
		rndVelocity.x = vel.x * randBetween(0.8f, 1.0f);
		rndVelocity.y = vel.y * randBetween(0.8f, 1.0f);

		if (inLocalSpace_ == false)
		{
			rndPosition += absPosition();
		}

		// Acquiring a particle from the pool
		particlePool_[poolTop_]->init(rndLife, rndPosition, rndVelocity, rotation, inLocalSpace_);
		addChildNode(particlePool_[poolTop_]);
		poolTop_--;
	}
}

void ParticleSystem::update(float interval)
{
	for (List<SceneNode *>::ConstIterator i = children_.begin(); i != children_.end(); ++i)
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
			particle->transform();

			// Releasing the particle if it has just died
			if (particle->isAlive() == false)
			{
				poolTop_++;
				particlePool_[poolTop_] = particle;
				removeChildNode(i++);
			}
		}
	}
}

}
