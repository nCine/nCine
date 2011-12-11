#include "ncParticle.h"

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Initialized a particle with initial life, position and velocity
void ncParticle::Init(unsigned long int ulLife, ncVector2f pos, ncVector2f vel)
{
	m_ulLife = ulLife;
	m_ulStartLife = ulLife;
	SetPosition(pos);
	m_Velocity = vel;
}

/// Updates particle data after the specified amount of milliseconds have passed
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
