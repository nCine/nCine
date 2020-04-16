#include "LuaParticleSystem.h"
#include "LuaClassWrapper.h"
#include "LuaClassTracker.h"
#include "LuaSceneNode.h"
#include "LuaRectUtils.h"
#include "LuaVector2Utils.h"
#include "LuaColorUtils.h"
#include "ParticleSystem.h"
#include "ParticleInitializer.h"

namespace ncine {

namespace LuaNames {
namespace ParticleSystem {
	static const char *ParticleSystem = "particle_system";

	static const char *addColorAffector = "add_color_affector";
	static const char *addSizeAffector = "add_size_affector";
	static const char *addRotationAffector = "add_rotation_affector";
	static const char *addPositionAffector = "add_position_affector";
	static const char *addVelocityAffector = "add_velocity_affector";
	static const char *clearAffectors = "clear_affectors";

	static const char *emitParticles = "emit_particles";
	static const char *killParticles = "kill_particles";
	static const char *inLocalSpace = "get_in_local_space";
	static const char *setInLocalSpace = "set_in_local_space";

	static const char *numParticles = "num_particles";
	static const char *numAliveParticles = "num_alive_particles";

	static const char *setTexture = "set_texture";
	static const char *setTexRect = "set_texture_rect";
	static const char *setAnchorPoint = "set_anchor_point";
	static const char *setFlippedX = "set_flipped_x";
	static const char *setFlippedY = "set_flipped_y";
	static const char *setBlendingPreset = "set_blending_preset";
	static const char *setBlendingFactors = "set_blending_factors";
	static const char *setLayer = "set_layer";
}}

namespace LuaNames {
namespace ParticleInitializer {
	static const char *amount = "amount";
	static const char *life = "life";
	static const char *position = "position";
	static const char *velocity = "velocity";
	static const char *rotation = "rotation";
	static const char *emitterRotation = "emitter_rotation";
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

	LuaUtils::addFunction(L, LuaNames::ParticleSystem::addColorAffector, addColorAffector);
	LuaUtils::addFunction(L, LuaNames::ParticleSystem::addSizeAffector, addSizeAffector);
	LuaUtils::addFunction(L, LuaNames::ParticleSystem::addRotationAffector, addRotationAffector);
	LuaUtils::addFunction(L, LuaNames::ParticleSystem::addPositionAffector, addPositionAffector);
	LuaUtils::addFunction(L, LuaNames::ParticleSystem::addVelocityAffector, addVelocityAffector);
	LuaUtils::addFunction(L, LuaNames::ParticleSystem::clearAffectors, clearAffectors);

	LuaUtils::addFunction(L, LuaNames::ParticleSystem::emitParticles, emitParticles);
	LuaUtils::addFunction(L, LuaNames::ParticleSystem::killParticles, killParticles);
	LuaUtils::addFunction(L, LuaNames::ParticleSystem::inLocalSpace, inLocalSpace);
	LuaUtils::addFunction(L, LuaNames::ParticleSystem::setInLocalSpace, setInLocalSpace);

	LuaUtils::addFunction(L, LuaNames::ParticleSystem::numParticles, numParticles);
	LuaUtils::addFunction(L, LuaNames::ParticleSystem::numAliveParticles, numAliveParticles);

	LuaUtils::addFunction(L, LuaNames::ParticleSystem::setTexture, setTexture);
	LuaUtils::addFunction(L, LuaNames::ParticleSystem::setTexRect, setTexRect);
	LuaUtils::addFunction(L, LuaNames::ParticleSystem::setAnchorPoint, setAnchorPoint);
	LuaUtils::addFunction(L, LuaNames::ParticleSystem::setFlippedX, setFlippedX);
	LuaUtils::addFunction(L, LuaNames::ParticleSystem::setFlippedY, setFlippedY);
	LuaUtils::addFunction(L, LuaNames::ParticleSystem::setBlendingPreset, setBlendingPreset);
	LuaUtils::addFunction(L, LuaNames::ParticleSystem::setBlendingFactors, setBlendingFactors);
	LuaUtils::addFunction(L, LuaNames::ParticleSystem::setLayer, setLayer);

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
			const float age = LuaUtils::retrieve<float>(L, -1);
			lua_pop(L, 1);
			lua_rawgeti(L, -1, 2);
			const Colorf color = LuaColorUtils::retrieveTable(L, -1);
			lua_pop(L, 1);

