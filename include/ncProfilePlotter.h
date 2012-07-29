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

	void SetBackgroundVertices();
	virtual void UpdateRenderCommand();

public:
	ncProfilePlotter(ncSceneNode* pParent, ncRect rect);
	virtual ~ncProfilePlotter();

	/// Adds a value to the specified variable
	void AddValue(unsigned int iVariable, float fValue);
	/// Returns the variable with the specified index
	ncPlottingVariable& Variable(unsigned int uIndex);
	/// Sets the backgound color for the graphs
	void SetBackgroundColor(ncColor backgroundColor) { m_backgroundColor = backgroundColor; }

	/// Adds a new variable to the plotter
	virtual unsigned int AddVariable(unsigned int uNumValues, unsigned int uRejectDelay) = 0;
};

#endif
