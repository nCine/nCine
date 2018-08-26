#ifndef CLASS_NCINE_PARTICLE
#define CLASS_NCINE_PARTICLE

#include "Sprite.h"
#include "Vector2.h"
#include "Color.h"

namespace ncine {

class Texture;

/// The class for a single particle data
class Particle : public Sprite
{
  public:
	/// Current particle remaining life in seconds
	float life_;
	/// Initial particle remaining life
	float startingLife; // for affectors
	/// Initial particle rotation
	float startingRotation; // for affectors
	/// Current particle velocity vector
	Vector2f velocity_;
	/// A flag indicating if particle transformations are in local space or not
	bool inLocalSpace_;

	/// Returns true if the particle is still alive
	inline bool isAlive() const { return life_ > 0.0f; }

  private:
	/// Private constructor accessible only by `ParticleSystem`
	Particle(SceneNode *parent, Texture *texture);

	/// Initializes a particle with initial life, position, velocity and rotation
	void init(float life, Vector2f pos, Vector2f vel, float rot, bool inLocalSpace);
	/// Updates particle data after the specified amount of seconds has passed
	void update(float interval) override;
	/// Custom transform method to allow independent position from parent
	void transform() override;

	friend class ParticleSystem;
};

}

#endif
