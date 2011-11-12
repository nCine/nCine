#include "ncParticle.h"

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncParticle::Init(unsigned long int ulLife, ncVector2f pos, ncVector2f vel)
{
	m_ulLife = ulLife;
	m_ulStartLife = ulLife;
	SetPosition(pos);
	m_Velocity = vel;
}

void ncParticle::Update(unsigned long int ulInterval)
{
	// m_ulLife is unsigned
	if (ulInterval > m_ulLife)
		m_ulLife = 0; // dead particle
	else
	{
		m_ulLife -= ulInterval;

		x += m_Velocity.x * ulInterval;
		y += m_Velocity.y * ulInterval;
		m_absX = x;
		m_absY = y;
	}
}
