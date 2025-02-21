#ifndef CLASS_NCINE_PARTICLE
#define CLASS_NCINE_PARTICLE

#include "Sprite.h"
#include "Vector2.h"

namespace ncine {

class Texture;

/// The class holding all the relevant data for a single particle
/*!
 * \warning Particle objects should only be constructed and manipulated by a `ParticleSystem` and particle affectors, never by the user.
 * \note The header is public only because the particle array inside the `ParticleSystem` cannot properly work with forward declaration.
 */
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

	/// Constructor for a particle with a parent and texture, positioned in the relative origin
	Particle(SceneNode *parent, Texture *texture);

	/// Default move constructor
	Particle(Particle &&) = default;
	/// Default move assignment operator
	Particle &operator=(Particle &&) = default;

	/// Returns true if the particle is still alive
	inline bool isAlive() const { return life_ > 0.0f; }

  protected:
	/// Returns a copy of this object
	/*! \note This method is protected as it should only be called by a `ParticleSystem` */
	inline Particle clone() const { return Particle(*this); }

	/// Protected copy constructor used to clone objects
	Particle(const Particle &other);

  private:
	/// Initializes a particle with initial life, position, velocity and rotation
	void init(float life, Vector2f pos, Vector2f vel, float rot, bool inLocalSpace);
	/// Updates particle data after the specified amount of seconds has passed
	void update(float frameTime) override;
	/// Custom transform method to allow independent position from parent
	void transform() override;

	/// Deleted assignment operator
	Particle &operator=(const Particle &) = delete;

	friend class ParticleSystem;
};

}

#endif
