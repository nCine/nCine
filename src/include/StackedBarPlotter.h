#ifndef CLASS_NCINE_STACKEDBARPLOTTER
#define CLASS_NCINE_STACKEDBARPLOTTER

#include "ProfilePlotter.h"

namespace ncine {

/// A class to plot a variable as a histogram
class StackedBarVariable : public PlottingVariable
{
  public:
	StackedBarVariable(unsigned int numValues, float rejectDelay);

  private:
	virtual void updateRenderCommand();
	virtual void updateMeanRenderCommand();
};

/// A class that plots variables as stacked histograms
class StackedBarPlotter : public ProfilePlotter
{
  public:
	StackedBarPlotter(SceneNode* parent, Rect rect) : ProfilePlotter(parent, rect) { }

	virtual unsigned int addVariable(unsigned int numValues, float rejectDelay);
	// Fill the buffer of every stacked variable with vertices
	void updateAllVertices(int x, int y, int w, int h);
	virtual void draw(RenderQueue& renderQueue);
};

}

#endif
