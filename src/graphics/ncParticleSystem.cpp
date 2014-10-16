#include "ncParticleSystem.h"
#include "ncVector2f.h"
#include "ncParticle.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Constructs a particle system made of the specified maximum amount of particles
ncParticleSystem::ncParticleSystem(ncSceneNode* parent, unsigned int count, ncTexture *texture, ncRect texRect)
	: ncDrawableNode(parent, 0, 0), poolSize_(count), poolTop_(count - 1), affectors_(4), inLocalSpace_(false)
{
	type_ = PARTICLESYSTEM_TYPE;
	setPriority(ncDrawableNode::SCENE_PRIORITY);

	particlePool_ = new ncParticle*[poolSize_];
	particleList_ = new ncParticle*[poolSize_];
	for (unsigned int i = 0; i < poolSize_; i++)
	{
		particlePool_[i] = new ncParticle(NULL, texture);
		particlePool_[i]->setTexRect(texRect);
		particleList_[i] = particlePool_[i];
	}
}

ncParticleSystem::~ncParticleSystem()
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

void ncParticleSystem::emitParticles(unsigned int amount, float life, const ncVector2f &vel)
{
	float rndLife;
	ncVector2f rndPosition;
	ncVector2f rndVelocity;

	// Particles are rotated towards the emission vector
	float rotation = -(atan2(vel.y, vel.x) - atan2(1.0f, 0.0f)) * 180.0f / M_PI;
	if (rotation < 0.0f)
	{
		rotation += 360;
	}

	for (unsigned int i = 0; i < amount; i++)
	{
		// No more unused particles in the pool
		if (poolTop_ == 0)
		{
			break;
		}

		rndLife = life * randBetween(0.85f, 1.0f);
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

void ncParticleSystem::update(float interval)
{
	// early return if the node has not to be updated
	if (!shouldUpdate_)
	{
		return;
	}

	for (ncList<ncSceneNode *>::Const_Iterator i = children_.begin(); i != children_.end(); ++i)
	{
		ncParticle *particle = static_cast<ncParticle *>(*i);

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
