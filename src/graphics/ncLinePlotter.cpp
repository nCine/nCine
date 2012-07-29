#include "ncLinePlotter.h"
#include "ncRenderQueue.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncLineVariable::ncLineVariable(unsigned int uNumValues, unsigned int uRejectDelay)
	: ncPlottingVariable(uNumValues, uRejectDelay)
{
	// Two vertices for the mean quote plus...
	// One vertex (2 coordinates each) for every recorded value
	m_fVertices = new GLfloat[4 + uNumValues*2];
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

void ncLinePlotter::Draw(ncRenderQueue& rRenderQueue)
{
	// Drawing the background
	ncDrawableNode::ApplyTransformations();
	ncDrawableNode::Draw(rRenderQueue);

	for (unsigned int i = 0; i < m_vVariables.Size(); i++)
	{
		m_vVariables[i]->UpdateVertices(0, 0, m_iWidth, m_iHeight);
		m_vVariables[i]->ApplyTransformations(m_fAbsX, m_fAbsY, m_fAbsRotation, m_fAbsScaleFactor);
		m_vVariables[i]->Draw(rRenderQueue);
		if (m_vVariables[i]->shouldPlotMean())
			m_vVariables[i]->DrawMean(rRenderQueue);
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
	for(unsigned int i = 0; i < uNumValues; i++)
	{	
		m_fVertices[4 + 2*i] = x + (w/uNumValues)*(i+1);
		m_fVertices[4 + 2*i + 1] = y + h*m_variable.NormValue((uNextIndex+i)%uNumValues);
	}
}

void ncLineVariable::UpdateRenderCommand()
{
    m_valuesCmd.Material().SetTextureGLId(0);
	m_valuesCmd.Material().SetColor(m_graphColor);
//	m_valuesCmd.Transformation().SetPosition(AbsPosition().x, AbsPosition().y);
    m_valuesCmd.Geometry().SetData(GL_LINE_STRIP, 2, m_variable.NumValues(), m_fVertices, NULL, NULL);
    m_valuesCmd.CalculateSortKey();
}

void ncLineVariable::UpdateMeanRenderCommand()
{
	m_meanCmd.Material().SetTextureGLId(0);
	m_meanCmd.Material().SetColor(m_meanColor);
//	m_meanCmd.Transformation().SetPosition(AbsPosition().x, AbsPosition().y);
	m_meanCmd.Geometry().SetData(GL_LINES, 0, 2, m_fVertices, NULL, NULL);
	m_meanCmd.CalculateSortKey();
}
