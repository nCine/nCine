#include <nctl/algorithms.h>
#include "ParticleAffectors.h"
#include "Particle.h"

namespace ncine {

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ParticleAffector::affect(Particle *particle)
{
	const float normalizedAge = 1.0f - particle->life_ / particle->startingLife;
	affect(particle, normalizedAge);
}

///////////////////////////////////////////////////////////
// COLOR AFFECTOR
///////////////////////////////////////////////////////////

void ColorAffector::addColorStep(float age, const Colorf &color)
{
	age = nctl::clamp(age, 0.0f, 1.0f);

	unsigned int index = 0;
	for (unsigned int i = 0; i < colorSteps_.size(); i++)
	{
		ColorAffector::ColorStep &step = colorSteps_[i];
		if (age >= step.age && (colorSteps_.size() <= i + 1 || age <= colorSteps_[i + 1].age))
		{
			index = i + 1;
			break;
		}
	}

	colorSteps_.emplaceAt(index, age, color);
}

void ColorAffector::removeStep(unsigned int index)
{
	if (index < colorSteps_.size())
		colorSteps_.removeAt(index);
}

void ColorAffector::affect(Particle *particle, float normalizedAge)
{
	ASSERT(particle);
	ASSERT(normalizedAge >= 0.0f && normalizedAge <= 1.0f);

	// Affector is disabled or has zero steps
	if (enabled_ == false || colorSteps_.isEmpty())
		return;

	if (normalizedAge <= colorSteps_[0].age)
	{
		particle->setColorF(colorSteps_[0].color);
		return;
	}
	else if (normalizedAge >= colorSteps_.back().age)
	{
		particle->setColorF(colorSteps_.back().color);
		return;
	}

	unsigned int index = 0;
	for (index = 0; index < colorSteps_.size() - 1; index++)
	{
		if (colorSteps_[index].age > normalizedAge)
			break;
	}

	FATAL_ASSERT(index > 0);
	const ColorStep &prevStep = colorSteps_[index - 1];
	const ColorStep &nextStep = colorSteps_[index];

	const float factor = (normalizedAge - prevStep.age) / (nextStep.age - prevStep.age);
	const float red = prevStep.color.r() + (nextStep.color.r() - prevStep.color.r()) * factor;
	const float green = prevStep.color.g() + (nextStep.color.g() - prevStep.color.g()) * factor;
	const float blue = prevStep.color.b() + (nextStep.color.b() - prevStep.color.b()) * factor;
	const float alpha = prevStep.color.a() + (nextStep.color.a() - prevStep.color.a()) * factor;
	const Colorf color(red, green, blue, alpha);

	particle->setColorF(color);
}

///////////////////////////////////////////////////////////
// SIZE AFFECTOR
///////////////////////////////////////////////////////////

void SizeAffector::addSizeStep(float age, float scaleX, float scaleY)
{
	age = nctl::clamp(age, 0.0f, 1.0f);

	unsigned int index = 0;
	for (unsigned int i = 0; i < sizeSteps_.size(); i++)
	{
		SizeAffector::SizeStep &step = sizeSteps_[i];
		if (age >= step.age && (sizeSteps_.size() <= i + 1 || age <= sizeSteps_[i + 1].age))
		{
			index = i + 1;
			break;
		}
	}

	sizeSteps_.emplaceAt(index, age, scaleX, scaleY);
}

void SizeAffector::removeStep(unsigned int index)
{
	if (index < sizeSteps_.size())
		sizeSteps_.removeAt(index);
}

void SizeAffector::affect(Particle *particle, float normalizedAge)
{
	ASSERT(particle);
	ASSERT(normalizedAge >= 0.0f && normalizedAge <= 1.0f);

	// Affector is disabled
	if (enabled_ == false)
		return;

	// Zero steps in the affector
	if (sizeSteps_.isEmpty())
	{
		// Applying base scale even with no steps
		particle->setScale(baseScale_);
		return;
	}

	if (normalizedAge <= sizeSteps_[0].age)
	{
		particle->setScale(baseScale_ * sizeSteps_[0].scale);
		return;
	}
	else if (normalizedAge >= sizeSteps_.back().age)
	{
		particle->setScale(baseScale_ * sizeSteps_.back().scale);
		return;
	}

	unsigned int index = 0;
	for (index = 0; index < sizeSteps_.size() - 1; index++)
	{
		if (sizeSteps_[index].age > normalizedAge)
			break;
	}

	FATAL_ASSERT(index > 0);
	const SizeStep &prevStep = sizeSteps_[index - 1];
	const SizeStep &nextStep = sizeSteps_[index];

	const float factor = (normalizedAge - prevStep.age) / (nextStep.age - prevStep.age);
	const Vector2f newScale = prevStep.scale + (nextStep.scale - prevStep.scale) * factor;

	particle->setScale(baseScale_ * newScale);
}

///////////////////////////////////////////////////////////
// ROTATION AFFECTOR
///////////////////////////////////////////////////////////

void RotationAffector::addRotationStep(float age, float angle)
{
	age = nctl::clamp(age, 0.0f, 1.0f);

	unsigned int index = 0;
	for (unsigned int i = 0; i < rotationSteps_.size(); i++)
	{
		RotationAffector::RotationStep &step = rotationSteps_[i];
		if (age >= step.age && (rotationSteps_.size() <= i + 1 || age <= rotationSteps_[i + 1].age))
		{
			index = i + 1;
			break;
		}
	}

	rotationSteps_.emplaceAt(index, age, angle);
}

void RotationAffector::removeStep(unsigned int index)
{
	if (index < rotationSteps_.size())
		rotationSteps_.removeAt(index);
}

void RotationAffector::affect(Particle *particle, float normalizedAge)
{
	ASSERT(particle);
	ASSERT(normalizedAge >= 0.0f && normalizedAge <= 1.0f);

	// Affector is disabled or has zero steps
	if (enabled_ == false || rotationSteps_.isEmpty())
		return;

	if (normalizedAge <= rotationSteps_[0].age)
	{
		particle->setRotation(particle->startingRotation + rotationSteps_[0].angle);
		return;
	}
	else if (normalizedAge >= rotationSteps_.back().age)
	{
		particle->setRotation(particle->startingRotation + rotationSteps_.back().angle);
		return;
	}

	unsigned int index = 0;
	for (index = 0; index < rotationSteps_.size() - 1; index++)
	{
		if (rotationSteps_[index].age > normalizedAge)
			break;
	}

	FATAL_ASSERT(index > 0);
	const RotationStep &prevStep = rotationSteps_[index - 1];
	const RotationStep &nextStep = rotationSteps_[index];

	const float factor = (normalizedAge - prevStep.age) / (nextStep.age - prevStep.age);
	const float newAngle = prevStep.angle + (nextStep.angle - prevStep.angle) * factor;

	particle->setRotation(particle->startingRotation + newAngle);
}

///////////////////////////////////////////////////////////
// POSITION AFFECTOR
///////////////////////////////////////////////////////////

void PositionAffector::addPositionStep(float age, float posX, float posY)
{
	age = nctl::clamp(age, 0.0f, 1.0f);

	unsigned int index = 0;
	for (unsigned int i = 0; i < positionSteps_.size(); i++)
	{
		PositionAffector::PositionStep &step = positionSteps_[i];
		if (age >= step.age && (positionSteps_.size() <= i + 1 || age <= positionSteps_[i + 1].age))
		{
			index = i + 1;
			break;
		}
	}

	positionSteps_.emplaceAt(index, age, posX, posY);
}

void PositionAffector::removeStep(unsigned int index)
{
	if (index < positionSteps_.size())
		positionSteps_.removeAt(index);
}

void PositionAffector::affect(Particle *particle, float normalizedAge)
{
	ASSERT(particle);
	ASSERT(normalizedAge >= 0.0f && normalizedAge <= 1.0f);

	// Affector is disabled or has zero steps
	if (enabled_ == false || positionSteps_.isEmpty())
		return;

	if (normalizedAge <= positionSteps_[0].age)
	{
		particle->move(positionSteps_[0].position);
		return;
	}
	else if (normalizedAge >= positionSteps_.back().age)
	{
		particle->move(positionSteps_.back().position);
		return;
	}

	unsigned int index = 0;
	for (index = 0; index < positionSteps_.size() - 1; index++)
	{
		if (positionSteps_[index].age > normalizedAge)
			break;
	}

	FATAL_ASSERT(index > 0);
	const PositionStep &prevStep = positionSteps_[index - 1];
	const PositionStep &nextStep = positionSteps_[index];

	const float factor = (normalizedAge - prevStep.age) / (nextStep.age - prevStep.age);
	const Vector2f newPosition = prevStep.position + (nextStep.position - prevStep.position) * factor;

	particle->move(newPosition);
}

///////////////////////////////////////////////////////////
// VELOCITY AFFECTOR
///////////////////////////////////////////////////////////

void VelocityAffector::addVelocityStep(float age, float velX, float velY)
{
	age = nctl::clamp(age, 0.0f, 1.0f);

	unsigned int index = 0;
	for (unsigned int i = 0; i < velocitySteps_.size(); i++)
	{
		VelocityAffector::VelocityStep &step = velocitySteps_[i];
		if (age >= step.age && (velocitySteps_.size() <= i + 1 || age <= velocitySteps_[i + 1].age))
		{
			index = i + 1;
			break;
		}
	}

	velocitySteps_.emplaceAt(index, age, velX, velY);
}

void VelocityAffector::removeStep(unsigned int index)
{
	if (index < velocitySteps_.size())
		velocitySteps_.removeAt(index);
}

void VelocityAffector::affect(Particle *particle, float normalizedAge)
{
	ASSERT(particle);
	ASSERT(normalizedAge >= 0.0f && normalizedAge <= 1.0f);

	// Affector is disabled or has zero steps
	if (enabled_ == false || velocitySteps_.isEmpty())
		return;

	if (normalizedAge <= velocitySteps_[0].age)
	{
		particle->velocity_ += velocitySteps_[0].velocity;
		return;
	}
	else if (normalizedAge >= velocitySteps_.back().age)
	{
		particle->velocity_ += velocitySteps_.back().velocity;
		return;
	}

	unsigned int index = 0;
	for (index = 0; index < velocitySteps_.size() - 1; index++)
	{
		if (velocitySteps_[index].age > normalizedAge)
			break;
	}

	FATAL_ASSERT(index > 0);
	const VelocityStep &prevStep = velocitySteps_[index - 1];
	const VelocityStep &nextStep = velocitySteps_[index];

	const float factor = (normalizedAge - prevStep.age) / (nextStep.age - prevStep.age);
	const Vector2f newVelocity = prevStep.velocity + (nextStep.velocity - prevStep.velocity) * factor;

	particle->velocity_ += newVelocity;
}

}
