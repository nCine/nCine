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

	/// The common sprite scale factor for particles
	float m_fSpriteScale;

	/// The array of particle affectors
	ncArray<ncParticleAffector *> m_vAffectors;

	virtual void UpdateRenderCommand() { }

public:
	ncParticleSystem(unsigned int uCount, ncTexture *pTexture, float fSpriteScale);
	~ncParticleSystem();

	/// Adds a particle affector
	void AddAffector(ncParticleAffector* affector) { m_vAffectors.InsertBack(affector); }
	/// Emits an amount of particles with a specified initial life and velocity
	void Emit(unsigned int amount, unsigned long int ulLife, const ncVector2f &vel);

	/// Gets the particle sprite scale factor
	float SpriteScale() { return m_fSpriteScale; }
	/// Sets the particle sprite scale factor
	void SetSpriteScale(float fSpriteScale) { m_fSpriteScale = fSpriteScale; }

	virtual void Update(unsigned long int ulInterval);
	virtual void Visit(ncRenderQueue& rRenderQueue);
	virtual void Draw(ncRenderQueue& rRenderQueue);

	inline static eObjectType sType() { return PARTICLESYSTEM_TYPE; }
	static ncParticleSystem* FromId(unsigned int uId);
};

/// Returns a random float between x0 and x1
inline float randBetween(float x0, float x1) { return x0 + (x1 - x0) * (rand() / float(RAND_MAX)); }

#endif
