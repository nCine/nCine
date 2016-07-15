#include "StackedBarPlotter.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

StackedBarVariable::StackedBarVariable(unsigned int numValues, float rejectDelay, const Matrix4x4f &worldMatrix)
	: PlottingVariable(numValues, rejectDelay, worldMatrix)
{
	// Two vertices for the mean quote plus...
	// Six vertices (two coordinates each) for every recorded value
	vertices_ = new GLfloat[4 + numValues * 2 * 6];

	valuesCmd_.material().setShaderProgram(Material::COLOR_PROGRAM);
	valuesCmd_.geometry().createCustomVbo(4 + numValues * 2 * 6, GL_DYNAMIC_DRAW);
	valuesCmd_.geometry().setDrawParameters(GL_TRIANGLES, 2, variable_.numValues() * 6);

	meanCmd_.material().setShaderProgram(Material::COLOR_PROGRAM);
	meanCmd_.geometry().shareVbo(valuesCmd_.geometry());
	meanCmd_.geometry().setDrawParameters(GL_LINES, 0, 2);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

unsigned int StackedBarPlotter::addVariable(unsigned int numValues, float rejectDelay)
{
	if (!variables_.isEmpty())
	{
		if (numValues != variables_[0]->variable()->numValues())
		{
			LOGW("Variable not added because number of values is inconsistent");
			return 0; // TODO: switch to signed int and return -1?
		}
		float diff = rejectDelay - variables_[0]->variable()->delay();
		if (diff < -0.001f || diff > 0.001f) // HACK: one millisecond fixed tolerance
		{
			LOGW("Variable not added because reject delay is inconsistent");
			return 0; // TODO: switch to signed int and return -1?
		}
	}

	StackedBarVariable *variable = new StackedBarVariable(numValues, rejectDelay, worldMatrix_);
	variables_.pushBack(variable);

	return variables_.size() - 1;
}

void StackedBarPlotter::draw(RenderQueue &renderQueue)
{
	// Drawing the background
	DrawableNode::draw(renderQueue);

	updateAllVertices(0.0f, 0.0f, width_, height_);

	// Drawing the reference value line
	if (shouldPlotRefValue())
	{
		drawRefValue(renderQueue);
	}

	for (unsigned int i = 0; i < variables_.size(); i++)
	{
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

void StackedBarPlotter::updateAllVertices(float x, float y, float w, float h)
{
	unsigned int numVariables = variables_.size();
	float scaledH = h / static_cast<float>(numVariables);

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
		const ProfileVariable *profVariable = variables_[i]->variable();
		GLfloat *vertices = variables_[i]->vertices();

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
		// Common values among every variable (enforced in StackedBarPlotter::addVariable)
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
			const ProfileVariable *profVariable = variables_[j]->variable();
			GLfloat *vertices = variables_[j]->vertices();

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

void StackedBarVariable::updateRenderCommand()
{
	valuesCmd_.transformation() = worldMatrix_;

	valuesCmd_.material().setTexture(NULL);
	valuesCmd_.material().uniform("color")->setFloatValue(graphColor_.fR(), graphColor_.fG(), graphColor_.fB(), graphColor_.fA());
	valuesCmd_.geometry().updateVboData(4, variable_.numValues() * 6 * 2, vertices_ + 4);
}

void StackedBarVariable::updateMeanRenderCommand()
{
	meanCmd_.transformation() = worldMatrix_;

	meanCmd_.material().setTexture(NULL);
	meanCmd_.material().uniform("color")->setFloatValue(meanColor_.fR(), meanColor_.fG(), meanColor_.fB(), meanColor_.fA());
	meanCmd_.geometry().updateVboData(0, 4, vertices_);
}

}
