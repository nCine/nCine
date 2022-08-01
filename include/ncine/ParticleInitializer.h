#ifndef CLASS_NCINE_PARTICLEINITIALIZER
#define CLASS_NCINE_PARTICLEINITIALIZER

#include "Vector2.h"

namespace ncine {

/// Initialization parameters for particles
/*! The vectors define a range between a minimum and a maximum value */
struct DLL_PUBLIC ParticleInitializer
{
	Vector2i rndAmount = Vector2i(1, 1);
	Vector2f rndLife = Vector2f(1.0f, 1.0f);
	Vector2f rndPositionX = Vector2f::Zero;
	Vector2f rndPositionY = Vector2f::Zero;
	Vector2f rndVelocityX = Vector2f::Zero;
	Vector2f rndVelocityY = Vector2f::Zero;
	Vector2f rndRotation = Vector2f::Zero;
	bool emitterRotation = true;

	/// Checks all vectors swapping values if the first component is greater than the second one
	void validateMinMax();

	void setAmount(int amount);
	void setAmount(int minAmount, int maxAmount);

	void setLife(float life);
	void setLife(float minLife, float maxLife);

	void setPosition(float x, float y);
	void setPosition(float minX, float minY, float maxX, float maxY);
	void setPositionAndRadius(float x, float y, float radius);
	void setPosition(const Vector2f &pos);
	void setPosition(const Vector2f &minPos, const Vector2f &maxPos);
	void setPositionAndRadius(const Vector2f &pos, float radius);
	void setPositionInDisc(float radius);

	void setVelocity(float x, float y);
	void setVelocity(float minX, float minY, float maxX, float maxY);
	void setVelocityAndRadius(float x, float y, float radius);
	void setVelocityAndScale(float x, float y, float minScale, float maxScale);
	void setVelocityAndAngle(float x, float y, float angle);
	void setVelocity(const Vector2f &vel);
	void setVelocity(const Vector2f &minVel, const Vector2f &maxVel);
	void setVelocityAndRadius(const Vector2f &vel, float radius);
	void setVelocityAndScale(const Vector2f &vel, float minScale, float maxScale);
	void setVelocityAndAngle(const Vector2f &vel, float angle);

	void setRotation(float rot);
	void setRotation(float minRot, float maxRot);
};

}

#endif
