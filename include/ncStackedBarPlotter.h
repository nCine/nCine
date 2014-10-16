#ifndef CLASS_NCSTACKEDBARPLOTTER
#define CLASS_NCSTACKEDBARPLOTTER

#include "ncProfilePlotter.h"

/// A class to plot a variable as a histogram
class ncStackedBarVariable : public ncPlottingVariable
{
  public:
	ncStackedBarVariable(unsigned int numValues, float rejectDelay);

  private:
	virtual void updateRenderCommand();
	virtual void updateMeanRenderCommand();
};

/// A class that plots variables as stacked histograms
class ncStackedBarPlotter : public ncProfilePlotter
{
  public:
	ncStackedBarPlotter(ncSceneNode* parent, ncRect rect) : ncProfilePlotter(parent, rect) { }

	virtual unsigned int addVariable(unsigned int numValues, float rejectDelay);
	// Fill the buffer of every stacked variable with vertices
	void updateAllVertices(int x, int y, int w, int h);
	virtual void draw(ncRenderQueue& renderQueue);
};

#endif
