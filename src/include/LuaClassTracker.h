#ifndef CLASS_NCINE_LUACLASSTRACKER
#define CLASS_NCINE_LUACLASSTRACKER

#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "common_macros.h"
#include "LuaStateManager.h"
#include "LuaNames.h"

#include <ncine/config.h>
#if NCINE_WITH_ALLOCATORS
	#include <nctl/AllocManager.h>
	#include <nctl/IAllocator.h>
#endif

namespace ncine {

/// Common methods to track a C++ class in Lua
template <class T>
class LuaClassTracker
{
  public:
	static inline void exposeDelete(lua_State *L);

	template <typename... Args>
	static inline void newObject(lua_State *L, Args &&... args);
	static inline int deleteObject(lua_State *L);

  private:
	static inline void wrapTrackedUserData(lua_State *L, T *object);
};

template <class T>
void LuaClassTracker<T>::exposeDelete(lua_State *L)
{
	lua_pushcfunction(L, deleteObject);
	lua_setfield(L, -2, LuaNames::deleteObject);
}

template <class T>
template <typename... Args>
void LuaClassTracker<T>::newObject(lua_State *L, Args &&... args)
{
#if !NCINE_WITH_ALLOCATORS
	T *ptr = new T(nctl::forward<Args>(args)...);
#else
	T *ptr = new (nctl::theLuaAllocator().allocate(sizeof(T))) T(nctl::forward<Args>(args)...);
#endif
	wrapTrackedUserData(L, ptr);
}

template <class T>
int LuaClassTracker<T>::deleteObject(lua_State *L)
{
	LuaStateManager *stateManager = LuaStateManager::manager(L);

	LuaStateManager::UserDataWrapper *wrapper = reinterpret_cast<LuaStateManager::UserDataWrapper *>(lua_touserdata(L, -1));

	T *object = reinterpret_cast<T *>(wrapper->object);
	FATAL_ASSERT(object);
	FATAL_ASSERT(wrapper->type == LuaTypes::classToUserDataType(object));

	nctl::Array<LuaStateManager::UserDataWrapper> &array = stateManager->trackedUserDatas();
	const unsigned int newSize = array.size() - 1;

	// If not deleting the last element of the array
	if (wrapper->arrayIndex != newSize)
	{
		array[wrapper->arrayIndex].object = array[newSize].object;
		array[wrapper->arrayIndex].type = array[newSize].type;
		// The `arrayIndex` should not be copied
	}

	array.setSize(newSize);
#if !NCINE_WITH_ALLOCATORS
	delete object;
#else
	nctl::theLuaAllocator().deleteObject(object);
#endif

	return 0;
}

template <class T>
void LuaClassTracker<T>::wrapTrackedUserData(lua_State *L, T *object)
{
	LuaStateManager *stateManager = LuaStateManager::manager(L);

	nctl::Array<LuaStateManager::UserDataWrapper> &array = stateManager->trackedUserDatas();

	const unsigned int oldSize = array.size();
	array.setSize(oldSize + 1);
	LuaStateManager::UserDataWrapper &wrapper = array[oldSize];

	wrapper.object = object;
	wrapper.type = LuaTypes::classToUserDataType(object);
	wrapper.arrayIndex = oldSize;

	lua_pushlightuserdata(L, reinterpret_cast<void *>(&wrapper));
}

}

#endif
