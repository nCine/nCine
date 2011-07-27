#include "ncLinePlotter.h"

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

void ncLinePlotter::Draw()
{
	glDisable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);

	// Rendering the background
	DrawBackground();

	for (int i = 0; i < m_vVariables.Size(); i++)
	{
		m_vVariables[i]->UpdateVertices(m_iX, m_iY, m_iWidth, m_iHeight);
		glVertexPointer(2, GL_FLOAT, 0, m_vVariables[i]->Vertices());

		// Rendering variable values
		const ncColor& graphColor = m_vVariables[i]->GraphColor();
		// glColor4ubv(graphColor.Vector()); // Not availble on GLES
		glColor4ub(graphColor.R(), graphColor.G(), graphColor.B(), graphColor.A());
		glDrawArrays(GL_LINE_STRIP, 2, m_vVariables[i]->NumValues());

		if (m_vVariables[i]->shouldPlotMean())
		{
			// Rendering variable mean
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
	for(int i = 0; i < uNumValues; i++)
	{	
		m_fVertices[4 + 2*i] = x + (w/uNumValues)*(i+1);
		m_fVertices[4 + 2*i + 1] = y + h*m_variable.NormValue((uNextIndex+i)%uNumValues);
	}
}
