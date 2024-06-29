#ifndef CLASS_NCINE_LUACLASSTRACKER
#define CLASS_NCINE_LUACLASSTRACKER

#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "common_macros.h"
#include "LuaStateManager.h"
#include "LuaNames.h"

#ifdef WITH_ALLOCATORS
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
	static inline void cloneNode(lua_State *L, const T &other);

  private:
	static inline void storeTrackedUserData(lua_State *L, T *object);
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
#if !defined(WITH_ALLOCATORS)
	T *ptr = new T(nctl::forward<Args>(args)...);
#else
	T *ptr = new (nctl::theLuaAllocator().allocate(sizeof(T))) T(nctl::forward<Args>(args)...);
#endif
	storeTrackedUserData(L, ptr);
}

template <class T>
int LuaClassTracker<T>::deleteObject(lua_State *L)
{
	void *pointer = nullptr;

	if (lua_isuserdata(L, -1))
		pointer = lua_touserdata(L, -1);

	if (pointer != nullptr)
	{
		T *object = reinterpret_cast<T *>(pointer);

		LuaStateManager *stateManager = LuaStateManager::manager(L);

		// A tracked object might also end up among untracked data
		stateManager->untrackedUserDatas().remove(pointer);

		const LuaTypes::UserDataType type = stateManager->trackedType(pointer);
		ASSERT(type == LuaTypes::classToUserDataType(object));
		const bool isTracked = (stateManager->trackedUserDatas().find(pointer) != nullptr);
		ASSERT(isTracked == true);

		if (isTracked)
		{
			stateManager->trackedUserDatas().remove(pointer);
#if !defined(WITH_ALLOCATORS)
			delete object;
#else
			nctl::theLuaAllocator().deleteObject(object);
#endif
		}
	}

	return 0;
}

template <class T>
void LuaClassTracker<T>::cloneNode(lua_State *L, const T &other)
{
#if !defined(WITH_ALLOCATORS)
	T *ptr = new T(nctl::move(other.clone()));
#else
	T *ptr = new (nctl::theLuaAllocator().allocate(sizeof(T))) T(nctl::move(other.clone()));
#endif
	storeTrackedUserData(L, ptr);
}

template <class T>
void LuaClassTracker<T>::storeTrackedUserData(lua_State *L, T *object)
{
	FATAL_ASSERT(object != nullptr);

	LuaStateManager *stateManager = LuaStateManager::manager(L);

	nctl::HashMap<void *, LuaTypes::UserDataType> &hashMap = stateManager->trackedUserDatas();
	if (hashMap.loadFactor() >= 0.8f)
		hashMap.rehash(hashMap.capacity() * 2);
	hashMap.insert(object, LuaTypes::classToUserDataType(object));

	lua_pushlightuserdata(L, reinterpret_cast<void *>(object));
}

}

#endif
