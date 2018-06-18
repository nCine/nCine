#include "common_macros.h"
#include "ProfilePlotter.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ProfilePlotter::ProfilePlotter(SceneNode *parent, Rectf rect)
	: DrawableNode(parent, rect.x, rect.y), variables_(2), shouldPlotRefValue_(false),
	  refValueColor_(1.0f, 1.0f, 1.0f), refValue_(0.0f),
	  backgroundColorBlock_(nullptr), refValueColorBlock_(nullptr)
{
	width_ = rect.w;
	height_ = rect.h;

	setLayer(DrawableNode::LayerBase::HUD);
	renderCommand_->setType(RenderCommand::CommandTypes::PLOTTER);
	renderCommand_->material().setShaderProgramType(Material::ShaderProgramType::COLOR);
	backgroundColorBlock_ = renderCommand_->material().uniformBlock("ColorBlock");
	renderCommand_->geometry().setDrawParameters(GL_TRIANGLE_STRIP, 0, 4);

	// Priority is one more than variable mean lines
	refValueCmd_.setLayer(DrawableNode::LayerBase::HUD + 3);
	refValueCmd_.setType(RenderCommand::CommandTypes::PLOTTER);
	refValueCmd_.material().setShaderProgramType(Material::ShaderProgramType::COLOR);
	refValueColorBlock_ = refValueCmd_.material().uniformBlock("ColorBlock");
	refValueCmd_.geometry().setDrawParameters(GL_LINES, 0, 2);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool ProfilePlotter::addValue(unsigned int varIndex, float value)
{
	bool valueRegistered = false;

	if (varIndex < variables_.size())
		valueRegistered = variables_[varIndex]->addValue(value);

	return valueRegistered;
}

PlottingVariable &ProfilePlotter::variable(unsigned int index)
{
	ASSERT_MSG_X(index < variables_.size(), "Index %u is out of bounds (size: %u)", index, variables_.size());
	return *variables_[index];
}

float ProfilePlotter::normBetweenRefValue(float min, float max) const
{
	float value = 0.0f;

	if (max - min > 0.0f)
	{
		if (refValue_ < min)
			value = 0.0f;
		else if (refValue_ > max)
			value = 1.0f;
		else
			value = (refValue_ - min) / (max - min);
	}

	return value;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ProfilePlotter::setBackgroundVertices(GLfloat *vertices)
{
	// Graph background vertices
	vertices[0] = 0.0f;			vertices[1] = 0.0f;
	vertices[2] = width_;		vertices[3] = 0.0f;
	vertices[4] = 0.0f;			vertices[5] = height_;

	vertices[6] = width_;		vertices[7] = height_;
}

void ProfilePlotter::updateRenderCommand()
{
	GLfloat *vertices = renderCommand_->geometry().acquireVertexPointer(8);
	setBackgroundVertices(vertices);
	renderCommand_->geometry().releaseVertexPointer();

	renderCommand_->transformation() = worldMatrix_;

	backgroundColorBlock_->uniform("color")->setFloatValue(backgroundColor_.fR(), backgroundColor_.fG(), backgroundColor_.fB(), backgroundColor_.fA());
	const bool isTransparent = backgroundColor_.a() < 255;
	renderCommand_->material().setTransparent(isTransparent);
}

void ProfilePlotter::updateRefValueRenderCommand()
{
	refValueCmd_.transformation() = worldMatrix_;
	refValueColorBlock_->uniform("color")->setFloatValue(refValueColor_.fR(), refValueColor_.fG(), refValueColor_.fB(), refValueColor_.fA());
}

}
