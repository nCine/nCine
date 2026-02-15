#include "LuaShaderState.h"
#include "LuaUntrackedUserData.h"
#include "LuaClassTracker.h"
#include "LuaVector2Utils.h"
#include "LuaVector3Utils.h"
#include "LuaVector4Utils.h"
#include "LuaColorfUtils.h"
#include "LuaUtils.h"
#include "ShaderState.h"
#include "DrawableNode.h"
#include "Shader.h"

namespace ncine {

namespace LuaNames {
namespace ShaderState {
	static const char *ShaderState = "shaderstate";

	static const char *node = "get_node";
	static const char *setNode = "set_node";

	static const char *shader = "get_shader";
	static const char *setShader = "set_shader";
	static const char *resetShader = "reset_shader";

	static const char *setTexture = "set_texture";

	static const char *setUniformIntVector = "set_uniform_int_vector";
	static const char *setUniformIntValue1 = "set_uniform_int_value1";
	static const char *setUniformIntValue2 = "set_uniform_int_value2";
	static const char *setUniformIntValue3 = "set_uniform_int_value3";
	static const char *setUniformIntValue4 = "set_uniform_int_value4";

	static const char *setUniformIntVector2 = "set_uniform_int_vector2";
	static const char *setUniformIntVector3 = "set_uniform_int_vector3";
	static const char *setUniformIntVector4 = "set_uniform_int_vector4";

	static const char *setUniformFloatVector = "set_uniform_float_vector";
	static const char *setUniformFloatValue1 = "set_uniform_float_value1";
	static const char *setUniformFloatValue2 = "set_uniform_float_value2";
	static const char *setUniformFloatValue3 = "set_uniform_float_value3";
	static const char *setUniformFloatValue4 = "set_uniform_float_value4";

