#include "ProfilePlotter.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ProfilePlotter::ProfilePlotter(SceneNode* parent, Rect rect)
	: DrawableNode(parent, rect.x, rect.y), width_(rect.w), height_(rect.h), variables_(2),
	  shouldPlotRefValue_(false), refValueColor_(1.0f, 1.0f, 1.0f), refValue_(0.0f)
{
	setPriority(DrawableNode::HUD_PRIORITY);
	renderCommand_->material().setShaderProgram(Material::COLOR_PROGRAM);
	renderCommand_->geometry().createCustomVbo(8, GL_STATIC_DRAW);
	renderCommand_->geometry().setDrawParameters(GL_TRIANGLE_STRIP, 0, 4);
	setBackgroundVertices();
	renderCommand_->geometry().updateVboData(0, 8, backgroundVertices_.data());

	// Priority is one more than variable mean lines
	refValueCmd_.setPriority(DrawableNode::HUD_PRIORITY + 3);
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

/// Adds a value to the specified variable
bool ProfilePlotter::addValue(unsigned int varIndex, float value)
{
	bool valueRegistered = false;

	if (varIndex < variables_.size())
	{
		valueRegistered = variables_[varIndex]->addValue(value);
	}

	return valueRegistered;
}

/// Returns the variable with the specified index
PlottingVariable& ProfilePlotter::variable(unsigned int index)
{
	if (index < variables_.size())
	{
		return *variables_[index];
	}
	else
	{
		LOGF("Index out of range");
		exit(EXIT_FAILURE);
	}
}

/// Returns the reference value normalized and clamped between the two numbers provided
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

/// Fill the background buffer with vertices
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

/// Updates the reference value rendering command
void ProfilePlotter::UpdateRefValueRenderCommand()
{
	refValueCmd_.transformation() = worldMatrix_;

	refValueCmd_.material().setTexture(NULL);
	refValueCmd_.material().uniform("color")->setFloatValue(refValueColor_.fR(), refValueColor_.fG(), refValueColor_.fB(), refValueColor_.fA());
	refValueCmd_.geometry().updateVboData(0, 4, refValueVertices_.data());
}

}
