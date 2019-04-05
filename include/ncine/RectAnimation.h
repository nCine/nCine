#ifndef CLASS_NCINE_RECTANIMATION
#define CLASS_NCINE_RECTANIMATION

#include <nctl/Array.h>
#include "Rect.h"

namespace ncine {

/// The class containing data for a rectangles based animation
class DLL_PUBLIC RectAnimation
{
  public:
	/// Loop modes
	enum class LoopMode
	{
		DISABLED,
		ENABLED
	};

	/// Rewind modes
	enum class RewindMode
	{
		/// When the animation reaches the last frame it begins again from start
		FROM_START,
		/// When the animation reaches the last frame it goes backward
		BACKWARD
	};

	RectAnimation(float frameTime, LoopMode loopMode, RewindMode rewindMode);

	/// Returns current frame
	inline unsigned int frame() const { return currentFrame_; }
	/// Sets current frame
	void setFrame(unsigned int frameNum);
	/// Returns frame time
	float frameTime() const { return frameTime_; }
	/// Sets frame time
	inline void setFrameTime(float frameTime) { frameTime_ = frameTime; }

	/// Adds a rectangle to the array
	inline void addRect(const Recti &rect) { rects_.pushBack(rect); }
	/// Creates a rectangle from origin and size and then adds it to the array
	inline void addRect(int x, int y, int w, int h) { rects_.pushBack(Recti(x, y, w, h)); }
	/// Returns the current rectangle
	inline const Recti &rect() const { return rects_[currentFrame_]; }

	/// Updates current frame based on time passed
	void updateFrame(float interval);

	/// Returns true if the animation is paused
	inline bool isPaused() const { return isPaused_; }
	/// Sets the pause flag
	inline void setPaused(bool isPaused) { isPaused_ = isPaused; }

  private:
	/// The rectangles array
	nctl::Array<Recti> rects_;
	/// Current frame
	unsigned int currentFrame_;
	/// The time until the next frame change
	float frameTime_;
	/// Elapsed time since the last frame change
	float elapsedFrameTime_;
	/// The flag about the frame advance direction
	bool goingForward_;
	/// The looping flag
	bool isLooping_;
	/// The ping-pong loop flag
	bool backward_;
	/// The pause flag
	bool isPaused_;
};

}

#endif
