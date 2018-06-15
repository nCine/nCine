#include "RenderCommand.h"
#include "GLShaderProgram.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

RenderCommand::RenderCommand(CommandTypes::Enum profilingType)
	: sortKey_(0), layer_(BottomLayer), numInstances_(0), batchSize_(0),
	  uniformBlocksCommitted_(false), verticesCommitted_(false), indicesCommitted_(false),
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
	if (geometry_.numVertices_ == 0)
		return;

	material_.bind();
	commitTransformation();
	material_.commitUniforms();
	commitUniformBlocks();
	uniformBlocksCommitted_ = false;

	commitVertices();
	verticesCommitted_ = false;
	commitIndices();
	indicesCommitted_ = false;

	unsigned int offset = 0;
#if defined(__ANDROID__) && !GL_ES_VERSION_3_2
	/// Simulating missing `glDrawElementsBaseVertex()` on OpenGL ES 3.0
	if (geometry_.numIndices_ > 0)
		offset = geometry_.vboParams_.offset + (geometry_.firstVertex_ * geometry_.numElementsPerVertex_ * sizeof(GLfloat));
#endif
	material_.defineVertexFormat(geometry_.vboParams_.object, geometry_.iboParams_.object, offset);
	geometry_.bind();
	geometry_.draw(numInstances_);
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
		const Material::ShaderProgramType shaderProgramType = material_.shaderProgramType();

		if (shaderProgramType == Material::ShaderProgramType::SPRITE)
			material_.uniformBlock("SpriteBlock")->uniform("modelView")->setFloatVector(modelView_.data());
		else if (shaderProgramType == Material::ShaderProgramType::MESH_SPRITE)
			material_.uniformBlock("MeshSpriteBlock")->uniform("modelView")->setFloatVector(modelView_.data());
		else if (shaderProgramType == Material::ShaderProgramType::COLOR)
			material_.uniformBlock("ColorBlock")->uniform("modelView")->setFloatVector(modelView_.data());
		else if (shaderProgramType == Material::ShaderProgramType::TEXTNODE_GRAY ||
		         shaderProgramType == Material::ShaderProgramType::TEXTNODE_COLOR)
			material_.uniformBlock("TextnodeBlock")->uniform("modelView")->setFloatVector(modelView_.data());
		else if (shaderProgramType != Material::ShaderProgramType::BATCHED_SPRITES &&
		         shaderProgramType != Material::ShaderProgramType::BATCHED_MESH_SPRITES &&
		         shaderProgramType != Material::ShaderProgramType::BATCHED_TEXTNODES_GRAY &&
		         shaderProgramType != Material::ShaderProgramType::BATCHED_TEXTNODES_COLOR)
			material_.uniform("modelView")->setFloatVector(modelView_.data());
	}
}

void RenderCommand::commitVertices()
{
	if (verticesCommitted_ == false)
	{
		geometry_.commitVertices();
		verticesCommitted_ = true;
	}
}

void RenderCommand::commitIndices()
{
	if (indicesCommitted_ == false)
	{
		geometry_.commitIndices();
		indicesCommitted_ = true;
	}
}

}
