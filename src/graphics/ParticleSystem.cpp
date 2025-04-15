#include "ParticleSystem.h"
#include "Random.h"
#include "Vector2.h"
#include "ParticleInitializer.h"
#include "Texture.h"
#include "Application.h"

#ifdef WITH_TRACY
	#include <nctl/StaticString.h>
#endif

#include "tracy.h"

namespace ncine {

namespace {
#ifdef WITH_TRACY
	nctl::StaticString<128> tracyInfoString;
#endif
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ParticleSystem::ParticleSystem(SceneNode *parent, unsigned int count, Texture *texture)
    : ParticleSystem(parent, count, texture, Recti(0, 0, texture->width(), texture->height()))
{
}

ParticleSystem::ParticleSystem(SceneNode *parent, unsigned int count, Texture *texture, Recti texRect)
    : SceneNode(parent, 0, 0), poolSize_(count), poolTop_(count - 1),
      particlePool_(poolSize_, nctl::ArrayMode::FIXED_CAPACITY),
      particleArray_(poolSize_, nctl::ArrayMode::FIXED_CAPACITY),
      affectors_(4), inLocalSpace_(false),
      particlesUpdateEnabled_(true), affectorsEnabled_(true)
{
	ZoneScoped;
	if (texture && texture->name() != nullptr)
	{
		// When Tracy is disabled the statement body is empty and braces are needed
		ZoneText(texture->name(), nctl::strnlen(texture->name(), Object::MaxNameLength));
	}

	type_ = ObjectType::PARTICLE_SYSTEM;

	children_.setCapacity(poolSize_);
	for (unsigned int i = 0; i < poolSize_; i++)
	{
		nctl::UniquePtr<Particle> particle = nctl::makeUnique<Particle>(nullptr, texture);
		particle->setTexRect(texRect);
		particlePool_.pushBack(particle.get());
		particleArray_.pushBack(nctl::move(particle));
	}
}

ParticleSystem::ParticleSystem(ParticleSystem &&) = default;

ParticleSystem &ParticleSystem::operator=(ParticleSystem &&) = default;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ParticleSystem::clearAffectors()
{
	affectors_.clear();
}

void ParticleSystem::emitParticles(const ParticleInitializer &init)
{
	if (updateEnabled_ == false)
		return;

	ZoneScoped;
	const unsigned int amount = static_cast<unsigned int>(random().integer(init.rndAmount.x, init.rndAmount.y));
#ifdef WITH_TRACY
	tracyInfoString.format("Count: %d", amount);
	ZoneText(tracyInfoString.data(), tracyInfoString.length());
#endif
	Vector2f position(0.0f, 0.0f);
	Vector2f velocity(0.0f, 0.0f);

	for (unsigned int i = 0; i < amount; i++)
	{
		// No more unused particles in the pool
		if (poolTop_ < 0)
			break;

		const float life = random().real(init.rndLife.x, init.rndLife.y);
		position.x = random().real(init.rndPositionX.x, init.rndPositionX.y);
		position.y = random().real(init.rndPositionY.x, init.rndPositionY.y);
		velocity.x = random().real(init.rndVelocityX.x, init.rndVelocityX.y);
		velocity.y = random().real(init.rndVelocityY.x, init.rndVelocityY.y);

		float rotation = 0.0f;
		if (init.emitterRotation)
		{
			// Particles are rotated towards the emission vector
			rotation = (atan2f(velocity.y, velocity.x) - atan2f(1.0f, 0.0f)) * 180.0f / fPi;
			if (rotation < 0.0f)
				rotation += 360.0f;
		}
		else
			rotation = random().real(init.rndRotation.x, init.rndRotation.y);

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
	for (int i = children_.size() - 1; i >= 0; i--)
	{
		Particle *particle = static_cast<Particle *>(children_[i]);

		if (particle->isAlive())
		{
			particle->life_ = 0.0f;
			particle->setEnabled(false);

			poolTop_++;
			particlePool_[poolTop_] = particle;
			removeChildNodeAt(i);
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

void ParticleSystem::setAnchorPoint(float xx, float yy)
{
	for (nctl::UniquePtr<Particle> &particle : particleArray_)
		particle->setAnchorPoint(xx, yy);
}

void ParticleSystem::setAnchorPoint(const Vector2f &point)
{
	for (nctl::UniquePtr<Particle> &particle : particleArray_)
		particle->setAnchorPoint(point);
}

void ParticleSystem::setFlippedX(bool flippedX)
{
	for (nctl::UniquePtr<Particle> &particle : particleArray_)
		particle->setFlippedX(flippedX);
}

void ParticleSystem::setFlippedY(bool flippedY)
{
	for (nctl::UniquePtr<Particle> &particle : particleArray_)
		particle->setFlippedY(flippedY);
}

void ParticleSystem::setBlendingPreset(DrawableNode::BlendingPreset blendingPreset)
{
	for (nctl::UniquePtr<Particle> &particle : particleArray_)
		particle->setBlendingPreset(blendingPreset);
}

void ParticleSystem::setBlendingFactors(DrawableNode::BlendingFactor srcBlendingFactor, DrawableNode::BlendingFactor destBlendingFactor)
{
	for (nctl::UniquePtr<Particle> &particle : particleArray_)
		particle->setBlendingFactors(srcBlendingFactor, destBlendingFactor);
}

void ParticleSystem::setLayer(uint16_t layer)
{
	for (nctl::UniquePtr<Particle> &particle : particleArray_)
		particle->setLayer(layer);
}

void ParticleSystem::update(float frameTime)
{
	if (updateEnabled_ == false)
		return;

	ZoneScoped;
	// Overridden `update()` method should call `transform()` like `SceneNode::update()` does
	SceneNode::transform();

	for (int i = children_.size() - 1; i >= 0; i--)
	{
		Particle *particle = static_cast<Particle *>(children_[i]);

		// Update the particle if it's alive
		if (particle->isAlive())
		{
			if (affectorsEnabled_)
			{
				// Calculating the normalized age only once per particle
				const float normalizedAge = 1.0f - particle->life_ / particle->startingLife;
				for (nctl::UniquePtr<ParticleAffector> &affector : affectors_)
					affector->affect(particle, normalizedAge);
			}

			if (particlesUpdateEnabled_)
			{
				particle->update(frameTime);

				// Releasing the particle if it has just died
				if (particle->isAlive() == false)
				{
					poolTop_++;
					particlePool_[poolTop_] = particle;
					removeChildNodeAt(i);
					continue;
				}
			}

			// Transforming the particle only if it's still alive
			particle->transform();
		}
	}

	lastFrameUpdated_ = theApplication().numFrames();

#ifdef WITH_TRACY
	tracyInfoString.format("Alive: %d", numAliveParticles());
	ZoneText(tracyInfoString.data(), tracyInfoString.length());
#endif
}

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

ParticleSystem::ParticleSystem(const ParticleSystem &other)
    : SceneNode(other), poolSize_(other.poolSize_), poolTop_(other.poolSize_ - 1),
      particlePool_(other.poolSize_, nctl::ArrayMode::FIXED_CAPACITY),
      particleArray_(other.poolSize_, nctl::ArrayMode::FIXED_CAPACITY),
      affectors_(4), inLocalSpace_(other.inLocalSpace_),
      particlesUpdateEnabled_(other.particlesUpdateEnabled_),
      affectorsEnabled_(other.affectorsEnabled_)
{
	ZoneScoped;
	type_ = ObjectType::PARTICLE_SYSTEM;

	for (unsigned int i = 0; i < other.affectors_.size(); i++)
	{
		const ParticleAffector &affector = *other.affectors_[i];
		switch (affector.type())
		{
			case ParticleAffector::Type::COLOR:
				affectors_.pushBack(nctl::makeUnique<ColorAffector>(static_cast<const ColorAffector &>(affector).clone()));
				break;
			case ParticleAffector::Type::SIZE:
				affectors_.pushBack(nctl::makeUnique<SizeAffector>(static_cast<const SizeAffector &>(affector).clone()));
				break;
			case ParticleAffector::Type::ROTATION:
				affectors_.pushBack(nctl::makeUnique<RotationAffector>(static_cast<const RotationAffector &>(affector).clone()));
				break;
			case ParticleAffector::Type::POSITION:
				affectors_.pushBack(nctl::makeUnique<PositionAffector>(static_cast<const PositionAffector &>(affector).clone()));
				break;
			case ParticleAffector::Type::VELOCITY:
				affectors_.pushBack(nctl::makeUnique<VelocityAffector>(static_cast<const VelocityAffector &>(affector).clone()));
				break;
		}
	}

	children_.setCapacity(poolSize_);
	if (poolSize_ > 0)
	{
		const Particle &otherParticle = *other.particlePool_.front();
		if (otherParticle.texture() && otherParticle.texture()->name() != nullptr)
		{
			// When Tracy is disabled the statement body is empty and braces are needed
			ZoneText(otherParticle.texture()->name(), nctl::strnlen(otherParticle.texture()->name(), Object::MaxNameLength));
		}

		for (unsigned int i = 0; i < poolSize_; i++)
		{
			nctl::UniquePtr<Particle> particle = nctl::makeUnique<Particle>(otherParticle.clone());
			particlePool_.pushBack(particle.get());
			particleArray_.pushBack(nctl::move(particle));
		}
	}
}

}
