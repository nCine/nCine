#include "common_macros.h"
#include "ProfilePlotter.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ProfilePlotter::ProfilePlotter(SceneNode *parent, Rectf rect)
	: DrawableNode(parent, rect.x, rect.y), variables_(2), shouldPlotRefValue_(false),
	  refValueColor_(1.0f, 1.0f, 1.0f), refValue_(0.0f)
{
	width_ = rect.w;
	height_ = rect.h;

	setLayer(DrawableNode::HUD_LAYER);
	renderCommand_->material().setShaderProgram(Material::COLOR_PROGRAM);
	renderCommand_->geometry().createCustomVbo(8, GL_STATIC_DRAW);
	renderCommand_->geometry().setDrawParameters(GL_TRIANGLE_STRIP, 0, 4);
	setBackgroundVertices();
	renderCommand_->geometry().updateVboData(0, 8, backgroundVertices_.data());

	// Priority is one more than variable mean lines
	refValueCmd_.setLayer(DrawableNode::HUD_LAYER + 3);
	refValueCmd_.setType(RenderCommand::PLOTTER_TYPE);
	refValueCmd_.material().setShaderProgram(Material::COLOR_PROGRAM);
	refValueCmd_.geometry().createCustomVbo(4, GL_DYNAMIC_DRAW);
	refValueCmd_.geometry().setDrawParameters(GL_LINES, 0, 2);
}

ProfilePlotter::~ProfilePlotter()
{
	for (unsigned int i = 0; i < variables_.size(); i++)
	{
		delete variables_[i];
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool ProfilePlotter::addValue(unsigned int varIndex, float value)
{
	bool valueRegistered = false;

	if (varIndex < variables_.size())
	{
		valueRegistered = variables_[varIndex]->addValue(value);
	}

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
		{
			value = 0.0f;
		}
		else if (refValue_ > max)
		{
			value = 1.0f;
		}
		else
		{
			value = (refValue_ - min) / (max - min);
		}
	}

	return value;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ProfilePlotter::setBackgroundVertices()
{
	// Graph background vertices
	backgroundVertices_[0] = 0.0f;			backgroundVertices_[1] = 0.0f;
	backgroundVertices_[2] = width_;		backgroundVertices_[3] = 0.0f;
	backgroundVertices_[4] = 0.0f;			backgroundVertices_[5] = height_;

	backgroundVertices_[6] = width_;		backgroundVertices_[7] = height_;
}

void ProfilePlotter::updateRenderCommand()
{
	renderCommand_->transformation() = worldMatrix_;

	renderCommand_->material().setTexture(NULL);
	renderCommand_->material().uniform("color")->setFloatValue(backgroundColor_.fR(), backgroundColor_.fG(), backgroundColor_.fB(), backgroundColor_.fA());
	bool isTransparent = backgroundColor_.a() < 255;
	renderCommand_->material().setTransparent(isTransparent);
}

void ProfilePlotter::UpdateRefValueRenderCommand()
{
	refValueCmd_.transformation() = worldMatrix_;

	refValueCmd_.material().setTexture(NULL);
	refValueCmd_.material().uniform("color")->setFloatValue(refValueColor_.fR(), refValueColor_.fG(), refValueColor_.fB(), refValueColor_.fA());
	refValueCmd_.geometry().updateVboData(0, 4, refValueVertices_.data());
}

}
