#include "ncParticleAffectors.h"
#include "ncColor.h"
#include "ncArray.h"

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncColorAffector::addColorStep(float time, ncColor color)
{
	if (colorSteps_.isEmpty() || time > colorSteps_[colorSteps_.size() - 1]->time)
	{
		colorSteps_.insertBack(new ColorStep(time, color));
	}
	else
	{
		ncServiceLocator::logger().write(ncILogger::LOG_WARN, "ncColorAffector::addColorStep - Out of order step not added");
	}
}

void ncColorAffector::affect(ncParticle* particle)
{
	unsigned int index;
	float normalizedLife = particle->life_ / particle->startingLife;

	for (index = 0; index < colorSteps_.size() - 1; index++)
	{
		if (colorSteps_[index]->time > normalizedLife)
		{
			break;
		}
	}

//	assert(colorSteps_[index - 1]->time <= normalizedLife);

	float prevTime = colorSteps_[index - 1]->time;
	float thisTime = colorSteps_[index]->time;
	ncColor &prevColor = colorSteps_[index - 1]->color;
	ncColor &thisColor = colorSteps_[index]->color;

	float factor = (normalizedLife - prevTime) / (thisTime - prevTime);
	GLubyte red = prevColor.r() + (thisColor.r() - prevColor.r()) * factor;
	GLubyte green = prevColor.g() + (thisColor.g() - prevColor.g()) * factor;
	GLubyte blue = prevColor.b() + (thisColor.b() - prevColor.b()) * factor;
	GLubyte alpha = prevColor.a() + (thisColor.a() - prevColor.a()) * factor;

	particle->setColor(red, green, blue, alpha);
}

void ncSizeAffector::addSizeStep(float time, float scale)
{
	if (sizeSteps_.isEmpty() || time > sizeSteps_[sizeSteps_.size() - 1]->time)
	{
		sizeSteps_.insertBack(new SizeStep(time, scale));
	}
	else
	{
		ncServiceLocator::logger().write(ncILogger::LOG_WARN, "ncSizeAffector::addSizeStep - Out of order step not added");
	}
}

void ncSizeAffector::affect(ncParticle* particle)
{
	unsigned int index;
	float normalizedLife = particle->life_ / particle->startingLife;

	for (index = 0; index < sizeSteps_.size() - 1; index++)
	{
		if (sizeSteps_[index]->time > normalizedLife)
		{
			break;
		}
	}

//	assert(sizeSteps[index - 1]->time <= normalizedLife);

	float prevTime = sizeSteps_[index - 1]->time;
	float thisTime = sizeSteps_[index]->time;
	float prevScale = sizeSteps_[index - 1]->scale;
	float thisScale = sizeSteps_[index]->scale;

	float factor = (normalizedLife - prevTime) / (thisTime - prevTime);
	float newScale = prevScale + (thisScale - prevScale) * factor;

	particle->setScale(baseScale_ * newScale);
}
