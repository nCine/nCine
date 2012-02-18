#include "ncParticleAffectors.h"
#include "ncColor.h"
#include "ncArray.h"

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncColorAffector::AddColorStep(float fTime, ncColor color)
{
	if (m_vColorSteps.isEmpty() || fTime > m_vColorSteps[m_vColorSteps.Size()-1]->fTime)
		m_vColorSteps.InsertBack(new ColorStep(fTime, color));
	else
		ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, "ncColorAffector::AddColorStep - Out of order step not added");
}

void ncColorAffector::Affect(ncParticle* pParticle)
{
	unsigned int idx;
	float fNormalizedLife = float(pParticle->m_ulLife) / float(pParticle->m_ulStartLife);

	for (idx = 0; idx < m_vColorSteps.Size()-1; idx++)
		if (m_vColorSteps[idx]->fTime > fNormalizedLife)
			break;

//	assert(m_vColorSteps[idx-1]->fTime <= fNormalizedLife);

	float prevTime = m_vColorSteps[idx-1]->fTime;
	float thisTime = m_vColorSteps[idx]->fTime;
	ncColor &prevColor = m_vColorSteps[idx-1]->color;
	ncColor &thisColor = m_vColorSteps[idx]->color;

	float fFactor = (fNormalizedLife - prevTime) / (thisTime - prevTime);
	GLubyte red = prevColor.R() + (thisColor.R() - prevColor.R()) * fFactor;
	GLubyte green = prevColor.G() + (thisColor.G() - prevColor.G()) * fFactor;
	GLubyte blue = prevColor.B() + (thisColor.B() - prevColor.B()) * fFactor;
	GLubyte alpha = prevColor.A() + (thisColor.A() - prevColor.A()) * fFactor;

	pParticle->SetColor(red, green, blue, alpha);
}

void ncSizeAffector::AddSizeStep(float fTime, float fScale)
{
	if (m_vSizeSteps.isEmpty() || fTime > m_vSizeSteps[m_vSizeSteps.Size()-1]->fTime)
		m_vSizeSteps.InsertBack(new SizeStep(fTime, fScale));
	else
		ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, "ncSizeAffector::AddSizeStep - Out of order step not added");
}

void ncSizeAffector::Affect(ncParticle* pParticle)
{
	unsigned int idx;
	float fNormalizedLife = float(pParticle->m_ulLife) / float(pParticle->m_ulStartLife);

	for (idx = 0; idx < m_vSizeSteps.Size()-1; idx++)
		if (m_vSizeSteps[idx]->fTime > fNormalizedLife)
			break;

//	assert(m_vSizeSteps[idx-1]->fTime <= fNormalizedLife);

	float prevTime = m_vSizeSteps[idx-1]->fTime;
	float thisTime = m_vSizeSteps[idx]->fTime;
	float prevScale = m_vSizeSteps[idx-1]->fScale;
	float thisScale = m_vSizeSteps[idx]->fScale;

	float fFactor = (fNormalizedLife - prevTime) / (thisTime - prevTime);
	float newScale = prevScale + (thisScale - prevScale) * fFactor;

	pParticle->SetScale(m_fBaseScale * newScale);
}
