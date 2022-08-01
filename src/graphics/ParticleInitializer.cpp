#include "common_constants.h"
#include "ParticleInitializer.h"
#include <nctl/algorithms.h>

namespace ncine {

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ParticleInitializer::validateMinMax()
{
	if (rndAmount.x > rndAmount.y)
		nctl::swap(rndAmount.x, rndAmount.y);
	if (rndLife.x > rndLife.y)
		nctl::swap(rndLife.x, rndLife.y);
	if (rndPositionX.x > rndPositionX.y)
		nctl::swap(rndPositionX.x, rndPositionX.y);
	if (rndPositionY.x > rndPositionY.y)
		nctl::swap(rndPositionY.x, rndPositionY.y);
	if (rndVelocityX.x > rndVelocityX.y)
		nctl::swap(rndVelocityX.x, rndVelocityX.y);
	if (rndVelocityY.x > rndVelocityY.y)
		nctl::swap(rndVelocityY.x, rndVelocityY.y);
	if (rndRotation.x > rndRotation.y)
		nctl::swap(rndRotation.x, rndRotation.y);
}

void ParticleInitializer::setAmount(int amount)
{
	if (amount <= 0)
		amount = 1;

	rndAmount.set(amount, amount);
}

void ParticleInitializer::setAmount(int minAmount, int maxAmount)
{
	if (minAmount <= 0)
		minAmount = 1;
	if (maxAmount <= 0)
		maxAmount = 1;

	if (minAmount > maxAmount)
		nctl::swap(minAmount, maxAmount);

	rndAmount.set(minAmount, maxAmount);
}

void ParticleInitializer::setLife(float life)
{
	rndLife.set(life, life);
}

void ParticleInitializer::setLife(float minLife, float maxLife)
{
	if (minLife > maxLife)
		nctl::swap(minLife, maxLife);

	rndLife.set(minLife, maxLife);
}

void ParticleInitializer::setPosition(float x, float y)
{
	rndPositionX.set(x, x);
	rndPositionY.set(y, y);
}

void ParticleInitializer::setPosition(float minX, float minY, float maxX, float maxY)
{
	if (minX > maxX)
		nctl::swap(minX, maxX);
	if (minY > maxY)
		nctl::swap(minY, maxY);

	rndPositionX.set(minX, maxX);
	rndPositionY.set(minY, maxY);
}

void ParticleInitializer::setPositionAndRadius(float x, float y, float radius)
{
	if (radius < 0.0f)
		radius = -radius;

	rndPositionX.set(x - radius, x + radius);
	rndPositionY.set(y - radius, y + radius);
}

void ParticleInitializer::setPosition(const Vector2f &pos)
{
	setPosition(pos.x, pos.y);
}

void ParticleInitializer::setPosition(const Vector2f &minPos, const Vector2f &maxPos)
{
	setPosition(minPos.x, minPos.y, maxPos.x, maxPos.y);
}

void ParticleInitializer::setPositionAndRadius(const Vector2f &pos, float radius)
{
	setPositionAndRadius(pos.x, pos.y, radius);
}

void ParticleInitializer::setPositionInDisc(float radius)
{
	setPositionAndRadius(0.0f, 0.0f, radius);
}

void ParticleInitializer::setVelocity(float x, float y)
{
	rndVelocityX.set(x, x);
	rndVelocityY.set(y, y);
}

void ParticleInitializer::setVelocity(float minX, float minY, float maxX, float maxY)
{
	if (minX > maxX)
		nctl::swap(minX, maxX);
	if (minY > maxY)
		nctl::swap(minY, maxY);

	rndVelocityX.set(minX, maxX);
	rndVelocityY.set(minY, maxY);
}

void ParticleInitializer::setVelocityAndRadius(float x, float y, float radius)
{
	if (radius < 0.0f)
		radius = -radius;

	rndVelocityX.set(x - radius, x + radius);
	rndVelocityY.set(y - radius, y + radius);
}

void ParticleInitializer::setVelocityAndScale(float x, float y, float minScale, float maxScale)
{
	if (minScale > maxScale)
		nctl::swap(minScale, maxScale);

	rndVelocityX.set(x * minScale, x * maxScale);
	rndVelocityY.set(y * minScale, y * maxScale);

	if (rndVelocityX.x > rndVelocityX.y)
		nctl::swap(rndVelocityX.x, rndVelocityX.y);
	if (rndVelocityY.x > rndVelocityY.y)
		nctl::swap(rndVelocityY.x, rndVelocityY.y);
}

void ParticleInitializer::setVelocityAndAngle(float x, float y, float angle)
{
	const float sinAngle = sinf(angle * 0.5f * fDegToRad);
	const float cosAngle = cosf(angle * 0.5f * fDegToRad);

	rndVelocityX.x = x * cosAngle - y * sinAngle;
	rndVelocityX.y = x * cosAngle - y * -sinAngle;
	rndVelocityY.x = x * sinAngle + y * cosAngle;
	rndVelocityY.y = x * -sinAngle + y * cosAngle;

	if (rndVelocityX.x > rndVelocityX.y)
		nctl::swap(rndVelocityX.x, rndVelocityX.y);
	if (rndVelocityY.x > rndVelocityY.y)
		nctl::swap(rndVelocityY.x, rndVelocityY.y);
}

void ParticleInitializer::setVelocity(const Vector2f &vel)
{
	setVelocity(vel.x, vel.y);
}

void ParticleInitializer::setVelocity(const Vector2f &minVel, const Vector2f &maxVel)
{
	setVelocity(minVel.x, minVel.y, maxVel.x, maxVel.y);
}

void ParticleInitializer::setVelocityAndRadius(const Vector2f &vel, float radius)
{
	setVelocityAndRadius(vel.x, vel.y, radius);
}

void ParticleInitializer::setVelocityAndScale(const Vector2f &vel, float minScale, float maxScale)
{
	setVelocityAndScale(vel.x, vel.y, minScale, maxScale);
}

void ParticleInitializer::setVelocityAndAngle(const Vector2f &vel, float angle)
{
	setVelocityAndAngle(vel.x, vel.y, angle);
}

void ParticleInitializer::setRotation(float rot)
{
	rndRotation.set(rot, rot);
}

void ParticleInitializer::setRotation(float minRot, float maxRot)
{
	if (minRot > maxRot)
		nctl::swap(minRot, maxRot);

	rndRotation.set(minRot, maxRot);
}

}
