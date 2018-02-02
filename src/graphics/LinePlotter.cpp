#include "LinePlotter.h"
#include "RenderQueue.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

LineVariable::LineVariable(unsigned int numValues, float rejectDelay, const Matrix4x4f &worldMatrix)
	: PlottingVariable(numValues, rejectDelay, worldMatrix)
{
	// Two vertices for the mean quote plus...
	// One vertex (2 coordinates each) for every recorded value
	vertices_ = new GLfloat[4 + numValues * 2];

	valuesCmd_.material().setShaderProgram(Material::ShaderProgram::COLOR);
	valuesCmd_.geometry().createCustomVbo(4 + numValues * 2, GL_DYNAMIC_DRAW);
	valuesCmd_.geometry().setDrawParameters(GL_LINE_STRIP, 2, variable_.numValues());

	meanCmd_.material().setShaderProgram(Material::ShaderProgram::COLOR);
	meanCmd_.geometry().shareVbo(valuesCmd_.geometry());
	meanCmd_.geometry().setDrawParameters(GL_LINES, 0, 2);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

unsigned int LinePlotter::addVariable(unsigned int numValues, float rejectDelay)
{
	LineVariable *variable = new LineVariable(numValues, rejectDelay, worldMatrix_);
	variables_.pushBack(variable);

	return variables_.size() - 1;
}

void LinePlotter::draw(RenderQueue &renderQueue)
{
	// Drawing the background
	DrawableNode::draw(renderQueue);

	updateAllVertices(0.0f, 0.0f, width_, height_);

	// Drawing the reference value line
	if (shouldPlotRefValue())
		drawRefValue(renderQueue);

	for (PlottingVariable *variable : variables_)
	{
		variable->draw(renderQueue);
		if (variable->shouldPlotMean())
			variable->drawMean(renderQueue);
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void LinePlotter::updateAllVertices(float x, float y, float w, float h)
{
	float commonMin = 0.0f;
	float commonMax = 0.0f;
	if (!variables_.isEmpty())
	{
		commonMin = variables_[0]->variable()->min();
		commonMax = variables_[0]->variable()->max();
	}

	for (PlottingVariable *variable : variables_)
	{
		if (variable->variable()->min() < commonMin)
			commonMin = variable->variable()->min();
		else if (variable->variable()->max() > commonMax)
			commonMax = variable->variable()->max();
	}

	const float normalizedRefValue = normBetweenRefValue(commonMin, commonMax);
	refValueVertices_[0] = x;		refValueVertices_[1] = y + h * normalizedRefValue;
	refValueVertices_[2] = x + w;	refValueVertices_[3] = y + h * normalizedRefValue;

	for (PlottingVariable *variable : variables_)
	{
		const ProfileVariable *profVariable = variable->variable();
		GLfloat *vertices = variable->vertices();

		const float normalizedMean = profVariable->normBetweenMean(commonMin, commonMax);
		// Variable mean vertices
		vertices[0] = x;			vertices[1] = y + h * normalizedMean;
		vertices[2] = x + w;		vertices[3] = y + h * normalizedMean;

		const unsigned int numValues = profVariable->numValues();
		const unsigned int nextIndex = profVariable->nextIndex();

		// Variable value vertices
		for (unsigned int j = 0; j < numValues; j++)
		{
			vertices[4 + 2 * j] = x + j * w / (numValues - 1);
			vertices[4 + 2 * j + 1] = y + h * profVariable->normBetweenValue((nextIndex + j) % numValues, commonMin, commonMax);
		}
	}
}

void LineVariable::updateRenderCommand()
{
	valuesCmd_.transformation() = worldMatrix_;

	valuesCmd_.material().setTexture(nullptr);
	valuesCmd_.material().uniform("color")->setFloatValue(graphColor_.fR(), graphColor_.fG(), graphColor_.fB(), graphColor_.fA());
	valuesCmd_.geometry().updateVboData(4, variable_.numValues() * 2, vertices_ + 4);
}

void LineVariable::updateMeanRenderCommand()
{
	meanCmd_.transformation() = worldMatrix_;

	meanCmd_.material().setTexture(nullptr);
	meanCmd_.material().uniform("color")->setFloatValue(meanColor_.fR(), meanColor_.fG(), meanColor_.fB(), meanColor_.fA());
	meanCmd_.geometry().updateVboData(0, 4, vertices_);
}

}
