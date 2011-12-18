#ifndef CLASS_NCSTACKEDBARPLOTTER
#define CLASS_NCSTACKEDBARPLOTTER

#include "ncProfilePlotter.h"

/// A class to plot a variable as a histogram
class ncStackedBarVariable : public ncPlottingVariable
{
public:
	ncStackedBarVariable(unsigned int uNumValues, unsigned int uRejectDelay);

	virtual void UpdateVertices(int x, int y, int w, int h) { }
	virtual void UpdateRenderCommand();
	virtual void UpdateMeanRenderCommand();
};

/// A class that plots variables as stacked histograms
class ncStackedBarPlotter : public ncProfilePlotter
{
public:
	ncStackedBarPlotter(ncSceneNode* pParent, ncRect rect) : ncProfilePlotter(pParent, rect) { }

	virtual unsigned int AddVariable(unsigned int uNumValues, unsigned int uRejectDelay);
	void UpdateAllVertices(int x, int y, int w, int h);
	virtual void Draw(ncRenderQueue& rRenderQueue);
};

#endif
