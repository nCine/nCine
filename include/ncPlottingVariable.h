#ifndef CLASS_NCPLOTTINGVARIABLE
#define CLASS_NCPLOTTINGVARIABLE

#include "ncColor.h"
#include "ncProfileVariable.h"
#include "ncDrawableNode.h"
#include "ncRenderCommand.h"
#include "ncRenderQueue.h"

/// A class to group and wrap all the information needed to plot a variable
class ncPlottingVariable
{
  public:
	ncPlottingVariable(unsigned int numValues, float rejectDelay);
	virtual ~ncPlottingVariable();

	/// Returns the number of value for the variable
	inline unsigned int numValues() const { return variable_.numValues(); }
	/// Returns the normalized mathematic mean of the variable
	inline float normMean() const { return variable_.normMean(); }
	/// Returns the graph color
	inline const ncColor& graphColor() const { return graphColor_; }
	/// Returns the mean line color
	inline const ncColor& meanColor() const { return meanColor_; }
	/// Returns a constant pointer to the vertices buffer
	inline const float* vertices() const { return vertices_; }
	/// Returns the pointer to the vertices buffer
	inline float* vertices() { return vertices_; }
	/// Returns the pointer to the variable
	inline const ncProfileVariable* variable() const { return &variable_; }
	/// Returns the state of the mean drawing flag
	inline bool shouldPlotMean() const { return shouldPlotMean_; }

	/// Adds a value to the variable
	bool addValue(float value) { return variable_.addValue(value); }
	/// Sets the graph color
	inline void setGraphColor(ncColor graphColor) { graphColor_ = graphColor; }
	/// Sets the mean line color
	inline void setMeanColor(ncColor meanColor) { meanColor_ = meanColor; }
	/// Sets the mean drawing flag state
	inline void setPlotMean(bool enabled) { shouldPlotMean_ = enabled; }

	// Applies parent transformations to both mean and values vertices
	void applyTransformations(float absX, float absY, float absRotation, float absScaleFactor);

	inline virtual void draw(ncRenderQueue& renderQueue)
	{
		updateRenderCommand();
		renderQueue.addCommand(&valuesCmd_);
	}
	inline virtual void drawMean(ncRenderQueue& renderQueue)
	{
		updateMeanRenderCommand();
		renderQueue.addCommand(&meanCmd_);
	}

  protected:
	/// Mean drawing flag
	bool shouldPlotMean_;
	/// Graph color
	ncColor graphColor_;
	/// Mean line color
	ncColor meanColor_;
	/// The variable to be plotted
	ncProfileVariable variable_;
	/// The vertices buffer
	float *vertices_;

	/// The command used to render variable values
	ncRenderCommand valuesCmd_;
	/// The command used to render the variable mean
	ncRenderCommand meanCmd_;

	/// Updates the values rendering command
	virtual void updateRenderCommand() = 0;
	/// Updates the mean rendering command
	virtual void updateMeanRenderCommand() = 0;
};

#endif
