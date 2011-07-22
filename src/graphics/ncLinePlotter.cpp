#include "ncLinePlotter.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncLineVariable::ncLineVariable(unsigned int uNumValues, unsigned int uRejectDelay)
	: m_variable(uNumValues, uRejectDelay), m_bPlotMean(true),
	  m_graphColor(1.0f, 1.0f, 1.0f), m_meanColor(1.0f, 0.0f, 0.0f)
{
	// Two coordinates for every vertex (X, Y)
	m_fVertices = new GLfloat[uNumValues*2];
}

ncLineVariable::~ncLineVariable()
{
	if (m_fVertices)
		delete[] m_fVertices;
}

ncLinePlotter::~ncLinePlotter()
{
	for (int i = 0; i < m_vVariables.Size(); i++)
		delete m_vVariables[i];
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

void ncLinePlotter::AddValue(unsigned int uIndex, float fValue)
{
	if (uIndex < m_vVariables.Size())
		m_vVariables[uIndex]->AddValue(fValue);
}

void ncLinePlotter::Draw()
{
	glDisable(GL_TEXTURE_2D);

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4ub(m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundColor.a);
	glBegin(GL_TRIANGLES);
	glVertex2i(m_iX, m_iY);
	glVertex2i(m_iX + m_iWidth, m_iY);
	glVertex2i(m_iX, m_iY + m_iHeight);
	glVertex2i(m_iX + m_iWidth, m_iY);
	glVertex2i(m_iX + m_iWidth, m_iY + m_iHeight);
	glVertex2i(m_iX, m_iY + m_iHeight);
	glEnd();
	glDisable(GL_BLEND);

	for (int i = 0; i < m_vVariables.Size(); i++)
	{
		m_vVariables[i]->UpdateVertices(m_iX, m_iY, m_iWidth, m_iHeight);

		const ncColor& graphColor = m_vVariables[i]->GraphColor();
		glColor4ub(graphColor.r, graphColor.g, graphColor.b, graphColor.a);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, m_vVariables[i]->Vertices());
		glDrawArrays(GL_LINE_STRIP, 0, m_vVariables[i]->NumValues());
		glDisableClientState(GL_VERTEX_ARRAY);

		if (m_vVariables[i]->shouldPlotMean())
		{
			const ncColor& meanColor = m_vVariables[i]->MeanColor();
			float fNormalizedMean = m_vVariables[i]->NormMean();
			glColor4ub(meanColor.r, meanColor.g, meanColor.b, meanColor.a);
			glBegin(GL_LINES);
			glVertex2i(m_iX, m_iY + m_iHeight*fNormalizedMean);
			glVertex2i(m_iX + m_iWidth, m_iY + m_iHeight*fNormalizedMean);
			glEnd();
		}
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
}

ncLineVariable& ncLinePlotter::Variable(unsigned int uIndex)
{
	if (uIndex < m_vVariables.Size())
		return *m_vVariables[uIndex];
	else
	{
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, (char *)"ncLinePlotter::Variable - Index out of range");
		exit(-1);
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Fill the buffer with vertices
void ncLineVariable::UpdateVertices(int x, int y, int w, int h)
{
	unsigned int uNumValues = m_variable.NumValues();
	unsigned int uNextIndex = m_variable.NextIndex();

	for(int i = 0; i < uNumValues; i++)
	{	
		m_fVertices[2*i] = x + (w/uNumValues)*(i+1);
		m_fVertices[2*i + 1] = y + h*m_variable.NormValue((uNextIndex+i)%uNumValues);
	}
}

