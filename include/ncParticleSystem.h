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

class ncParticleSystem : public ncDrawableNode
{
	unsigned int m_uPoolSize;
	unsigned int m_uPoolTop;
	ncParticle **m_pParticlePool;
	ncParticle **m_pParticleList;

	ncTexture *m_Texture;
	float m_fSpriteScale;

	ncArray<ncParticleAffector *> m_vAffectors;

	virtual void UpdateRenderCommand() { }
public:
	ncParticleSystem(unsigned int count, ncTexture *pTexture, float fSpriteScale);
	~ncParticleSystem();

	void AddAffector(ncParticleAffector* affector) { m_vAffectors.InsertBack(affector); }
	void Emit(unsigned int amount, unsigned long int ulLife, const ncVector2f vel);

	/// Get the particle sprite scale factor
	float SpriteScale() { return m_fSpriteScale; }
	/// Set the particle sprite scale factor
	void SetSpriteScale(float fSpriteScale) { m_fSpriteScale = fSpriteScale; }

	virtual void Update(unsigned long int ulInterval);
	virtual void Visit(ncRenderQueue& rRenderQueue);
	virtual void Draw(ncRenderQueue& rRenderQueue);

	inline static eObjectType sType() { return PARTICLESYSTEM_TYPE; }
	static ncParticleSystem* FromId(unsigned int uId);
};

/// Return a random float between x0 and x1
inline float randBetween(float x0, float x1) { return x0 + (x1 - x0) * (rand() / float(RAND_MAX)); }

#endif
