#ifndef CLASS_NCINE_LUAUNTRACKEDUSERDATA
#define CLASS_NCINE_LUAUNTRACKEDUSERDATA

#include "common_macros.h"
#include "LuaStateManager.h"
#include "LuaUtils.h"
#include "LuaDebug.h"

namespace ncine {

/// Common methods to retrieve an untracked C++ class in Lua
template <class T>
class LuaUntrackedUserData
{
  public:
	static T *retrieve(lua_State *L, int index) { return retrieve(L, index, RetrieveNull::TRACE); }
	static T *retrieveOrNil(lua_State *L, int index) { return retrieve(L, index, RetrieveNull::ACCEPT); }

	static void push(lua_State *L, T *object);
	static void push(lua_State *L, const T *object);

	static void pushField(lua_State *L, const char *name, T *object);
	static void pushField(lua_State *L, const char *name, const T *object);

  private:
	enum class RetrieveNull
	{
		TRACE,
		ACCEPT
	};

	static T *retrieve(lua_State *L, int index, RetrieveNull unwrapType);
	static T *store(lua_State *L, T *object);
};

template <class T>
T *LuaUntrackedUserData<T>::retrieve(lua_State *L, int index, RetrieveNull unwrapType)
{
	if (LuaUtils::isNil(L, index))
	{
		if (unwrapType == RetrieveNull::TRACE)
			LuaDebug::traceError(L, "Expecting an object instead of a null pointer");

		return nullptr;
	}

	void *pointer = LuaUtils::retrieveUserData<void *>(L, index);

	LuaStateManager *stateManager = LuaStateManager::manager(L);
	LuaTypes::UserDataType type = stateManager->trackedType(pointer);

	if (type == LuaTypes::UNKNOWN)
		type = stateManager->untrackedType(pointer);

	if (type == LuaTypes::UNKNOWN)
		return nullptr;

	T *object = reinterpret_cast<T *>(pointer);
	ASSERT(object != nullptr && type != LuaTypes::UNKNOWN);

	const LuaTypes::UserDataType objectType = LuaTypes::classToUserDataType(object);
	switch (type)
	{
		case LuaTypes::ANIMATED_SPRITE:
			if (objectType != type &&
			    objectType != LuaTypes::SPRITE &&
			    objectType != LuaTypes::BASE_SPRITE &&
			    objectType != LuaTypes::DRAWABLENODE &&
			    objectType != LuaTypes::SCENENODE)
			{
				LuaDebug::traceError(L, "Expecting a %s object instead of a %s", LuaTypes::classToName(object), LuaTypes::userDataTypeToName(type));
				object = nullptr;
			}
			break;
		case LuaTypes::MESH_SPRITE:
		case LuaTypes::SPRITE:
			if (objectType != type &&
			    objectType != LuaTypes::BASE_SPRITE &&
			    objectType != LuaTypes::DRAWABLENODE &&
			    objectType != LuaTypes::SCENENODE)
			{
				LuaDebug::traceError(L, "Expecting a %s object instead of a %s", LuaTypes::classToName(object), LuaTypes::userDataTypeToName(type));
				object = nullptr;
			}
			break;
		case LuaTypes::TEXTNODE:
			if (objectType != type &&
			    objectType != LuaTypes::DRAWABLENODE &&
			    objectType != LuaTypes::SCENENODE)
			{
				LuaDebug::traceError(L, "Expecting a %s object instead of a %s", LuaTypes::classToName(object), LuaTypes::userDataTypeToName(type));
				object = nullptr;
			}
			break;
		case LuaTypes::PARTICLE_SYSTEM:
			if (objectType != type &&
			    objectType != LuaTypes::SCENENODE)
			{
				LuaDebug::traceError(L, "Expecting a %s object instead of a %s", LuaTypes::classToName(object), LuaTypes::userDataTypeToName(type));
				object = nullptr;
			}
			break;
		case LuaTypes::AUDIOBUFFER_PLAYER:
			if (objectType != type &&
			    objectType != LuaTypes::IAUDIOPLAYER)
			{
				LuaDebug::traceError(L, "Expecting a %s object instead of a %s", LuaTypes::classToName(object), LuaTypes::userDataTypeToName(type));
				object = nullptr;
			}
			break;
		case LuaTypes::AUDIOSTREAM_PLAYER:
			if (objectType != type &&
			    objectType != LuaTypes::IAUDIOPLAYER)
			{
				LuaDebug::traceError(L, "Expecting a %s object instead of a %s", LuaTypes::classToName(object), LuaTypes::userDataTypeToName(type));
				object = nullptr;
			}
			break;
		default:
			if (objectType != type)
			{
				LuaDebug::traceError(L, "Expecting a %s object instead of a %s", LuaTypes::classToName(object), LuaTypes::userDataTypeToName(type));
				object = nullptr;
			}
			break;
	}

	return object;
}

template <class T>
void LuaUntrackedUserData<T>::push(lua_State *L, T *object)
{
	if (object != nullptr)
	{
		store(L, object);
		LuaUtils::push(L, reinterpret_cast<void *>(object));
	}
	else
		LuaUtils::pushNil(L);
}

template <class T>
void LuaUntrackedUserData<T>::push(lua_State *L, const T *object)
{
	push(L, const_cast<T *>(object));
}

template <class T>
void LuaUntrackedUserData<T>::pushField(lua_State *L, const char *name, T *object)
{
	if (object != nullptr)
	{
		store(L, object);
		LuaUtils::pushField(L, name, reinterpret_cast<void *>(object));
	}
	else
		LuaUtils::pushFieldNil(L, name);
}

template <class T>
void LuaUntrackedUserData<T>::pushField(lua_State *L, const char *name, const T *object)
{
	pushField(L, name, const_cast<T *>(object));
}

template <class T>
T *LuaUntrackedUserData<T>::store(lua_State *L, T *object)
{
	FATAL_ASSERT(object != nullptr);

	LuaStateManager *stateManager = LuaStateManager::manager(L);

	nctl::HashMap<void *, LuaTypes::UserDataType> &hashMap = stateManager->untrackedUserDatas();
	if (hashMap.loadFactor() >= 0.8f)
		hashMap.rehash(hashMap.capacity() * 2);
	hashMap.insert(object, LuaTypes::classToUserDataType(object));

	return object;
}

}

#endif
