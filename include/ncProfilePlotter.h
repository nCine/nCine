#ifndef CLASS_NCPROFILEPLOTTER
#define CLASS_NCPROFILEPLOTTER

#include "ncRect.h"
#include "ncColor.h"
#include "ncArray.h"
#include "ncDrawableNode.h"
#include "ncPlottingVariable.h"

/// A class that plots a graphic representation of a time/value function
class ncProfilePlotter : public ncDrawableNode
{
  public:
	ncProfilePlotter(ncSceneNode* parent, ncRect rect);
	virtual ~ncProfilePlotter();

	// Adds a value to the specified variable
	bool addValue(unsigned int varIndex, float value);
	// Returns the variable with the specified index
	ncPlottingVariable& variable(unsigned int index);
	/// Sets the backgound color for the graphs
	void setBackgroundColor(ncColor backgroundColor) { backgroundColor_ = backgroundColor; }

	/// Returns the reference value line color
	inline const ncColor& refValueColor() const { return refValueColor_; }
	/// Returns the state of the reference value drawing flag
	inline bool shouldPlotRefValue() const { return shouldPlotRefValue_; }
	/// Returns the reference value
	inline float refValue() const { return refValue_; }

	/// Sets the reference value line color
	inline void setRefValueColor(ncColor refValueColor) { refValueColor_ = refValueColor; }
	/// Sets the reference value drawing flag state
	inline void setPlotRefValue(bool enabled) { shouldPlotRefValue_ = enabled; }
	/// Sets the reference value
	inline void setRefValue(float value) { refValue_ = value; }
	// Returns the reference value normalized and clamped between the two numbers provided
	float normBetweenRefValue(float min, float max) const;

	// Applies parent transformations to reference value vertices
	void applyTransformations(float absX, float absY, float absRotation, float absScaleFactor);

	inline virtual void drawRefValue(ncRenderQueue& renderQueue)
	{
		UpdateRefValueRenderCommand();
		renderQueue.addCommand(&refValueCmd_);
	}

	/// Adds a new variable to the plotter
	virtual unsigned int addVariable(unsigned int numValues, float rejectDelay) = 0;

  protected:
	/// Background width
	int width_;
	/// Background height
	int height_;

	/// Background color
	ncColor backgroundColor_;
	/// The vertices for the background
	float backgroundVertices_[8]; // Quad with a triangle strip
	/// The array of variables
	ncArray<ncPlottingVariable *> variables_;

	/// Reference value drawing flag
	bool shouldPlotRefValue_;
	/// Reference value line color
	ncColor refValueColor_;
	/// The vertices for the reference value line
	float refValueVertices_[4];
	/// The reference value
	float refValue_;
	/// The command used to render the reference value
	ncRenderCommand refValueCmd_;

	void setBackgroundVertices();
	virtual void updateRenderCommand();
	// Updates the reference value rendering command
	void UpdateRefValueRenderCommand();
};

#endif
