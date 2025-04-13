#include "RenderCommand.h"
#include "GLShaderProgram.h"
#include "GLScissorTest.h"
#include "RenderResources.h"
#include "Camera.h"
#include "DrawableNode.h"
#include "tracy.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

const float RenderCommand::LayerStep = 1.0f / static_cast<float>(0xFFFF);

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

RenderCommand::RenderCommand(CommandTypes::Enum type)
    : materialSortKey_(0), layer_(0),
      numInstances_(0), batchSize_(0), transformationCommitted_(false),
      type_(type), modelMatrix_(Matrix4x4f::Identity)
{
}

RenderCommand::RenderCommand()
    : RenderCommand(CommandTypes::UNSPECIFIED)
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void RenderCommand::calculateMaterialSortKey()
{
	const uint64_t upper = static_cast<uint64_t>(layerSortKey()) << 32;
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

void RenderCommand::setTransformation(const Matrix4x4f &modelMatrix)
{
	modelMatrix_ = modelMatrix;
	transformationCommitted_ = false;
}

void RenderCommand::commitNodeTransformation()
{
	if (transformationCommitted_)
		return;

	ZoneScoped;

	const Camera::ProjectionValues cameraValues = RenderResources::currentCamera()->projectionValues();
	modelMatrix_[3][2] = calculateDepth(layer_, cameraValues.near, cameraValues.far);

	if (material_.shaderProgram_ && material_.shaderProgram_->status() == GLShaderProgram::Status::LINKED_WITH_INTROSPECTION)
	{
		GLUniformBlockCache *instanceBlock = material_.uniformBlock(Material::InstanceBlockName);
		GLUniformCache *matrixUniform = instanceBlock
		                                    ? instanceBlock->uniform(Material::ModelMatrixUniformName)
		                                    : material_.uniform(Material::ModelMatrixUniformName);
		if (matrixUniform)
		{
			ZoneScopedN("Set model matrix");
			matrixUniform->setFloatVector(modelMatrix_.data());
		}
	}

	transformationCommitted_ = true;
}

void RenderCommand::commitCameraTransformation()
{
	ZoneScoped;

	RenderResources::CameraUniformData *cameraUniformData = RenderResources::findCameraUniformData(material_.shaderProgram_);
	if (cameraUniformData == nullptr)
	{
		RenderResources::CameraUniformData newCameraUniformData;
		newCameraUniformData.shaderUniforms.setProgram(material_.shaderProgram_, Material::ProjectionViewMatrixExcludeString, nullptr);
		if (newCameraUniformData.shaderUniforms.numUniforms() == 2)
		{
			newCameraUniformData.shaderUniforms.setUniformsDataPointer(RenderResources::cameraUniformsBuffer());
			newCameraUniformData.shaderUniforms.uniform(Material::ProjectionMatrixUniformName)->setDirty(true);
			newCameraUniformData.shaderUniforms.uniform(Material::ViewMatrixUniformName)->setDirty(true);
			newCameraUniformData.shaderUniforms.commitUniforms();

			RenderResources::insertCameraUniformData(material_.shaderProgram_, nctl::move(newCameraUniformData));
		}
	}
	else
		cameraUniformData->shaderUniforms.commitUniforms();
}

void RenderCommand::commitAll()
{
	// Copy the vertices and indices stored in host memory to video memory
	/* This step is not needed if the command uses a custom VBO or IBO
	 * or directly writes into the common one */
	geometry_.commitVertices();
	geometry_.commitIndices();

	// The model matrix should always be updated before committing uniform blocks
	commitNodeTransformation();

	// Commits all the uniform blocks of command's shader program
	material_.commitUniformBlocks();
}

float RenderCommand::calculateDepth(uint16_t layer, float near, float far)
{
	// The layer translates to depth, from near to far
	const float depth = near + LayerStep + (far - near - LayerStep) * layer * LayerStep;
	return depth;
}

}
