#include "ncPlottingVariable.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncPlottingVariable::ncPlottingVariable(unsigned int uNumValues, float fRejectDelay)
	: m_bPlotMean(true), m_graphColor(1.0f, 1.0f, 1.0f), m_meanColor(1.0f, 0.0f, 0.0f),
	  m_variable(uNumValues, fRejectDelay), m_fVertices(NULL)
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
	{
		delete[] m_fVertices;
	}
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
		sine = sinf(-fAbsRotation * M_PI / 180.0f);
		cosine = cosf(-fAbsRotation * M_PI / 180.0f);
	}

	// Total number of vertices comprises both mean and variable vertices
	int iNumVertices = rGeom.NumVertices() + m_meanCmd.Geometry().NumVertices();
	// iStartingVertex is used to transform all the values if the mean is not rendered
	int iStartingVertex = rGeom.FirstVertex() - m_meanCmd.Geometry().NumVertices();
	for (int i = iStartingVertex; i < iStartingVertex + iNumVertices; i++)
	{
		float fX = rGeom.VertexPointer()[2 * i] * fAbsScaleFactor;			float fY = rGeom.VertexPointer()[2 * i + 1] * fAbsScaleFactor;
		rGeom.VertexPointer()[2 * i] = fAbsX + fX * cosine - fY * sine;		rGeom.VertexPointer()[2 * i + 1] = fAbsY + fY * cosine + fX * sine;
	}
}
