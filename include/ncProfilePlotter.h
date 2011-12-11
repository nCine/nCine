#ifndef CLASS_NCPROFILEPLOTTER
#define CLASS_NCPROFILEPLOTTER

#if defined(__ANDROID__)
	#include <GLES/gl.h>
	#include <GLES/glext.h>
#elif !defined(NO_GLEW)
	#include <GL/glew.h>
#else
	#include <GL/gl.h>
	#include <GL/glext.h>
#endif

#include "ncRect.h"
#include "ncColor.h"
#include "ncArray.h"
#include "ncProfileVariable.h"

/// A class to group and wrap all the information needed to plot a variable
class ncPlottingVariable
{
protected:
	/// Mean drawing flag
	bool m_bPlotMean;
	/// Graph color
	ncColor m_graphColor;
	/// Mean line color
	ncColor m_meanColor;
	/// The variable to be plotted
	ncProfileVariable m_variable;
	/// The vertices buffer
	GLfloat *m_fVertices;

public:
	ncPlottingVariable(unsigned int uNumValues, unsigned int uRejectDelay);
	virtual ~ncPlottingVariable();

	/// Returns the number of value for the variable
	inline unsigned int NumValues() const { return m_variable.NumValues(); }
	/// Returns the normalized mathematic mean of the variable
	inline float NormMean() const { return m_variable.NormMean(); }
	/// Returns the graph color
	inline const ncColor& GraphColor() const { return m_graphColor; }
	/// Returns the mean line color
	inline const ncColor& MeanColor() const { return m_meanColor; }
	/// Returns the constant pointer to the vertices buffer
	inline const GLfloat* Vertices() const { return m_fVertices; }
	/// Returns the constant pointer to the vertices buffer
	inline GLfloat* Vertices() { return m_fVertices; }
	/// Returns the pointer to the variable
	inline const ncProfileVariable* Variable() const { return &m_variable; }
	/// Returns the state of the mean drawing flag
	inline bool shouldPlotMean() const { return m_bPlotMean; }

	/// Adds a value to the variable
	void AddValue(float fValue) { m_variable.AddValue(fValue); }
	/// Sets the graph color
	void SetGraphColor(ncColor graphColor) { m_graphColor = graphColor; }
	/// Sets the mean line color
	void SetMeanColor(ncColor meanColor) { m_meanColor = meanColor; }
	/// Sets the mean drawing flag state
	void SetPlotMean(bool bEnabled) { m_bPlotMean = bEnabled; }

	virtual void UpdateVertices(int x, int y, int w, int h) = 0;
};

/// A class that plots a graphic representation of a time/value function
class ncProfilePlotter
{
protected:
	/// Background left coordinate
	int m_iX;
	/// Background top coordinate
	int m_iY;
	/// Background width
	int m_iWidth;
	/// Background height
	int m_iHeight;

	/// Background color
	ncColor m_backgroundColor;
	/// The vertices for the background
	GLfloat m_fBackgroundVertices[8]; // Quad with a triangle strip
	/// The array of variables
	ncArray<ncPlottingVariable *> m_vVariables;

	void SetBackgroundVertices();
	void DrawBackground();
public:
	ncProfilePlotter(ncRect rect)
		: m_iX(rect.x), m_iY(rect.y), m_iWidth(rect.w), m_iHeight(rect.h), m_vVariables(2)
	{
		SetBackgroundVertices();
	}
	virtual ~ncProfilePlotter();

	/// Returns the variable with the specified index
	ncPlottingVariable& Variable(unsigned int uIndex);
	/// Adds a value to the specified variable
	void AddValue(unsigned int iVariable, float fValue);
	/// Sets the backgound color for the graphs
	void SetBackgroundColor(ncColor backgroundColor) { m_backgroundColor = backgroundColor; }

	/// Adds a new variable to the plotter
	virtual unsigned int AddVariable(unsigned int uNumValues, unsigned int uRejectDelay) = 0;
	virtual void Draw() = 0;
};

#endif
