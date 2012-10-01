#include "ncParticleSystem.h"
#include "ncVector2f.h"
#include "ncParticle.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Constructs a particle system made of the specified maximum amount of particles
ncParticleSystem::ncParticleSystem(ncSceneNode* pParent, unsigned int uCount, ncTexture *pTexture, ncRect texRect)
	: ncDrawableNode(pParent, 0, 0), m_uPoolSize(uCount), m_uPoolTop(uCount-1), m_vAffectors(4), m_bLocalSpace(false)
{
	m_eType = PARTICLESYSTEM_TYPE;
	SetPriority(ncDrawableNode::SCENE_PRIORITY);

	m_pParticlePool = new ncParticle*[m_uPoolSize];
	m_pParticleList = new ncParticle*[m_uPoolSize];
	for (unsigned int i = 0; i < m_uPoolSize; i++)
	{
		m_pParticlePool[i] = new ncParticle(NULL, pTexture);
		m_pParticlePool[i]->SetTexRect(texRect);
		m_pParticleList[i] = m_pParticlePool[i];
	}
}

ncParticleSystem::~ncParticleSystem()
{
	// Empty the children list before the mass deletion
	m_children.Clear();

	unsigned int i;

	for (i = 0; i < m_vAffectors.Size(); i++)
		delete m_vAffectors[i];

	for (i = 0; i < m_uPoolSize; i++)
		delete m_pParticleList[i];

	delete[] m_pParticlePool;
	delete[] m_pParticleList;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncParticleSystem::Emit(unsigned int amount, unsigned long int ulLife, const ncVector2f &vel)
{
	unsigned long int ulRndLife;
	ncVector2f RndPosition;
	ncVector2f RndVelocity;

	// Particles are rotated towards the emission vector
	float fRotation = -(atan2(vel.y, vel.x) - atan2(1.0f, 0.0f)) * 180.0f/M_PI;
	if (fRotation < 0.0f)
		fRotation += 360;

	for(unsigned int i = 0; i < amount; i++)
	{
		// No more unused particles in the pool
		if (m_uPoolTop == 0)
			break;

		ulRndLife = ulLife * randBetween(0.85f, 1.0f);
		// FIXME: arbitrary random position amount
		RndPosition.x = 10.0f * randBetween(-1.0f, 1.0f); // 25
		RndPosition.y = 10.0f * randBetween(-1.0f, 1.0f);
		RndVelocity.x = vel.x * randBetween(0.8f, 1.0f);
		RndVelocity.y = vel.y * randBetween(0.8f, 1.0f);

		if (m_bLocalSpace == false)
			RndPosition += AbsPosition();

		// acquiring a particle from the pool
		m_pParticlePool[m_uPoolTop]->Init(ulRndLife, RndPosition, RndVelocity, fRotation, m_bLocalSpace);
		AddChildNode(m_pParticlePool[m_uPoolTop]);
		m_uPoolTop--;
	}
}

void ncParticleSystem::Update(unsigned long int ulInterval)
{
	// early return if the node has not to be updated
	if(!bShouldUpdate)
		return;

	for(ncList<ncSceneNode *>::Const_Iterator i = m_children.Begin(); i != m_children.End(); i++)
	{
		ncParticle *pParticle = static_cast<ncParticle *>(*i);

		// Update the particle if it's alive
		if (pParticle->isAlive())
		{
			for (unsigned int j = 0; j < m_vAffectors.Size(); j++)
				m_vAffectors[j]->Affect(pParticle);

			pParticle->Update(ulInterval);

			// Releasing the particle if it has just died
			if (pParticle->isAlive() == false)
			{
				// releasing a particle
				m_uPoolTop++;
				m_pParticlePool[m_uPoolTop] = pParticle;
				RemoveChildNode(i++);
			}
		}
	}
}
