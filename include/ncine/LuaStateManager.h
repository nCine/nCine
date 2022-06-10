#ifndef CLASS_NCINE_LUAMANAGER
#define CLASS_NCINE_LUAMANAGER

#include "common_defines.h"
#include <nctl/HashMap.h>
#include "LuaTypes.h"

struct lua_State;
struct lua_Debug;

namespace nctl {
class String;
}

namespace ncine {

namespace LuaUtils {
	class RunInfo;
}

/// The Lua scripting state manager
class DLL_PUBLIC LuaStateManager
{
  public:
	/// The Lua API type to enable
	enum class ApiType
	{
		/// All API methods
		FULL,
		/// All API methods except the ones that create or destroy objects
		EDIT_ONLY,
		/// Only constants, without methods
		CONSTANTS_ONLY,
		/// No Lua API
		NONE
	};

	/// To enable or not memory and instruction count statistics
	enum class StatisticsTracking
	{
		ENABLED,
		DISABLED
	};

	/// To load or not standard Lua system libraries
	enum class StandardLibraries
	{
		LOADED,
		NOT_LOADED
	};

	struct StateToManager
	{
		StateToManager()
		    : luaState(nullptr), stateManager(nullptr) {}
		StateToManager(lua_State *L, LuaStateManager *manager)
		    : luaState(L), stateManager(manager) {}

		lua_State *luaState;
		LuaStateManager *stateManager;
	};

	LuaStateManager(ApiType apiType, StatisticsTracking statsTracking, StandardLibraries stdLibraries);
	LuaStateManager(lua_State *L, ApiType apiType, StatisticsTracking statsTracking, StandardLibraries stdLibraries);
	~LuaStateManager();

	void reopen(ApiType apiType, StatisticsTracking statsTracking, StandardLibraries stdLibraries);
	void reopen();

	/// Loads (without running) a script from a file, with the specified chunk name (used for debug), a string for an error message, and the status code
	bool loadFromFile(const char *filename, const char *chunkName, nctl::String *errorMsg, int *status);
	/// Loads (without running) a script from a file, with the specified chunk name (used for debug), and a string for an error message
	bool loadFromFile(const char *filename, const char *chunkName, nctl::String *errorMsg);
	/// Loads (without running) a script from a file, with the specified chunk name (used for debug)
	bool loadFromFile(const char *filename, const char *chunkName);
	/// Loads (without running) a script from a file
	bool loadFromFile(const char *filename);
	/// Loads (without running) a script from a memory buffer, with a string for an error message, and the status code
	bool loadFromMemory(const char *bufferName, const char *bufferPtr, unsigned long int bufferSize, nctl::String *errorMsg, int *status);
	/// Loads (without running) a script from a memory buffer, with a string for an error message
	bool loadFromMemory(const char *bufferName, const char *bufferPtr, unsigned long int bufferSize, nctl::String *errorMsg);
	/// Loads (without running) a script from a memory buffer
	bool loadFromMemory(const char *bufferName, const char *bufferPtr, unsigned long int bufferSize);

	/// Loads and then runs a script from a file, with the specified chunk name (used for debug), a string for an error message, the status code, and an information object
	bool runFromFile(const char *filename, const char *chunkName, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo);
	/// Loads and then runs a script from a file, with the specified chunk name (used for debug), and a string for an error message
	bool runFromFile(const char *filename, const char *chunkName, nctl::String *errorMsg);
	/// Loads and then runs a script from a file, with the specified chunk name (used for debug)
	bool runFromFile(const char *filename, const char *chunkName);
	/// Loads and then runs a script from a file
	bool runFromFile(const char *filename);
	/// Loads and then runs a script from a memory buffer, with the status code, a string for an error message, the status code and an information object
	bool runFromMemory(const char *bufferName, const char *bufferPtr, unsigned long int bufferSize, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo);
	/// Loads and then runs a script from a memory buffer, with the status code, and a string for an error message
	bool runFromMemory(const char *bufferName, const char *bufferPtr, unsigned long int bufferSize, nctl::String *errorMsg);
	/// Loads and then runs a script from a memory buffer
	bool runFromMemory(const char *bufferName, const char *bufferPtr, unsigned long int bufferSize);

	inline lua_State *state() { return L_; }
	inline ApiType apiType() const { return apiType_; }
	inline StatisticsTracking statisticsTracking() const { return statsTracking_; }
	inline StandardLibraries standardLibraries() const { return stdLibraries_; }

	LuaTypes::UserDataType trackedType(void *pointer) const;
	inline nctl::HashMap<void *, LuaTypes::UserDataType> &trackedUserDatas() { return trackedUserDatas_; }
	LuaTypes::UserDataType untrackedType(void *pointer) const;
	inline nctl::HashMap<void *, LuaTypes::UserDataType> &untrackedUserDatas() { return untrackedUserDatas_; }

	static LuaStateManager *manager(lua_State *L);

  private:
	static nctl::Array<StateToManager> managers_;

	lua_State *L_;
	ApiType apiType_;
	StatisticsTracking statsTracking_;
	StandardLibraries stdLibraries_;
	nctl::HashMap<void *, LuaTypes::UserDataType> trackedUserDatas_;
	nctl::HashMap<void *, LuaTypes::UserDataType> untrackedUserDatas_;
	/// True if the Lua state should be closed upon destruction
	bool closeOnDestruction_;

	static void *luaAllocator(void *ud, void *ptr, size_t osize, size_t nsize);
	static void *luaAllocatorWithStatistics(void *ud, void *ptr, size_t osize, size_t nsize);
	static void luaCountHook(lua_State *L, lua_Debug *ar);

	void init(ApiType apiType, StatisticsTracking statsTracking, StandardLibraries stdLibraries);
	void shutdown();
	void unregisterState();
	void releaseTrackedMemory();

	void exposeScriptApi();
	void exposeModuleApi();
	void exposeApi();
	void exposeConstants();

	friend class LuaEventHandler;
	friend class LuaStatistics;
};

}

#endif
