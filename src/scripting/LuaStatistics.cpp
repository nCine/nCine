#include <nctl/String.h>
#include "LuaStatistics.h"
#include "LuaStateManager.h"
#include "tracy.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

nctl::Array<LuaStateManager *> LuaStatistics::managers_(2);
unsigned int LuaStatistics::numTrackedUserDatas_;
unsigned int LuaStatistics::numTypedUserDatas_[LuaTypes::UserDataType::UNKNOWN + 1];
size_t LuaStatistics::usedMemory_ = 0;
TimeStamp LuaStatistics::lastOpsUpdateTime_;
unsigned int LuaStatistics::index_ = 0;
int LuaStatistics::operations_[2] = { 0, 0 };

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaStatistics::update()
{
	numTrackedUserDatas_ = 0;
	for (unsigned int i = 0; i < LuaTypes::UserDataType::UNKNOWN + 1; i++)
		numTypedUserDatas_[i] = 0;

	for (const LuaStateManager *manager : managers_)
	{
		numTrackedUserDatas_ += manager->trackedUserDatas_.size();
		for (const LuaStateManager::UserDataWrapper &wrapper : manager->trackedUserDatas_)
			numTypedUserDatas_[wrapper.type]++;
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void LuaStatistics::registerState(LuaStateManager *stateManager)
{
	managers_.pushBack(stateManager);
	if (managers_.size() == 1)
		lastOpsUpdateTime_ = TimeStamp::now();
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
		managers_.unorderedRemoveAt(index);
}

void LuaStatistics::countOperations()
{
	operations_[index_] += OperationsCount;

	const float secsSinceLastUpdate = lastOpsUpdateTime_.secondsSince();
	if (secsSinceLastUpdate >= 1.0f)
	{
		operations_[index_] = int(float(operations_[index_]) / secsSinceLastUpdate);
		TracyPlot("Lua Ops", static_cast<int64_t>(operations_[index_]));
		lastOpsUpdateTime_ = TimeStamp::now();

		// Ping pong index for last and current measurement
		index_ = (index_ + 1) % 2;
		operations_[index_] = 0;
	}
}

}
