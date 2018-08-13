#ifndef CLASS_NCINE_LUASTATISTICS
#define CLASS_NCINE_LUASTATISTICS

#include "nctl/Array.h"
#include "Timer.h"

namespace nctl {

class String;

}

namespace ncine {

class LuaStateManager;

/// Lua statistics about all active states
class LuaStatistics
{
  public:
	static void appendStatistics(nctl::String &string);

  private:
	static const int OperationsCount = 1000;

	static nctl::Array<LuaStateManager *> managers_;
	static size_t usedMemory_;
	static Timer timer_;
	static unsigned int index_;
	static int operations_[2];

	static void registerState(LuaStateManager *manager);
	static void unregisterState(LuaStateManager *manager);

	static inline void allocMemory(size_t bytes) { usedMemory_ += bytes; }
	static inline void freeMemory(size_t bytes) { usedMemory_ -= bytes; }
	static void countOperations();

	friend class LuaStateManager;
};

}

#endif
