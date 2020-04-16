#ifndef CLASS_NCINE_LUASTATISTICS
#define CLASS_NCINE_LUASTATISTICS

#include <nctl/Array.h>
#include "LuaTypes.h"
#include "TimeStamp.h"

namespace nctl {

class String;

}

namespace ncine {

class LuaStateManager;

/// Lua statistics about all active states
class LuaStatistics
{
  public:
	static void update();

	static inline unsigned int numRegistered() { return managers_.size(); }
	static inline unsigned int numTrackedUserDatas() { return numTrackedUserDatas_; }
	static inline unsigned int numTypedUserDatas(LuaTypes::UserDataType type) { return numTypedUserDatas_[type]; }
	static inline size_t usedMemory() { return usedMemory_; }
	static inline int operations() { return operations_[(index_ + 1) % 2]; }

  private:
	static const int OperationsCount = 1000;

	static nctl::Array<LuaStateManager *> managers_;
	static unsigned int numTrackedUserDatas_;
	static unsigned int numTypedUserDatas_[LuaTypes::UserDataType::UNKNOWN + 1];
	static size_t usedMemory_;
	static TimeStamp lastOpsUpdateTime_;
	static unsigned int index_;
	static int operations_[2];

	static void registerState(LuaStateManager *manager);
	static void unregisterState(LuaStateManager *manager);

	static inline void allocMemory(size_t bytes) { usedMemory_ += bytes; }
	static inline void freeMemory(size_t bytes) { ASSERT(usedMemory_ >= bytes); usedMemory_ -= bytes; }
	static void countOperations();

	friend class LuaStateManager;
};

}

#endif
