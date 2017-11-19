#ifndef CLASS_NCINE_LINEPLOTTER
#define CLASS_NCINE_LINEPLOTTER

#include "ProfilePlotter.h"

namespace ncine {

class RenderCommand;

/// A class to plot a variable as a line strip
class LineVariable : public PlottingVariable
{
  public:
	LineVariable(unsigned int numValues, float rejectDelay, const Matrix4x4f &worldMatrix);

  private:
	void updateRenderCommand() override;
	void updateMeanRenderCommand() override;
};

/// A class that plots line strip variables
class LinePlotter : public ProfilePlotter
{
  public:
	LinePlotter(SceneNode *parent, Rectf rect) : ProfilePlotter(parent, rect) { }

	unsigned int addVariable(unsigned int numValues, float rejectDelay) override;

	void draw(RenderQueue &renderQueue) override;

  private:
	/// Fill the buffer of every line variable with vertices
	void updateAllVertices(float x, float y, float w, float h);
};

}

#endif
