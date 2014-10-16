#ifndef CLASS_NCPARTICLESYSTEM
#define CLASS_NCPARTICLESYSTEM

#include <ctime>
#include <cstdlib>

#include "ncArray.h"
#include "ncDrawableNode.h"
#include "ncParticleAffectors.h"
class ncTexture;
class ncVector2f;
class ncParticle;

/// The class representing a particle system
class ncParticleSystem : public ncDrawableNode
{
  public:
	// Constructs a particle system made of the specified maximum amount of particles
	ncParticleSystem(ncSceneNode* parent, unsigned int count, ncTexture *texture, ncRect texRect);
	~ncParticleSystem();

	/// Adds a particle affector
	void addAffector(ncParticleAffector* affector) { affectors_.insertBack(affector); }
	/// Emits an amount of particles with a specified initial life and velocity
	void emitParticles(unsigned int amount, float life, const ncVector2f &vel);

	/// Gets the local space flag of the system
	inline bool inLocalSpace(void) const { return inLocalSpace_; }
	/// Sets the local space flag of the system
	inline void setInLocalSpace(bool inLocalSpace) { inLocalSpace_ = inLocalSpace; }

	virtual void update(float interval);

	inline static ObjectType sType() { return PARTICLESYSTEM_TYPE; }

  private:
	/// The particle pool size
	unsigned int poolSize_;
	/// The index of the next free particle in the pool
	unsigned int poolTop_;
	/// The pool containing available particles (only dead ones)
	ncParticle **particlePool_;
	/// The array containing every particle (dead or alive)
	ncParticle **particleList_;

	/// The array of particle affectors
	ncArray<ncParticleAffector *> affectors_;

	/// Should the system simulate in local space?
	bool inLocalSpace_;

	virtual void updateRenderCommand() { }
};

/// Returns a random float between x0 and x1
inline float randBetween(float x0, float x1) { return x0 + (x1 - x0) * (rand() / float(RAND_MAX)); }

#endif
