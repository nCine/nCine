#include "ParticleAffectors.h"
#include "Color.h"
#include "Particle.h"

namespace ncine {

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void AccelerationAffector::affect(Particle *particle)
{
	ASSERT(particle);
	particle->velocity_ += acceleration_;
}

void ColorAffector::addColorStep(float time, Color color)
{
	if (colorSteps_.isEmpty() || time > colorSteps_[colorSteps_.size() - 1]->time)
		colorSteps_.pushBack(new ColorStep(time, color));
	else
		LOGW("Out of order step not added");
}

void ColorAffector::affect(Particle *particle)
{
	ASSERT(particle);

	// one or no color steps in the affector
	if (colorSteps_.size() < 2)
	{
		if (colorSteps_.size() == 1)
			particle->setColor(colorSteps_[0]->color);
		return;
	}

	const float normalizedLife = particle->life_ / particle->startingLife;

	unsigned int index = 0;
	for (index = 0; index < colorSteps_.size() - 1; index++)
	{
		if (colorSteps_[index]->time > normalizedLife)
			break;
	}

	ASSERT(colorSteps_[index - 1]->time <= normalizedLife);

	const float prevTime = colorSteps_[index - 1]->time;
	const float thisTime = colorSteps_[index]->time;
	const Color &prevColor = colorSteps_[index - 1]->color;
	const Color &thisColor = colorSteps_[index]->color;

	const float factor = (normalizedLife - prevTime) / (thisTime - prevTime);
	const unsigned char red = static_cast<unsigned char>(prevColor.r() + (thisColor.r() - prevColor.r()) * factor);
	const unsigned char green = static_cast<unsigned char>(prevColor.g() + (thisColor.g() - prevColor.g()) * factor);
	const unsigned char blue = static_cast<unsigned char>(prevColor.b() + (thisColor.b() - prevColor.b()) * factor);
	const unsigned char alpha = static_cast<unsigned char>(prevColor.a() + (thisColor.a() - prevColor.a()) * factor);

	particle->setColor(red, green, blue, alpha);
}

void SizeAffector::addSizeStep(float time, float scale)
{
	if (sizeSteps_.isEmpty() || time > sizeSteps_[sizeSteps_.size() - 1]->time)
		sizeSteps_.pushBack(new SizeStep(time, scale));
	else
		LOGW("Out of order step not added");
}

void SizeAffector::affect(Particle *particle)
{
	ASSERT(particle);

	// one or no size steps in the affector
	if (sizeSteps_.size() < 2)
	{
		if (sizeSteps_.size() == 1)
			particle->setScale(sizeSteps_[0]->scale);
		return;
	}

	const float normalizedLife = particle->life_ / particle->startingLife;

	unsigned int index = 0;
	for (index = 0; index < sizeSteps_.size() - 1; index++)
	{
		if (sizeSteps_[index]->time > normalizedLife)
			break;
	}

	ASSERT(sizeSteps_[index - 1]->time <= normalizedLife);

	const float prevTime = sizeSteps_[index - 1]->time;
	const float thisTime = sizeSteps_[index]->time;
	const float prevScale = sizeSteps_[index - 1]->scale;
	const float thisScale = sizeSteps_[index]->scale;

	const float factor = (normalizedLife - prevTime) / (thisTime - prevTime);
	const float newScale = prevScale + (thisScale - prevScale) * factor;

	particle->setScale(baseScale_ * newScale);
}

}
