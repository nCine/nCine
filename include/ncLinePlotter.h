#ifndef CLASS_NCLINEPLOTTER
#define CLASS_NCLINEPLOTTER

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

// A class to group and wrap all the information needed to plot a variable
class ncLineVariable
{
private:
	bool m_bPlotMean;
	ncColor m_graphColor;
	ncColor m_meanColor;
	ncProfileVariable m_variable;
	GLfloat *m_fVertices;

public:
	ncLineVariable(unsigned int uNumValues, unsigned int uRejectDelay);
	~ncLineVariable();

	inline unsigned int NumValues() const { return m_variable.NumValues(); }
	inline float NormMean() const { return m_variable.NormMean(); }
	inline const ncColor& GraphColor() const { return m_graphColor; }
	inline const ncColor& MeanColor() const { return m_meanColor; }
	inline const GLfloat* Vertices() const { return m_fVertices; }
	inline bool shouldPlotMean() const { return m_bPlotMean; }

	void AddValue(float fValue) { m_variable.AddValue(fValue); }
	void SetGraphColor(ncColor graphColor) { m_graphColor = graphColor; }
	void SetMeanColor(ncColor meanColor) { m_meanColor = meanColor; }
	void SetPlotMean(bool bEnabled) { m_bPlotMean = bEnabled; }
	void UpdateVertices(int x, int y, int w, int h);
};



/// A class that plots a graphic representation of a time/value function
class ncLinePlotter
{
private:
	int m_iX;
	int m_iY;
	int m_iWidth;
	int m_iHeight;

	ncColor m_backgroundColor;
	GLfloat m_fBackgroundVertices[12];
	ncArray<ncLineVariable *> m_vVariables;

	void SetBackgroundVertices();
public:
	ncLinePlotter(ncRect rect)
		: m_iX(rect.x), m_iY(rect.y), m_iWidth(rect.w), m_iHeight(rect.h), m_vVariables(2)
	{
		SetBackgroundVertices();
	}
	~ncLinePlotter();

	unsigned int AddVariable(unsigned int uNumValues, unsigned int uRejectDelay);
	void AddValue(unsigned int iVariable, float fValue);
	void Draw();
	void SetBackgroundColor(ncColor backgroundColor) { m_backgroundColor = backgroundColor; }
	ncLineVariable& Variable(unsigned int uIndex);
};

#endif
