#include "ncProfilePlotter.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncProfilePlotter::ncProfilePlotter(ncSceneNode* pParent, ncRect rect)
	: ncDrawableNode(pParent, rect.x, rect.y), m_iWidth(rect.w), m_iHeight(rect.h), m_vVariables(2),
	  m_bPlotRefValue(false), m_refValueColor(1.0f, 1.0f, 1.0f), m_fRefValue(0.0f)
{
	SetPriority(ncDrawableNode::HUD_PRIORITY);

	// One more than variable mean lines
	m_refValueCmd.SetPriority(ncDrawableNode::HUD_PRIORITY + 3);
	m_refValueCmd.SetType(ncRenderCommand::PLOTTER_TYPE);
}

ncProfilePlotter::~ncProfilePlotter()
{
	for (unsigned int i = 0; i < m_vVariables.Size(); i++)
	{
		delete m_vVariables[i];
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Adds a value to the specified variable
bool ncProfilePlotter::AddValue(unsigned int uIndex, float fValue)
{
	bool bValueRegistered = false;

	if (uIndex < m_vVariables.Size())
	{
		bValueRegistered = m_vVariables[uIndex]->AddValue(fValue);
	}

	return bValueRegistered;
}

/// Returns the variable with the specified index
ncPlottingVariable& ncProfilePlotter::Variable(unsigned int uIndex)
{
	if (uIndex < m_vVariables.Size())
	{
		return *m_vVariables[uIndex];
	}
	else
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncProfilePlotter::Variable - Index out of range");
		exit(EXIT_FAILURE);
	}
}

/// Returns the reference value normalized and clamped between the two numbers provided
float ncProfilePlotter::NormBetweenRefValue(float fMin, float fMax) const
{
	float fValue = 0.0f;

	if (fMax - fMin > 0.0f)
	{
		if (m_fRefValue < fMin)
		{
			fValue = 0.0f;
		}
		else if (m_fRefValue > fMax)
		{
			fValue = 1.0f;
		}
		else
		{
			fValue = (m_fRefValue - fMin) / (fMax - fMin);
		}
	}

	return fValue;
}

/// Applies parent transformations to reference value vertices
void ncProfilePlotter::ApplyTransformations(float fAbsX, float fAbsY, float fAbsRotation, float fAbsScaleFactor)
{
	// Variable values geometry
	ncRenderGeometry &rGeom = m_refValueCmd.Geometry();

	float sine = 0.0f;
	float cosine = 1.0f;
	if (abs(fAbsRotation) > ncDrawableNode::sMinRotation && abs(fAbsRotation) < 360.0f - ncDrawableNode::sMinRotation)
	{
		sine = sinf(-fAbsRotation * M_PI / 180.0f);
		cosine = cosf(-fAbsRotation * M_PI / 180.0f);
	}

	for (int i = rGeom.FirstVertex(); i < rGeom.NumVertices(); i++)
	{
		float fX = rGeom.VertexPointer()[2 * i] * fAbsScaleFactor;			float fY = rGeom.VertexPointer()[2 * i + 1] * fAbsScaleFactor;
		rGeom.VertexPointer()[2 * i] = fAbsX + fX * cosine - fY * sine;		rGeom.VertexPointer()[2 * i + 1] = fAbsY + fY * cosine + fX * sine;
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

	ncDrawableNode::ApplyTransformations();
}

/// Updates the reference value rendering command
void ncProfilePlotter::UpdateRefValueRenderCommand()
{
	m_refValueCmd.Material().SetTextureGLId(0);
	m_refValueCmd.Material().SetColor(m_refValueColor);
	//m_refValueCmd.Transformation().SetPosition(AbsPosition().x, AbsPosition().y);
	m_refValueCmd.Geometry().SetData(GL_LINES, 0, 2, m_fRefValueVertices, NULL, NULL);
	m_refValueCmd.CalculateSortKey();
}
