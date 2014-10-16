#include "ncPlottingVariable.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncPlottingVariable::ncPlottingVariable(unsigned int numValues, float rejectDelay)
	: shouldPlotMean_(true), graphColor_(1.0f, 1.0f, 1.0f), meanColor_(1.0f, 0.0f, 0.0f),
	  variable_(numValues, rejectDelay), vertices_(NULL)
{
	// One more than the profile plotter background
	valuesCmd_.setPriority(ncDrawableNode::HUD_PRIORITY + 1);
	// One more than the variable graph
	meanCmd_.setPriority(ncDrawableNode::HUD_PRIORITY + 2);

	valuesCmd_.setType(ncRenderCommand::PLOTTER_TYPE);
	meanCmd_.setType(ncRenderCommand::PLOTTER_TYPE);
}

ncPlottingVariable::~ncPlottingVariable()
{
	if (vertices_)
	{
		delete[] vertices_;
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Applies parent transformations to both mean and values vertices
void ncPlottingVariable::applyTransformations(float absX, float absY, float absRotation, float absScaleFactor)
{
	// Variable values geometry
	ncRenderGeometry &geometry = valuesCmd_.geometry();

	float sine = 0.0f;
	float cosine = 1.0f;
	if (abs(absRotation) > ncDrawableNode::MinRotation && abs(absRotation) < 360.0f - ncDrawableNode::MinRotation)
	{
		sine = sinf(-absRotation * M_PI / 180.0f);
		cosine = cosf(-absRotation * M_PI / 180.0f);
	}

	// Total number of vertices comprises both mean and variable vertices
	int numVertices = geometry.numVertices() + meanCmd_.geometry().numVertices();
	// startingVertex is used to transform all the values if the mean is not rendered
	int startingVertex = geometry.firstVertex() - meanCmd_.geometry().numVertices();
	for (int i = startingVertex; i < startingVertex + numVertices; i++)
	{
		float x = geometry.vertexPointer()[2 * i] * absScaleFactor;			float y = geometry.vertexPointer()[2 * i + 1] * absScaleFactor;
		geometry.vertexPointer()[2 * i] = absX + x * cosine - y * sine;		geometry.vertexPointer()[2 * i + 1] = absY + y * cosine + x * sine;
	}
}
