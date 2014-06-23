#include "ncLinePlotter.h"
#include "ncRenderQueue.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncLineVariable::ncLineVariable(unsigned int uNumValues, float fRejectDelay)
	: ncPlottingVariable(uNumValues, fRejectDelay)
{
	// Two vertices for the mean quote plus...
	// One vertex (2 coordinates each) for every recorded value
	m_fVertices = new GLfloat[4 + uNumValues*2];
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

unsigned int ncLinePlotter::AddVariable(unsigned int uNumValues, float fRejectDelay)
{
	ncLineVariable* pVariable = new ncLineVariable(uNumValues, fRejectDelay);
	m_vVariables.InsertBack(pVariable);

	return m_vVariables.Size()-1;
}

/// Fill the buffer of every line variable with vertices
void ncLinePlotter::UpdateAllVertices(int x, int y, int w, int h)
{
	float fCommonMin = 0.0f;
	float fCommonMax = 0.0f;
	if (!m_vVariables.isEmpty())
	{
		fCommonMin = m_vVariables[0]->Variable()->Min();
		fCommonMax = m_vVariables[0]->Variable()->Max();
	}
	for (unsigned int i = 1; i < m_vVariables.Size(); i++)
	{
		if (m_vVariables[i]->Variable()->Min() < fCommonMin)
			fCommonMin = m_vVariables[i]->Variable()->Min();
		else if (m_vVariables[i]->Variable()->Max() > fCommonMax)
			fCommonMax = m_vVariables[i]->Variable()->Max();
	}

	float fNormalizedRefValue = NormBetweenRefValue(fCommonMin, fCommonMax);
	m_fRefValueVertices[0] = x;		m_fRefValueVertices[1] = y + h * fNormalizedRefValue;
	m_fRefValueVertices[2] = x + w;	m_fRefValueVertices[3] = y + h * fNormalizedRefValue;

	for (unsigned int i = 0; i < m_vVariables.Size(); i++)
	{
		const ncProfileVariable* profVariable = m_vVariables[i]->Variable();
		GLfloat* fVertices = m_vVariables[i]->Vertices();

		float fNormalizedMean = profVariable->NormBetweenMean(fCommonMin, fCommonMax);
		// Variable mean vertices
		fVertices[0] = x;			fVertices[1] = y + h * fNormalizedMean;
		fVertices[2] = x + w;		fVertices[3] = y + h * fNormalizedMean;

		unsigned int uNumValues = profVariable->NumValues();
		unsigned int uNextIndex = profVariable->NextIndex();

		// Variable value vertices
		for(unsigned int i = 0; i < uNumValues; i++)
		{
			fVertices[4 + 2*i] = x + i * w/(uNumValues-1);
			fVertices[4 + 2*i + 1] = y + h*profVariable->NormBetweenValue((uNextIndex+i)%uNumValues, fCommonMin, fCommonMax);
		}
	}
}

void ncLinePlotter::Draw(ncRenderQueue& rRenderQueue)
{
	// Drawing the background
	ncDrawableNode::Draw(rRenderQueue);

	UpdateAllVertices(0, 0, m_iWidth, m_iHeight);

	// Drawing the reference value line
	if (shouldPlotRefValue())
	{
		ApplyTransformations(m_fAbsX, m_fAbsY, m_fAbsRotation, m_fAbsScaleFactor);
		DrawRefValue(rRenderQueue);
	}

	for (unsigned int i = 0; i < m_vVariables.Size(); i++)
	{
		m_vVariables[i]->ApplyTransformations(m_fAbsX, m_fAbsY, m_fAbsRotation, m_fAbsScaleFactor);
		m_vVariables[i]->Draw(rRenderQueue);
		if (m_vVariables[i]->shouldPlotMean())
			m_vVariables[i]->DrawMean(rRenderQueue);
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

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
