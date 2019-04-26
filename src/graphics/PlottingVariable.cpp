#include "PlottingVariable.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

PlottingVariable::PlottingVariable(unsigned int numValues, float rejectDelay, const Matrix4x4f &worldMatrix)
    : shouldPlotMean_(true), graphColor_(255, 255, 255), meanColor_(255, 255, 255),
      variable_(numValues, rejectDelay), worldMatrix_(worldMatrix),
      valuesColorBlock_(nullptr), meanColorBlock_(nullptr)
{
	// One more than the profile plotter background
	valuesCmd_.setLayer(DrawableNode::LayerBase::HUD + 1);
	// One more than the variable graph
	meanCmd_.setLayer(DrawableNode::LayerBase::HUD + 2);

	valuesCmd_.setType(RenderCommand::CommandTypes::PLOTTER);
	meanCmd_.setType(RenderCommand::CommandTypes::PLOTTER);

	valuesCmd_.material().setShaderProgramType(Material::ShaderProgramType::COLOR);
	meanCmd_.material().setShaderProgramType(Material::ShaderProgramType::COLOR);

	valuesColorBlock_ = valuesCmd_.material().uniformBlock("ColorBlock");
	meanColorBlock_ = meanCmd_.material().uniformBlock("ColorBlock");
}

}
