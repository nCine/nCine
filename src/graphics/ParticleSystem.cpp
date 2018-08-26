#include "nctl/algorithms.h"
#include "ParticleSystem.h"
#include "Vector2.h"
#include "Particle.h"
#include "ParticleInitializer.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ParticleSystem::ParticleSystem(SceneNode *parent, unsigned int count, Texture *texture, Recti texRect)
	: SceneNode(parent, 0, 0), poolSize_(count), poolTop_(count - 1),
	  particlePool_(poolSize_, nctl::ArrayMode::FIXED_CAPACITY),
	  particleArray_(poolSize_, nctl::ArrayMode::FIXED_CAPACITY),
	  affectors_(4), inLocalSpace_(false)
{
	type_ = ObjectType::PARTICLE_SYSTEM;

	for (unsigned int i = 0; i < poolSize_; i++)
	{
		particleArray_[i] = nctl::UniquePtr<Particle>(new Particle(nullptr, texture));
		particleArray_[i]->setTexRect(texRect);
		particlePool_[i] = particleArray_[i].get();
	}
}

ParticleSystem::~ParticleSystem()
{
	// Empty the children list before the mass deletion
	children_.clear();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ParticleSystem::clearAffectors()
{
	for (nctl::UniquePtr<ParticleAffector> &affector : affectors_)
		affector.reset(nullptr);
	affectors_.clear();
}

void ParticleSystem::emitParticles(const ParticleInitializer &init)
{
	const unsigned int amount = static_cast<unsigned int>(nctl::randBetween(init.rndAmount.x, init.rndAmount.y));
	Vector2f position(0.0f, 0.0f);
	Vector2f velocity(0.0f, 0.0f);

	for (unsigned int i = 0; i < amount; i++)
	{
		// No more unused particles in the pool
		if (poolTop_ < 0)
			break;

		const float life = nctl::randBetween(init.rndLife.x, init.rndLife.y);
		position.x = nctl::randBetween(init.rndPositionX.x, init.rndPositionX.y);
		position.y = nctl::randBetween(init.rndPositionY.x, init.rndPositionY.y);
		velocity.x = nctl::randBetween(init.rndVelocityX.x, init.rndVelocityX.y);
		velocity.y = nctl::randBetween(init.rndVelocityY.x, init.rndVelocityY.y);

		float rotation = 0.0f;
		if (init.emitterRotation)
		{
			// Particles are rotated towards the emission vector
			rotation = (atan2f(velocity.y, velocity.x) - atan2f(1.0f, 0.0f)) * 180.0f / fPi;
			if (rotation < 0.0f)
				rotation += 360.0f;
		}
		else
			rotation = nctl::randBetween(init.rndRotation.x, init.rndRotation.y);

		if (inLocalSpace_ == false)
			position += absPosition();

		// Acquiring a particle from the pool
		particlePool_[poolTop_]->init(life, position, velocity, rotation, inLocalSpace_);
		addChildNode(particlePool_[poolTop_]);
		poolTop_--;
	}
}

void ParticleSystem::killParticles()
{
	for (nctl::List<SceneNode *>::ConstIterator i = children_.begin(); i != children_.end(); ++i)
	{
		Particle *particle = static_cast<Particle *>(*i);

		if (particle->isAlive())
		{
			particle->life_ = 0.0f;
			particle->setEnabled(false);

			poolTop_++;
			particlePool_[poolTop_] = particle;
			removeChildNode(i++);
		}
	}
}

void ParticleSystem::setTexture(Texture *texture)
{
	for (nctl::UniquePtr<Particle> &particle : particleArray_)
		particle->setTexture(texture);
}

void ParticleSystem::setTexRect(const Recti &rect)
{
	for (nctl::UniquePtr<Particle> &particle : particleArray_)
		particle->setTexRect(rect);
}

void ParticleSystem::setLayer(unsigned int layer)
{
	for (nctl::UniquePtr<Particle> &particle : particleArray_)
		particle->setLayer(layer);
}

void ParticleSystem::update(float interval)
{
	for (nctl::List<SceneNode *>::ConstIterator i = children_.begin(); i != children_.end(); ++i)
	{
		Particle *particle = static_cast<Particle *>(*i);

		// Update the particle if it's alive
		if (particle->isAlive())
		{
			// Calculating the normalized age only once per particle
			const float normalizedAge = 1.0f - particle->life_ / particle->startingLife;
			for (nctl::UniquePtr<ParticleAffector> &affector : affectors_)
				affector->affect(particle, normalizedAge);

			particle->update(interval);

			// Releasing the particle if it has just died
			if (particle->isAlive() == false)
			{
				poolTop_++;
				particlePool_[poolTop_] = particle;
				removeChildNode(i++);
				continue;
			}

			// Transforming the particle only if it's still alive
			particle->transform();
		}
	}
}

}
