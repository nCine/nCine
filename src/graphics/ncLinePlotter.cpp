#include "ncLinePlotter.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncLineVariable::ncLineVariable(unsigned int uNumValues, unsigned int uRejectDelay)
	: m_variable(uNumValues, uRejectDelay), m_bPlotMean(true),
	  m_graphColor(1.0f, 1.0f, 1.0f), m_meanColor(1.0f, 0.0f, 0.0f)
{
	// Two vertices for the mean quote plus...
	// Two coordinates for every recorder value vertex (X, Y)
	m_fVertices = new GLfloat[4 + uNumValues*2];
}

ncLineVariable::~ncLineVariable()
{
	if (m_fVertices)
		delete[] m_fVertices;
}

ncLinePlotter::~ncLinePlotter()
{
	for (int i = 0; i < m_vVariables.Size(); i++)
		delete m_vVariables[i];
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

unsigned int ncLinePlotter::AddVariable(unsigned int uNumValues, unsigned int uRejectDelay)
{
	ncLineVariable* pVariable = new ncLineVariable(uNumValues, uRejectDelay);
	m_vVariables.InsertBack(pVariable);

	return m_vVariables.Size()-1;
}

void ncLinePlotter::AddValue(unsigned int uIndex, float fValue)
{
	if (uIndex < m_vVariables.Size())
		m_vVariables[uIndex]->AddValue(fValue);
}

void ncLinePlotter::Draw()
{
	glDisable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);

	// Rendering the background
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4ub(m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundColor.a);
	glVertexPointer(2, GL_FLOAT, 0, m_fBackgroundVertices);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisable(GL_BLEND);

	for (int i = 0; i < m_vVariables.Size(); i++)
	{
		m_vVariables[i]->UpdateVertices(m_iX, m_iY, m_iWidth, m_iHeight);
		glVertexPointer(2, GL_FLOAT, 0, m_vVariables[i]->Vertices());

		// Rendering variable values
		const ncColor& graphColor = m_vVariables[i]->GraphColor();
		glColor4ub(graphColor.r, graphColor.g, graphColor.b, graphColor.a);	
		glDrawArrays(GL_LINE_STRIP, 2, m_vVariables[i]->NumValues());

		if (m_vVariables[i]->shouldPlotMean())
		{
			// Rendering variable mean
			const ncColor& meanColor = m_vVariables[i]->MeanColor();
			glColor4ub(meanColor.r, meanColor.g, meanColor.b, meanColor.a);
			glDrawArrays(GL_LINES, 0, 2);
		}	
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
}

ncLineVariable& ncLinePlotter::Variable(unsigned int uIndex)
{
	if (uIndex < m_vVariables.Size())
		return *m_vVariables[uIndex];
	else
	{
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncLinePlotter::Variable - Index out of range");
		exit(-1);
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Fill the buffer with vertices
void ncLineVariable::UpdateVertices(int x, int y, int w, int h)
{
	float fNormalizedMean = m_variable.NormMean();
	// Variable mean vertices
	m_fVertices[0] = x;			m_fVertices[1] = y + h * fNormalizedMean;
	m_fVertices[2] = x + w;		m_fVertices[3] = y + h * fNormalizedMean;

	unsigned int uNumValues = m_variable.NumValues();
	unsigned int uNextIndex = m_variable.NextIndex();

	// Variable value vertices
	for(int i = 0; i < uNumValues; i++)
	{	
		m_fVertices[4 + 2*i] = x + (w/uNumValues)*(i+1);
		m_fVertices[4 + 2*i + 1] = y + h*m_variable.NormValue((uNextIndex+i)%uNumValues);
	}
}

/// Fill the background buffer with vertices
void ncLinePlotter::SetBackgroundVertices()
{
	// Graph background vertices
	m_fBackgroundVertices[0] = m_iX;				m_fBackgroundVertices[1] = m_iY;
	m_fBackgroundVertices[2] = m_iX + m_iWidth;		m_fBackgroundVertices[3] = m_iY;
	m_fBackgroundVertices[4] = m_iX;				m_fBackgroundVertices[5] = m_iY + m_iHeight;
	m_fBackgroundVertices[6] = m_iX + m_iWidth;		m_fBackgroundVertices[7] = m_iY;
	m_fBackgroundVertices[8] = m_iX + m_iWidth;		m_fBackgroundVertices[9] = m_iY + m_iHeight;
	m_fBackgroundVertices[10] = m_iX;				m_fBackgroundVertices[11] = m_iY + m_iHeight;
}

