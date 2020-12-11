#include "RectAnimation.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

RectAnimation::RectAnimation(float defaultFrameDuration, LoopMode loopMode, RewindMode rewindMode)
    : defaultFrameDuration_(defaultFrameDuration), loopMode_(loopMode), rewindMode_(rewindMode),
      rects_(4), frameDurations_(4), currentFrame_(0), elapsedFrameTime_(0.0f),
      goingForward_(true), isPaused_(true)
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void RectAnimation::updateFrame(float interval)
{
	// No frame calculation if the animation is paused or has only one rect
	if (isPaused_ == true || rects_.size() < 2)
		return;

	elapsedFrameTime_ += interval;
	// Determine the next frame rectangle
	while (elapsedFrameTime_ >= frameDurations_[currentFrame_])
	{
		elapsedFrameTime_ -= frameDurations_[currentFrame_];

		if (goingForward_)
		{
			if (currentFrame_ == rects_.size() - 1)
			{
				if (rewindMode_ == RewindMode::BACKWARD)
				{
					goingForward_ = false;
					currentFrame_--;
				}
				else
				{
					if (loopMode_ == LoopMode::DISABLED)
						isPaused_ = true;
					else
						currentFrame_ = 0;
				}
			}
			else
				currentFrame_++;
		}
		else
		{
			if (currentFrame_ == 0)
			{
				if (loopMode_ == LoopMode::DISABLED)
					isPaused_ = true;
				else
				{
					goingForward_ = true;
					currentFrame_++;
				}
			}
			else
				currentFrame_--;
		}
	}
}

void RectAnimation::setFrame(unsigned int frameNum)
{
	ASSERT(frameNum < rects_.size());
	currentFrame_ = frameNum;
}

void RectAnimation::addRect(const Recti &rect, float frameDuration)
{
	rects_.pushBack(rect);
	frameDurations_.pushBack(frameDuration);
}

void RectAnimation::addRect(int x, int y, int w, int h, float frameDuration)
{
	rects_.pushBack(Recti(x, y, w, h));
	frameDurations_.pushBack(frameDuration);
}

}
