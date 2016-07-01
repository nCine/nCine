#include "PlottingVariable.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

PlottingVariable::PlottingVariable(unsigned int numValues, float rejectDelay, const Matrix4x4f& worldMatrix)
	: shouldPlotMean_(true), graphColor_(1.0f, 1.0f, 1.0f), meanColor_(1.0f, 0.0f, 0.0f),
	  variable_(numValues, rejectDelay), vertices_(NULL), worldMatrix_(worldMatrix)
{
	// One more than the profile plotter background
	valuesCmd_.setLayer(DrawableNode::HUD_LAYER + 1);
	// One more than the variable graph
	meanCmd_.setLayer(DrawableNode::HUD_LAYER + 2);

	valuesCmd_.setType(RenderCommand::PLOTTER_TYPE);
	meanCmd_.setType(RenderCommand::PLOTTER_TYPE);

	valuesCmd_.material().setShaderProgram(Material::COLOR_PROGRAM);
	meanCmd_.material().setShaderProgram(Material::COLOR_PROGRAM);
}

PlottingVariable::~PlottingVariable()
{
	if (vertices_)
	{
		delete[] vertices_;
	}
}

}