	static const char *setUniformFloatVector2 = "set_uniform_float_vector2";
	static const char *setUniformFloatVector3 = "set_uniform_float_vector3";
	static const char *setUniformFloatVector4 = "set_uniform_float_vector4";
	static const char *setUniformFloatColor = "set_uniform_float_color";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaShaderState::expose(LuaStateManager *stateManager)
{
	lua_State *L = stateManager->state();
	lua_createtable(L, 0, 25);

	if (stateManager->apiType() == LuaStateManager::ApiType::FULL)
	{
		LuaClassTracker<ShaderState>::exposeDelete(L);
		LuaUtils::addFunction(L, LuaNames::newObject, newObject);
	}

	LuaUtils::addFunction(L, LuaNames::ShaderState::node, node);
	LuaUtils::addFunction(L, LuaNames::ShaderState::setNode, setNode);

	LuaUtils::addFunction(L, LuaNames::ShaderState::shader, shader);
	LuaUtils::addFunction(L, LuaNames::ShaderState::setShader, setShader);
	LuaUtils::addFunction(L, LuaNames::ShaderState::resetShader, resetShader);

	LuaUtils::addFunction(L, LuaNames::ShaderState::setTexture, setTexture);

	LuaUtils::addFunction(L, LuaNames::ShaderState::setUniformIntVector, setUniformIntVector);
	LuaUtils::addFunction(L, LuaNames::ShaderState::setUniformIntValue1, setUniformIntValue1);
	LuaUtils::addFunction(L, LuaNames::ShaderState::setUniformIntValue2, setUniformIntValue2);
	LuaUtils::addFunction(L, LuaNames::ShaderState::setUniformIntValue3, setUniformIntValue3);
	LuaUtils::addFunction(L, LuaNames::ShaderState::setUniformIntValue4, setUniformIntValue4);

	LuaUtils::addFunction(L, LuaNames::ShaderState::setUniformIntVector2, setUniformIntVector2);
	LuaUtils::addFunction(L, LuaNames::ShaderState::setUniformIntVector3, setUniformIntVector3);
	LuaUtils::addFunction(L, LuaNames::ShaderState::setUniformIntVector4, setUniformIntVector4);

	LuaUtils::addFunction(L, LuaNames::ShaderState::setUniformFloatVector, setUniformFloatVector);
	LuaUtils::addFunction(L, LuaNames::ShaderState::setUniformFloatValue1, setUniformFloatValue1);
	LuaUtils::addFunction(L, LuaNames::ShaderState::setUniformFloatValue2, setUniformFloatValue2);
	LuaUtils::addFunction(L, LuaNames::ShaderState::setUniformFloatValue3, setUniformFloatValue3);
	LuaUtils::addFunction(L, LuaNames::ShaderState::setUniformFloatValue4, setUniformFloatValue4);

	LuaUtils::addFunction(L, LuaNames::ShaderState::setUniformFloatVector2, setUniformFloatVector2);
	LuaUtils::addFunction(L, LuaNames::ShaderState::setUniformFloatVector3, setUniformFloatVector3);
	LuaUtils::addFunction(L, LuaNames::ShaderState::setUniformFloatVector4, setUniformFloatVector4);
	LuaUtils::addFunction(L, LuaNames::ShaderState::setUniformFloatColor, setUniformFloatColor);

	lua_setfield(L, -2, LuaNames::ShaderState::ShaderState);
}

void LuaShaderState::release(void *object)
{
	ShaderState *shaderState = reinterpret_cast<ShaderState *>(object);
	delete shaderState;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

namespace {

	template <typename T>
	void retrieveUniformVector(lua_State *L, int index, T vec[16])
	{
		const unsigned int length = lua_rawlen(L, index);
		const unsigned clampedLength = (length > 16) ? 16 : length;

		for (unsigned int i = 0; i < clampedLength; i++)
		{
			lua_rawgeti(L, index, i + 1);
			vec[i] = LuaUtils::retrieve<T>(L, -1);
			lua_pop(L, 1);
		}
	}

}

int LuaShaderState::newObject(lua_State *L)
{
	DrawableNode *node = LuaUntrackedUserData<DrawableNode>::retrieve(L, -2);
	Shader *shader = LuaUntrackedUserData<Shader>::retrieve(L, -1);

	LuaClassTracker<ShaderState>::newObject(L, node, shader);

	return 1;
}

int LuaShaderState::node(lua_State *L)
{
	ShaderState *shaderState = LuaUntrackedUserData<ShaderState>::retrieve(L, -1);

	if (shaderState)
		LuaUntrackedUserData<DrawableNode>::push(L, shaderState->node());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaShaderState::setNode(lua_State *L)
{
	ShaderState *shaderState = LuaUntrackedUserData<ShaderState>::retrieve(L, -2);
	DrawableNode *node = LuaUntrackedUserData<DrawableNode>::retrieveOrNil(L, -1);

	bool nodeHasChanged = false;
	if (shaderState)
		nodeHasChanged = shaderState->setNode(node);
	LuaUtils::push(L, nodeHasChanged);

	return 1;
}

int LuaShaderState::shader(lua_State *L)
{
	ShaderState *shaderState = LuaUntrackedUserData<ShaderState>::retrieve(L, -1);

	if (shaderState)
		LuaUntrackedUserData<Shader>::push(L, shaderState->shader());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaShaderState::setShader(lua_State *L)
{
	ShaderState *shaderState = LuaUntrackedUserData<ShaderState>::retrieve(L, -2);
	Shader *shader = LuaUntrackedUserData<Shader>::retrieveOrNil(L, -1);

	bool shaderHaschanged = false;
	if (shaderState)
		shaderHaschanged = shaderState->setShader(shader);
	LuaUtils::push(L, shaderHaschanged);

	return 1;
}

int LuaShaderState::resetShader(lua_State *L)
{
	ShaderState *shaderState = LuaUntrackedUserData<ShaderState>::retrieve(L, -1);

	if (shaderState)
		shaderState->resetShader();

	return 0;
}

int LuaShaderState::setTexture(lua_State *L)
{
	ShaderState *shaderState = LuaUntrackedUserData<ShaderState>::retrieve(L, -3);
	const unsigned int unit = LuaUtils::retrieve<unsigned int>(L, -2);
	Texture *texture = LuaUntrackedUserData<Texture>::retrieveOrNil(L, -1);

	bool textureAssigned = false;
	if (shaderState)
		textureAssigned = shaderState->setTexture(unit, texture);
	LuaUtils::push(L, textureAssigned);

	return 1;
}

int LuaShaderState::setUniformIntVector(lua_State *L)
{
	static int vector[16];

	ShaderState *shaderState = LuaUntrackedUserData<ShaderState>::retrieve(L, -4);
	const char *blockName = LuaUtils::retrieve<const char *>(L, -3);
	const char *name = LuaUtils::retrieve<const char *>(L, -2);
	retrieveUniformVector<int>(L, -1, vector);

	bool uniformHasBeenSet = false;
	if (shaderState)
		uniformHasBeenSet = shaderState->setUniformInt(blockName, name, vector);
	LuaUtils::push(L, uniformHasBeenSet);

	return 1;
}

int LuaShaderState::setUniformIntValue1(lua_State *L)
{
	ShaderState *shaderState = LuaUntrackedUserData<ShaderState>::retrieve(L, -4);
	const char *blockName = LuaUtils::retrieve<const char *>(L, -3);
	const char *name = LuaUtils::retrieve<const char *>(L, -2);
	const int value0 = LuaUtils::retrieve<int>(L, -1);

	bool uniformHasBeenSet = false;
	if (shaderState)
		uniformHasBeenSet = shaderState->setUniformInt(blockName, name, value0);
	LuaUtils::push(L, uniformHasBeenSet);

	return 1;
}

int LuaShaderState::setUniformIntValue2(lua_State *L)
{
	ShaderState *shaderState = LuaUntrackedUserData<ShaderState>::retrieve(L, -5);
	const char *blockName = LuaUtils::retrieve<const char *>(L, -4);
	const char *name = LuaUtils::retrieve<const char *>(L, -3);
	const int value0 = LuaUtils::retrieve<int>(L, -2);
	const int value1 = LuaUtils::retrieve<int>(L, -1);

	bool uniformHasBeenSet = false;
	if (shaderState)
		uniformHasBeenSet = shaderState->setUniformInt(blockName, name, value0, value1);
	LuaUtils::push(L, uniformHasBeenSet);

	return 1;
}

int LuaShaderState::setUniformIntValue3(lua_State *L)
{
	ShaderState *shaderState = LuaUntrackedUserData<ShaderState>::retrieve(L, -6);
	const char *blockName = LuaUtils::retrieve<const char *>(L, -5);
	const char *name = LuaUtils::retrieve<const char *>(L, -4);
	const int value0 = LuaUtils::retrieve<int>(L, -3);
	const int value1 = LuaUtils::retrieve<int>(L, -2);
	const int value2 = LuaUtils::retrieve<int>(L, -1);

	bool uniformHasBeenSet = false;
	if (shaderState)
		uniformHasBeenSet = shaderState->setUniformInt(blockName, name, value0, value1, value2);
	LuaUtils::push(L, uniformHasBeenSet);

	return 1;
}

int LuaShaderState::setUniformIntValue4(lua_State *L)
{
	ShaderState *shaderState = LuaUntrackedUserData<ShaderState>::retrieve(L, -7);
	const char *blockName = LuaUtils::retrieve<const char *>(L, -6);
	const char *name = LuaUtils::retrieve<const char *>(L, -5);
	const int value0 = LuaUtils::retrieve<int>(L, -4);
	const int value1 = LuaUtils::retrieve<int>(L, -3);
	const int value2 = LuaUtils::retrieve<int>(L, -2);
	const int value3 = LuaUtils::retrieve<int>(L, -1);

	bool uniformHasBeenSet = false;
	if (shaderState)
		uniformHasBeenSet = shaderState->setUniformInt(blockName, name, value0, value1, value2, value3);
	LuaUtils::push(L, uniformHasBeenSet);

	return 1;
}

int LuaShaderState::setUniformIntVector2(lua_State *L)
{
	ShaderState *shaderState = LuaUntrackedUserData<ShaderState>::retrieve(L, -4);
	const char *blockName = LuaUtils::retrieve<const char *>(L, -3);
	const char *name = LuaUtils::retrieve<const char *>(L, -2);
	const Vector2i vector = LuaVector2iUtils::retrieveTable(L, -1);

	bool uniformHasBeenSet = false;
	if (shaderState)
		uniformHasBeenSet = shaderState->setUniformInt(blockName, name, vector);
	LuaUtils::push(L, uniformHasBeenSet);

	return 1;
}

int LuaShaderState::setUniformIntVector3(lua_State *L)
{
	ShaderState *shaderState = LuaUntrackedUserData<ShaderState>::retrieve(L, -4);
	const char *blockName = LuaUtils::retrieve<const char *>(L, -3);
	const char *name = LuaUtils::retrieve<const char *>(L, -2);
	const Vector3i vector = LuaVector3iUtils::retrieveTable(L, -1);

	bool uniformHasBeenSet = false;
	if (shaderState)
		uniformHasBeenSet = shaderState->setUniformInt(blockName, name, vector);
	LuaUtils::push(L, uniformHasBeenSet);

	return 1;
}

int LuaShaderState::setUniformIntVector4(lua_State *L)
{
	ShaderState *shaderState = LuaUntrackedUserData<ShaderState>::retrieve(L, -4);
	const char *blockName = LuaUtils::retrieve<const char *>(L, -3);
	const char *name = LuaUtils::retrieve<const char *>(L, -2);
	const Vector4i vector = LuaVector4iUtils::retrieveTable(L, -1);

	bool uniformHasBeenSet = false;
	if (shaderState)
		uniformHasBeenSet = shaderState->setUniformInt(blockName, name, vector);
	LuaUtils::push(L, uniformHasBeenSet);

	return 1;
}

int LuaShaderState::setUniformFloatVector(lua_State *L)
{
	static float vector[16];

	ShaderState *shaderState = LuaUntrackedUserData<ShaderState>::retrieve(L, -4);
	const char *blockName = LuaUtils::retrieve<const char *>(L, -3);
	const char *name = LuaUtils::retrieve<const char *>(L, -2);
	retrieveUniformVector<float>(L, -1, vector);

	bool uniformHasBeenSet = false;
	if (shaderState)
		uniformHasBeenSet = shaderState->setUniformFloat(blockName, name, vector);
	LuaUtils::push(L, uniformHasBeenSet);

	return 1;
}

int LuaShaderState::setUniformFloatValue1(lua_State *L)
{
	ShaderState *shaderState = LuaUntrackedUserData<ShaderState>::retrieve(L, -4);
	const char *blockName = LuaUtils::retrieve<const char *>(L, -3);
	const char *name = LuaUtils::retrieve<const char *>(L, -2);
	const float value0 = LuaUtils::retrieve<float>(L, -1);

	bool uniformHasBeenSet = false;
	if (shaderState)
		uniformHasBeenSet = shaderState->setUniformFloat(blockName, name, value0);
	LuaUtils::push(L, uniformHasBeenSet);

	return 1;
}

int LuaShaderState::setUniformFloatValue2(lua_State *L)
{
	ShaderState *shaderState = LuaUntrackedUserData<ShaderState>::retrieve(L, -5);
	const char *blockName = LuaUtils::retrieve<const char *>(L, -4);
	const char *name = LuaUtils::retrieve<const char *>(L, -3);
	const float value0 = LuaUtils::retrieve<float>(L, -2);
	const float value1 = LuaUtils::retrieve<float>(L, -1);

	bool uniformHasBeenSet = false;
	if (shaderState)
		uniformHasBeenSet = shaderState->setUniformFloat(blockName, name, value0, value1);
	LuaUtils::push(L, uniformHasBeenSet);

	return 1;
}

int LuaShaderState::setUniformFloatValue3(lua_State *L)
{
	ShaderState *shaderState = LuaUntrackedUserData<ShaderState>::retrieve(L, -6);
	const char *blockName = LuaUtils::retrieve<const char *>(L, -5);
	const char *name = LuaUtils::retrieve<const char *>(L, -4);
	const float value0 = LuaUtils::retrieve<float>(L, -3);
	const float value1 = LuaUtils::retrieve<float>(L, -2);
	const float value2 = LuaUtils::retrieve<float>(L, -1);

	bool uniformHasBeenSet = false;
	if (shaderState)
		uniformHasBeenSet = shaderState->setUniformFloat(blockName, name, value0, value1, value2);
	LuaUtils::push(L, uniformHasBeenSet);

	return 1;
}

int LuaShaderState::setUniformFloatValue4(lua_State *L)
{
	ShaderState *shaderState = LuaUntrackedUserData<ShaderState>::retrieve(L, -7);
	const char *blockName = LuaUtils::retrieve<const char *>(L, -6);
	const char *name = LuaUtils::retrieve<const char *>(L, -5);
	const float value0 = LuaUtils::retrieve<float>(L, -4);
	const float value1 = LuaUtils::retrieve<float>(L, -3);
	const float value2 = LuaUtils::retrieve<float>(L, -2);
	const float value3 = LuaUtils::retrieve<float>(L, -1);

	bool uniformHasBeenSet = false;
	if (shaderState)
		uniformHasBeenSet = shaderState->setUniformFloat(blockName, name, value0, value1, value2, value3);
	LuaUtils::push(L, uniformHasBeenSet);

	return 1;
}

int LuaShaderState::setUniformFloatVector2(lua_State *L)
{
	ShaderState *shaderState = LuaUntrackedUserData<ShaderState>::retrieve(L, -4);
	const char *blockName = LuaUtils::retrieve<const char *>(L, -3);
	const char *name = LuaUtils::retrieve<const char *>(L, -2);
	const Vector2f vector = LuaVector2fUtils::retrieveTable(L, -1);

	bool uniformHasBeenSet = false;
	if (shaderState)
		uniformHasBeenSet = shaderState->setUniformFloat(blockName, name, vector);
	LuaUtils::push(L, uniformHasBeenSet);

	return 1;
}

int LuaShaderState::setUniformFloatVector3(lua_State *L)
{
	ShaderState *shaderState = LuaUntrackedUserData<ShaderState>::retrieve(L, -4);
	const char *blockName = LuaUtils::retrieve<const char *>(L, -3);
	const char *name = LuaUtils::retrieve<const char *>(L, -2);
	const Vector3f vector = LuaVector3fUtils::retrieveTable(L, -1);

	bool uniformHasBeenSet = false;
	if (shaderState)
		uniformHasBeenSet = shaderState->setUniformFloat(blockName, name, vector);
	LuaUtils::push(L, uniformHasBeenSet);

	return 1;
}

int LuaShaderState::setUniformFloatVector4(lua_State *L)
{
	ShaderState *shaderState = LuaUntrackedUserData<ShaderState>::retrieve(L, -4);
	const char *blockName = LuaUtils::retrieve<const char *>(L, -3);
	const char *name = LuaUtils::retrieve<const char *>(L, -2);
	const Vector4f vector = LuaVector4fUtils::retrieveTable(L, -1);

	bool uniformHasBeenSet = false;
	if (shaderState)
		uniformHasBeenSet = shaderState->setUniformFloat(blockName, name, vector);
	LuaUtils::push(L, uniformHasBeenSet);

	return 1;
}

int LuaShaderState::setUniformFloatColor(lua_State *L)
{
	ShaderState *shaderState = LuaUntrackedUserData<ShaderState>::retrieve(L, -4);
	const char *blockName = LuaUtils::retrieve<const char *>(L, -3);
	const char *name = LuaUtils::retrieve<const char *>(L, -2);
	const Colorf color = LuaColorfUtils::retrieveTable(L, -1);

	bool uniformHasBeenSet = false;
	if (shaderState)
		uniformHasBeenSet = shaderState->setUniformFloat(blockName, name, color);
	LuaUtils::push(L, uniformHasBeenSet);

	return 1;
}

}
