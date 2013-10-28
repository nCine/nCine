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
	float m_fFrameTime;
	/// Elapsed time since the last frame change
	float m_fElapsedFrameTime;
	/// The flag about the frame advance direction
	bool m_bGoingForward;
	/// The looping flag
	bool m_bLooping;
	/// The ping-pong loop flag
	bool m_bBackward;
	/// The pause flag
	bool m_bPaused;

public:
	ncRectAnimation(float fFrameTime, bool bLooping, bool bBackward)
		: m_vRects(4), m_uCurrentFrame(0), m_fFrameTime(fFrameTime), m_fElapsedFrameTime(0.0f),
		  m_bGoingForward(true), m_bLooping(bLooping), m_bBackward(bBackward), m_bPaused(false)
	{ }

	/// Returns current frame
	inline unsigned int Frame() const { return m_uCurrentFrame; }
	// Sets current frame
	void SetFrame(unsigned int uFrameNum);
	/// Returns frame time
	float FrameTime() const { return m_fFrameTime; }
	/// Sets frame time
	inline void SetFrameTime(float fFrameTime) { m_fFrameTime = fFrameTime; }

	/// Adds a rectangles to the array
	inline void AddRect(const ncRect& rRect) { m_vRects.InsertBack(rRect); }
	/// Creates a rectangles from origin and size and then adds it to the array
	inline void AddRect(int iX, int iY, int iW, int iH) { m_vRects.InsertBack(ncRect(iX, iY, iW, iH)); }
	/// Returns the current rectangle
	inline const ncRect& Rect() const { return m_vRects[m_uCurrentFrame]; }

	// Updates current frame based on time passed
	void UpdateFrame(float fInterval);

	/// Is the animation paused?
	inline bool IsPaused() const { return m_bPaused; }
	/// Sets the animation flag
	inline void SetPaused(bool bPaused) { m_bPaused = bPaused; }
	// Pause on a specified frame
	void Pause(unsigned int uFrameNum);
};

#endif
