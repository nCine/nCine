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
	float life_;
	/// Initial particle life
	float startingLife; // for affectors
	/// Current particle velocity vector
	ncVector2f velocity_;
	/// Should the particle transformations be in local space?
	bool inLocalSpace_;

	ncParticle(ncSceneNode* parent, ncTexture *texture);

	// Initializes a particle with initial life, position, velocity and rotation
	void init(float life, ncVector2f pos, ncVector2f vel, float rot, bool inLocalSpace);

	/// Returns true if the particle is still alive
	inline bool isAlive() const { return life_ > 0.0; }

	// Updates particle data after the specified amount of seconds has passed
	virtual void update(float interval);

  private:
	// Custom transform method to allow independent position from parent
	void transform();
};

#endif
