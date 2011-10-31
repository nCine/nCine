#include "ncParticleSystem.h"
#include "ncVector2f.h"
#include "ncParticle.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncParticleSystem::ncParticleSystem(unsigned int count, ncTexture *pTexture, float fSpriteScale)
	: m_uPoolSize(count), m_uPoolTop(count-1), m_fSpriteScale(fSpriteScale), m_vAffectors(8)
{
	m_eType = PARTICLESYSTEM_TYPE;

	m_pParticlePool = new ncParticle*[count];
	m_pParticleList = new ncParticle*[count];
	for (unsigned int i = 0; i < m_uPoolSize; i++)
	{
		m_pParticlePool[i] = new ncParticle(this, pTexture);
		m_pParticlePool[i]->SetScale(fSpriteScale);
		m_pParticleList[i] = m_pParticlePool[i];
	}
}

ncParticleSystem::~ncParticleSystem()
{
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

void ncParticleSystem::Emit(unsigned int amount, unsigned long int ulLife, ncVector2f vel)
{
	unsigned long int ulRndLife;
	ncPoint RndPosition;
	ncVector2f RndVelocity;

	for(unsigned int i = 0; i < amount; i++)
	{
		// No more unused particles in the pool
		if (m_uPoolTop == 0)
			break;

		ulRndLife = ulLife * randBetween(0.85f, 1.0f);
		// FIXME: arbitrary random position amount
		RndPosition.x = 25 * randBetween(-1.0f, 1.0f);
		RndPosition.y = 10 * randBetween(-1.0f, 1.0f);
		RndVelocity.x = vel.x * randBetween(0.8f, 1.0f);
		RndVelocity.y = vel.y * randBetween(0.8f, 1.0f);

		// acquiring a particle from the pool
		m_pParticlePool[m_uPoolTop]->Init(ulRndLife, RndPosition, RndVelocity);
		m_uPoolTop--;
	}
}

void ncParticleSystem::Update(unsigned long int ulInterval)
{
	unsigned int i, j;

	for(i = 0; i < m_uPoolSize; i++)
	{
		ncParticle *pParticle = m_pParticleList[i];

		if (pParticle->bShouldDraw == false)
			continue;

		if (pParticle->m_ulLife > 0)
		{
			for (j = 0; j < m_vAffectors.Size(); j++)
				m_vAffectors[j]->Affect(pParticle);

			pParticle->Update(ulInterval);
		}
		else
		{
			pParticle->bShouldDraw = false;

			// releasing a particle
			m_uPoolTop++;
			m_pParticlePool[m_uPoolTop] = pParticle;
		}
	}
}

void ncParticleSystem::Visit(ncRenderQueue& rRenderQueue)
{
	// early return if a node is invisible
	if(!bShouldDraw)
		return;

	Transform();

	Draw(rRenderQueue);
}

void ncParticleSystem::Draw(ncRenderQueue& rRenderQueue)
{

	for(unsigned int i = 0; i < m_uPoolSize; i++)
	{
		if (m_pParticleList[i]->bShouldDraw)
			m_pParticleList[i]->Draw(rRenderQueue);
	}
}

ncParticleSystem* ncParticleSystem::FromId(unsigned int uId)
{
	ncObject *pObject = ncServiceLocator::GetIndexer().Object(uId);

	if(pObject)
	{
		if (pObject->Type() == sType())
			return static_cast<ncParticleSystem *>(pObject);
		else // Cannot cast
		{
			ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, "ncParticleSystem::FromId - Object of wrong type");
			exit(-1);
		}
	}
	else // NULL
	{
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_WARN, "ncParticleSystem::FromId - Object not found");
		return NULL;
	}
}
