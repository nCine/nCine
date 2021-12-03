#include "RenderCommand.h"
#include "GLShaderProgram.h"
#include "GLScissorTest.h"
#include "DrawableNode.h"
#include "RenderResources.h"
#include "Camera.h"
#include "tracy.h"

namespace ncine {

namespace {

	bool isBatchedType(Material::ShaderProgramType type)
	{
		return (type == Material::ShaderProgramType::BATCHED_SPRITES ||
		        type == Material::ShaderProgramType::BATCHED_SPRITES_GRAY ||
		        type == Material::ShaderProgramType::BATCHED_SPRITES_NO_TEXTURE ||
		        type == Material::ShaderProgramType::BATCHED_MESH_SPRITES ||
		        type == Material::ShaderProgramType::BATCHED_MESH_SPRITES_GRAY ||
		        type == Material::ShaderProgramType::BATCHED_MESH_SPRITES_NO_TEXTURE ||
		        type == Material::ShaderProgramType::BATCHED_TEXTNODES_ALPHA ||
		        type == Material::ShaderProgramType::BATCHED_TEXTNODES_RED);
	}

}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

RenderCommand::RenderCommand(CommandTypes::Enum profilingType)
    : materialSortKey_(0), layer_(DrawableNode::LayerBase::LOWEST), numInstances_(0), batchSize_(0),
      uniformBlocksCommitted_(false), verticesCommitted_(false), indicesCommitted_(false),
      profilingType_(profilingType), modelMatrix_(Matrix4x4f::Identity)
{
}

RenderCommand::RenderCommand()
    : RenderCommand(CommandTypes::UNSPECIFIED)
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

void RenderCommand::calculateMaterialSortKey()
{
	const uint64_t upper = static_cast<uint64_t>(layer_) << 32;
	const uint32_t lower = material_.sortKey();
	materialSortKey_ = upper + lower;
}

void RenderCommand::issue()
{
	ZoneScoped;

	if (geometry_.numVertices_ == 0 && geometry_.numIndices_ == 0)
		return;

	material_.bind();
	material_.commitUniforms();

	uniformBlocksCommitted_ = false;
	verticesCommitted_ = false;
	indicesCommitted_ = false;

	GLScissorTest::State scissorTestState = GLScissorTest::state();
	if (scissorRect_.w > 0 && scissorRect_.h > 0)
		GLScissorTest::enable(scissorRect_);

	unsigned int offset = 0;
#if (defined(WITH_OPENGLES) && !GL_ES_VERSION_3_2) || defined(__EMSCRIPTEN__)
	// Simulating missing `glDrawElementsBaseVertex()` on OpenGL ES 3.0
	if (geometry_.numIndices_ > 0)
		offset = geometry_.vboParams().offset + (geometry_.firstVertex_ * geometry_.numElementsPerVertex_ * sizeof(GLfloat));
#endif
	material_.defineVertexFormat(geometry_.vboParams().object, geometry_.iboParams().object, offset);
	geometry_.bind();
	geometry_.draw(numInstances_);

	GLScissorTest::setState(scissorTestState);
}

void RenderCommand::setScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
	scissorRect_.set(x, y, width, height);
}

void RenderCommand::commitNodeTransformation()
{
	ZoneScoped;

	// The layer translates to depth, from near to far
	const Camera::ProjectionValues cameraValues = RenderResources::currentCamera()->projectionValues();
	const float layerStep = 1.0f / static_cast<float>(DrawableNode::LayerBase::HIGHEST);
	modelMatrix_[3][2] = cameraValues.near + layerStep + (cameraValues.far - cameraValues.near - layerStep) * (layer_ * layerStep);

	if (material_.shaderProgram_ && material_.shaderProgram_->status() == GLShaderProgram::Status::LINKED_WITH_INTROSPECTION)
	{
		ZoneScopedN("Set model matrix");
		const Material::ShaderProgramType shaderProgramType = material_.shaderProgramType();

		if (shaderProgramType == Material::ShaderProgramType::SPRITE ||
		    shaderProgramType == Material::ShaderProgramType::SPRITE_GRAY ||
		    shaderProgramType == Material::ShaderProgramType::SPRITE_NO_TEXTURE)
			material_.uniformBlock("SpriteBlock")->uniform("modelMatrix")->setFloatVector(modelMatrix_.data());
		else if (shaderProgramType == Material::ShaderProgramType::MESH_SPRITE ||
		         shaderProgramType == Material::ShaderProgramType::MESH_SPRITE_GRAY ||
		         shaderProgramType == Material::ShaderProgramType::MESH_SPRITE_NO_TEXTURE)
			material_.uniformBlock("MeshSpriteBlock")->uniform("modelMatrix")->setFloatVector(modelMatrix_.data());
		else if (shaderProgramType == Material::ShaderProgramType::TEXTNODE_ALPHA ||
		         shaderProgramType == Material::ShaderProgramType::TEXTNODE_RED)
			material_.uniformBlock("TextnodeBlock")->uniform("modelMatrix")->setFloatVector(modelMatrix_.data());
		else if (!isBatchedType(shaderProgramType) && shaderProgramType != Material::ShaderProgramType::CUSTOM)
			material_.uniform("modelMatrix")->setFloatVector(modelMatrix_.data());
	}
}

void RenderCommand::commitCameraTransformation()
{
	ZoneScopedN("Commit camera transformation");

	RenderResources::CameraUniformData *cameraUniformData = RenderResources::cameraUniformDataMap().find(material_.shaderProgram_);
	if (cameraUniformData == nullptr)
	{
		RenderResources::CameraUniformData newCameraUniformData;
		newCameraUniformData.shaderUniforms.setProgram(material_.shaderProgram_, "uProjectionMatrix\0uViewMatrix\0", nullptr);
		if (newCameraUniformData.shaderUniforms.numUniforms() == 2)
		{
			newCameraUniformData.shaderUniforms.setUniformsDataPointer(RenderResources::cameraUniformsBuffer());
			newCameraUniformData.shaderUniforms.uniform("uProjectionMatrix")->setDirty(true);
			newCameraUniformData.shaderUniforms.uniform("uViewMatrix")->setDirty(true);
			newCameraUniformData.shaderUniforms.commitUniforms();

			if (RenderResources::cameraUniformDataMap().loadFactor() >= 0.8f)
				RenderResources::cameraUniformDataMap().rehash(RenderResources::cameraUniformDataMap().capacity() * 2);
			RenderResources::cameraUniformDataMap().insert(material_.shaderProgram_, nctl::move(newCameraUniformData));
		}
	}
	else
		cameraUniformData->shaderUniforms.commitUniforms();
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

void RenderCommand::commitAll()
{
	commitVertices();
	commitIndices();
	commitNodeTransformation();
	commitUniformBlocks();
}

}
