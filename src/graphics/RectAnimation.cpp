#include "RectAnimation.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

RectAnimation::RectAnimation(float frameTime, LoopMode loopMode, RewindMode rewindMode)
	: rects_(4), currentFrame_(0), frameTime_(frameTime), elapsedFrameTime_(0.0f), goingForward_(true),
	  isLooping_(loopMode == LOOPING), backward_(rewindMode == BACKWARD), isPaused_(true)
{

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void RectAnimation::setFrame(unsigned int frameNum)
{
	if (frameNum >= rects_.size())
	{
		currentFrame_ = rects_.size() - 1;
	}
	else
	{
		currentFrame_ = frameNum;
	}
}

void RectAnimation::updateFrame(float interval)
{
	// No frame calculation if the animation is paused or has only one rect
	if (isPaused_ == true || rects_.size() < 2)
	{
		return;
	}

	elapsedFrameTime_ += interval;
	// A NEXT frame rectangle should be determined
	if (elapsedFrameTime_ >= frameTime_)
	{
		elapsedFrameTime_ = 0.0f;

		if (goingForward_)
		{
			if (currentFrame_ == rects_.size() - 1)
			{
				if (backward_)
				{
					goingForward_ = false;
					currentFrame_--;
				}
				else
				{
					if (isLooping_ == false)
					{
						isPaused_ = true;
					}
					else
					{
						currentFrame_ = 0;
					}
				}
			}
			else
			{
				currentFrame_++;
			}
		}
		else
		{
			if (currentFrame_ == 0)
			{
				if (isLooping_ == false)
				{
					isPaused_ = true;
				}
				else
				{
					goingForward_ = true;
					currentFrame_++;
				}
			}
			else
			{
				currentFrame_--;
			}
		}
	}
}

}
