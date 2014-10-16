#include "ncStackedBarPlotter.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncStackedBarVariable::ncStackedBarVariable(unsigned int numValues, float rejectDelay)
	: ncPlottingVariable(numValues, rejectDelay)
{
	// Two vertices for the mean quote plus...
	// Six vertices (two coordinates each) for every recorded value
	vertices_ = new GLfloat[4 + numValues * 2 * 6];
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

unsigned int ncStackedBarPlotter::addVariable(unsigned int numValues, float rejectDelay)
{
	if (!variables_.isEmpty())
	{
		if (numValues != variables_[0]->variable()->numValues())
		{
			ncServiceLocator::logger().write(ncILogger::LOG_WARN, (const char *)"ncStackedBarPlotter::addVariable - Variable not added because number of values is inconsistent");
			return 0; // TODO: switch to signed int and return -1?
		}
		float diff = rejectDelay - variables_[0]->variable()->delay();
		if (diff < -0.001f || diff > 0.001f) // HACK: one millisecond fixed tolerance
		{
			ncServiceLocator::logger().write(ncILogger::LOG_WARN, (const char *)"ncStackedBarPlotter::addVariable - Variable not added because reject delay is inconsistent");
			return 0; // TODO: switch to signed int and return -1?
		}
	}

	ncStackedBarVariable* variable = new ncStackedBarVariable(numValues, rejectDelay);
	variables_.insertBack(variable);

	return variables_.size() - 1;
}

void ncStackedBarPlotter::draw(ncRenderQueue& renderQueue)
{
	// Drawing the background
	ncDrawableNode::draw(renderQueue);

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


/// Fill the buffer of every stacked variable with vertices
void ncStackedBarPlotter::updateAllVertices(int x, int y, int w, int h)
{
	unsigned int numVariables = variables_.size();
	float scaledH = h / numVariables;

	// In a stacked plot every variable should be scaled in relationship with the sum
	float minSum = 0.0f;
	float maxSum = 0.0f;
	for (unsigned int i = 0; i < variables_.size(); i++)
	{
		minSum += variables_[i]->variable()->min();
		maxSum += variables_[i]->variable()->max();
	}

	float normalizedRefValue = normBetweenRefValue(minSum, maxSum);
	refValueVertices_[0] = x;		refValueVertices_[1] = y + h * normalizedRefValue;
	refValueVertices_[2] = x + w;	refValueVertices_[3] = y + h * normalizedRefValue;

	// Each variable should be normalized against its "slice" of the sum
	minSum /= numVariables;
	maxSum /= numVariables;

	float meanVerticalOffset = 0.0f;
	for (unsigned int i = 0; i < variables_.size(); i++)
	{
		const ncProfileVariable* profVariable = variables_[i]->variable();
		GLfloat* vertices = variables_[i]->vertices();

		float normalizedMean = profVariable->normBetweenMean(minSum, maxSum);
		// Variable mean vertices
		vertices[0] = x;			vertices[1] = y + scaledH * (meanVerticalOffset + normalizedMean);
		vertices[2] = x + w;		vertices[3] = y + scaledH * (meanVerticalOffset + normalizedMean);

		meanVerticalOffset += normalizedMean;
	}

	unsigned int numValues = 0;
	unsigned int nextIndex = 0;
	if (!variables_.isEmpty())
	{
		// Common values among every variable (enforced in ncStackedBarPlotter::addVariable)
		numValues = variables_[0]->variable()->numValues();
		nextIndex = variables_[0]->variable()->nextIndex();
	}
	for (unsigned int i = 0; i < numValues; i++)
	{
		float verticalOffset = 0.0f;
		float step = (float(w) / float(numValues)) * 0.5f;
		float center = 2.0f * step * (i + 1) - step;

		for (unsigned int j = 0; j < variables_.size(); j++)
		{
			const ncProfileVariable* profVariable = variables_[j]->variable();
			GLfloat* vertices = variables_[j]->vertices();

			float normValue = profVariable->normBetweenValue((nextIndex + i) % numValues, minSum, maxSum);

			vertices[4 + 12 * i + 0] = x + center - step;
			vertices[4 + 12 * i + 1] = y + scaledH * verticalOffset;
			vertices[4 + 12 * i + 2] = x + center + step;
			vertices[4 + 12 * i + 3] = y + scaledH * verticalOffset;
			vertices[4 + 12 * i + 4] = x + center - step;
			vertices[4 + 12 * i + 5] = y + scaledH * (verticalOffset + normValue);

			vertices[4 + 12 * i + 6] = x + center + step;
			vertices[4 + 12 * i + 7] = y + scaledH * verticalOffset;
			vertices[4 + 12 * i + 8] = x + center + step;
			vertices[4 + 12 * i + 9] = y + scaledH * (verticalOffset + normValue);
			vertices[4 + 12 * i + 10] = x + center - step;
			vertices[4 + 12 * i + 11] = y + scaledH * (verticalOffset + normValue);

			verticalOffset += normValue;
		}
	}
}

void ncStackedBarVariable::updateRenderCommand()
{
	valuesCmd_.material().setTextureGLId(0);
	valuesCmd_.material().setColor(graphColor_);
//	valuesCmd_.transformation().setPosition(absPosition().x, absPosition().y);
	valuesCmd_.geometry().setData(GL_TRIANGLES, 2, variable_.numValues() * 6, vertices_, NULL, NULL);
	valuesCmd_.calculateSortKey();
}

void ncStackedBarVariable::updateMeanRenderCommand()
{
	meanCmd_.material().setTextureGLId(0);
	meanCmd_.material().setColor(meanColor_);
//	meanCmd_.transformation().setPosition(absPosition().x, absPosition().y);
	meanCmd_.geometry().setData(GL_LINES, 0, 2, vertices_, NULL, NULL);
	meanCmd_.calculateSortKey();
}

