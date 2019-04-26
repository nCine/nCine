#ifndef CLASS_NCINE_PARTICLESYSTEM
#define CLASS_NCINE_PARTICLESYSTEM

#include <ctime>
#include <cstdlib>
#include "Rect.h"
#include "SceneNode.h"
#include "ParticleAffectors.h"

namespace ncine {

class Texture;
class Particle;
struct ParticleInitializer;

/// The class representing a particle system
class DLL_PUBLIC ParticleSystem : public SceneNode
{
  public:
	/// Constructs a particle system made of the specified maximum amount of particles
	ParticleSystem(SceneNode *parent, unsigned int count, Texture *texture, Recti texRect);
	~ParticleSystem() override;

	/// Adds a particle affector
	inline void addAffector(nctl::UniquePtr<ParticleAffector> affector) { affectors_.pushBack(nctl::move(affector)); }
	/// Deletes all particle affectors
	void clearAffectors();
	/// Emits particles with the specified initialization parameters
	void emitParticles(const ParticleInitializer &init);
	/// Kills all alive particles
	void killParticles();

	/// Returns the local space flag of the system
	inline bool inLocalSpace(void) const { return inLocalSpace_; }
	/// Sets the local space flag of the system
	inline void setInLocalSpace(bool inLocalSpace) { inLocalSpace_ = inLocalSpace; }

	/// Returns the total number of particles in the system
	unsigned int numParticles() const { return particleArray_.size(); }
	/// Returns the number of particles currently alive
	unsigned int numAliveParticles() const { return particleArray_.size() - poolTop_ - 1; }

	/// Sets the texture object for every particle
	void setTexture(Texture *texture);
	/// Sets the texture source rectangle for every particle
	void setTexRect(const Recti &rect);

	/// Sets the rendering layer for every particle
	void setLayer(unsigned int layer);

	void update(float interval) override;

	inline static ObjectType sType() { return ObjectType::PARTICLE_SYSTEM; }

  private:
	/// The particle pool size
	unsigned int poolSize_;
	/// The index of the next free particle in the pool
	int poolTop_;
	/// The pool containing available particles (only dead ones)
	nctl::Array<Particle *> particlePool_;
	/// The array containing every particle (dead or alive)
	nctl::Array<nctl::UniquePtr<Particle>> particleArray_;

	/// The array of particle affectors
	nctl::Array<nctl::UniquePtr<ParticleAffector>> affectors_;

	/// A flag indicating whether the system should be simulated in local space
	bool inLocalSpace_;

	/// Deleted copy constructor
	ParticleSystem(const ParticleSystem &) = delete;
	/// Deleted assignment operator
	ParticleSystem &operator=(const ParticleSystem &) = delete;
};

}

#endif
