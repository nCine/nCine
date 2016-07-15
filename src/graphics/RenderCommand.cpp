#include "RenderCommand.h"
#include "GLShaderProgram.h"
#include "Matrix4x4.h" // TODO: Remove dependency
#include "Application.h" // TODO: Remove dependency

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

RenderCommand::RenderCommand()
	: sortKey_(0), layer_(BOTTOM_LAYER), profilingType_(GENERIC_TYPE),
	  modelView_(Matrix4x4f::Identity)
{

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void RenderCommand::calculateSortKey()
{
	unsigned long int upper = layer_ << 16;
	unsigned int lower = material_.sortKey();
	sortKey_ = upper + lower;
}

void RenderCommand::issue()
{
	geometry_.bind();

	material_.bind();
	setTransformation();
	material_.commitUniforms();
	material_.defineVertexPointers(geometry_.vboHandle());

	draw();
}

void RenderCommand::setVertexAttribute(const char *name, GLsizei vboStride, const GLvoid *vboPointer)
{
	GLVertexAttribute *vertexAttribute = material_.attribute(name);
	vertexAttribute->setVboParameters(vboStride, vboPointer);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void RenderCommand::setTransformation()
{
	float width = static_cast<float>(theApplication().width());
	float height = static_cast<float>(theApplication().height());
	float near = -1.0f;
	float far = 1.0f;

	// TODO: Projection is hard-coded, should go in a camera class (Y-axis points downward)
	Matrix4x4f projection = Matrix4x4f::ortho(0.0f, width, 0.0f, height, near, far);

	// The layer translates to depth, from near to far
	float layerStep = 1.0f / static_cast<float>(TOP_LAYER);
	modelView_[3][2] = near + layerStep + (far - near - layerStep) * (layer_ * layerStep);

	if (material_.shaderProgram_)
	{
		material_.uniform("modelView")->setFloatVector(modelView_.data());
		material_.uniform("projection")->setFloatVector(projection.data());
	}
}

void RenderCommand::draw()
{
	if (geometry_.ibo_)
	{
		glDrawElements(geometry_.primitiveType_, geometry_.numVertices_, GL_UNSIGNED_SHORT, 0);
	}
	else
	{
		glDrawArrays(geometry_.primitiveType_, geometry_.firstVertex_, geometry_.numVertices_);
	}
}

}
