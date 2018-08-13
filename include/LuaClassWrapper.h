#ifndef CLASS_NCINE_LUACLASSWRAPPER
#define CLASS_NCINE_LUACLASSWRAPPER

#include "common_macros.h"
#include "LuaStateManager.h"
#include "LuaUtils.h"
#include "LuaDebug.h"

namespace ncine {

/// Lua common methods to unwrap a C++ class
template <class T>
class LuaClassWrapper
{
  public:
	static T *unwrapUserData(lua_State *L, int index) { return unwrapUserData(L, index, UnwrapType::ASSERT_NULL); }
	static T *unwrapUserDataOrNil(lua_State *L, int index) { return unwrapUserData(L, index, UnwrapType::RETURN_NULL); }

	static void pushUntrackedUserData(lua_State *L, T *object);
	static void pushUntrackedUserData(lua_State *L, const T *object);

	static void pushFieldUntrackedUserData(lua_State *L, const char *name, T *object);
	static void pushFieldUntrackedUserData(lua_State *L, const char *name, const T *object);

  private:
	enum class UnwrapType
	{
		ASSERT_NULL,
		RETURN_NULL
	};

	static T *unwrapUserData(lua_State *L, int index, UnwrapType unwrapType);
	static LuaStateManager::UserDataWrapper &wrapUserData(lua_State *L, T *object);
};

template <class T>
T *LuaClassWrapper<T>::unwrapUserData(lua_State *L, int index, UnwrapType unwrapType)
{
	if (unwrapType == UnwrapType::RETURN_NULL && LuaUtils::isNil(L, index))
		return nullptr;

	LuaStateManager::UserDataWrapper *wrapper = LuaUtils::retrieveUserData<LuaStateManager::UserDataWrapper>(L, index);

	T *object = reinterpret_cast<T *>(wrapper->object);
	FATAL_ASSERT(object);

	switch (wrapper->type)
	{
		case LuaTypes::ANIMATED_SPRITE:
			if (LuaTypes::classToUserDataType(object) != wrapper->type &&
			    LuaTypes::classToUserDataType(object) != LuaTypes::SPRITE &&
			    LuaTypes::classToUserDataType(object) != LuaTypes::BASE_SPRITE &&
			    LuaTypes::classToUserDataType(object) != LuaTypes::DRAWABLENODE &&
			    LuaTypes::classToUserDataType(object) != LuaTypes::SCENENODE)
				LuaDebug::traceError(L, "Expecting a %s object instead of %s", LuaTypes::classToName(object), LuaTypes::wrapperToName(wrapper->type));
			break;
		case LuaTypes::MESH_SPRITE:
		case LuaTypes::SPRITE:
			if (LuaTypes::classToUserDataType(object) != wrapper->type &&
			    LuaTypes::classToUserDataType(object) != LuaTypes::BASE_SPRITE &&
			    LuaTypes::classToUserDataType(object) != LuaTypes::DRAWABLENODE &&
			    LuaTypes::classToUserDataType(object) != LuaTypes::SCENENODE)
				LuaDebug::traceError(L, "Expecting a %s object instead of %s", LuaTypes::classToName(object), LuaTypes::wrapperToName(wrapper->type));
			break;
		case LuaTypes::TEXTNODE:
			if (LuaTypes::classToUserDataType(object) != wrapper->type &&
			    LuaTypes::classToUserDataType(object) != LuaTypes::DRAWABLENODE &&
			    LuaTypes::classToUserDataType(object) != LuaTypes::SCENENODE)
				LuaDebug::traceError(L, "Expecting a %s object instead of %s", LuaTypes::classToName(object), LuaTypes::wrapperToName(wrapper->type));
			break;
		case LuaTypes::PARTICLE_SYSTEM:
			if (LuaTypes::classToUserDataType(object) != wrapper->type &&
			    LuaTypes::classToUserDataType(object) != LuaTypes::SCENENODE)
				LuaDebug::traceError(L, "Expecting a %s object instead of %s", LuaTypes::classToName(object), LuaTypes::wrapperToName(wrapper->type));
			break;
		case LuaTypes::AUDIOBUFFER_PLAYER:
			if (LuaTypes::classToUserDataType(object) != wrapper->type &&
			    LuaTypes::classToUserDataType(object) != LuaTypes::IAUDIOPLAYER)
				LuaDebug::traceError(L, "Expecting a %s object instead of %s", LuaTypes::classToName(object), LuaTypes::wrapperToName(wrapper->type));
			break;
		case LuaTypes::AUDIOSTREAM_PLAYER:
			if (LuaTypes::classToUserDataType(object) != wrapper->type &&
			    LuaTypes::classToUserDataType(object) != LuaTypes::IAUDIOPLAYER)
				LuaDebug::traceError(L, "Expecting a %s object instead of %s", LuaTypes::classToName(object), LuaTypes::wrapperToName(wrapper->type));
			break;
		default:
			if (LuaTypes::classToUserDataType(object) != wrapper->type)
				LuaDebug::traceError(L, "Expecting a %s object instead of %s", LuaTypes::classToName(object), LuaTypes::wrapperToName(wrapper->type));
			break;
	}

	return object;
}

template <class T>
void LuaClassWrapper<T>::pushUntrackedUserData(lua_State *L, T *object)
{
	LuaStateManager::UserDataWrapper &wrapper = wrapUserData(L, object);
	LuaUtils::push(L, reinterpret_cast<void *>(&wrapper));
}

template <class T>
void LuaClassWrapper<T>::pushUntrackedUserData(lua_State *L, const T *object)
{
	pushUntrackedUserData(L, object);
}

template <class T>
void LuaClassWrapper<T>::pushFieldUntrackedUserData(lua_State *L, const char *name, T *object)
{
	LuaStateManager::UserDataWrapper &wrapper = wrapUserData(L, object);
	LuaUtils::pushField(L, name, reinterpret_cast<void *>(&wrapper));
}

template <class T>
void LuaClassWrapper<T>::pushFieldUntrackedUserData(lua_State *L, const char *name, const T *object)
{
	pushFieldUntrackedUserData(L, name, object);
}

template <class T>
LuaStateManager::UserDataWrapper &LuaClassWrapper<T>::wrapUserData(lua_State *L, T *object)
{
	LuaStateManager *stateManager = LuaStateManager::manager(L);

	nctl::Array<LuaStateManager::UserDataWrapper> &array = stateManager->untrackedUserDatas();

	const unsigned int oldSize = array.size();
	array.setSize(oldSize + 1);
	LuaStateManager::UserDataWrapper &wrapper = array[oldSize];

	wrapper.object = object;
	wrapper.type = LuaTypes::classToUserDataType(object);
	wrapper.arrayIndex = oldSize;

	return wrapper;
}

}

#endif
