#include "ncParticle.h"

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Initializes a particle with initial life, position, velocity and rotation
void ncParticle::Init(unsigned long int ulLife, ncVector2f pos, ncVector2f vel, float fRot, bool bLocalSpace)
{
	m_ulLife = ulLife;
	m_ulStartLife = ulLife;
	SetPosition(pos);
	m_Velocity = vel;
	SetRotation(fRot);
	m_bLocalSpace = bLocalSpace;
}

/// Updates particle data after the specified amount of milliseconds has passed
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
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Custom transform method to allow independent position from parent
void ncParticle::Transform()
{
	// If in local space transform as any other scene node
	if (m_bLocalSpace)
		ncSceneNode::Transform();
	else
	{
		if (m_pParent)
		{
			m_fAbsScaleFactor = m_pParent->AbsScale() * m_fScaleFactor;
			m_absColor = m_pParent->AbsColor() * m_color;
		}
		else
		{
			m_fAbsScaleFactor = m_fScaleFactor;
			m_absColor = m_color;
		}

		// Always independent movement
		m_fAbsRotation = m_fRotation;
		m_fAbsX = x;
		m_fAbsY = y;
	}
}