			lua_pop(L, 1);
			affector->addColorStep(age, color);
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
			const float age = LuaUtils::retrieve<float>(L, -1);
			lua_pop(L, 1);
			lua_rawgeti(L, -1, 2);
			const float scale = LuaUtils::retrieve<float>(L, -1);
			lua_pop(L, 1);

			lua_pop(L, 1);
			affector->addSizeStep(age, scale);
		}

		return nctl::move(affector);
	}

	nctl::UniquePtr<RotationAffector> retrieveRotationSteps(lua_State *L, int index)
	{
		const unsigned int numSteps = lua_rawlen(L, index);

		nctl::UniquePtr<RotationAffector> affector = nctl::makeUnique<RotationAffector>();

		for (unsigned int i = 0; i < numSteps; i++)
		{
			const int type = lua_rawgeti(L, index, i + 1);
			if (type != LUA_TTABLE)
				luaL_argerror(L, -1, "Expecting a table");

			lua_rawgeti(L, -1, 1);
			const float age = LuaUtils::retrieve<float>(L, -1);
			lua_pop(L, 1);
			lua_rawgeti(L, -1, 2);
			const float angle = LuaUtils::retrieve<float>(L, -1);
			lua_pop(L, 1);

			lua_pop(L, 1);
			affector->addRotationStep(age, angle);
		}

		return nctl::move(affector);
	}

	nctl::UniquePtr<PositionAffector> retrievePositionSteps(lua_State *L, int index)
	{
		const unsigned int numSteps = lua_rawlen(L, index);

		nctl::UniquePtr<PositionAffector> affector = nctl::makeUnique<PositionAffector>();

		for (unsigned int i = 0; i < numSteps; i++)
		{
			const int type = lua_rawgeti(L, index, i + 1);
			if (type != LUA_TTABLE)
				luaL_argerror(L, -1, "Expecting a table");

			lua_rawgeti(L, -1, 1);
			const float age = LuaUtils::retrieve<float>(L, -1);
			lua_pop(L, 1);
			lua_rawgeti(L, -1, 2);
			const Vector2f position = LuaVector2fUtils::retrieveTable(L, -1);
			lua_pop(L, 1);

			lua_pop(L, 1);
			affector->addPositionStep(age, position);
		}

		return nctl::move(affector);
	}

	nctl::UniquePtr<VelocityAffector> retrieveVelocitySteps(lua_State *L, int index)
	{
		const unsigned int numSteps = lua_rawlen(L, index);

		nctl::UniquePtr<VelocityAffector> affector = nctl::makeUnique<VelocityAffector>();

		for (unsigned int i = 0; i < numSteps; i++)
		{
			const int type = lua_rawgeti(L, index, i + 1);
			if (type != LUA_TTABLE)
				luaL_argerror(L, -1, "Expecting a table");

			lua_rawgeti(L, -1, 1);
			const float age = LuaUtils::retrieve<float>(L, -1);
			lua_pop(L, 1);
			lua_rawgeti(L, -1, 2);
			const Vector2f velocity = LuaVector2fUtils::retrieveTable(L, -1);
			lua_pop(L, 1);

			lua_pop(L, 1);
			affector->addVelocityStep(age, velocity);
		}

		return nctl::move(affector);
	}

