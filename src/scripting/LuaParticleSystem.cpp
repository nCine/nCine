#include "LuaParticleSystem.h"
#include "LuaClassWrapper.h"
#include "LuaClassTracker.h"
#include "LuaSceneNode.h"
#include "LuaRectUtils.h"
#include "LuaVector2Utils.h"
#include "LuaColorUtils.h"
#include "ParticleSystem.h"

namespace ncine {

namespace LuaNames {
namespace ParticleSystem
{
	static const char *ParticleSystem = "particle_system";

	static const char *addAccelerationAffector = "add_acceleration_affector";
	static const char *addColorAffector = "add_color_affector";
	static const char *addSizeAffector = "add_size_affector";
	static const char *clearAffectors = "clear_affectors";

	static const char *emitParticles = "emit_particles";
	static const char *inLocalSpace = "get_in_local_space";
	static const char *setInLocalSpace = "set_in_local_space";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaParticleSystem::expose(LuaStateManager *stateManager)
{
	lua_State *L = stateManager->state();
	lua_newtable(L);

	if (stateManager->apiType() == LuaStateManager::ApiType::FULL)
	{
		LuaClassTracker<ParticleSystem>::exposeDelete(L);
		LuaUtils::addFunction(L, LuaNames::newObject, newObject);
	}

	LuaUtils::addFunction(L, LuaNames::ParticleSystem::addAccelerationAffector, addAccelerationAffector);
	LuaUtils::addFunction(L, LuaNames::ParticleSystem::addColorAffector, addColorAffector);
	LuaUtils::addFunction(L, LuaNames::ParticleSystem::addSizeAffector, addSizeAffector);
	LuaUtils::addFunction(L, LuaNames::ParticleSystem::clearAffectors, clearAffectors);

	LuaUtils::addFunction(L, LuaNames::ParticleSystem::emitParticles, emitParticles);
	LuaUtils::addFunction(L, LuaNames::ParticleSystem::inLocalSpace, inLocalSpace);
	LuaUtils::addFunction(L, LuaNames::ParticleSystem::setInLocalSpace, setInLocalSpace);

	LuaSceneNode::exposeFunctions(L);

	lua_setfield(L, -2, LuaNames::ParticleSystem::ParticleSystem);
}

void LuaParticleSystem::release(void *object)
{
	ParticleSystem *particleSys = reinterpret_cast<ParticleSystem *>(object);
	delete particleSys;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

namespace {

nctl::UniquePtr<ColorAffector> retrieveColorSteps(lua_State *L, int index)
{
	const unsigned int numSteps = lua_rawlen(L, index);

	nctl::UniquePtr<ColorAffector> affector = nctl::makeUnique<ColorAffector>();

	for (unsigned int i = 0; i < numSteps; i++)
	{
		const int type = lua_rawgeti(L, index, i + 1);
		if (type != LUA_TTABLE)
			luaL_argerror(L, -1, "Expecting a table");

		lua_rawgeti(L, -1, 1);
		const float time = LuaUtils::retrieve<float>(L, -1);
		lua_pop(L, 1);
		lua_rawgeti(L, -1, 2);
		const Color color = LuaColorUtils::retrieveTable(L, -1);
		lua_pop(L, 1);

		lua_pop(L, 1);
		affector->addColorStep(time, color);
	}

	return nctl::move(affector);
}

nctl::UniquePtr<SizeAffector> retrieveSizeSteps(lua_State *L, int index, float baseScale)
{
	const unsigned int numSteps = lua_rawlen(L, index);

	nctl::UniquePtr<SizeAffector> affector = nctl::makeUnique<SizeAffector>(baseScale);

	for (unsigned int i = 0; i < numSteps; i++)
	{
		const int type = lua_rawgeti(L, index, i + 1);
		if (type != LUA_TTABLE)
			luaL_argerror(L, -1, "Expecting a table");

		lua_rawgeti(L, -1, 1);
		const float time = LuaUtils::retrieve<float>(L, -1);
		lua_pop(L, 1);
		lua_rawgeti(L, -1, 2);
		const float scale = LuaUtils::retrieve<float>(L, -1);
		lua_pop(L, 1);

		lua_pop(L, 1);
		affector->addSizeStep(time, scale);
	}

	return nctl::move(affector);
}

}

int LuaParticleSystem::newObject(lua_State *L)
{
	SceneNode *parent = LuaClassWrapper<SceneNode>::unwrapUserDataOrNil(L, -4);
	const unsigned int count = LuaUtils::retrieve<uint32_t>(L, -3);
	Texture *texture = LuaClassWrapper<Texture>::unwrapUserData(L, -2);
	const Recti texRect = LuaRectiUtils::retrieveTable(L, -1);

	ParticleSystem *particleSys = new ParticleSystem(parent, count, texture, texRect);
	LuaClassTracker<ParticleSystem>::wrapTrackedUserData(L, particleSys);

	return 1;
}

int LuaParticleSystem::addAccelerationAffector(lua_State *L)
{
	int newIndex = 0;
	ParticleSystem *particleSys = LuaClassWrapper<ParticleSystem>::unwrapUserData(L, -2);
	const Vector2f acc = LuaVector2fUtils::retrieve(L, -1, newIndex);

	nctl::UniquePtr<AccelerationAffector> affector = nctl::makeUnique<AccelerationAffector>(acc);
	particleSys->addAffector(nctl::move(affector));

	return 0;
}

int LuaParticleSystem::addColorAffector(lua_State *L)
{
	ParticleSystem *particleSys = LuaClassWrapper<ParticleSystem>::unwrapUserData(L, -2);
	nctl::UniquePtr<ColorAffector> affector = retrieveColorSteps(L, -1);

	particleSys->addAffector(nctl::move(affector));

	return 0;
}

int LuaParticleSystem::addSizeAffector(lua_State *L)
{
	ParticleSystem *particleSys = LuaClassWrapper<ParticleSystem>::unwrapUserData(L, -3);
	const float baseScale = LuaUtils::retrieve<float>(L, -2);
	nctl::UniquePtr<SizeAffector> affector = retrieveSizeSteps(L, -1, baseScale);

	particleSys->addAffector(nctl::move(affector));

	return 0;
}

int LuaParticleSystem::clearAffectors(lua_State *L)
{
	ParticleSystem *particleSys = LuaClassWrapper<ParticleSystem>::unwrapUserData(L, -1);
	particleSys->clearAffectors();
	return 0;
}

int LuaParticleSystem::emitParticles(lua_State *L)
{
	ParticleSystem *particleSys = LuaClassWrapper<ParticleSystem>::unwrapUserData(L, -4);
	const unsigned int amount = LuaUtils::retrieve<uint32_t>(L, -3);
	const float life =  LuaUtils::retrieve<float>(L, -2);
	const Vector2f vel = LuaVector2fUtils::retrieveTable(L, -1);

	particleSys->emitParticles(amount, life, vel);

	return 0;
}

int LuaParticleSystem::inLocalSpace(lua_State *L)
{
	ParticleSystem *particleSys = LuaClassWrapper<ParticleSystem>::unwrapUserData(L, -1);

	const bool inLocalSpace = particleSys->inLocalSpace();
	LuaUtils::push(L, inLocalSpace);

	return 1;
}

int LuaParticleSystem::setInLocalSpace(lua_State *L)
{
	ParticleSystem *particleSys = LuaClassWrapper<ParticleSystem>::unwrapUserData(L, -2);
	const bool inLocalSpace = LuaUtils::retrieve<bool>(L, -1);

	particleSys->setInLocalSpace(inLocalSpace);

	return 0;
}

}
