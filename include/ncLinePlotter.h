#ifndef CLASS_NCLINEPLOTTER
#define CLASS_NCLINEPLOTTER

#include "ncProfilePlotter.h"

/// A class to plot a variable as a line strip
class ncLineVariable : public ncPlottingVariable
{
public:
	ncLineVariable(unsigned int uNumValues, unsigned int uRejectDelay);

	virtual void UpdateVertices(int x, int y, int w, int h);
};

/// A class that plots line strip variables
class ncLinePlotter : public ncProfilePlotter
{
public:
	ncLinePlotter(ncRect rect) : ncProfilePlotter(rect) { }

	virtual unsigned int AddVariable(unsigned int uNumValues, unsigned int uRejectDelay);
	virtual void Draw();
};

#endif
