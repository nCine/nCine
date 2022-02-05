#include <nctl/CString.h>
#include "ShaderState.h"
#include "Shader.h"
#include "DrawableNode.h"
#include "RenderCommand.h"
#include "Material.h"

namespace ncine {

namespace {

	GLUniformCache *retrieveUniform(Material &material, const char *blockName, const char *name)
	{
		GLUniformCache *uniform = nullptr;
		if (blockName)
		{
			GLUniformBlockCache *uniformBlock = material.uniformBlock(blockName);
			if (uniformBlock)
				uniform = uniformBlock->uniform(name);
		}
		else
			uniform = material.uniform(name);

		return uniform;
	}

}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ShaderState::ShaderState()
    : ShaderState(nullptr, nullptr)
{
}

ShaderState::ShaderState(DrawableNode *node, Shader *shader)
    : node_(nullptr), shader_(nullptr),
      previousShaderType_(static_cast<int>(Material::ShaderProgramType::CUSTOM))
{
	setNode(node);
	setShader(shader);
}

ShaderState::~ShaderState()
{
	setNode(nullptr);
	setShader(nullptr);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool ShaderState::setNode(DrawableNode *node)
{
	bool nodeHasChanged = false;

	if (node != node_)
	{
		if (node_ != nullptr)
		{
			Material &prevMaterial = node_->renderCommand_->material();
			const Material::ShaderProgramType programType = static_cast<Material::ShaderProgramType>(previousShaderType_);
			prevMaterial.setShaderProgramType(programType);
		}

		if (node != nullptr)
		{
			Material &material = node->renderCommand_->material();
			previousShaderType_ = static_cast<int>(material.shaderProgramType());
		}
		node_ = node;

		if (shader_ != nullptr)
			setShader(shader_);

		nodeHasChanged = true;
	}

	return nodeHasChanged;
}

bool ShaderState::setShader(Shader *shader)
{
	bool shaderHasChanged = false;

	if (node_ != nullptr && shader != shader_)
	{
		Material &material = node_->renderCommand_->material();
		if (shader == nullptr)
		{
			const Material::ShaderProgramType programType = static_cast<Material::ShaderProgramType>(previousShaderType_);
			material.setShaderProgramType(programType);
		}
		else if (shader->isLinked())
		{
			if (material.shaderProgramType() != Material::ShaderProgramType::CUSTOM)
				previousShaderType_ = static_cast<int>(material.shaderProgramType());

			material.setShaderProgram(shader->glShaderProgram_.get());
		}

		shader_ = shader;
		node_->shaderHasChanged();
		shaderHasChanged = true;
	}

	return shaderHasChanged;
}

bool ShaderState::setAttribute(const char *name, int stride, unsigned long int pointer)
{
	if (node_ == nullptr || shader_ == nullptr || name == nullptr)
		return false;

	Material &material = node_->renderCommand_->material();
	if (material.attribute(name))
	{
		material.attribute(name)->setVboParameters(stride, reinterpret_cast<void *>(pointer));
		return true;
	}

	return false;
}

bool ShaderState::setUniformInt(const char *blockName, const char *name, const int *vector)
{
	if (node_ == nullptr || shader_ == nullptr || name == nullptr || vector == nullptr)
		return false;

	GLUniformCache *uniform = retrieveUniform(node_->renderCommand_->material(), blockName, name);
	if (uniform)
		uniform->setIntVector(vector);

	return (uniform != nullptr);
}

bool ShaderState::setUniformInt(const char *blockName, const char *name, int value0)
{
	if (node_ == nullptr || shader_ == nullptr || name == nullptr)
		return false;

	GLUniformCache *uniform = retrieveUniform(node_->renderCommand_->material(), blockName, name);
	if (uniform)
		uniform->setIntValue(value0);

	return (uniform != nullptr);
}

bool ShaderState::setUniformInt(const char *blockName, const char *name, int value0, int value1)
{
	if (node_ == nullptr || shader_ == nullptr || name == nullptr)
		return false;

	GLUniformCache *uniform = retrieveUniform(node_->renderCommand_->material(), blockName, name);
	if (uniform)
		uniform->setIntValue(value0, value1);

	return (uniform != nullptr);
}

bool ShaderState::setUniformInt(const char *blockName, const char *name, int value0, int value1, int value2)
{
	if (node_ == nullptr || shader_ == nullptr || name == nullptr)
		return false;

	GLUniformCache *uniform = retrieveUniform(node_->renderCommand_->material(), blockName, name);
	if (uniform)
		uniform->setIntValue(value0, value1, value2);

	return (uniform != nullptr);
}

bool ShaderState::setUniformInt(const char *blockName, const char *name, int value0, int value1, int value2, int value3)
{
	if (node_ == nullptr || shader_ == nullptr || name == nullptr)
		return false;

	GLUniformCache *uniform = retrieveUniform(node_->renderCommand_->material(), blockName, name);
	if (uniform)
		uniform->setIntValue(value0, value1, value2, value3);

	return (uniform != nullptr);
}

bool ShaderState::setUniformInt(const char *blockName, const char *name, const Vector2i &vec)
{
	return setUniformInt(blockName, name, vec.x, vec.y);
}

bool ShaderState::setUniformInt(const char *blockName, const char *name, const Vector3i &vector)
{
	return setUniformInt(blockName, name, vector.x, vector.y, vector.z);
}

bool ShaderState::setUniformInt(const char *blockName, const char *name, const Vector4i &vector)
{
	return setUniformInt(blockName, name, vector.x, vector.y, vector.z, vector.w);
}

bool ShaderState::setUniformFloat(const char *blockName, const char *name, const float *vector)
{
	if (node_ == nullptr || shader_ == nullptr || name == nullptr || vector == nullptr)
		return false;

	GLUniformCache *uniform = retrieveUniform(node_->renderCommand_->material(), blockName, name);
	if (uniform)
		uniform->setFloatVector(vector);

	return (uniform != nullptr);
}

bool ShaderState::setUniformFloat(const char *blockName, const char *name, float value0)
{
	if (node_ == nullptr || shader_ == nullptr || name == nullptr)
		return false;

	GLUniformCache *uniform = retrieveUniform(node_->renderCommand_->material(), blockName, name);
	if (uniform)
		uniform->setFloatValue(value0);

	return (uniform != nullptr);
}

bool ShaderState::setUniformFloat(const char *blockName, const char *name, float value0, float value1)
{
	if (node_ == nullptr || shader_ == nullptr || name == nullptr)
		return false;

	GLUniformCache *uniform = retrieveUniform(node_->renderCommand_->material(), blockName, name);
	if (uniform)
		uniform->setFloatValue(value0, value1);

	return (uniform != nullptr);
}

bool ShaderState::setUniformFloat(const char *blockName, const char *name, float value0, float value1, float value2)
{
	if (node_ == nullptr || shader_ == nullptr || name == nullptr)
		return false;

	GLUniformCache *uniform = retrieveUniform(node_->renderCommand_->material(), blockName, name);
	if (uniform)
		uniform->setFloatValue(value0, value1, value2);

	return (uniform != nullptr);
}

bool ShaderState::setUniformFloat(const char *blockName, const char *name, float value0, float value1, float value2, float value3)
{
	if (node_ == nullptr || shader_ == nullptr || name == nullptr)
		return false;

	GLUniformCache *uniform = retrieveUniform(node_->renderCommand_->material(), blockName, name);
	if (uniform)
		uniform->setFloatValue(value0, value1, value2, value3);

	return (uniform != nullptr);
}

bool ShaderState::setUniformFloat(const char *blockName, const char *name, const Vector2f &vector)
{
	return setUniformFloat(blockName, name, vector.x, vector.y);
}

bool ShaderState::setUniformFloat(const char *blockName, const char *name, const Vector3f &vector)
{
	return setUniformFloat(blockName, name, vector.x, vector.y, vector.z);
}

bool ShaderState::setUniformFloat(const char *blockName, const char *name, const Vector4f &vector)
{
	return setUniformFloat(blockName, name, vector.x, vector.y, vector.z, vector.w);
}

bool ShaderState::setUniformFloat(const char *blockName, const char *name, const Colorf &color)
{
	return setUniformFloat(blockName, name, color.r(), color.g(), color.b(), color.a());
}

}
