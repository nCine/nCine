#include "nctl/String.h"
#include "LuaStatistics.h"
#include "LuaStateManager.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

nctl::Array<LuaStateManager *> LuaStatistics::managers_(2);
size_t LuaStatistics::usedMemory_ = 0;
Timer LuaStatistics::timer_;
unsigned int LuaStatistics::index_ = 0;
int LuaStatistics::operations_[2] = { 0, 0 };

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaStatistics::appendStatistics(nctl::String &string)
{
	// Do not append statistics if there are no registered state managers
	if (managers_.isEmpty())
		return;

	unsigned int numTrackedUserDatas = 0;
	unsigned int numTypedUserDatas[LuaTypes::UserDataType::UNKNOWN + 1] = { };

	for (const LuaStateManager *manager : managers_)
	{
		numTrackedUserDatas += manager->trackedUserDatas_.size();
		for (const LuaStateManager::UserDataWrapper &wrapper : manager->trackedUserDatas_)
			numTypedUserDatas[wrapper.type]++;
	}

	string.formatAppend(
		"Lua States: %u, %u tracked, %zu Kb, %d ops/s\n"\
		"Textures: %u, Sprites: %u, Mesh sprites: %u\n"\
		"Animated sprites: %u, Fonts: %u, Textnodes: %u\n"\
		"Audio buffers: %u, Buffer players: %u\n"\
		"Stream players: %u, Particle systems: %u",
		managers_.size(), numTrackedUserDatas,
		usedMemory_ / 1024, operations_[(index_ + 1) % 2],
		numTypedUserDatas[LuaTypes::UserDataType::TEXTURE],
		numTypedUserDatas[LuaTypes::UserDataType::SPRITE],
		numTypedUserDatas[LuaTypes::UserDataType::MESH_SPRITE],
		numTypedUserDatas[LuaTypes::UserDataType::ANIMATED_SPRITE],
		numTypedUserDatas[LuaTypes::UserDataType::FONT],
		numTypedUserDatas[LuaTypes::UserDataType::TEXTNODE],
		numTypedUserDatas[LuaTypes::UserDataType::AUDIOBUFFER],
		numTypedUserDatas[LuaTypes::UserDataType::AUDIOBUFFER_PLAYER],
		numTypedUserDatas[LuaTypes::UserDataType::AUDIOSTREAM_PLAYER],
		numTypedUserDatas[LuaTypes::UserDataType::PARTICLE_SYSTEM]);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void LuaStatistics::registerState(LuaStateManager *stateManager)
{
	managers_.pushBack(stateManager);
	if (managers_.size() == 1)
		timer_.start();
}

void LuaStatistics::unregisterState(LuaStateManager *stateManager)
{
	int index = -1;

	for (unsigned int i = 0; i < managers_.size(); i++)
	{
		if (managers_[i] == stateManager)
		{
			index = static_cast<int>(i);
			break;
		}
	}

	ASSERT(index != -1);
	if (index != -1)
	{
		const unsigned int newSize = managers_.size() - 1;
		managers_[index] = managers_[newSize];
		managers_.setSize(newSize);
	}
}

void LuaStatistics::countOperations()
{
	operations_[index_] += OperationsCount;
	if (timer_.interval() >= 1.0f)
	{
		operations_[index_] = int(float(operations_[index_]) / timer_.interval());
		timer_.start();

		// Ping pong index for last and current measurement
		index_ = (index_ + 1) % 2;
		operations_[index_] = 0;
	}
}

}
