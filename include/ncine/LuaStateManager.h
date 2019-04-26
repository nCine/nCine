#ifndef CLASS_NCINE_LUAMANAGER
#define CLASS_NCINE_LUAMANAGER

#include "common_defines.h"
#include <nctl/Array.h>
#include "LuaTypes.h"

struct lua_State;
struct lua_Debug;

namespace ncine {

/// The Lua scripting state manager
class DLL_PUBLIC LuaStateManager
{
  public:
	enum class ApiType
	{
		FULL,
		EDIT_ONLY,
		CONSTANTS_ONLY,
		NONE
	};

	enum class StatisticsTracking
	{
		ENABLED,
		DISABLED
	};

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

	struct UserDataWrapper
	{
		UserDataWrapper()
		    : object(nullptr), type(LuaTypes::UNKNOWN), arrayIndex(0) {}

		void *object;
		enum LuaTypes::UserDataType type;
		unsigned int arrayIndex;
	};

	LuaStateManager(ApiType apiType, StatisticsTracking statsTracking, StandardLibraries stdLibraries);
	LuaStateManager(lua_State *L, ApiType apiType, StatisticsTracking statsTracking, StandardLibraries stdLibraries);
	~LuaStateManager();

	void releaseTrackedMemory();
	void exposeModuleApi();
	void exposeScriptApi();
	bool run(const char *filename);

	inline lua_State *state() { return L_; }
	inline ApiType apiType() const { return apiType_; }
	inline StatisticsTracking statisticsTracking() const { return statsTracking_; }
	inline StandardLibraries standardLibraries() const { return stdLibraries_; }
	inline nctl::Array<UserDataWrapper> &trackedUserDatas() { return trackedUserDatas_; }
	inline nctl::Array<UserDataWrapper> &untrackedUserDatas() { return untrackedUserDatas_; }

	static LuaStateManager *manager(lua_State *L);

  private:
	static nctl::Array<StateToManager> managers_;

	lua_State *L_;
	ApiType apiType_;
	StatisticsTracking statsTracking_;
	StandardLibraries stdLibraries_;
	nctl::Array<UserDataWrapper> trackedUserDatas_;
	nctl::Array<UserDataWrapper> untrackedUserDatas_;
	/// True if the Lua state should be closed upon destruction
	bool closeOnDestruction_;

	static void *luaAllocator(void *ud, void *ptr, size_t osize, size_t nsize);
	static void *luaAllocatorWithStatistics(void *ud, void *ptr, size_t osize, size_t nsize);
	static void luaCountHook(lua_State *L, lua_Debug *ar);

	void exposeApi();
	void exposeConstants();

	friend class LuaEventHandler;
	friend class LuaStatistics;
};

}

#endif
