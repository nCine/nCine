#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaParticleAffector.h"
#include "LuaUntrackedUserData.h"
#include "LuaVector2Utils.h"
#include "LuaColorfUtils.h"
#include "ParticleAffectors.h"

namespace ncine {

namespace LuaNames {
namespace ParticleAffector {
	static const char *ParticleAffector = "particle_affector";

	static const char *COLOR = "COLOR";
	static const char *SIZE = "SIZE";
	static const char *ROTATION = "ROTATION";
	static const char *POSITION = "POSITION";
	static const char *VELOCITY = "VELOCITY";
	static const char *Type = "affector_type";

	static const char *type = "get_type";
	static const char *isEnabled = "is_enabled";
	static const char *setEnabled = "set_enabled";
	static const char *numSteps = "num_steps";
	static const char *removeStep = "remove_step";
	static const char *clearSteps = "clear_steps";

	static const char *colorSteps = "get_color_steps";
	static const char *sizeSteps = "get_size_steps";
	static const char *rotationSteps = "get_rotation_steps";
	static const char *positionSteps = "get_position_steps";
	static const char *velocitySteps = "get_velocity_steps";

	static const char *addColorStep = "add_color_step";
	static const char *baseScale = "get_base_scale";
	static const char *setBaseScale = "set_base_scale";
	static const char *addSizeStep = "add_size_step";
	static const char *addRotationStep = "add_rotation_step";
	static const char *addPositionStep = "add_position_step";
	static const char *addVelocityStep = "add_velocity_step";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaParticleAffector::expose(LuaStateManager *stateManager)
{
	lua_State *L = stateManager->state();
	lua_newtable(L);

	LuaUtils::addFunction(L, LuaNames::ParticleAffector::type, type);
	LuaUtils::addFunction(L, LuaNames::ParticleAffector::isEnabled, isEnabled);
	LuaUtils::addFunction(L, LuaNames::ParticleAffector::setEnabled, setEnabled);
	LuaUtils::addFunction(L, LuaNames::ParticleAffector::numSteps, numSteps);
	LuaUtils::addFunction(L, LuaNames::ParticleAffector::removeStep, removeStep);
	LuaUtils::addFunction(L, LuaNames::ParticleAffector::clearSteps, clearSteps);

	LuaUtils::addFunction(L, LuaNames::ParticleAffector::colorSteps, colorSteps);
	LuaUtils::addFunction(L, LuaNames::ParticleAffector::sizeSteps, sizeSteps);
	LuaUtils::addFunction(L, LuaNames::ParticleAffector::rotationSteps, rotationSteps);
	LuaUtils::addFunction(L, LuaNames::ParticleAffector::positionSteps, positionSteps);
	LuaUtils::addFunction(L, LuaNames::ParticleAffector::velocitySteps, velocitySteps);

	LuaUtils::addFunction(L, LuaNames::ParticleAffector::addColorStep, addColorStep);
	LuaUtils::addFunction(L, LuaNames::ParticleAffector::baseScale, baseScale);
	LuaUtils::addFunction(L, LuaNames::ParticleAffector::setBaseScale, setBaseScale);
	LuaUtils::addFunction(L, LuaNames::ParticleAffector::addSizeStep, addSizeStep);
	LuaUtils::addFunction(L, LuaNames::ParticleAffector::addRotationStep, addRotationStep);
	LuaUtils::addFunction(L, LuaNames::ParticleAffector::addPositionStep, addPositionStep);
	LuaUtils::addFunction(L, LuaNames::ParticleAffector::addVelocityStep, addVelocityStep);

	lua_setfield(L, -2, LuaNames::ParticleAffector::ParticleAffector);
}

void LuaParticleAffector::exposeConstants(lua_State *L)
{
	lua_createtable(L, 0, 5);

	LuaUtils::pushField(L, LuaNames::ParticleAffector::COLOR, static_cast<int64_t>(ParticleAffector::Type::COLOR));
	LuaUtils::pushField(L, LuaNames::ParticleAffector::SIZE, static_cast<int64_t>(ParticleAffector::Type::SIZE));
	LuaUtils::pushField(L, LuaNames::ParticleAffector::ROTATION, static_cast<int64_t>(ParticleAffector::Type::ROTATION));
	LuaUtils::pushField(L, LuaNames::ParticleAffector::POSITION, static_cast<int64_t>(ParticleAffector::Type::POSITION));
	LuaUtils::pushField(L, LuaNames::ParticleAffector::VELOCITY, static_cast<int64_t>(ParticleAffector::Type::VELOCITY));

	lua_setfield(L, -2, LuaNames::ParticleAffector::Type);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

namespace {

	void pushColorSteps(lua_State *L, const ColorAffector &affector)
	{
		const unsigned int numSteps = affector.numSteps();
		lua_createtable(L, numSteps, 0);

		for (unsigned int i = 0; i < numSteps; i++)
		{
			const ColorAffector::ColorStep &step = affector.steps()[i];

			lua_createtable(L, 0, 2);

			LuaUtils::push(L, step.age);
			lua_rawseti(L, -2, 1); // Lua arrays start from index 1
			LuaColorfUtils::push(L, step.color);
			lua_rawseti(L, -2, 2);

			lua_rawseti(L, -2, i + 1); // Lua arrays start from index 1
		}
	}

	void pushSizeSteps(lua_State *L, const SizeAffector &affector)
	{
		const unsigned int numSteps = affector.numSteps();
		lua_createtable(L, numSteps, 0);

		for (unsigned int i = 0; i < numSteps; i++)
		{
			const SizeAffector::SizeStep &step = affector.steps()[i];

			lua_createtable(L, 0, 2);

			LuaUtils::push(L, step.age);
			lua_rawseti(L, -2, 1); // Lua arrays start from index 1
			LuaVector2fUtils::push(L, step.scale);
			lua_rawseti(L, -2, 2);

			lua_rawseti(L, -2, i + 1); // Lua arrays start from index 1
		}
	}

	void pushRotationSteps(lua_State *L, const RotationAffector &affector)
	{
		const unsigned int numSteps = affector.numSteps();
		lua_createtable(L, numSteps, 0);

		for (unsigned int i = 0; i < numSteps; i++)
		{
			const RotationAffector::RotationStep &step = affector.steps()[i];

			lua_createtable(L, 0, 2);

			LuaUtils::push(L, step.age);
			lua_rawseti(L, -2, 1); // Lua arrays start from index 1
			LuaUtils::push(L, step.angle);
			lua_rawseti(L, -2, 2);

			lua_rawseti(L, -2, i + 1); // Lua arrays start from index 1
		}
	}

	void pushPositionSteps(lua_State *L, const PositionAffector &affector)
	{
		const unsigned int numSteps = affector.numSteps();
		lua_createtable(L, numSteps, 0);

		for (unsigned int i = 0; i < numSteps; i++)
		{
			const PositionAffector::PositionStep &step = affector.steps()[i];

			lua_createtable(L, 0, 2);

			LuaUtils::push(L, step.age);
			lua_rawseti(L, -2, 1); // Lua arrays start from index 1
			LuaVector2fUtils::push(L, step.position);
			lua_rawseti(L, -2, 2);

			lua_rawseti(L, -2, i + 1); // Lua arrays start from index 1
		}
	}

	void pushVelocitySteps(lua_State *L, const VelocityAffector &affector)
	{
		const unsigned int numSteps = affector.numSteps();
		lua_createtable(L, numSteps, 0);

		for (unsigned int i = 0; i < numSteps; i++)
		{
			const VelocityAffector::VelocityStep &step = affector.steps()[i];

			lua_createtable(L, 0, 2);

			LuaUtils::push(L, step.age);
			lua_rawseti(L, -2, 1); // Lua arrays start from index 1
			LuaVector2fUtils::push(L, step.velocity);
			lua_rawseti(L, -2, 2);

			lua_rawseti(L, -2, i + 1); // Lua arrays start from index 1
		}
	}

}

int LuaParticleAffector::type(lua_State *L)
{
	ParticleAffector *affector = LuaUntrackedUserData<ParticleAffector>::retrieve(L, -1);

	if (affector)
		LuaUtils::push(L, static_cast<int64_t>(affector->type()));
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaParticleAffector::isEnabled(lua_State *L)
{
	ParticleAffector *affector = LuaUntrackedUserData<ParticleAffector>::retrieve(L, -1);

	if (affector)
		LuaUtils::push(L, affector->isEnabled());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaParticleAffector::setEnabled(lua_State *L)
{
	ParticleAffector *affector = LuaUntrackedUserData<ParticleAffector>::retrieve(L, -2);
	const bool enabled = LuaUtils::retrieve<bool>(L, -1);

	if (affector)
		affector->setEnabled(enabled);

	return 0;
}

int LuaParticleAffector::numSteps(lua_State *L)
{
	ParticleAffector *affector = LuaUntrackedUserData<ParticleAffector>::retrieve(L, -1);

	if (affector)
		LuaUtils::push(L, affector->numSteps());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaParticleAffector::removeStep(lua_State *L)
{
	ParticleAffector *affector = LuaUntrackedUserData<ParticleAffector>::retrieve(L, -2);
	const int index = LuaUtils::retrieve<int>(L, -1);

	if (affector)
		affector->removeStep(index);

	return 0;
}

int LuaParticleAffector::clearSteps(lua_State *L)
{
	ParticleAffector *affector = LuaUntrackedUserData<ParticleAffector>::retrieve(L, -1);

	if (affector)
		affector->clearSteps();

	return 0;
}

int LuaParticleAffector::colorSteps(lua_State *L)
{
	ParticleAffector *affector = LuaUntrackedUserData<ParticleAffector>::retrieve(L, -1);

	if (affector && affector->type() == ParticleAffector::Type::COLOR)
	{
		ColorAffector *colorAffector = static_cast<ColorAffector *>(affector);
		pushColorSteps(L, *colorAffector);
	}
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaParticleAffector::sizeSteps(lua_State *L)
{
	ParticleAffector *affector = LuaUntrackedUserData<ParticleAffector>::retrieve(L, -1);

	if (affector && affector->type() == ParticleAffector::Type::SIZE)
	{
		SizeAffector *sizeAffector = static_cast<SizeAffector *>(affector);
		pushSizeSteps(L, *sizeAffector);
	}
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaParticleAffector::rotationSteps(lua_State *L)
{
	ParticleAffector *affector = LuaUntrackedUserData<ParticleAffector>::retrieve(L, -1);

	if (affector && affector->type() == ParticleAffector::Type::ROTATION)
	{
		RotationAffector *rotationAffector = static_cast<RotationAffector *>(affector);
		pushRotationSteps(L, *rotationAffector);
	}
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaParticleAffector::positionSteps(lua_State *L)
{
	ParticleAffector *affector = LuaUntrackedUserData<ParticleAffector>::retrieve(L, -1);

	if (affector && affector->type() == ParticleAffector::Type::POSITION)
	{
		PositionAffector *positionAffector = static_cast<PositionAffector *>(affector);
		pushPositionSteps(L, *positionAffector);
	}
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaParticleAffector::velocitySteps(lua_State *L)
{
	ParticleAffector *affector = LuaUntrackedUserData<ParticleAffector>::retrieve(L, -1);

	if (affector && affector->type() == ParticleAffector::Type::VELOCITY)
	{
		VelocityAffector *velocityAffector = static_cast<VelocityAffector *>(affector);
		pushVelocitySteps(L, *velocityAffector);
	}
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaParticleAffector::addColorStep(lua_State *L)
{
	int colorIndex = 0;
	const Colorf color = LuaColorfUtils::retrieve(L, -1, colorIndex);
	ParticleAffector *affector = LuaUntrackedUserData<ParticleAffector>::retrieve(L, colorIndex - 2);
	const float age = LuaUtils::retrieve<float>(L, colorIndex - 1);

	if (affector && affector->type() == ParticleAffector::Type::COLOR)
	{
		ColorAffector *colorAffector = static_cast<ColorAffector *>(affector);
		colorAffector->addColorStep(age, color);
	}

	return 0;
}

int LuaParticleAffector::baseScale(lua_State *L)
{
	ParticleAffector *affector = LuaUntrackedUserData<ParticleAffector>::retrieve(L, -1);

	if (affector && affector->type() == ParticleAffector::Type::SIZE)
	{
		SizeAffector *sizeAffector = static_cast<SizeAffector *>(affector);
		LuaVector2fUtils::push(L, sizeAffector->baseScale());
	}
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaParticleAffector::setBaseScale(lua_State *L)
{
	int vectorIndex = 0;
	const Vector2f baseScale = LuaVector2fUtils::retrieve(L, -1, vectorIndex);
	ParticleAffector *affector = LuaUntrackedUserData<ParticleAffector>::retrieve(L, vectorIndex - 1);

	if (affector && affector->type() == ParticleAffector::Type::SIZE)
	{
		SizeAffector *sizeAffector = static_cast<SizeAffector *>(affector);
		sizeAffector->setBaseScale(baseScale);
	}

	return 0;
}

int LuaParticleAffector::addSizeStep(lua_State *L)
{
	int vectorIndex = 0;
	const Vector2f scale = LuaVector2fUtils::retrieve(L, -1, vectorIndex);
	ParticleAffector *affector = LuaUntrackedUserData<ParticleAffector>::retrieve(L, vectorIndex - 2);
	const float age = LuaUtils::retrieve<float>(L, vectorIndex - 1);

	if (affector && affector->type() == ParticleAffector::Type::SIZE)
	{
		SizeAffector *sizeAffector = static_cast<SizeAffector *>(affector);
		sizeAffector->addSizeStep(age, scale);
	}

	return 0;
}

int LuaParticleAffector::addRotationStep(lua_State *L)
{
	ParticleAffector *affector = LuaUntrackedUserData<ParticleAffector>::retrieve(L, -3);
	const float age = LuaUtils::retrieve<float>(L, -2);
	const float angle = LuaUtils::retrieve<float>(L, -1);

	if (affector && affector->type() == ParticleAffector::Type::ROTATION)
	{
		RotationAffector *rotationAffector = static_cast<RotationAffector *>(affector);
		rotationAffector->addRotationStep(age, angle);
	}

	return 0;
}

int LuaParticleAffector::addPositionStep(lua_State *L)
{
	int vectorIndex = 0;
	const Vector2f position = LuaVector2fUtils::retrieve(L, -1, vectorIndex);
	ParticleAffector *affector = LuaUntrackedUserData<ParticleAffector>::retrieve(L, vectorIndex - 2);
	const float age = LuaUtils::retrieve<float>(L, vectorIndex - 1);

	if (affector && affector->type() == ParticleAffector::Type::POSITION)
	{
		PositionAffector *positionAffector = static_cast<PositionAffector *>(affector);
		positionAffector->addPositionStep(age, position);
	}

	return 0;
}

int LuaParticleAffector::addVelocityStep(lua_State *L)
{
	int vectorIndex = 0;
	const Vector2f velocity = LuaVector2fUtils::retrieve(L, -1, vectorIndex);
	ParticleAffector *affector = LuaUntrackedUserData<ParticleAffector>::retrieve(L, vectorIndex - 2);
	const float age = LuaUtils::retrieve<float>(L, vectorIndex - 1);

	if (affector && affector->type() == ParticleAffector::Type::VELOCITY)
	{
		VelocityAffector *velocityAffector = static_cast<VelocityAffector *>(affector);
		velocityAffector->addVelocityStep(age, velocity);
	}

	return 0;
}

}
