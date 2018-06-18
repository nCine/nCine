#include "StackedBarPlotter.h"
#include "GLDebug.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

StackedBarVariable::StackedBarVariable(unsigned int numValues, float rejectDelay, const Matrix4x4f &worldMatrix)
	: PlottingVariable(numValues, rejectDelay, worldMatrix)
{
	valuesCmd_.material().setShaderProgramType(Material::ShaderProgramType::COLOR);
	valuesCmd_.geometry().setDrawParameters(GL_TRIANGLES, 2, variable_.numValues() * 6);

	meanCmd_.material().setShaderProgramType(Material::ShaderProgramType::COLOR);
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
		const float diff = rejectDelay - variables_[0]->variable()->delay();
		if (diff < -0.001f || diff > 0.001f) // HACK: one millisecond fixed tolerance
		{
			LOGW("Variable not added because reject delay is inconsistent");
			return 0; // TODO: switch to signed int and return -1?
		}
	}

	variables_.pushBack(nctl::makeUnique<StackedBarVariable>(numValues, rejectDelay, worldMatrix_));
	return variables_.size() - 1;
}

void StackedBarPlotter::draw(RenderQueue &renderQueue)
{
	// Drawing the background
	DrawableNode::draw(renderQueue);

	updateAllVertices(0.0f, 0.0f, width_, height_);

	// Drawing the reference value line
	if (shouldPlotRefValue())
		drawRefValue(renderQueue);

	for (nctl::UniquePtr<PlottingVariable> &variable : variables_)
	{
		variable->draw(renderQueue);
		if (variable->shouldPlotMean())
			variable->drawMean(renderQueue);
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void StackedBarPlotter::updateAllVertices(float x, float y, float w, float h)
{
	GLDebug::ScopedGroup scoped("StackedBarPlotter vertices");

	const unsigned int numVariables = variables_.size();
	const float scaledH = h / static_cast<float>(numVariables);

	// In a stacked plot every variable should be scaled in relationship with the sum
	float minSum = 0.0f;
	float maxSum = 0.0f;
	for (nctl::UniquePtr<PlottingVariable> &variable : variables_)
	{
		minSum += variable->variable()->min();
		maxSum += variable->variable()->max();
	}

	const float normalizedRefValue = normBetweenRefValue(minSum, maxSum);
	GLfloat *refValueVertices = refValueCmd_.geometry().acquireVertexPointer(4);
	refValueVertices[0] = x;			refValueVertices[1] = y + h * normalizedRefValue;
	refValueVertices[2] = x + w;		refValueVertices[3] = y + h * normalizedRefValue;
	refValueCmd_.geometry().releaseVertexPointer();

	// Each variable should be normalized against its "slice" of the sum
	minSum /= numVariables;
	maxSum /= numVariables;

	unsigned int numValues = 0;
	unsigned int nextIndex = 0;
	if (!variables_.isEmpty())
	{
		// Common values among every variable (enforced in StackedBarPlotter::addVariable)
		numValues = variables_[0]->variable()->numValues();
		nextIndex = variables_[0]->variable()->nextIndex();
	}

	const unsigned int maxVerticalOffsets = 50;
	ASSERT(maxVerticalOffsets >= numValues);
	float verticalOffsets[maxVerticalOffsets];
	for (unsigned int i = 0; i < maxVerticalOffsets; i++)
		verticalOffsets[i] = 0.0f;

	float meanVerticalOffset = 0.0f;
	for (nctl::UniquePtr<PlottingVariable> &variable : variables_)
	{
		GLDebug::ScopedGroup scoped("StackedBarPlotter variable vertices");

		const ProfileVariable *profVariable = variable->variable();
		// 2 vertices for the mean quote plus 6 vertices (2 coordinates each) for every recorded value
		GLfloat *vertices = variable->acquireVertices(4 + numValues * 2 * 6);

		float normalizedMean = profVariable->normBetweenMean(minSum, maxSum);
		// Variable mean vertices
		vertices[0] = x;			vertices[1] = y + scaledH * (meanVerticalOffset + normalizedMean);
		vertices[2] = x + w;		vertices[3] = y + scaledH * (meanVerticalOffset + normalizedMean);
		meanVerticalOffset += normalizedMean;

		const float step = (float(w) / float(numValues)) * 0.5f;
		for (unsigned int i = 0; i < numValues; i++)
		{
			const float center = 2.0f * step * (i + 1) - step;
			const float normValue = profVariable->normBetweenValue((nextIndex + i) % numValues, minSum, maxSum);

			vertices[4 + 12 * i + 0] = x + center - step;
			vertices[4 + 12 * i + 1] = y + scaledH * verticalOffsets[i];
			vertices[4 + 12 * i + 2] = x + center + step;
			vertices[4 + 12 * i + 3] = y + scaledH * verticalOffsets[i];
			vertices[4 + 12 * i + 4] = x + center - step;
			vertices[4 + 12 * i + 5] = y + scaledH * (verticalOffsets[i] + normValue);

			vertices[4 + 12 * i + 6] = x + center + step;
			vertices[4 + 12 * i + 7] = y + scaledH * verticalOffsets[i];
			vertices[4 + 12 * i + 8] = x + center + step;
			vertices[4 + 12 * i + 9] = y + scaledH * (verticalOffsets[i] + normValue);
			vertices[4 + 12 * i + 10] = x + center - step;
			vertices[4 + 12 * i + 11] = y + scaledH * (verticalOffsets[i] + normValue);

			verticalOffsets[i] += normValue;
		}

		variable->releaseVertices();
	}
}

void StackedBarVariable::updateRenderCommand()
{
	valuesCmd_.transformation() = worldMatrix_;
	valuesColorBlock_->uniform("color")->setFloatValue(graphColor_.fR(), graphColor_.fG(), graphColor_.fB(), graphColor_.fA());
}

void StackedBarVariable::updateMeanRenderCommand()
{
	meanCmd_.transformation() = worldMatrix_;
	meanColorBlock_->uniform("color")->setFloatValue(meanColor_.fR(), meanColor_.fG(), meanColor_.fB(), meanColor_.fA());
}

}
