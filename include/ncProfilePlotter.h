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
	bool m_bPlotMean;
	ncColor m_graphColor;
	ncColor m_meanColor;
	ncProfileVariable m_variable;
	GLfloat *m_fVertices;

public:
	ncPlottingVariable(unsigned int uNumValues, unsigned int uRejectDelay);
	virtual ~ncPlottingVariable();

	inline unsigned int NumValues() const { return m_variable.NumValues(); }
	inline float NormMean() const { return m_variable.NormMean(); }
	inline const ncColor& GraphColor() const { return m_graphColor; }
	inline const ncColor& MeanColor() const { return m_meanColor; }
	inline const GLfloat* Vertices() const { return m_fVertices; }
	inline GLfloat* Vertices() { return m_fVertices; }
	inline const ncProfileVariable* Variable() const { return &m_variable; }
	inline bool shouldPlotMean() const { return m_bPlotMean; }

	void AddValue(float fValue) { m_variable.AddValue(fValue); }
	void SetGraphColor(ncColor graphColor) { m_graphColor = graphColor; }
	void SetMeanColor(ncColor meanColor) { m_meanColor = meanColor; }
	void SetPlotMean(bool bEnabled) { m_bPlotMean = bEnabled; }

	virtual void UpdateVertices(int x, int y, int w, int h) = 0;
};

/// A class that plots a graphic representation of a time/value function
class ncProfilePlotter
{
protected:
	int m_iX;
	int m_iY;
	int m_iWidth;
	int m_iHeight;

	ncColor m_backgroundColor;
	GLfloat m_fBackgroundVertices[8]; // Quad with a triangle strip
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

	ncPlottingVariable& Variable(unsigned int uIndex);
	void AddValue(unsigned int iVariable, float fValue);
	void SetBackgroundColor(ncColor backgroundColor) { m_backgroundColor = backgroundColor; }

	virtual unsigned int AddVariable(unsigned int uNumValues, unsigned int uRejectDelay) = 0;
	virtual void Draw() = 0;
};

#endif
