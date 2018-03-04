#include "RenderCommand.h"
#include "GLShaderProgram.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

RenderCommand::RenderCommand(CommandTypes::Enum profilingType)
	: sortKey_(0), layer_(BottomLayer), numInstances_(0), uniformBlocksCommitted_(false),
	  profilingType_(profilingType), modelView_(Matrix4x4f::Identity)
{

}

RenderCommand::RenderCommand()
	: RenderCommand(CommandTypes::GENERIC)
{

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void RenderCommand::commitUniformBlocks()
{
	if (uniformBlocksCommitted_ == false)
	{
		material_.commitUniformBlocks();
		uniformBlocksCommitted_ = true;
	}
}

void RenderCommand::calculateSortKey()
{
	const unsigned long int upper = layer_ << 16;
	const unsigned int lower = material_.sortKey();
	sortKey_ = upper + lower;
}

void RenderCommand::issue()
{
	material_.bind();
	commitTransformation();
	material_.commitUniforms();
	commitUniformBlocks();
	uniformBlocksCommitted_ = false;

	geometry_.bind();
	material_.defineVertexPointers(geometry_.vboParams_.object);
	geometry_.draw(numInstances_);
}

void RenderCommand::setVertexAttribute(const char *name, GLsizei vboStride, const GLvoid *vboPointer)
{
	GLVertexAttribute *vertexAttribute = material_.attribute(name);
	vertexAttribute->setVboParameters(vboStride, vboPointer);
}

void RenderCommand::commitTransformation()
{
	// `near` and `far` planes should be consistent with the projection matrix
	const float near = -1.0f;
	const float far = 1.0f;

	// The layer translates to depth, from near to far
	const float layerStep = 1.0f / static_cast<float>(TopLayer);
	modelView_[3][2] = near + layerStep + (far - near - layerStep) * (layer_ * layerStep);

	if (material_.shaderProgram_ && material_.shaderProgram_->status() == GLShaderProgram::Status::LINKED_WITH_INTROSPECTION)
	{
		if (material_.shaderProgramType() == Material::ShaderProgramType::SPRITE)
			material_.uniformBlock("SpriteBlock")->uniform("modelView")->setFloatVector(modelView_.data());
		else if (material_.shaderProgramType() == Material::ShaderProgramType::COLOR)
			material_.uniformBlock("ColorBlock")->uniform("modelView")->setFloatVector(modelView_.data());
		else if (material_.shaderProgramType() == Material::ShaderProgramType::TEXTNODE_GRAY ||
		         material_.shaderProgramType() == Material::ShaderProgramType::TEXTNODE_COLOR)
			material_.uniformBlock("TextnodeBlock")->uniform("modelView")->setFloatVector(modelView_.data());
		else if (material_.shaderProgramType() != Material::ShaderProgramType::INSTANCED_SPRITES)
			material_.uniform("modelView")->setFloatVector(modelView_.data());
	}
}

}
