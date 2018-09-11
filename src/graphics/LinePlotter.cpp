#include "LinePlotter.h"
#include "GLDebug.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

LineVariable::LineVariable(unsigned int numValues, float rejectDelay, const Matrix4x4f &worldMatrix)
	: PlottingVariable(numValues, rejectDelay, worldMatrix)
{
	valuesCmd_.material().setShaderProgramType(Material::ShaderProgramType::COLOR);
	valuesCmd_.geometry().setDrawParameters(GL_LINE_STRIP, 2, variable_.numValues());

	meanCmd_.material().setShaderProgramType(Material::ShaderProgramType::COLOR);
	meanCmd_.geometry().shareVbo(&valuesCmd_.geometry());
	meanCmd_.geometry().setDrawParameters(GL_LINES, 0, 2);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

unsigned int LinePlotter::addVariable(unsigned int numValues, float rejectDelay)
{
	variables_.pushBack(nctl::makeUnique<LineVariable>(numValues, rejectDelay, worldMatrix_));
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

void LinePlotter::updateAllVertices(float x, float y, float w, float h)
{
	GLDebug::ScopedGroup scoped("LinePlotter vertices");

	float commonMin = 0.0f;
	float commonMax = 0.0f;
	if (!variables_.isEmpty())
	{
		commonMin = variables_[0]->variable()->min();
		commonMax = variables_[0]->variable()->max();
	}

	for (nctl::UniquePtr<PlottingVariable> &variable : variables_)
	{
		if (variable->variable()->min() < commonMin)
			commonMin = variable->variable()->min();
		else if (variable->variable()->max() > commonMax)
			commonMax = variable->variable()->max();
	}

	const float normalizedRefValue = normBetweenRefValue(commonMin, commonMax);
	GLfloat *refValueVertices = refValueCmd_.geometry().acquireVertexPointer(4);
	refValueVertices[0] = x;		refValueVertices[1] = y + h * normalizedRefValue;
	refValueVertices[2] = x + w;	refValueVertices[3] = y + h * normalizedRefValue;
	refValueCmd_.geometry().releaseVertexPointer();

	for (nctl::UniquePtr<PlottingVariable> &variable : variables_)
	{
		GLDebug::ScopedGroup scoped("LinePlotter variable vertices");

		const ProfileVariable *profVariable = variable->variable();
		const unsigned int numValues = profVariable->numValues();
		// 2 vertices for the mean quote plus 1 vertex (2 coordinates each) for every recorded value
		GLfloat *vertices = variable->acquireVertices(4 + numValues * 2);

		const float normalizedMean = profVariable->normBetweenMean(commonMin, commonMax);
		// Variable mean vertices
		vertices[0] = x;			vertices[1] = y + h * normalizedMean;
		vertices[2] = x + w;		vertices[3] = y + h * normalizedMean;

		const unsigned int nextIndex = profVariable->nextIndex();

		// Variable value vertices
		for (unsigned int j = 0; j < numValues; j++)
		{
			vertices[4 + 2 * j] = x + j * w / (numValues - 1);
			vertices[4 + 2 * j + 1] = y + h * profVariable->normBetweenValue((nextIndex + j) % numValues, commonMin, commonMax);
		}
		variable->releaseVertices();
	}
}

void LineVariable::updateRenderCommand()
{
	valuesCmd_.transformation() = worldMatrix_;
	valuesColorBlock_->uniform("color")->setFloatVector(Colorf(graphColor_).data());
}

void LineVariable::updateMeanRenderCommand()
{
	meanCmd_.transformation() = worldMatrix_;
	meanColorBlock_->uniform("color")->setFloatVector(Colorf(graphColor_).data());
}

}
