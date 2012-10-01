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
private:
	/// The particle pool size
	unsigned int m_uPoolSize;
	/// The index of the next free particle in the pool
	unsigned int m_uPoolTop;
	/// The pool containing available particles (only dead ones)
	ncParticle **m_pParticlePool;
	/// The array containing every particle (dead or alive)
	ncParticle **m_pParticleList;

	/// The array of particle affectors
	ncArray<ncParticleAffector *> m_vAffectors;

	/// Should the system simulate in local space?
	bool m_bLocalSpace;

	virtual void UpdateRenderCommand() { }

public:
	// Constructs a particle system made of the specified maximum amount of particles
	ncParticleSystem(ncSceneNode* pParent, unsigned int uCount, ncTexture *pTexture, ncRect texRect);
	~ncParticleSystem();

	/// Adds a particle affector
	void AddAffector(ncParticleAffector* affector) { m_vAffectors.InsertBack(affector); }
	/// Emits an amount of particles with a specified initial life and velocity
	void Emit(unsigned int amount, unsigned long int ulLife, const ncVector2f &vel);


	/// Gets the local space flag of the system
	inline bool LocalSpace(void) { return m_bLocalSpace; }
	/// Sets the local space flag of the system
	inline void SetLocalSpace(bool bLocalSpace) { m_bLocalSpace = bLocalSpace; }

	virtual void Update(unsigned long int ulInterval);

	inline static eObjectType sType() { return PARTICLESYSTEM_TYPE; }
};

/// Returns a random float between x0 and x1
inline float randBetween(float x0, float x1) { return x0 + (x1 - x0) * (rand() / float(RAND_MAX)); }

#endif
