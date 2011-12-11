#ifndef CLASS_NCPARTICLE
#define CLASS_NCPARTICLE

#include "ncSprite.h"
#include "ncVector2f.h"
#include "ncColor.h"
class ncTexture;

/// The class for a single particle data
class ncParticle : public ncSprite
{
public:
	/// Current particle life
	unsigned long int m_ulLife;
	/// Initial particle life
	unsigned long int m_ulStartLife; // for affectors
	/// Current particle velocity vector
	ncVector2f m_Velocity;

	ncParticle(ncSceneNode* pParent, ncTexture *pTexture) : ncSprite(pParent, pTexture), m_ulLife(0) { }
	// Initialized a particle with initial life, position and velocity
	void Init(unsigned long int ulLife, ncVector2f pos, ncVector2f vel);

	/// Returns true if the particle is still alive
	inline bool isAlive() { return m_ulLife > 0; }

	// Updates particle data after the specified amount of milliseconds have passed
	virtual void Update(unsigned long int ulInterval);
};

#endif