	void retrieveParticleInitializer(lua_State *L, int index, ParticleInitializer &init)
	{
		if (lua_istable(L, index) == false)
			luaL_argerror(L, -1, "Expecting a table");

		lua_getfield(L, index, LuaNames::ParticleInitializer::amount);
		if (lua_isinteger(L, -1))
		{
			const int amount = LuaUtils::retrieve<int32_t>(L, -1);
			init.setAmount(amount);
		}
		else if (lua_istable(L, -1))
		{
			lua_rawgeti(L, -1, 1);
			const int minAmount = LuaUtils::retrieve<int32_t>(L, -1);
			lua_pop(L, 1);
			lua_rawgeti(L, -1, 2);
			const int maxAmount = LuaUtils::retrieve<int32_t>(L, -1);
			lua_pop(L, 1);
			init.setAmount(minAmount, maxAmount);
		}
		lua_pop(L, 1);

		lua_getfield(L, index, LuaNames::ParticleInitializer::life);
		if (lua_isnumber(L, -1))
		{
			const float life = LuaUtils::retrieve<float>(L, -1);
			init.setLife(life);
		}
		else if (lua_istable(L, -1))
		{
			lua_rawgeti(L, -1, 1);
			const float minLife = LuaUtils::retrieve<float>(L, -1);
			lua_pop(L, 1);
			lua_rawgeti(L, -1, 2);
			const float maxLife = LuaUtils::retrieve<float>(L, -1);
			lua_pop(L, 1);
			init.setLife(minLife, maxLife);
		}
		lua_pop(L, 1);

		lua_getfield(L, index, LuaNames::ParticleInitializer::position);
		if (lua_istable(L, -1) && lua_rawlen(L, -1) == 0)
		{
			const Vector2f position = LuaVector2fUtils::retrieveTable(L, -1);
			init.setPosition(position);
		}
		else if (lua_istable(L, -1) && lua_rawlen(L, -1) == 2)
		{
			lua_rawgeti(L, -1, 1);
			const Vector2f minPosition = LuaVector2fUtils::retrieveTable(L, -1);
			lua_pop(L, 1);
			lua_rawgeti(L, -1, 2);
			const Vector2f maxPosition = LuaVector2fUtils::retrieveTable(L, -1);
			lua_pop(L, 1);
			init.setPosition(minPosition, maxPosition);
		}
		lua_pop(L, 1);

		lua_getfield(L, index, LuaNames::ParticleInitializer::velocity);
		if (lua_istable(L, -1) && lua_rawlen(L, -1) == 0)
		{
			const Vector2f velocity = LuaVector2fUtils::retrieveTable(L, -1);
			init.setVelocity(velocity);
		}
		else if (lua_istable(L, -1) && lua_rawlen(L, -1) == 2)
		{
			lua_rawgeti(L, -1, 1);
			const Vector2f minVelocity = LuaVector2fUtils::retrieveTable(L, -1);
			lua_pop(L, 1);
			lua_rawgeti(L, -1, 2);
			const Vector2f maxVelocity = LuaVector2fUtils::retrieveTable(L, -1);
			lua_pop(L, 1);
			init.setVelocity(minVelocity, maxVelocity);
		}
		lua_pop(L, 1);

		lua_getfield(L, index, LuaNames::ParticleInitializer::rotation);
		if (lua_isnumber(L, -1))
		{
			const float rotation = LuaUtils::retrieve<float>(L, -1);
			init.setRotation(rotation);
		}
		else if (lua_istable(L, -1))
		{
			lua_rawgeti(L, -1, 1);
			const float minRotation = LuaUtils::retrieve<float>(L, -1);
			lua_pop(L, 1);
			lua_rawgeti(L, -1, 2);
			const float maxRotation = LuaUtils::retrieve<float>(L, -1);
			lua_pop(L, 1);
			init.setRotation(minRotation, maxRotation);
		}
		lua_pop(L, 1);

		LuaUtils::tryRetrieveField<bool>(L, -1, LuaNames::ParticleInitializer::emitterRotation, init.emitterRotation);
	}

}

