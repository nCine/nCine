#ifndef CLASS_NCSTACKEDBARPLOTTER
#define CLASS_NCSTACKEDBARPLOTTER

#include "ncProfilePlotter.h"

/// A class to plot a variable as a histogram
class ncStackedBarVariable : public ncPlottingVariable
{
private:
	const ncProfileVariable *m_pBelowVariable;
public:
	ncStackedBarVariable(unsigned int uNumValues, unsigned int uRejectDelay);

	virtual void UpdateVertices(int x, int y, int w, int h) { }
};

/// A class that plots variables as stacked histograms
class ncStackedBarPlotter : public ncProfilePlotter
{
public:
	ncStackedBarPlotter(ncRect rect) : ncProfilePlotter(rect) { }

	virtual unsigned int AddVariable(unsigned int uNumValues, unsigned int uRejectDelay);
	void UpdateAllVertices(int x, int y, int w, int h);
	virtual void Draw();
};

#endif
