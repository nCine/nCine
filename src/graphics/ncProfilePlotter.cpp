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

	m_valuesCmd.SetType(ncRenderCommand::PLOTTER_TYPE);
	m_meanCmd.SetType(ncRenderCommand::PLOTTER_TYPE);
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

/// Applies parent transformations to both mean and values vertices
void ncPlottingVariable::ApplyTransformations(float fAbsX, float fAbsY, float fAbsRotation, float fAbsScaleFactor)
{
	// Variable values geometry
	ncRenderGeometry &rGeom = m_valuesCmd.Geometry();

	float sine = 0.0f;
	float cosine = 1.0f;
	if (abs(fAbsRotation) > ncDrawableNode::sMinRotation && abs(fAbsRotation) < 360.0f - ncDrawableNode::sMinRotation)
	{
		sine = sinf(-fAbsRotation * M_PI/180.0f);
		cosine = cosf(-fAbsRotation * M_PI/180.0f);
	}

	// Total number of vertices comprises both mean and variable vertices
	int iNumVertices = rGeom.NumVertices() + m_meanCmd.Geometry().NumVertices();
	for (int i = 0; i < iNumVertices*2; i=i+2) // First vertex is zero to include mean values
	{
		float fX = rGeom.VertexPointer()[i]*fAbsScaleFactor;			float fY = rGeom.VertexPointer()[i+1]*fAbsScaleFactor;
		rGeom.VertexPointer()[i] = fAbsX + fX*cosine - fY*sine;			rGeom.VertexPointer()[i+1] = fAbsY + fY*cosine + fX*sine;
	}
}


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
	m_fBackgroundVertices[0] = 0;				m_fBackgroundVertices[1] = 0;
	m_fBackgroundVertices[2] = m_iWidth;		m_fBackgroundVertices[3] = 0;
	m_fBackgroundVertices[4] = 0;				m_fBackgroundVertices[5] = m_iHeight;

	m_fBackgroundVertices[6] = m_iWidth;		m_fBackgroundVertices[7] = m_iHeight;
}

void ncProfilePlotter::UpdateRenderCommand()
{
	m_renderCmd.Material().SetTextureGLId(0);
	m_renderCmd.Material().SetColor(m_backgroundColor);
	m_renderCmd.Transformation().SetPosition(AbsPosition().x, AbsPosition().y);
	SetBackgroundVertices();
	m_renderCmd.Geometry().SetData(GL_TRIANGLE_STRIP, 0, 4, m_fBackgroundVertices, NULL, NULL);
	m_renderCmd.CalculateSortKey();

	ApplyTransformations();
}
