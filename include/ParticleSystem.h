#ifndef CLASS_NCINE_PARTICLESYSTEM
#define CLASS_NCINE_PARTICLESYSTEM

#include <ctime>
#include <cstdlib>
#include "Array.h"
#include "Rect.h"
#include "SceneNode.h"
#include "ParticleAffectors.h"

namespace ncine {

class Texture;
class Particle;

/// The class representing a particle system
class DLL_PUBLIC ParticleSystem : public SceneNode
{
  public:
	/// Constructs a particle system made of the specified maximum amount of particles
	ParticleSystem(SceneNode *parent, unsigned int count, Texture *texture, Recti texRect);
	~ParticleSystem();

	/// Adds a particle affector
	void addAffector(ParticleAffector *affector) { affectors_.pushBack(affector); }
	/// Emits an amount of particles with a specified initial life and velocity
	void emitParticles(unsigned int amount, float life, const Vector2f &vel);

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
	Array<Particle *> particlePool_;
	/// The array containing every particle (dead or alive)
	Array<Particle *> particleList_;

	/// The array of particle affectors
	Array<ParticleAffector *> affectors_;

	/// A flag indicating whether the system should be simulated in local space
	bool inLocalSpace_;

	/// Deleted copy constructor
	ParticleSystem(const ParticleSystem &) = delete;
	/// Deleted assignment operator
	ParticleSystem &operator=(const ParticleSystem &) = delete;
};

}

#endif
