#include "PlottingVariable.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

PlottingVariable::PlottingVariable(unsigned int numValues, float rejectDelay, const Matrix4x4f &worldMatrix)
	: shouldPlotMean_(true), graphColor_(1.0f, 1.0f, 1.0f), meanColor_(1.0f, 0.0f, 0.0f),
	  variable_(numValues, rejectDelay), vertices_(nullptr), worldMatrix_(worldMatrix)
{
	// One more than the profile plotter background
	valuesCmd_.setLayer(DrawableNode::LayerBase::HUD + 1);
	// One more than the variable graph
	meanCmd_.setLayer(DrawableNode::LayerBase::HUD + 2);

	valuesCmd_.setType(RenderCommand::CommandType::PLOTTER);
	meanCmd_.setType(RenderCommand::CommandType::PLOTTER);

	valuesCmd_.material().setShaderProgram(Material::ShaderProgram::COLOR);
	meanCmd_.material().setShaderProgram(Material::ShaderProgram::COLOR);
}

PlottingVariable::~PlottingVariable()
{
	if (vertices_)
		delete[] vertices_;
}

}
