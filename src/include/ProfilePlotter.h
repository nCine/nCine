#ifndef CLASS_NCINE_PROFILEPLOTTER
#define CLASS_NCINE_PROFILEPLOTTER

#include "Rect.h"
#include "Color.h"
#include "Array.h"
#include "StaticArray.h"
#include "DrawableNode.h"
#include "PlottingVariable.h"

namespace ncine {

/// A class that plots a graphic representation of a time/value function
class ProfilePlotter : public DrawableNode
{
  public:
	ProfilePlotter(SceneNode* parent, Rect rect);
	virtual ~ProfilePlotter();

	// Adds a value to the specified variable
	bool addValue(unsigned int varIndex, float value);
	// Returns the variable with the specified index
	PlottingVariable& variable(unsigned int index);
	/// Sets the backgound color for the graphs
	void setBackgroundColor(Color backgroundColor) { backgroundColor_ = backgroundColor; }

	/// Returns the reference value line color
	inline const Color& refValueColor() const { return refValueColor_; }
	/// Returns the state of the reference value drawing flag
	inline bool shouldPlotRefValue() const { return shouldPlotRefValue_; }
	/// Returns the reference value
	inline float refValue() const { return refValue_; }

	/// Sets the reference value line color
	inline void setRefValueColor(Color refValueColor) { refValueColor_ = refValueColor; }
	/// Sets the reference value drawing flag state
	inline void setPlotRefValue(bool enabled) { shouldPlotRefValue_ = enabled; }
	/// Sets the reference value
	inline void setRefValue(float value) { refValue_ = value; }
	// Returns the reference value normalized and clamped between the two numbers provided
	float normBetweenRefValue(float min, float max) const;

	// Applies parent transformations to reference value vertices
	void applyTransformations(float absX, float absY, float absRotation, float absScaleFactor);

	inline virtual void drawRefValue(RenderQueue& renderQueue)
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
	Color backgroundColor_;
	/// The vertices for the background
	StaticArray<float, 8> backgroundVertices_; // Quad with a triangle strip
	/// The array of variables
	Array<PlottingVariable *> variables_;

	/// Reference value drawing flag
	bool shouldPlotRefValue_;
	/// Reference value line color
	Color refValueColor_;
	/// The vertices for the reference value line
	StaticArray<float, 4> refValueVertices_;
	/// The reference value
	float refValue_;
	/// The command used to render the reference value
	RenderCommand refValueCmd_;

	void setBackgroundVertices();
	virtual void updateRenderCommand();
	// Updates the reference value rendering command
	void UpdateRefValueRenderCommand();

  private:
	/// Private copy constructor
	ProfilePlotter(const ProfilePlotter&);
	/// Private assignment operator
	ProfilePlotter& operator=(const ProfilePlotter&);
};

}

#endif
