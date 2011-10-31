#ifndef CLASS_NCPARTICLEAFFECTORS
#define CLASS_NCPARTICLEAFFECTORS

#include <ctime>
#include <cstdlib>

#include "ncParticle.h"
class ncVector2f;
class ncColor;
template <class T> class ncArray;

class ncParticleAffector
{
public:
	virtual void Affect(ncParticle* pParticle) = 0;
};

class ncAccelerationAffector: public ncParticleAffector
{
	ncVector2f m_vAcceleration;
public:
	ncAccelerationAffector(float fX, float fY) : m_vAcceleration(fX, fY) { }
	virtual void Affect(ncParticle* pParticle)
	{
		pParticle->m_Velocity += m_vAcceleration;
	}
};

class ncColorAffector: public ncParticleAffector
{
public:
	ncColorAffector() : m_vColorSteps(4) { }
	~ncColorAffector()
	{
		for (int i = 0; i < m_vColorSteps.Size(); i++)
			delete m_vColorSteps[i];
	}

	virtual void Affect(ncParticle* pParticle);
	void AddColorStep(float fTime, ncColor color);
private:
	struct ColorStep {
		float fTime;
		ncColor color;

		ColorStep(float newTime, ncColor newColor): fTime(newTime), color(newColor) { }
	};

	ncArray<ColorStep *> m_vColorSteps;
};

class ncSizeAffector: public ncParticleAffector
{
public:
	ncSizeAffector(float fBaseScale) : m_vSizeSteps(4), m_fBaseScale(fBaseScale) { }
	~ncSizeAffector()
	{
		for (int i = 0; i < m_vSizeSteps.Size(); i++)
			delete m_vSizeSteps[i];
	}

	virtual void Affect(ncParticle* pParticle);
	void AddSizeStep(float fTime, float fScale);
private:
	struct SizeStep {
		float fTime;
		float fScale;

		SizeStep(float newTime, float newScale): fTime(newTime), fScale(newScale) { }
	};

	ncArray<SizeStep *> m_vSizeSteps;
	float m_fBaseScale;
};

#endif
