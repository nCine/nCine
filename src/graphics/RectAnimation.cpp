#include "RectAnimation.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

RectAnimation::RectAnimation(float frameTime, bool isLooping, bool backward)
	: rects_(4), currentFrame_(0), frameTime_(frameTime), elapsedFrameTime_(0.0f),
	  goingForward_(true), isLooping_(isLooping), backward_(backward), isPaused_(false)
{

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Sets current frame
void RectAnimation::SetFrame(unsigned int frameNum)
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

/// Updates current frame based on time passed
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

/// Pause on a specified frame
void RectAnimation::pause(unsigned int frameNum)
{
	if (frameNum >= rects_.size())
	{
		currentFrame_ = rects_.size() - 1;
	}
	else
	{
		currentFrame_ = frameNum;
	}

	isPaused_ = true;
}

}
