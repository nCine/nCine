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
	/// Current particle life in seconds
	float m_fLife;
	/// Initial particle life
	float m_fStartLife; // for affectors
	/// Current particle velocity vector
	ncVector2f m_Velocity;
	/// Should the particle transformations be in local space?
	bool m_bLocalSpace;

	ncParticle(ncSceneNode* pParent, ncTexture *pTexture);

	// Initializes a particle with initial life, position, velocity and rotation
	void Init(float fLife, ncVector2f pos, ncVector2f vel, float fRot, bool bLocalSpace);

	/// Returns true if the particle is still alive
	inline bool isAlive() const { return m_fLife > 0.0; }

	// Updates particle data after the specified amount of seconds has passed
	virtual void Update(float fInterval);

  private:
	// Custom transform method to allow independent position from parent
	void Transform();
};

#endif
