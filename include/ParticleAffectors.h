#ifndef CLASS_NCINE_PARTICLEAFFECTORS
#define CLASS_NCINE_PARTICLEAFFECTORS

#include "Vector2f.h"
#include "Color.h"
#include "Array.h"

namespace ncine {

class Particle;

/// Base class for particle affectors
class ParticleAffector
{
  public:
	virtual ~ParticleAffector() { }
	virtual void affect(Particle* particle) = 0;
};

/// Particle acceleration affector
class AccelerationAffector: public ParticleAffector
{
  public:
	/// X and Y components of the accelerator vector
	AccelerationAffector(float x, float y) : acceleration_(x, y) { }

	virtual void affect(Particle* particle);

  private:
	Vector2f acceleration_;
};

/// Particle color affector
class ColorAffector: public ParticleAffector
{
  public:
	ColorAffector() : colorSteps_(4) { }
	~ColorAffector()
	{
		for (unsigned int i = 0; i < colorSteps_.size(); i++)
		{
			delete colorSteps_[i];
		}
	}

	virtual void affect(Particle* particle);
	void addColorStep(float time, Color color);

  private:
	struct ColorStep
	{
		float time;
		Color color;

		ColorStep(float newTime, Color newColor): time(newTime), color(newColor) { }
	};

	Array<ColorStep *> colorSteps_;

	/// Private copy constructor
	ColorAffector(const ColorAffector&);
	/// Private assignment operator
	ColorAffector& operator=(const ColorAffector&);
};

/// Particle size affector
class SizeAffector: public ParticleAffector
{
  public:
	/// Constructs a size affector with a base scale factor as a reference
	SizeAffector(float baseScale) : sizeSteps_(4), baseScale_(baseScale) { }
	~SizeAffector()
	{
		for (unsigned int i = 0; i < sizeSteps_.size(); i++)
		{
			delete sizeSteps_[i];
		}
	}

	virtual void affect(Particle* particle);
	void addSizeStep(float time, float scale);

  private:
	struct SizeStep
	{
		float time;
		float scale;

		SizeStep(float newTime, float newScale): time(newTime), scale(newScale) { }
	};

	Array<SizeStep *> sizeSteps_;
	float baseScale_;

	/// Private copy constructor
	SizeAffector(const SizeAffector&);
	/// Private assignment operator
	SizeAffector& operator=(const SizeAffector&);
};

}

#endif
