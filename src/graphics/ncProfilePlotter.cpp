#include "ncProfilePlotter.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncPlottingVariable::ncPlottingVariable(unsigned int uNumValues, unsigned int uRejectDelay)
	: m_variable(uNumValues, uRejectDelay), m_bPlotMean(true),
	  m_graphColor(1.0f, 1.0f, 1.0f), m_meanColor(1.0f, 0.0f, 0.0f)
{

}

ncPlottingVariable::~ncPlottingVariable()
{
	if (m_fVertices)
		delete[] m_fVertices;
}

ncProfilePlotter::~ncProfilePlotter()
{
	for (int i = 0; i < m_vVariables.Size(); i++)
		delete m_vVariables[i];
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void ncProfilePlotter::AddValue(unsigned int uIndex, float fValue)
{
	if (uIndex < m_vVariables.Size())
		m_vVariables[uIndex]->AddValue(fValue);
}

ncPlottingVariable& ncProfilePlotter::Variable(unsigned int uIndex)
{
	if (uIndex < m_vVariables.Size())
		return *m_vVariables[uIndex];
	else
	{
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncProfilePlotter::Variable - Index out of range");
		exit(-1);
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Fill the background buffer with vertices
void ncProfilePlotter::SetBackgroundVertices()
{
	// Graph background vertices
	m_fBackgroundVertices[0] = m_iX;				m_fBackgroundVertices[1] = m_iY;
	m_fBackgroundVertices[2] = m_iX + m_iWidth;		m_fBackgroundVertices[3] = m_iY;
	m_fBackgroundVertices[4] = m_iX;				m_fBackgroundVertices[5] = m_iY + m_iHeight;

	m_fBackgroundVertices[6] = m_iX + m_iWidth;		m_fBackgroundVertices[7] = m_iY + m_iHeight;
}

/// Draw the background
void ncProfilePlotter::DrawBackground()
{
	glEnable(GL_BLEND);
	//glColor4ubv(m_backgroundColor.Vector()); // Not available on GLES
	glColor4ub(m_backgroundColor.R(), m_backgroundColor.G(), m_backgroundColor.B(), m_backgroundColor.A());
	glVertexPointer(2, GL_FLOAT, 0, m_fBackgroundVertices);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisable(GL_BLEND);
}
