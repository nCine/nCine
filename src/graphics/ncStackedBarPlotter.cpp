#include "ncStackedBarPlotter.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncStackedBarVariable::ncStackedBarVariable(unsigned int uNumValues, unsigned int uRejectDelay)
	: ncPlottingVariable(uNumValues, uRejectDelay)
{
	// Two vertices for the mean quote plus...
	// Six vertices (two coordinates each) for every recorded value
	m_fVertices = new GLfloat[4 + uNumValues * 2*6];
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

unsigned int ncStackedBarPlotter::AddVariable(unsigned int uNumValues, unsigned int uRejectDelay)
{
	if (!m_vVariables.isEmpty())
	{
		if (uNumValues != m_vVariables[0]->Variable()->NumValues())
		{
			ncServiceLocator::GetLogger().Write(ncILogger::LOG_WARN, (char *)"ncStackedBarPlotter::AddVariable - Variable not added because number of values is inconsistent");
			return 0; // TODO: switch to signed int and return -1?
		}
		if (uRejectDelay != m_vVariables[0]->Variable()->Delay())
		{
			ncServiceLocator::GetLogger().Write(ncILogger::LOG_WARN, (char *)"ncStackedBarPlotter::AddVariable - Variable not added because reject delay is inconsistent");
			return 0; // TODO: switch to signed int and return -1?
		}
	}

	ncStackedBarVariable* pVariable = new ncStackedBarVariable(uNumValues, uRejectDelay);
	m_vVariables.InsertBack(pVariable);

	return m_vVariables.Size()-1;
}

void ncStackedBarPlotter::Draw()
{
	glDisable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);

	// Rendering the background
	DrawBackground();

	UpdateAllVertices(m_iX, m_iY, m_iWidth, m_iHeight);
	for (int i = 0; i < m_vVariables.Size(); i++)
	{
		glVertexPointer(2, GL_FLOAT, 0, m_vVariables[i]->Vertices());

		// Rendering variable values
		const ncColor& graphColor = m_vVariables[i]->GraphColor();
		// glColor4ubv(graphColor.Vector()); // Not availble on GLES
		glColor4ub(graphColor.R(), graphColor.G(), graphColor.B(), graphColor.A());
		glDrawArrays(GL_TRIANGLES, 2, m_vVariables[i]->NumValues()*6);
	}

	for (int i = 0; i < m_vVariables.Size(); i++)
	{
		if (m_vVariables[i]->shouldPlotMean())
		{
			glVertexPointer(2, GL_FLOAT, 0, m_vVariables[i]->Vertices());

			// Rendering variable mean on top of everything else
			const ncColor& meanColor = m_vVariables[i]->MeanColor();
			// glColor4ubv(meanColor.Vector()); // Not availble on GLES
			glColor4ub(meanColor.R(), meanColor.G(), meanColor.B(), meanColor.A());
			glDrawArrays(GL_LINES, 0, 2);
		}
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
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
	for (int i = 0; i < m_vVariables.Size(); i++)
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
	for (int i = 0; i < uNumValues; i++)
	{
		float fVerticalOffset = 0.0f;
		float fStep = (float(w)/float(uNumValues)) * 0.5f;
		float fCenter = 2.0f*fStep*(i+1) - fStep;

		for (int j = 0; j < m_vVariables.Size(); j++)
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
