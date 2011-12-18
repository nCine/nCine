#ifndef CLASS_NCLINEPLOTTER
#define CLASS_NCLINEPLOTTER

#include "ncProfilePlotter.h"
class ncRenderCommand;

/// A class to plot a variable as a line strip
class ncLineVariable : public ncPlottingVariable
{
private:
	virtual void UpdateRenderCommand();
	virtual void UpdateMeanRenderCommand();

public:
	ncLineVariable(unsigned int uNumValues, unsigned int uRejectDelay);

	virtual void UpdateVertices(int x, int y, int w, int h);
};

/// A class that plots line strip variables
class ncLinePlotter : public ncProfilePlotter
{
public:
	ncLinePlotter(ncSceneNode* pParent, ncRect rect) : ncProfilePlotter(pParent, rect) { }

	virtual unsigned int AddVariable(unsigned int uNumValues, unsigned int uRejectDelay);
	virtual void Draw(ncRenderQueue& rRenderQueue);
};

#endif
