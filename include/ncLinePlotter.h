#ifndef CLASS_NCLINEPLOTTER
#define CLASS_NCLINEPLOTTER

#include "ncProfilePlotter.h"
class ncRenderCommand;

/// A class to plot a variable as a line strip
class ncLineVariable : public ncPlottingVariable
{
  public:
	ncLineVariable(unsigned int uNumValues, float fRejectDelay);

  private:
	virtual void UpdateRenderCommand();
	virtual void UpdateMeanRenderCommand();
};

/// A class that plots line strip variables
class ncLinePlotter : public ncProfilePlotter
{
  public:
	ncLinePlotter(ncSceneNode* pParent, ncRect rect) : ncProfilePlotter(pParent, rect) { }

	virtual unsigned int AddVariable(unsigned int uNumValues, float fRejectDelay);
	// Fill the buffer of every line variable with vertices
	void UpdateAllVertices(int x, int y, int w, int h);
	virtual void Draw(ncRenderQueue& rRenderQueue);
};

#endif
