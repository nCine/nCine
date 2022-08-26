#include "LuaShader.h"
#include "LuaUntrackedUserData.h"
#include "LuaClassTracker.h"
#include "LuaUtils.h"
#include "Shader.h"

namespace ncine {

namespace LuaNames {
namespace Shader {
	static const char *Shader = "shader";

	static const char *ENABLED = "ENABLED";
	static const char *NO_UNIFORMS_IN_BLOCKS = "NO_UNIFORMS_IN_BLOCKS";
	static const char *DISABLED = "DISABLED";
	static const char *Introspection = "shader_introspection";

	static const char *SPRITE = "SPRITE";
	static const char *SPRITE_NOTEXTURE = "SPRITE_NOTEXTURE";
	static const char *MESHSPRITE = "MESHSPRITE";
	static const char *MESHSPRITE_NOTEXTURE = "MESHSPRITE_NOTEXTURE";
	static const char *TEXTNODE = "TEXTNODE";
	static const char *BATCHED_SPRITES = "BATCHED_SPRITES";
	static const char *BATCHED_SPRITES_NOTEXTURE = "BATCHED_SPRITES_NOTEXTURE";
	static const char *BATCHED_MESHSPRITES = "BATCHED_MESHSPRITES";
	static const char *BATCHED_MESHSPRITES_NOTEXTURE = "BATCHED_MESHSPRITES_NOTEXTURE";
	static const char *BATCHED_TEXTNODES = "BATCHED_TEXTNODES";
	static const char *DefaultVertex = "shader_default_vertex";

	//static const char *SPRITE = "SPRITE";
	static const char *SPRITE_GRAY = "SPRITE_GRAY";
	//static const char *SPRITE_NOTEXTURE = "SPRITE_NOTEXTURE";
	static const char *TEXTNODE_ALPHA = "TEXTNODE_ALPHA";
	static const char *TEXTNODE_RED = "TEXTNODE_RED";
	static const char *DefaultFragment = "shader_default_fragment";

	static const char *loadFromMemory = "load_from_memory";
	static const char *loadFromMemoryDefaultVertex = "load_from_memory_default_vertex";
	static const char *loadFromMemoryDefaultFragment = "load_from_memory_default_fragment";

	static const char *loadFromFile = "load_from_file";
	static const char *loadFromFileDefaultVertex = "load_from_file_default_vertex";
	static const char *loadFromFileDefaultFragment = "load_from_file_default_fragment";

	static const char *setAttribute = "set_attribute";

	static const char *isLinked = "is_linked";

	static const char *retrieveInfoLogLength = "retrieve_infolog_length";
	static const char *retrieveInfoLog = "retrieve_infolog";

	static const char *logOnErrors = "get_log_on_errors";
	static const char *setLogOnErrors = "set_log_on_errors";

	static const char *setGLShaderProgramLabel = "set_glshaderprogram_label";

