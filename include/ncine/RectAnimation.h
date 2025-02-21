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

	/// Default constructor
	RectAnimation()
	    : RectAnimation(1.0f / 60.0f, LoopMode::DISABLED, RewindMode::FROM_START) {}
	/// Constructor for an animation with a specified default frame duration, loop and rewind mode
	RectAnimation(float defaultFrameDuration, LoopMode loopMode, RewindMode rewindMode);

	/// Returns true if the animation is paused
	inline bool isPaused() const { return isPaused_; }
	/// Sets the pause flag
	inline void setPaused(bool isPaused) { isPaused_ = isPaused; }

	/// Updates current frame based on time passed
	void updateFrame(float frameTime);

	/// Returns the number of frames (rectangles)
	inline unsigned int numFrames() const { return rects_.size(); }
	/// Returns current frame
	inline unsigned int frame() const { return currentFrame_; }
	/// Sets current frame
	void setFrame(unsigned int frameNum);

	/// Returns the default frame duration in seconds
	float defaultFrameDuration() const { return defaultFrameDuration_; }
	/// Sets the default frame duration in seconds
	inline void setDefaultFrameDuration(float defaultFrameDuration) { defaultFrameDuration_ = defaultFrameDuration; }

	/// Returns the loop mode
	LoopMode loopMode() const { return loopMode_; }
	/// Sets the loop mode
	inline void setLoopMode(LoopMode loopMode) { loopMode_ = loopMode; }

	/// Returns the rewind mode
	RewindMode rewindMode() const { return rewindMode_; }
	/// Sets the rewind mode
	inline void setRewindMode(RewindMode rewindMode) { rewindMode_ = rewindMode; }

	/// Adds a rectangle to the array specifying the frame duration
	void addRect(const Recti &rect, float frameDuration);
	/// Creates a rectangle from origin and size and then adds it to the array, specifying the frame duration
	void addRect(int x, int y, int w, int h, float frameDuration);

	/// Adds a rectangle to the array with the default frame duration
	inline void addRect(const Recti &rect) { addRect(rect, defaultFrameDuration_); }
	/// Creates a rectangle from origin and size and then adds it to the array, with the default frame duration
	inline void addRect(int x, int y, int w, int h) { addRect(x, y, w, h, defaultFrameDuration_); }

	/// Adds rectangles to the array specifying their size, the source rectangle, a number of rectangles to skip, a padding, and the frame duration
	unsigned int addRects(const Vector2i &size, const Recti &srcRect, unsigned int rectsToSkip, const Vector2i &padding, float frameDuration);

	/// Adds rectangles to the array specifying their size, the source rectangle, and a number of rectangles to skip
	/*! \return The number of rectangles added */
	inline unsigned int addRects(const Vector2i &size, const Recti &srcRect, unsigned int rectsToSkip) { return addRects(size, srcRect, rectsToSkip, Vector2i(0, 0), defaultFrameDuration_); }

	/// Adds rectangles to the array specifying their size and the source rectangle
	/*! \return The number of rectangles added */
	inline unsigned int addRects(const Vector2i &size, const Recti &srcRect) { return addRects(size, srcRect, 0, Vector2i(0, 0), defaultFrameDuration_); }

	/// Returns the current rectangle
	inline const Recti &rect() const { return rects_[currentFrame_]; }
	/// Returns the current frame duration in seconds
	inline float frameDuration() const { return frameDurations_[currentFrame_]; }

	/// Returns the array of all rectangles
	inline nctl::Array<Recti> &rectangles() { return rects_; }
	/// Returns the constant array of all rectangles
	inline const nctl::Array<Recti> &rectangles() const { return rects_; }

	/// Returns the array of all frame durations
	inline nctl::Array<float> &frameDurations() { return frameDurations_; }
	/// Returns the constant array of all frame durations
	inline const nctl::Array<float> &frameDurations() const { return frameDurations_; }

  private:
	/// The default frame duratin if a custom one is not specified when adding a rectangle
	float defaultFrameDuration_;
	/// The looping mode (on or off)
	LoopMode loopMode_;
	/// The rewind mode (ping-pong or not)
	RewindMode rewindMode_;

	/// The rectangles array
	nctl::Array<Recti> rects_;
	/// The frame durations array
	nctl::Array<float> frameDurations_;
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
