#ifndef CLASS_NCRECTANIMATION
#define CLASS_NCRECTANIMATION

#include "ncArray.h"
#include "ncRect.h"

/// The class containing data for a rectangles based animation
class ncRectAnimation
{
private:
	/// The rectangles array
	ncArray<ncRect> m_vRects;
	/// Current frame
	unsigned int m_uCurrentFrame;
	/// The time until the next frame change
	unsigned int m_uFrameTime;
	/// Elapsed time since the last frame change
	unsigned long int m_ulElapsedFrameTime;
	/// The flag about the frame advance direction
	bool m_bGoingForward;
	/// The looping flag
	bool m_bLooping;
	/// The ping-pong loop flag
	bool m_bBackward;
	/// The pause flag
	bool m_bPaused;

public:
	ncRectAnimation(unsigned int uFrameTime, bool bLooping, bool bBackward)
		: m_vRects(4), m_uCurrentFrame(0), m_uFrameTime(uFrameTime), m_ulElapsedFrameTime(0),
		  m_bGoingForward(true), m_bLooping(bLooping), m_bBackward(bBackward), m_bPaused(false)
	{ }

	/// Returns current frame
	inline unsigned int Frame() const { return m_uCurrentFrame; }
	// Sets current frame
	void SetFrame(unsigned int uFrameNum);
	/// Returns frame time
	inline unsigned int FrameTime() const { return m_uFrameTime; }
	/// Sets frame time
	inline void SetFrameTime(unsigned int uFrameTime) { m_uFrameTime = uFrameTime; }

	/// Adds a rectangles to the array
	inline void AddRect(const ncRect& rRect) { m_vRects.InsertBack(rRect); }
	/// Creates a rectangles from origin and size and then adds it to the array
	inline void AddRect(int iX, int iY, int iW, int iH) { m_vRects.InsertBack(ncRect(iX, iY, iW, iH)); }
	/// Returns the current rectangle
	inline const ncRect& Rect() const { return m_vRects[m_uCurrentFrame]; }

	// Returns true if a frame rect has changed
	void UpdateFrame(unsigned long ulInterval);

	/// Is the animation paused?
	inline bool IsPaused() const { return m_bPaused; }
	/// Sets the animation flag
	inline void SetPaused(bool bPaused) { m_bPaused = bPaused; }
	// Pause on a specified frame
	void Pause(unsigned int uFrameNum);
};

#endif
