#ifndef CLASS_NCINE_PARTICLEAFFECTORS
#define CLASS_NCINE_PARTICLEAFFECTORS

#include "Vector2.h"
#include "Color.h"
#include "nctl/Array.h"

namespace ncine {

class Particle;

/// Base class for particle affectors
class DLL_PUBLIC ParticleAffector
{
  public:
	virtual ~ParticleAffector() { }
	/// Affects a property of the specified particle
	virtual void affect(Particle *particle) = 0;
};

/// Particle acceleration affector
class DLL_PUBLIC AccelerationAffector : public ParticleAffector
{
  public:
	/// X and Y components of the accelerator vector
	AccelerationAffector(float x, float y) : acceleration_(x, y) { }

	/// Affects the acceleration of the specified particle
	void affect(Particle *particle) override;

  private:
	Vector2f acceleration_;
};

/// Particle color affector
class DLL_PUBLIC ColorAffector : public ParticleAffector
{
  public:
	ColorAffector() : colorSteps_(4) { }
	~ColorAffector() override
	{
		for (ColorStep *colorStep : colorSteps_)
			delete colorStep;
	}

	/// Affects the color of the specified particle
	void affect(Particle *particle) override;
	void addColorStep(float time, Color color);

  private:
	struct ColorStep
	{
		float time;
		Color color;

		ColorStep(float newTime, Color newColor) : time(newTime), color(newColor) { }
	};

	nctl::Array<ColorStep *> colorSteps_;

	/// Deleted copy constructor
	ColorAffector(const ColorAffector &) = delete;
	/// Deleted assignment operator
	ColorAffector &operator=(const ColorAffector &) = delete;
};

/// Particle size affector
class DLL_PUBLIC SizeAffector : public ParticleAffector
{
  public:
	/// Constructs a size affector with a base scale factor as a reference
	explicit SizeAffector(float baseScale) : sizeSteps_(4), baseScale_(baseScale) { }
	~SizeAffector() override
	{
		for (SizeStep *sizeStep : sizeSteps_)
			delete sizeStep;
	}

	/// Affects the size of the specified particle
	void affect(Particle *particle) override;
	void addSizeStep(float time, float scale);

  private:
	struct SizeStep
	{
		float time;
		float scale;

		SizeStep(float newTime, float newScale) : time(newTime), scale(newScale) { }
	};

	nctl::Array<SizeStep *> sizeSteps_;
	float baseScale_;

	/// Deleted copy constructor
	SizeAffector(const SizeAffector &) = delete;
	/// Deleted assignment operator
	SizeAffector &operator=(const SizeAffector &) = delete;
};

}

#endif
