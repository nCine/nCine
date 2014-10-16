#ifndef CLASS_NCPARTICLEAFFECTORS
#define CLASS_NCPARTICLEAFFECTORS

#include <ctime>
#include <cstdlib>

#include "ncParticle.h"
class ncVector2f;
class ncColor;
template <class T> class ncArray;

/// Base class for particle affectors
class ncParticleAffector
{
  public:
	virtual ~ncParticleAffector() { }
	virtual void affect(ncParticle* particle) = 0;
};

/// Particle acceleration affector
class ncAccelerationAffector: public ncParticleAffector
{
  public:
	/// X and Y components of the accelerator vector
	ncAccelerationAffector(float x, float y) : acceleration_(x, y) { }

	virtual void affect(ncParticle* particle)
	{
		particle->velocity_ += acceleration_;
	}

  private:
	ncVector2f acceleration_;
};

/// Particle color affector
class ncColorAffector: public ncParticleAffector
{
  public:
	ncColorAffector() : colorSteps_(4) { }
	~ncColorAffector()
	{
		for (unsigned int i = 0; i < colorSteps_.size(); i++)
		{
			delete colorSteps_[i];
		}
	}

	virtual void affect(ncParticle* particle);
	void addColorStep(float time, ncColor color);

  private:
	struct ColorStep
	{
		float time;
		ncColor color;

		ColorStep(float newTime, ncColor newColor): time(newTime), color(newColor) { }
	};

	ncArray<ColorStep *> colorSteps_;
};

/// Particle size affector
class ncSizeAffector: public ncParticleAffector
{
  public:
	/// Constructs a size affector with a base scale factor as a reference
	ncSizeAffector(float baseScale) : sizeSteps_(4), baseScale_(baseScale) { }
	~ncSizeAffector()
	{
		for (unsigned int i = 0; i < sizeSteps_.size(); i++)
		{
			delete sizeSteps_[i];
		}
	}

	virtual void affect(ncParticle* particle);
	void addSizeStep(float time, float scale);

  private:
	struct SizeStep
	{
		float time;
		float scale;

		SizeStep(float newTime, float newScale): time(newTime), scale(newScale) { }
	};

	ncArray<SizeStep *> sizeSteps_;
	float baseScale_;
};

#endif
