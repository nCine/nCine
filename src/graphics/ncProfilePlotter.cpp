#include "ncProfilePlotter.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncPlottingVariable::ncPlottingVariable(unsigned int uNumValues, unsigned int uRejectDelay)
	: m_bPlotMean(true), m_graphColor(1.0f, 1.0f, 1.0f), m_meanColor(1.0f, 0.0f, 0.0f),
	  m_variable(uNumValues, uRejectDelay), m_fVertices(NULL)
{
	// One more than the profile plotter background
	m_valuesCmd.SetPriority(ncDrawableNode::HUD_PRIORITY + 1);
	// One more than the variable graph
	m_meanCmd.SetPriority(ncDrawableNode::HUD_PRIORITY + 2);
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
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncProfilePlotter::Variable - Index out of range");
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
	m_fBackgroundVertices[0] = x;				m_fBackgroundVertices[1] = y;
	m_fBackgroundVertices[2] = x + m_iWidth;	m_fBackgroundVertices[3] = y;
	m_fBackgroundVertices[4] = x;				m_fBackgroundVertices[5] = y + m_iHeight;

	m_fBackgroundVertices[6] = x + m_iWidth;	m_fBackgroundVertices[7] = y + m_iHeight;
}

void ncProfilePlotter::UpdateRenderCommand()
{
	m_renderCmd.Material().SetTextureGLId(0);
	m_renderCmd.Material().SetColor(m_backgroundColor);
	m_renderCmd.Transformation().SetPosition(AbsPosition().x, AbsPosition().y);
	SetBackgroundVertices();
	m_renderCmd.Geometry().SetData(GL_TRIANGLE_STRIP, 0, 4, m_fBackgroundVertices, NULL, NULL);
	m_renderCmd.CalculateSortKey();
}
