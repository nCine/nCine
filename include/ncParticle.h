#ifndef CLASS_NCPARTICLE
#define CLASS_NCPARTICLE

#include "ncSprite.h"
#include "ncVector2f.h"
#include "ncColor.h"
class ncTexture;

/// The class for a single particle data
class ncParticle : public ncSprite
{
private:
	// Custom transform method to allow independent position from parent
	void Transform();

public:
	/// Current particle life
	unsigned long int m_ulLife;
	/// Initial particle life
	unsigned long int m_ulStartLife; // for affectors
	/// Current particle velocity vector
	ncVector2f m_Velocity;

	ncParticle(ncSceneNode* pParent, ncTexture *pTexture) : ncSprite(pParent, pTexture), m_ulLife(0), m_ulStartLife(0) { }
	// Initializes a particle with initial life, position, velocity and rotation
	void Init(unsigned long int ulLife, ncVector2f pos, ncVector2f vel, float fRot);

	/// Returns true if the particle is still alive
	inline bool isAlive() { return m_ulLife > 0; }

	// Updates particle data after the specified amount of milliseconds has passed
	virtual void Update(unsigned long int ulInterval);
};

#endif