	static const char *registerBatchedShader = "register_batched_shader";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaShader::exposeConstants(lua_State *L)
{
	lua_createtable(L, 0, 3);

	LuaUtils::pushField(L, LuaNames::Shader::ENABLED, static_cast<int64_t>(Shader::Introspection::ENABLED));
	LuaUtils::pushField(L, LuaNames::Shader::NO_UNIFORMS_IN_BLOCKS, static_cast<int64_t>(Shader::Introspection::NO_UNIFORMS_IN_BLOCKS));
	LuaUtils::pushField(L, LuaNames::Shader::DISABLED, static_cast<int64_t>(Shader::Introspection::DISABLED));

	lua_setfield(L, -2, LuaNames::Shader::Introspection);

	lua_createtable(L, 0, 10);

	LuaUtils::pushField(L, LuaNames::Shader::SPRITE, static_cast<int64_t>(Shader::DefaultVertex::SPRITE));
	LuaUtils::pushField(L, LuaNames::Shader::SPRITE_NOTEXTURE, static_cast<int64_t>(Shader::DefaultVertex::SPRITE_NOTEXTURE));
	LuaUtils::pushField(L, LuaNames::Shader::MESHSPRITE, static_cast<int64_t>(Shader::DefaultVertex::MESHSPRITE));
	LuaUtils::pushField(L, LuaNames::Shader::MESHSPRITE_NOTEXTURE, static_cast<int64_t>(Shader::DefaultVertex::MESHSPRITE_NOTEXTURE));
	LuaUtils::pushField(L, LuaNames::Shader::TEXTNODE, static_cast<int64_t>(Shader::DefaultVertex::TEXTNODE));
	LuaUtils::pushField(L, LuaNames::Shader::BATCHED_SPRITES, static_cast<int64_t>(Shader::DefaultVertex::BATCHED_SPRITES));
	LuaUtils::pushField(L, LuaNames::Shader::BATCHED_SPRITES_NOTEXTURE, static_cast<int64_t>(Shader::DefaultVertex::BATCHED_SPRITES_NOTEXTURE));
	LuaUtils::pushField(L, LuaNames::Shader::BATCHED_MESHSPRITES, static_cast<int64_t>(Shader::DefaultVertex::BATCHED_MESHSPRITES));
	LuaUtils::pushField(L, LuaNames::Shader::BATCHED_MESHSPRITES_NOTEXTURE, static_cast<int64_t>(Shader::DefaultVertex::BATCHED_MESHSPRITES_NOTEXTURE));
	LuaUtils::pushField(L, LuaNames::Shader::BATCHED_TEXTNODES, static_cast<int64_t>(Shader::DefaultVertex::BATCHED_TEXTNODES));

	lua_setfield(L, -2, LuaNames::Shader::DefaultVertex);

	lua_createtable(L, 0, 5);

	LuaUtils::pushField(L, LuaNames::Shader::SPRITE, static_cast<int64_t>(Shader::DefaultFragment::SPRITE));
	LuaUtils::pushField(L, LuaNames::Shader::SPRITE_GRAY, static_cast<int64_t>(Shader::DefaultFragment::SPRITE_GRAY));
	LuaUtils::pushField(L, LuaNames::Shader::SPRITE_NOTEXTURE, static_cast<int64_t>(Shader::DefaultFragment::SPRITE_NOTEXTURE));
	LuaUtils::pushField(L, LuaNames::Shader::TEXTNODE_ALPHA, static_cast<int64_t>(Shader::DefaultFragment::TEXTNODE_ALPHA));
	LuaUtils::pushField(L, LuaNames::Shader::TEXTNODE_RED, static_cast<int64_t>(Shader::DefaultFragment::TEXTNODE_RED));

	lua_setfield(L, -2, LuaNames::Shader::DefaultFragment);
}

void LuaShader::expose(LuaStateManager *stateManager)
{
	lua_State *L = stateManager->state();
	lua_createtable(L, 0, 12);

	if (stateManager->apiType() == LuaStateManager::ApiType::FULL)
	{
		LuaClassTracker<Shader>::exposeDelete(L);
		LuaUtils::addFunction(L, LuaNames::newObject, newObject);
	}

	LuaUtils::addFunction(L, LuaNames::Shader::loadFromMemory, loadFromMemory);
	LuaUtils::addFunction(L, LuaNames::Shader::loadFromMemoryDefaultVertex, loadFromMemoryDefaultVertex);
	LuaUtils::addFunction(L, LuaNames::Shader::loadFromMemoryDefaultFragment, loadFromMemoryDefaultFragment);

	LuaUtils::addFunction(L, LuaNames::Shader::loadFromFile, loadFromFile);
	LuaUtils::addFunction(L, LuaNames::Shader::loadFromFileDefaultVertex, loadFromFileDefaultVertex);
	LuaUtils::addFunction(L, LuaNames::Shader::loadFromFileDefaultFragment, loadFromFileDefaultFragment);

	LuaUtils::addFunction(L, LuaNames::Shader::setAttribute, setAttribute);

	LuaUtils::addFunction(L, LuaNames::Shader::isLinked, isLinked);

	LuaUtils::addFunction(L, LuaNames::Shader::retrieveInfoLogLength, retrieveInfoLogLength);
	LuaUtils::addFunction(L, LuaNames::Shader::retrieveInfoLog, retrieveInfoLog);

	LuaUtils::addFunction(L, LuaNames::Shader::logOnErrors, logOnErrors);
	LuaUtils::addFunction(L, LuaNames::Shader::setLogOnErrors, setLogOnErrors);

	LuaUtils::addFunction(L, LuaNames::Shader::setGLShaderProgramLabel, setGLShaderProgramLabel);

	LuaUtils::addFunction(L, LuaNames::Shader::registerBatchedShader, registerBatchedShader);

	lua_setfield(L, -2, LuaNames::Shader::Shader);
}

void LuaShader::release(void *object)
{
	Shader *shader = reinterpret_cast<Shader *>(object);
	delete shader;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

int LuaShader::newObject(lua_State *L)
{
	LuaClassTracker<Shader>::newObject(L);

	return 1;
}

int LuaShader::loadFromMemory(lua_State *L)
{
	Shader *shader = LuaUntrackedUserData<Shader>::retrieve(L, -5);
	const char *shaderName = LuaUtils::retrieve<const char *>(L, -4);
	const Shader::Introspection introspection = static_cast<Shader::Introspection>(LuaUtils::retrieve<int64_t>(L, -3));
	const char *vertex = LuaUtils::retrieve<const char *>(L, -2);
	const char *fragment = LuaUtils::retrieve<const char *>(L, -1);

	if (shader)
		shader->loadFromMemory(shaderName, introspection, vertex, fragment);

	return 0;
}

int LuaShader::loadFromMemoryDefaultVertex(lua_State *L)
{
	Shader *shader = LuaUntrackedUserData<Shader>::retrieve(L, -5);
	const char *shaderName = LuaUtils::retrieve<const char *>(L, -4);
	const Shader::Introspection introspection = static_cast<Shader::Introspection>(LuaUtils::retrieve<int64_t>(L, -3));
	const Shader::DefaultVertex vertex = static_cast<Shader::DefaultVertex>(LuaUtils::retrieve<int64_t>(L, -2));
	const char *fragment = LuaUtils::retrieve<const char *>(L, -1);

	if (shader)
		shader->loadFromMemory(shaderName, introspection, vertex, fragment);

	return 0;
}

int LuaShader::loadFromMemoryDefaultFragment(lua_State *L)
{
	Shader *shader = LuaUntrackedUserData<Shader>::retrieve(L, -5);
	const char *shaderName = LuaUtils::retrieve<const char *>(L, -4);
	const Shader::Introspection introspection = static_cast<Shader::Introspection>(LuaUtils::retrieve<int64_t>(L, -3));
	const char *vertex = LuaUtils::retrieve<const char *>(L, -2);
	const Shader::DefaultFragment fragment = static_cast<Shader::DefaultFragment>(LuaUtils::retrieve<int64_t>(L, -1));

	if (shader)
		shader->loadFromMemory(shaderName, introspection, vertex, fragment);

	return 0;
}

int LuaShader::loadFromFile(lua_State *L)
{
	Shader *shader = LuaUntrackedUserData<Shader>::retrieve(L, -5);
	const char *shaderName = LuaUtils::retrieve<const char *>(L, -4);
	const Shader::Introspection introspection = static_cast<Shader::Introspection>(LuaUtils::retrieve<int64_t>(L, -3));
	const char *vertex = LuaUtils::retrieve<const char *>(L, -2);
	const char *fragment = LuaUtils::retrieve<const char *>(L, -1);

	if (shader)
		shader->loadFromFile(shaderName, introspection, vertex, fragment);

	return 0;
}

int LuaShader::loadFromFileDefaultVertex(lua_State *L)
{
	Shader *shader = LuaUntrackedUserData<Shader>::retrieve(L, -5);
	const char *shaderName = LuaUtils::retrieve<const char *>(L, -4);
	const Shader::Introspection introspection = static_cast<Shader::Introspection>(LuaUtils::retrieve<int64_t>(L, -3));
	const Shader::DefaultVertex vertex = static_cast<Shader::DefaultVertex>(LuaUtils::retrieve<int64_t>(L, -2));
	const char *fragment = LuaUtils::retrieve<const char *>(L, -1);

	if (shader)
		shader->loadFromFile(shaderName, introspection, vertex, fragment);

	return 0;
}

int LuaShader::loadFromFileDefaultFragment(lua_State *L)
{
	Shader *shader = LuaUntrackedUserData<Shader>::retrieve(L, -5);
	const char *shaderName = LuaUtils::retrieve<const char *>(L, -4);
	const Shader::Introspection introspection = static_cast<Shader::Introspection>(LuaUtils::retrieve<int64_t>(L, -3));
	const char *vertex = LuaUtils::retrieve<const char *>(L, -2);
	const Shader::DefaultFragment fragment = static_cast<Shader::DefaultFragment>(LuaUtils::retrieve<int64_t>(L, -1));

	if (shader)
		shader->loadFromFile(shaderName, introspection, vertex, fragment);

	return 0;
}

int LuaShader::setAttribute(lua_State *L)
{
	Shader *shader = LuaUntrackedUserData<Shader>::retrieve(L, -4);
	const char *name = LuaUtils::retrieve<const char *>(L, -3);
	const int stride = LuaUtils::retrieve<int>(L, -2);
	const unsigned long int pointer = LuaUtils::retrieve<uint64_t>(L, -1);

	if (shader)
		shader->setAttribute(name, stride, pointer);

	return 0;
}

int LuaShader::isLinked(lua_State *L)
{
	Shader *shader = LuaUntrackedUserData<Shader>::retrieve(L, -1);

	if (shader)
		LuaUtils::push(L, shader->isLinked());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaShader::retrieveInfoLogLength(lua_State *L)
{
	Shader *shader = LuaUntrackedUserData<Shader>::retrieve(L, -1);

	if (shader)
		LuaUtils::push(L, shader->retrieveInfoLogLength());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaShader::retrieveInfoLog(lua_State *L)
{
	static nctl::String infoLog(256);
	Shader *shader = LuaUntrackedUserData<Shader>::retrieve(L, -1);
	shader->retrieveInfoLog(infoLog);

	if (shader)
		LuaUtils::push(L, infoLog.data());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaShader::logOnErrors(lua_State *L)
{
	Shader *shader = LuaUntrackedUserData<Shader>::retrieve(L, -1);

	if (shader)
		LuaUtils::push(L, shader->logOnErrors());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaShader::setLogOnErrors(lua_State *L)
{
	Shader *shader = LuaUntrackedUserData<Shader>::retrieve(L, -2);
	const bool logOnErrors = LuaUtils::retrieve<bool>(L, -1);

	if (shader)
		shader->setLogOnErrors(logOnErrors);

	return 0;
}

int LuaShader::setGLShaderProgramLabel(lua_State *L)
{
	Shader *shader = LuaUntrackedUserData<Shader>::retrieve(L, -2);
	const char *label = LuaUtils::retrieve<const char *>(L, -1);

	if (shader)
		shader->setGLShaderProgramLabel(label);

	return 0;
}

int LuaShader::registerBatchedShader(lua_State *L)
{
	Shader *shader = LuaUntrackedUserData<Shader>::retrieve(L, -2);
	Shader *batchedShader = LuaUntrackedUserData<Shader>::retrieve(L, -1);

	if (shader)
		shader->registerBatchedShader(*batchedShader);

	return 0;
}

}
