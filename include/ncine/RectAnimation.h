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

	/// Constructor for an animation with a specified frame time, loop and rewind mode
	RectAnimation(float frameTime, LoopMode loopMode, RewindMode rewindMode);

	/// Returns true if the animation is paused
	inline bool isPaused() const { return isPaused_; }
	/// Sets the pause flag
	inline void setPaused(bool isPaused) { isPaused_ = isPaused; }

	/// Updates current frame based on time passed
	void updateFrame(float interval);

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

	/// Returns the number of rectangles
	inline unsigned int numRectangles() { return rects_.size(); }
	/// Returns the array of all rectangles
	inline nctl::Array<Recti> &rectangles() { return rects_; }
	/// Returns the constant array of all rectangles
	inline const nctl::Array<Recti> &rectangles() const { return rects_; }

  private:
	/// The time until the next frame change
	float frameTime_;
	/// The looping mode (on or off)
	LoopMode loopMode_;
	/// The rewind mode (ping-pong or not)
	RewindMode rewindMode_;

	/// The rectangles array
	nctl::Array<Recti> rects_;
	/// Current frame
	unsigned int currentFrame_;
	/// Elapsed time since the last frame change
	float elapsedFrameTime_;
	/// The flag about the frame advance direction
	bool goingForward_;
	/// The pause flag
	bool isPaused_;
};

}

#endif
