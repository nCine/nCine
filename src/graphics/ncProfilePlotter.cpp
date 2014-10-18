#include "ncProfilePlotter.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncProfilePlotter::ncProfilePlotter(ncSceneNode* parent, ncRect rect)
	: ncDrawableNode(parent, rect.x, rect.y), width_(rect.w), height_(rect.h), variables_(2),
	  shouldPlotRefValue_(false), refValueColor_(1.0f, 1.0f, 1.0f), refValue_(0.0f)
{
	setPriority(ncDrawableNode::HUD_PRIORITY);

	// One more than variable mean lines
	refValueCmd_.setPriority(ncDrawableNode::HUD_PRIORITY + 3);
	refValueCmd_.setType(ncRenderCommand::PLOTTER_TYPE);
}

ncProfilePlotter::~ncProfilePlotter()
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
bool ncProfilePlotter::addValue(unsigned int varIndex, float value)
{
	bool valueRegistered = false;

	if (varIndex < variables_.size())
	{
		valueRegistered = variables_[varIndex]->addValue(value);
	}

	return valueRegistered;
}

/// Returns the variable with the specified index
ncPlottingVariable& ncProfilePlotter::variable(unsigned int index)
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
float ncProfilePlotter::normBetweenRefValue(float min, float max) const
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

/// Applies parent transformations to reference value vertices
void ncProfilePlotter::applyTransformations(float absX, float absY, float absRotation, float absScaleFactor)
{
	// Variable values geometry
	ncRenderGeometry &geometry = refValueCmd_.geometry();

	float sine = 0.0f;
	float cosine = 1.0f;
	if (abs(absRotation) > ncDrawableNode::MinRotation && abs(absRotation) < 360.0f - ncDrawableNode::MinRotation)
	{
		sine = sinf(-absRotation * M_PI / 180.0f);
		cosine = cosf(-absRotation * M_PI / 180.0f);
	}

	for (int i = geometry.firstVertex(); i < geometry.numVertices(); i++)
	{
		float x = geometry.vertexPointer()[2 * i] * absScaleFactor;			float y = geometry.vertexPointer()[2 * i + 1] * absScaleFactor;
		geometry.vertexPointer()[2 * i] = absX + x * cosine - y * sine;		geometry.vertexPointer()[2 * i + 1] = absY + y * cosine + x * sine;
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Fill the background buffer with vertices
void ncProfilePlotter::setBackgroundVertices()
{
	// Graph background vertices
	backgroundVertices_[0] = 0;				backgroundVertices_[1] = 0;
	backgroundVertices_[2] = width_;		backgroundVertices_[3] = 0;
	backgroundVertices_[4] = 0;				backgroundVertices_[5] = height_;

	backgroundVertices_[6] = width_;		backgroundVertices_[7] = height_;
}

void ncProfilePlotter::updateRenderCommand()
{
	renderCommand_.material().setTextureGLId(0);
	renderCommand_.material().setColor(backgroundColor_);
	renderCommand_.transformation().setPosition(absPosition().x, absPosition().y);
	setBackgroundVertices();
	renderCommand_.geometry().setData(GL_TRIANGLE_STRIP, 0, 4, backgroundVertices_, NULL, NULL);
	renderCommand_.calculateSortKey();

	ncDrawableNode::applyTransformations();
}

/// Updates the reference value rendering command
void ncProfilePlotter::UpdateRefValueRenderCommand()
{
	refValueCmd_.material().setTextureGLId(0);
	refValueCmd_.material().setColor(refValueColor_);
//	refValueCmd_.transformation().setPosition(absPosition().x, absPosition().y);
	refValueCmd_.geometry().setData(GL_LINES, 0, 2, refValueVertices_, NULL, NULL);
	refValueCmd_.calculateSortKey();
}
