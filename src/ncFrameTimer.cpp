#include <cstdio>
#include "ncFrameTimer.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/*! Constructs a timer which calculates average FPS every dAvgInterval
	seconds and prints in the log every dLogInterval seconds
*/
ncFrameTimer::ncFrameTimer(float logInterval, float avgInterval)
	: logInterval_(logInterval), avgInterval_(avgInterval),
	  frameInterval_(0.0f), avgNumFrames_(0L), logNumFrames_(0L),
	  lastAvgUpdate_(0.0f), lastLogUpdate_(0.0f), fps_(0.0f)
{
	// To prevent the overflow of Interval() on the very first call of AddFrame()
	start();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Adds a frame to the counter and calculates the interval since the previous one
void ncFrameTimer::addFrame()
{
	frameInterval_ = ncTimer::interval();

	avgNumFrames_++;
	logNumFrames_++;

	// Update the FPS average calculation every avgInterval_ seconds
	if (avgInterval_ > 0.0f && (now() - lastAvgUpdate_ > avgInterval_))
	{
		fps_ = float(avgNumFrames_) / (now() - lastAvgUpdate_);

		avgNumFrames_ = 0L;
		lastAvgUpdate_ = now();
	}


	// Log number of frames and FPS every logInterval_ seconds
	if (logInterval_ > 0.0f && avgNumFrames_ != 0 && (now() - lastLogUpdate_ > logInterval_))
	{
		fps_ = float(logNumFrames_) / logInterval_;
		float msPerFrame = (logInterval_ * 1000.0f) / float(logNumFrames_);
		ncServiceLocator::logger().write(ncILogger::LOG_VERBOSE, (const char *)"ncFrameTimer::addFrame - %lu frames in %.0f seconds = %f FPS (%.3fms per frame)",  logNumFrames_, logInterval_, fps_, msPerFrame);

		logNumFrames_ = 0L;
		lastLogUpdate_ = now();
	}

	// Start counting for the next frame interval
	start();
}
