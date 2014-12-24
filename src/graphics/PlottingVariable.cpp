#include "PlottingVariable.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

PlottingVariable::PlottingVariable(unsigned int numValues, float rejectDelay)
	: shouldPlotMean_(true), graphColor_(1.0f, 1.0f, 1.0f), meanColor_(1.0f, 0.0f, 0.0f),
	  variable_(numValues, rejectDelay), vertices_(NULL)
{
	// One more than the profile plotter background
	valuesCmd_.setPriority(DrawableNode::HUD_PRIORITY + 1);
	// One more than the variable graph
	meanCmd_.setPriority(DrawableNode::HUD_PRIORITY + 2);

	valuesCmd_.setType(RenderCommand::PLOTTER_TYPE);
	meanCmd_.setType(RenderCommand::PLOTTER_TYPE);
}

PlottingVariable::~PlottingVariable()
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
void PlottingVariable::applyTransformations(float absX, float absY, float absRotation, float absScaleFactor)
{
	// Variable values geometry
	RenderGeometry &geometry = valuesCmd_.geometry();

	float sine = 0.0f;
	float cosine = 1.0f;
	if (fabs(absRotation) > DrawableNode::MinRotation && fabs(absRotation) < 360.0f - DrawableNode::MinRotation)
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

}
