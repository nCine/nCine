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
	ncProfilePlotter(ncSceneNode* pParent, ncRect rect);
	virtual ~ncProfilePlotter();

	// Adds a value to the specified variable
	bool AddValue(unsigned int iVariable, float fValue);
	// Returns the variable with the specified index
	ncPlottingVariable& Variable(unsigned int uIndex);
	/// Sets the backgound color for the graphs
	void SetBackgroundColor(ncColor backgroundColor) { m_backgroundColor = backgroundColor; }

	/// Returns the reference value line color
	inline const ncColor& RefValueColor() const { return m_refValueColor; }
	/// Returns the state of the reference value drawing flag
	inline bool shouldPlotRefValue() const { return m_bPlotRefValue; }
	/// Returns the reference value
	inline float refValue() const { return m_fRefValue; }

	/// Sets the reference value line color
	inline void SetRefValueColor(ncColor refValueColor) { m_refValueColor = refValueColor; }
	/// Sets the reference value drawing flag state
	inline void SetPlotRefValue(bool bEnabled) { m_bPlotRefValue = bEnabled; }
	/// Sets the reference value
	inline void SetRefValue(float fValue) { m_fRefValue = fValue; }
	// Returns the reference value normalized and clamped between the two numbers provided
	float NormBetweenRefValue(float fMin, float fMax) const;

	// Applies parent transformations to reference value vertices
	void ApplyTransformations(float fAbsX, float fAbsY, float fAbsRotation, float fAbsScaleFactor);

	inline virtual void DrawRefValue(ncRenderQueue& rRenderQueue)
	{
		UpdateRefValueRenderCommand();
		rRenderQueue.AddCommand(&m_refValueCmd);
	}

	/// Adds a new variable to the plotter
	virtual unsigned int AddVariable(unsigned int uNumValues, float fRejectDelay) = 0;

 protected:
	/// Background width
	int m_iWidth;
	/// Background height
	int m_iHeight;

	/// Background color
	ncColor m_backgroundColor;
	/// The vertices for the background
	float m_fBackgroundVertices[8]; // Quad with a triangle strip
	/// The array of variables
	ncArray<ncPlottingVariable *> m_vVariables;

	/// Reference value drawing flag
	bool m_bPlotRefValue;
	/// Reference value line color
	ncColor m_refValueColor;
	/// The vertices for the reference value line
	float m_fRefValueVertices[4];
	/// The reference value
	float m_fRefValue;
	/// The command used to render the reference value
	ncRenderCommand m_refValueCmd;

	void SetBackgroundVertices();
	virtual void UpdateRenderCommand();
	// Updates the reference value rendering command
	void UpdateRefValueRenderCommand();
};

#endif
