#ifndef CLASS_NCLINEPLOTTER
#define CLASS_NCLINEPLOTTER

#include "ncProfilePlotter.h"
class ncRenderCommand;

/// A class to plot a variable as a line strip
class ncLineVariable : public ncPlottingVariable
{
  public:
	ncLineVariable(unsigned int numValues, float rejectDelay);

  private:
	virtual void updateRenderCommand();
	virtual void updateMeanRenderCommand();
};

/// A class that plots line strip variables
class ncLinePlotter : public ncProfilePlotter
{
  public:
	ncLinePlotter(ncSceneNode* parent, ncRect rect) : ncProfilePlotter(parent, rect) { }

	virtual unsigned int addVariable(unsigned int numValues, float rejectDelay);
	// Fill the buffer of every line variable with vertices
	void updateAllVertices(int x, int y, int w, int h);
	virtual void draw(ncRenderQueue& renderQueue);
};

#endif
