#include "ncProfilePlotter.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncProfilePlotter::ncProfilePlotter(ncSceneNode* pParent, ncRect rect)
	: ncDrawableNode(pParent, rect.x, rect.y), m_iWidth(rect.w), m_iHeight(rect.h), m_vVariables(2)
{
	SetPriority(ncDrawableNode::HUD_PRIORITY);
}

ncProfilePlotter::~ncProfilePlotter()
{
	for (unsigned int i = 0; i < m_vVariables.Size(); i++)
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
