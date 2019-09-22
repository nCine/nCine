#include "RectAnimation.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

RectAnimation::RectAnimation(float frameTime, LoopMode loopMode, RewindMode rewindMode)
    : frameTime_(frameTime), loopMode_(loopMode), rewindMode_(rewindMode), rects_(4),
      currentFrame_(0), elapsedFrameTime_(0.0f), goingForward_(true), isPaused_(true)
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
	// A NEXT frame rectangle should be determined
	if (elapsedFrameTime_ >= frameTime_)
	{
		elapsedFrameTime_ = 0.0f;

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

}
