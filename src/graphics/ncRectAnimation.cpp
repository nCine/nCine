#include "ncRectAnimation.h"

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Sets current frame
void ncRectAnimation::SetFrame(int iFrameNum)
{
	if (iFrameNum >= m_vRects.Size())
		m_iCurrentFrame = m_vRects.Size() - 1;
	else if (iFrameNum < 0)
		m_iCurrentFrame = 0;
	else
		m_iCurrentFrame = iFrameNum;
}

/// Returns true if a frame rect has changed
void ncRectAnimation::UpdateFrame(unsigned long ulInterval)
{
	// No frame calculation if the animation is paused or has only one rect
	if (m_bPaused == true || m_vRects.Size() < 2)
		return;

	m_ulElapsedFrameTime += ulInterval;
	// A NEXT frame rectangle should be determined
	if (m_ulElapsedFrameTime >= m_uFrameTime)
	{
		m_ulElapsedFrameTime = 0;

		if (m_bGoingForward)
			m_iCurrentFrame++;
		else
			m_iCurrentFrame--;

		// First testing for < 0, otherwise the comparison with unsigned ncArray size could fail
		if (m_iCurrentFrame < 0)
		{
			if (m_bLooping == false)
			{
				m_iCurrentFrame = 0;
				m_bPaused = true;
			}
			else
			{
				m_bGoingForward = true;
				m_iCurrentFrame += 2;
			}
		}
		else if (m_iCurrentFrame >= m_vRects.Size())
		{
			if (m_bBackward)
			{
				m_bGoingForward = false;
				m_iCurrentFrame -= 2;
			}
			else
			{
				if (m_bLooping == false)
				{
					m_iCurrentFrame = m_vRects.Size() - 1;
					m_bPaused = true;
				}
				else
					m_iCurrentFrame = 0;
			}
		}
	}
}

/// Pause on a specified frame
void ncRectAnimation::Pause(int iFrameNum)
{
	if (iFrameNum >= m_vRects.Size())
		m_iCurrentFrame = m_vRects.Size() - 1;
	else if (iFrameNum < 0)
		m_iCurrentFrame = 0;
	else
		m_iCurrentFrame = iFrameNum;

	m_bPaused = true;
}
