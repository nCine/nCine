#include "LinePlotter.h"
#include "RenderQueue.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

LineVariable::LineVariable(unsigned int numValues, float rejectDelay)
	: PlottingVariable(numValues, rejectDelay)
{
	// Two vertices for the mean quote plus...
	// One vertex (2 coordinates each) for every recorded value
	vertices_ = new GLfloat[4 + numValues * 2];
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

unsigned int LinePlotter::addVariable(unsigned int numValues, float rejectDelay)
{
	LineVariable* variable = new LineVariable(numValues, rejectDelay);
	variables_.insertBack(variable);

	return variables_.size() - 1;
}

/// Fill the buffer of every line variable with vertices
void LinePlotter::updateAllVertices(int x, int y, int w, int h)
{
	float commonMin = 0.0f;
	float commonMax = 0.0f;
	if (!variables_.isEmpty())
	{
		commonMin = variables_[0]->variable()->min();
		commonMax = variables_[0]->variable()->max();
	}
	for (unsigned int i = 1; i < variables_.size(); i++)
	{
		if (variables_[i]->variable()->min() < commonMin)
		{
			commonMin = variables_[i]->variable()->min();
		}
		else if (variables_[i]->variable()->max() > commonMax)
		{
			commonMax = variables_[i]->variable()->max();
		}
	}

	float normalizedRefValue = normBetweenRefValue(commonMin, commonMax);
	refValueVertices_[0] = x;		refValueVertices_[1] = y + h * normalizedRefValue;
	refValueVertices_[2] = x + w;	refValueVertices_[3] = y + h * normalizedRefValue;

	for (unsigned int i = 0; i < variables_.size(); i++)
	{
		const ProfileVariable* profVariable = variables_[i]->variable();
		GLfloat* vertices = variables_[i]->vertices();

		float normalizedMean = profVariable->normBetweenMean(commonMin, commonMax);
		// Variable mean vertices
		vertices[0] = x;			vertices[1] = y + h * normalizedMean;
		vertices[2] = x + w;		vertices[3] = y + h * normalizedMean;

		unsigned int numValues = profVariable->numValues();
		unsigned int nextIndex = profVariable->nextIndex();

		// Variable value vertices
		for (unsigned int i = 0; i < numValues; i++)
		{
			vertices[4 + 2 * i] = x + i * w / (numValues - 1);
			vertices[4 + 2 * i + 1] = y + h * profVariable->normBetweenValue((nextIndex + i) % numValues, commonMin, commonMax);
		}
	}
}

void LinePlotter::draw(RenderQueue& renderQueue)
{
	// Drawing the background
	DrawableNode::draw(renderQueue);

	updateAllVertices(0, 0, width_, height_);

	// Drawing the reference value line
	if (shouldPlotRefValue())
	{
		applyTransformations(absX_, absY_, absRotation_, absScaleFactor_);
		drawRefValue(renderQueue);
	}

	for (unsigned int i = 0; i < variables_.size(); i++)
	{
		variables_[i]->applyTransformations(absX_, absY_, absRotation_, absScaleFactor_);
		variables_[i]->draw(renderQueue);
		if (variables_[i]->shouldPlotMean())
		{
			variables_[i]->drawMean(renderQueue);
		}
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void LineVariable::updateRenderCommand()
{
	valuesCmd_.material().setTextureGLId(0);
	valuesCmd_.material().setColor(graphColor_);
//	valuesCmd_.transformation().setPosition(absPosition().x, absPosition().y);
	valuesCmd_.geometry().setData(GL_LINE_STRIP, 2, variable_.numValues(), vertices_, NULL, NULL);
	valuesCmd_.calculateSortKey();
}

void LineVariable::updateMeanRenderCommand()
{
	meanCmd_.material().setTextureGLId(0);
	meanCmd_.material().setColor(meanColor_);
//	meanCmd_.transformation().setPosition(absPosition().x, absPosition().y);
	meanCmd_.geometry().setData(GL_LINES, 0, 2, vertices_, NULL, NULL);
	meanCmd_.calculateSortKey();
}

}
