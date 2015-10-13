#include "RenderCommand.h"
#include "GLShaderProgram.h"
#include "Matrix4x4.h" // TODO: Remove dependency
#ifdef __ANDROID__
	#include "AndroidApplication.h" // TODO: Remove dependency
#else
	#include "Application.h" // TODO: Remove dependency
#endif

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

RenderCommand::RenderCommand()
	: sortKey_(0), priority_(0), profilingType_(GENERIC_TYPE),
	  modelView_(Matrix4x4f::Identity)
{

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Calculates a sort key for the queue
void RenderCommand::calculateSortKey()
{
	unsigned long int upper = priority_ << 16;
	unsigned int lower = material_.sortKey();
	sortKey_ = upper + lower;
}

/// Issues the render command
void RenderCommand::issue()
{
	geometry_.bind();

	material_.bind();
	setTransformation();
	material_.commitUniforms();
	material_.defineVertexPointers(geometry_.vboHandle());

	draw();
}

void RenderCommand::setVertexAttribute(const char* name, GLsizei vboStride, const GLvoid *vboPointer)
{
	GLVertexAttribute* vertexAttribute = material_.attribute(name);
	vertexAttribute->setVboParameters(vboStride, vboPointer);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void RenderCommand::setTransformation()
{
	float width = static_cast<float>(theApplication().width());
	float height = static_cast<float>(theApplication().height());

	// TODO: Projection is hard-coded, should go in a camera class (Y-axis points downward)
	Matrix4x4f projection = Matrix4x4f::ortho(0.0f, width, 0.0f, height, -1.0f, 1.0f);
	// Priority becomes depth
	modelView_[3][2] = priority_ / 1000.0f; // TODO: Check that is always inside the frustum

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
		glDrawElements(geometry_.drawType_, geometry_.numVertices_, GL_UNSIGNED_SHORT, 0);
	}
	else
	{
		glDrawArrays(geometry_.drawType_, geometry_.firstVertex_, geometry_.numVertices_);
	}
}

}
