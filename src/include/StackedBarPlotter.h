#ifndef CLASS_NCINE_STACKEDBARPLOTTER
#define CLASS_NCINE_STACKEDBARPLOTTER

#include "ProfilePlotter.h"

namespace ncine {

/// A class to plot a variable as a histogram
class StackedBarVariable : public PlottingVariable
{
  public:
	StackedBarVariable(unsigned int numValues, float rejectDelay, const Matrix4x4f &worldMatrix);

  private:
	void updateRenderCommand() override;
	void updateMeanRenderCommand() override;
};

/// A class that plots variables as stacked histograms
class StackedBarPlotter : public ProfilePlotter
{
  public:
	StackedBarPlotter(SceneNode *parent, Rectf rect) : ProfilePlotter(parent, rect) { }

	unsigned int addVariable(unsigned int numValues, float rejectDelay) override;
	void draw(RenderQueue &renderQueue) override;

  private:
	/// Fill the buffer of every stacked variable with vertices
	void updateAllVertices(float x, float y, float w, float h);
};

}

#endif