int LuaParticleSystem::newObject(lua_State *L)
{
	SceneNode *parent = LuaClassWrapper<SceneNode>::unwrapUserDataOrNil(L, -4);
	const unsigned int count = LuaUtils::retrieve<uint32_t>(L, -3);
	Texture *texture = LuaClassWrapper<Texture>::unwrapUserData(L, -2);
	const Recti texRect = LuaRectiUtils::retrieveTable(L, -1);

	LuaClassTracker<ParticleSystem>::newObject(L, parent, count, texture, texRect);

	return 1;
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

int LuaParticleSystem::addRotationAffector(lua_State *L)
{
	ParticleSystem *particleSys = LuaClassWrapper<ParticleSystem>::unwrapUserData(L, -2);
	nctl::UniquePtr<RotationAffector> affector = retrieveRotationSteps(L, -1);

	particleSys->addAffector(nctl::move(affector));

	return 0;
}

int LuaParticleSystem::addPositionAffector(lua_State *L)
{
	ParticleSystem *particleSys = LuaClassWrapper<ParticleSystem>::unwrapUserData(L, -2);
	nctl::UniquePtr<PositionAffector> affector = retrievePositionSteps(L, -1);

	particleSys->addAffector(nctl::move(affector));

	return 0;
}

int LuaParticleSystem::addVelocityAffector(lua_State *L)
{
	ParticleSystem *particleSys = LuaClassWrapper<ParticleSystem>::unwrapUserData(L, -2);
	nctl::UniquePtr<VelocityAffector> affector = retrieveVelocitySteps(L, -1);

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
	ParticleSystem *particleSys = LuaClassWrapper<ParticleSystem>::unwrapUserData(L, -2);
	ParticleInitializer init;
	retrieveParticleInitializer(L, -1, init);

	particleSys->emitParticles(init);

	return 0;
}

int LuaParticleSystem::killParticles(lua_State *L)
{
	ParticleSystem *particleSys = LuaClassWrapper<ParticleSystem>::unwrapUserData(L, -1);

	particleSys->killParticles();

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

int LuaParticleSystem::numParticles(lua_State *L)
{
	ParticleSystem *particleSys = LuaClassWrapper<ParticleSystem>::unwrapUserData(L, -1);

	LuaUtils::push(L, particleSys->numParticles());

	return 1;
}

int LuaParticleSystem::numAliveParticles(lua_State *L)
{
	ParticleSystem *particleSys = LuaClassWrapper<ParticleSystem>::unwrapUserData(L, -1);

	LuaUtils::push(L, particleSys->numAliveParticles());

	return 1;
}

int LuaParticleSystem::setTexture(lua_State *L)
{
	ParticleSystem *particleSys = LuaClassWrapper<ParticleSystem>::unwrapUserData(L, -2);
	Texture *texture = LuaClassWrapper<Texture>::unwrapUserData(L, -1);

	particleSys->setTexture(texture);

	return 0;
}

int LuaParticleSystem::setTexRect(lua_State *L)
{
	int rectIndex = 0;
	const Recti texRect = LuaRectiUtils::retrieve(L, -1, rectIndex);
	ParticleSystem *particleSys = LuaClassWrapper<ParticleSystem>::unwrapUserData(L, rectIndex - 1);

	particleSys->setTexRect(texRect);

	return 0;
}

int LuaParticleSystem::setAnchorPoint(lua_State *L)
{
	int vectorIndex = 0;
	const Vector2f anchorPoint = LuaVector2fUtils::retrieve(L, -1, vectorIndex);
	ParticleSystem *particleSys = LuaClassWrapper<ParticleSystem>::unwrapUserData(L, vectorIndex - 1);

	particleSys->setAnchorPoint(anchorPoint);

	return 0;
}

int LuaParticleSystem::setFlippedX(lua_State *L)
{
	ParticleSystem *particleSys = LuaClassWrapper<ParticleSystem>::unwrapUserData(L, -2);
	const bool flippedX = LuaUtils::retrieve<bool>(L, -1);

	particleSys->setFlippedX(flippedX);

	return 0;
}

int LuaParticleSystem::setFlippedY(lua_State *L)
{
	ParticleSystem *particleSys = LuaClassWrapper<ParticleSystem>::unwrapUserData(L, -2);
	const bool flippedY = LuaUtils::retrieve<bool>(L, -1);

	particleSys->setFlippedY(flippedY);

	return 0;
}

int LuaParticleSystem::setBlendingPreset(lua_State *L)
{
	ParticleSystem *particleSys = LuaClassWrapper<ParticleSystem>::unwrapUserData(L, -2);
	const DrawableNode::BlendingPreset blendingPreset = static_cast<DrawableNode::BlendingPreset>(LuaUtils::retrieve<int64_t>(L, -1));

	particleSys->setBlendingPreset(blendingPreset);

	return 0;
}

int LuaParticleSystem::setBlendingFactors(lua_State *L)
{
	ParticleSystem *particleSys = LuaClassWrapper<ParticleSystem>::unwrapUserData(L, -3);
	const DrawableNode::BlendingFactor srcBlendingFactor = static_cast<DrawableNode::BlendingFactor>(LuaUtils::retrieve<int64_t>(L, -2));
	const DrawableNode::BlendingFactor destBlendingFactor = static_cast<DrawableNode::BlendingFactor>(LuaUtils::retrieve<int64_t>(L, -1));

	particleSys->setBlendingFactors(srcBlendingFactor, destBlendingFactor);

	return 0;
}

int LuaParticleSystem::setLayer(lua_State *L)
{
	ParticleSystem *particleSys = LuaClassWrapper<ParticleSystem>::unwrapUserData(L, -2);
	const uint32_t layer = LuaUtils::retrieve<uint32_t>(L, -1);

	particleSys->setLayer(layer);

	return 0;
}

}
