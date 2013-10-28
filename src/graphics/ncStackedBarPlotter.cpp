#include "ncStackedBarPlotter.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncStackedBarVariable::ncStackedBarVariable(unsigned int uNumValues, float fRejectDelay)
	: ncPlottingVariable(uNumValues, fRejectDelay)
{
	// Two vertices for the mean quote plus...
	// Six vertices (two coordinates each) for every recorded value
	m_fVertices = new GLfloat[4 + uNumValues * 2*6];
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

unsigned int ncStackedBarPlotter::AddVariable(unsigned int uNumValues, float fRejectDelay)
{
	if (!m_vVariables.isEmpty())
	{
		if (uNumValues != m_vVariables[0]->Variable()->NumValues())
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"ncStackedBarPlotter::AddVariable - Variable not added because number of values is inconsistent");
			return 0; // TODO: switch to signed int and return -1?
		}
		float fDiff = fRejectDelay - m_vVariables[0]->Variable()->Delay();
		if (fDiff < -0.001f || fDiff > 0.001f) // HACK: one millisecond fixed tolerance
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"ncStackedBarPlotter::AddVariable - Variable not added because reject delay is inconsistent");
			return 0; // TODO: switch to signed int and return -1?
		}
	}

	ncStackedBarVariable* pVariable = new ncStackedBarVariable(uNumValues, fRejectDelay);
	m_vVariables.InsertBack(pVariable);

	return m_vVariables.Size()-1;
}

void ncStackedBarPlotter::Draw(ncRenderQueue& rRenderQueue)
{
	// Drawing the background
	ncDrawableNode::Draw(rRenderQueue);

	UpdateAllVertices(0, 0, m_iWidth, m_iHeight);

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


/// Fill the buffer of every stacked variable with vertices
void ncStackedBarPlotter::UpdateAllVertices(int x, int y, int w, int h)
{
	unsigned int uNumVariables = m_vVariables.Size();
	float fScaledH = h / uNumVariables;

	float fMeanVerticalOffset = 0.0f;
	for (unsigned int i = 0; i < m_vVariables.Size(); i++)
	{
		const ncProfileVariable* profVariable = m_vVariables[i]->Variable();
		GLfloat* fVertices = m_vVariables[i]->Vertices();

		float fNormalizedMean = profVariable->NormMean();
		// Variable mean vertices
		fVertices[0] = x;			fVertices[1] = y + fScaledH * (fMeanVerticalOffset + fNormalizedMean);
		fVertices[2] = x + w;		fVertices[3] = y + fScaledH * (fMeanVerticalOffset + fNormalizedMean);

		fMeanVerticalOffset += fNormalizedMean;
	}

	unsigned int uNumValues = 0;
	unsigned int uNextIndex = 0;
	if (!m_vVariables.isEmpty())
	{
		// Common values among every variable (enforced in ncStackedBarPlotter::AddVariable)
		uNumValues = m_vVariables[0]->Variable()->NumValues();
		uNextIndex = m_vVariables[0]->Variable()->NextIndex();
	}
	for (unsigned int i = 0; i < uNumValues; i++)
	{
		float fVerticalOffset = 0.0f;
		float fStep = (float(w)/float(uNumValues)) * 0.5f;
		float fCenter = 2.0f*fStep*(i+1) - fStep;

		for (unsigned int j = 0; j < m_vVariables.Size(); j++)
		{
			const ncProfileVariable* profVariable = m_vVariables[j]->Variable();
			GLfloat* fVertices = m_vVariables[j]->Vertices();

			float fNormValue = profVariable->NormValue((uNextIndex+i)%uNumValues);

			fVertices[4 + 12*i + 0] = x + fCenter - fStep;
			fVertices[4 + 12*i + 1] = y + fScaledH * fVerticalOffset;
			fVertices[4 + 12*i + 2] = x + fCenter + fStep;
			fVertices[4 + 12*i + 3] = y + fScaledH * fVerticalOffset;
			fVertices[4 + 12*i + 4] = x + fCenter - fStep;
			fVertices[4 + 12*i + 5] = y + fScaledH * (fVerticalOffset + fNormValue);

			fVertices[4 + 12*i + 6] = x + fCenter + fStep;
			fVertices[4 + 12*i + 7] = y + fScaledH * fVerticalOffset;
			fVertices[4 + 12*i + 8] = x + fCenter + fStep;
			fVertices[4 + 12*i + 9] = y + fScaledH * (fVerticalOffset + fNormValue);
			fVertices[4 + 12*i +10] = x + fCenter - fStep;
			fVertices[4 + 12*i +11] = y + fScaledH * (fVerticalOffset + fNormValue);

			fVerticalOffset += fNormValue;
		}
	}
}

void ncStackedBarVariable::UpdateRenderCommand()
{
    m_valuesCmd.Material().SetTextureGLId(0);
	m_valuesCmd.Material().SetColor(m_graphColor);
//	m_valuesCmd.Transformation().SetPosition(AbsPosition().x, AbsPosition().y);
    m_valuesCmd.Geometry().SetData(GL_TRIANGLES, 2, m_variable.NumValues()*6, m_fVertices, NULL, NULL);
    m_valuesCmd.CalculateSortKey();
}

void ncStackedBarVariable::UpdateMeanRenderCommand()
{
	m_meanCmd.Material().SetTextureGLId(0);
	m_meanCmd.Material().SetColor(m_meanColor);
//	m_meanCmd.Transformation().SetPosition(AbsPosition().x, AbsPosition().y);
	m_meanCmd.Geometry().SetData(GL_LINES, 0, 2, m_fVertices, NULL, NULL);
	m_meanCmd.CalculateSortKey();
}

