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
	unsigned int uIdx;
	float fNormalizedLife = float(pParticle->m_ulLife) / float(pParticle->m_ulStartLife);

	for (uIdx = 0; uIdx < m_vColorSteps.Size()-1; uIdx++)
		if (m_vColorSteps[uIdx]->fTime > fNormalizedLife)
			break;

//	assert(m_vColorSteps[uIdx-1]->fTime <= fNormalizedLife);

	float fPrevTime = m_vColorSteps[uIdx-1]->fTime;
	float fThisTime = m_vColorSteps[uIdx]->fTime;
	ncColor &prevColor = m_vColorSteps[uIdx-1]->color;
	ncColor &thisColor = m_vColorSteps[uIdx]->color;

	float fFactor = (fNormalizedLife - fPrevTime) / (fThisTime - fPrevTime);
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
	unsigned int uIdx;
	float fNormalizedLife = float(pParticle->m_ulLife) / float(pParticle->m_ulStartLife);

	for (uIdx = 0; uIdx < m_vSizeSteps.Size()-1; uIdx++)
		if (m_vSizeSteps[uIdx]->fTime > fNormalizedLife)
			break;

//	assert(m_vSizeSteps[uIdx-1]->fTime <= fNormalizedLife);

	float fPrevTime = m_vSizeSteps[uIdx-1]->fTime;
	float fThisTime = m_vSizeSteps[uIdx]->fTime;
	float fPrevScale = m_vSizeSteps[uIdx-1]->fScale;
	float fThisScale = m_vSizeSteps[uIdx]->fScale;

	float fFactor = (fNormalizedLife - fPrevTime) / (fThisTime - fPrevTime);
	float fNewScale = fPrevScale + (fThisScale - fPrevScale) * fFactor;

	pParticle->SetScale(m_fBaseScale * fNewScale);
}
