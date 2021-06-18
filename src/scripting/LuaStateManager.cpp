#define NCINE_INCLUDE_LUA
#include "common_headers.h"
#include "common_macros.h"
#include <nctl/CString.h>

#include "LuaStateManager.h"
#include "LuaDebug.h"
#include "LuaStatistics.h"
#include "LuaNames.h"

#include "LuaRect.h"
#include "LuaVector2.h"
#include "LuaVector3.h"
#include "LuaColor.h"
#include "LuaILogger.h"

#include "LuaIInputManager.h"
#include "LuaMouseEvents.h"
#include "LuaKeyboardEvents.h"
#include "LuaKeys.h"
#include "LuaJoystickEvents.h"

#include "LuaTimeStamp.h"
#include "LuaFileSystem.h"
#include "LuaApplication.h"
#include "LuaTexture.h"
#include "LuaSceneNode.h"
#include "LuaSprite.h"
#include "LuaMeshSprite.h"
#include "LuaAnimatedSprite.h"
#include "LuaFont.h"
#include "LuaTextNode.h"
#include "LuaDrawableNode.h"
#include "LuaRectAnimation.h"
#include "LuaParticleSystem.h"

#ifdef WITH_AUDIO
	#include "LuaIAudioDevice.h"
	#include "LuaAudioBuffer.h"
	#include "LuaAudioBufferPlayer.h"
	#include "LuaAudioStreamPlayer.h"
#endif

#include "Application.h"
#include <cstring> // for memchr()
#include "IFile.h"

#include <ncine/config.h>
#if NCINE_WITH_ALLOCATORS
	#include <nctl/AllocManager.h>
	#include <nctl/IAllocator.h>
#endif

#ifdef WITH_TRACY
	#include "TracyLua.hpp"
#endif

#ifdef WITH_GIT_VERSION
	#include "version.h"
#endif

namespace ncine {

namespace LuaNames {
#ifdef WITH_GIT_VERSION
	static const char *Version = "_VERSION";
	static const char *GitRevCount = "_GITREVCOUNT";
	static const char *GitShortHash = "_GITSHORTHASH";
	static const char *GitLastCommitDate = "_GITLATCOMMITDATE";
	static const char *GitBranch = "_GITBRANCH";
	static const char *GitTag = "_GITTAG";
#endif

#if defined(_WIN32)
	#if defined(__MINGW32__)
	static const char *MinGW = "MINGW";
	#else
	static const char *Windows = "WINDOWS";
	#endif
#elif defined(__APPLE__)
	static const char *macOS = "MACOS";
#elif defined(__ANDROID__)
	static const char *Android = "ANDROID";
#elif defined(__EMSCRIPTEN__)
	static const char *Emscripten = "EMSCRIPTEN";
#else
	static const char *Linux = "LINUX";
#endif

#ifdef NCINE_DEBUG
	static const char *Debug = "DEBUG";
#endif
}

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

nctl::Array<LuaStateManager::StateToManager> LuaStateManager::managers_(2);

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

LuaStateManager::LuaStateManager(ApiType apiType, StatisticsTracking statsTracking, StandardLibraries stdLibraries)
    : LuaStateManager(lua_newstate(statsTracking == StatisticsTracking::ENABLED ? luaAllocatorWithStatistics : luaAllocator, nullptr),
                      apiType, statsTracking, stdLibraries)
{
	closeOnDestruction_ = true;
}

LuaStateManager::LuaStateManager(lua_State *L, ApiType apiType, StatisticsTracking statsTracking, StandardLibraries stdLibraries)
    : L_(L), apiType_(apiType), statsTracking_(statsTracking), stdLibraries_(stdLibraries),
      trackedUserDatas_(apiType == ApiType::FULL ? 16 : 0),
      untrackedUserDatas_(16), closeOnDestruction_(false)
{
	ASSERT(L_);
	init(apiType, statsTracking, stdLibraries);
}

LuaStateManager::~LuaStateManager()
{
	shutdown();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaStateManager::reopen(ApiType apiType, StatisticsTracking statsTracking, StandardLibraries stdLibraries)
{
	shutdown();

	L_ = lua_newstate(statsTracking == StatisticsTracking::ENABLED ? luaAllocatorWithStatistics : luaAllocator, nullptr);
	init(apiType, statsTracking, stdLibraries);
}

void LuaStateManager::reopen()
{
	reopen(apiType_, statsTracking_, stdLibraries_);
}

bool LuaStateManager::loadFromFile(const char *filename, const char *chunkName, nctl::String *errorMsg, int *status)
{
	nctl::UniquePtr<IFile> fileHandle = IFile::createFileHandle(filename);
	fileHandle->setExitOnFailToOpen(false);
	LOGI_X("Loading file: \"%s\"", fileHandle->filename());

	fileHandle->open(IFile::OpenMode::READ | IFile::OpenMode::BINARY);
	if (fileHandle->isOpened() == false)
		return false;

	const unsigned long fileSize = fileHandle->size();
	nctl::UniquePtr<char[]> buffer = nctl::makeUnique<char[]>(fileSize);
	fileHandle->read(buffer.get(), fileSize);

	return loadFromMemory(chunkName, buffer.get(), fileSize, errorMsg, status);
}

bool LuaStateManager::loadFromFile(const char *filename, const char *chunkName, nctl::String *errorMsg)
{
	return loadFromFile(filename, chunkName, errorMsg, nullptr);
}

bool LuaStateManager::loadFromFile(const char *filename, const char *chunkName)
{
	return loadFromFile(filename, chunkName, nullptr, nullptr);
}

bool LuaStateManager::loadFromFile(const char *filename)
{
	return loadFromFile(filename, filename, nullptr, nullptr);
}

bool LuaStateManager::loadFromMemory(const char *bufferName, const char *bufferPtr, unsigned long int bufferSize, nctl::String *errorMsg, int *status)
{
	if (apiType_ == ApiType::FULL)
		releaseTrackedMemory();
	untrackedUserDatas_.clear();

	const char *bufferRead = bufferPtr;

	// Skip shebang as `luaL_loadfile` does
	if (bufferRead[0] == '#')
	{
		bufferRead = static_cast<const char *>(memchr(bufferRead, '\n', bufferSize)) + 1;
		bufferSize -= bufferRead - bufferPtr;
	}

	const int loadStatus = luaL_loadbufferx(L_, bufferRead, bufferSize, bufferName, "bt");
	if (loadStatus != LUA_OK)
	{
		LOGE_X("Error loading Lua script \"%s\" (%s):\n%s", bufferName, LuaDebug::statusToString(loadStatus), lua_tostring(L_, -1));
		if (errorMsg)
			*errorMsg = lua_tostring(L_, -1);
		if (status)
			*status = loadStatus;
		LuaUtils::pop(L_);
		return false;
	}

	return true;
}

bool LuaStateManager::loadFromMemory(const char *bufferName, const char *bufferPtr, unsigned long int bufferSize, nctl::String *errorMsg)
{
	return loadFromMemory(bufferName, bufferPtr, bufferSize, errorMsg, nullptr);
}

bool LuaStateManager::loadFromMemory(const char *bufferName, const char *bufferPtr, unsigned long int bufferSize)
{
	return loadFromMemory(bufferName, bufferPtr, bufferSize, nullptr, nullptr);
}

bool LuaStateManager::runFromFile(const char *filename, const char *chunkName, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo)
{
	const bool hasLoaded = loadFromFile(filename, chunkName, errorMsg, status);
	if (hasLoaded == false)
		return false;

	const int callStatus = LuaUtils::pcall(L_, 0, LUA_MULTRET, runInfo);
	if (callStatus != LUA_OK)
	{
		LOGE_X("Error running Lua script \"%s\" (%s):\n%s", filename, LuaDebug::statusToString(callStatus), lua_tostring(L_, -1));
		if (errorMsg)
			*errorMsg = lua_tostring(L_, -1);
		if (status)
			*status = callStatus;
		LuaUtils::pop(L_);
		return false;
	}

	return true;
}

bool LuaStateManager::runFromFile(const char *filename, const char *chunkName, nctl::String *errorMsg)
{
	return runFromFile(filename, chunkName, errorMsg, nullptr, nullptr);
}

bool LuaStateManager::runFromFile(const char *filename, const char *chunkName)
{
	return runFromFile(filename, chunkName, nullptr, nullptr, nullptr);
}

bool LuaStateManager::runFromFile(const char *filename)
{
	return runFromFile(filename, filename, nullptr, nullptr, nullptr);
}

bool LuaStateManager::runFromMemory(const char *bufferName, const char *bufferPtr, unsigned long int bufferSize, nctl::String *errorMsg, int *status, LuaUtils::RunInfo *runInfo)
{
	const bool hasLoaded = loadFromMemory(bufferName, bufferPtr, bufferSize, errorMsg, status);
	if (hasLoaded == false)
		return false;

	const int callStatus = LuaUtils::pcall(L_, 0, LUA_MULTRET, runInfo);
	if (callStatus != LUA_OK)
	{
		LOGE_X("Error running Lua script \"%s\" (%s):\n%s", bufferName, LuaDebug::statusToString(callStatus), lua_tostring(L_, -1));
		if (errorMsg)
			*errorMsg = lua_tostring(L_, -1);
		if (status)
			*status = callStatus;
		LuaUtils::pop(L_);
		return false;
	}

	return true;
}

bool LuaStateManager::runFromMemory(const char *bufferName, const char *bufferPtr, unsigned long int bufferSize, nctl::String *errorMsg)
{
	return runFromMemory(bufferName, bufferPtr, bufferSize, errorMsg, nullptr, nullptr);
}

bool LuaStateManager::runFromMemory(const char *bufferName, const char *bufferPtr, unsigned long int bufferSize)
{
	return runFromMemory(bufferName, bufferPtr, bufferSize, nullptr, nullptr, nullptr);
}

LuaStateManager *LuaStateManager::manager(lua_State *L)
{
	LuaStateManager *stateManager = nullptr;

	for (const StateToManager &manager : managers_)
	{
		if (manager.luaState == L)
		{
			stateManager = manager.stateManager;
			break;
		}
	}

	FATAL_ASSERT(stateManager != nullptr);
	return stateManager;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void *LuaStateManager::luaAllocator(void *ud, void *ptr, size_t osize, size_t nsize)
{
	if (nsize == 0)
	{
#if !NCINE_WITH_ALLOCATORS
		free(ptr);
#else
		nctl::theLuaAllocator().deallocate(ptr);
#endif
		return nullptr;
	}
	else
	{
#if !NCINE_WITH_ALLOCATORS
		return realloc(ptr, nsize);
#else
		return nctl::theLuaAllocator().reallocate(ptr, nsize);
#endif
	}
}

void *LuaStateManager::luaAllocatorWithStatistics(void *ud, void *ptr, size_t osize, size_t nsize)
{
	if (nsize == 0)
	{
		LuaStatistics::freeMemory(osize);
#if !NCINE_WITH_ALLOCATORS
		free(ptr);
#else
		nctl::theLuaAllocator().deallocate(ptr);
#endif
		return nullptr;
	}
	else
	{
		LuaStatistics::allocMemory(ptr != nullptr ? (nsize - osize) : nsize);
#if !NCINE_WITH_ALLOCATORS
		return realloc(ptr, nsize);
#else
		return nctl::theLuaAllocator().reallocate(ptr, nsize);
#endif
	}
}

void LuaStateManager::luaCountHook(lua_State *L, lua_Debug *ar)
{
	if (ar->event == LUA_HOOKCOUNT)
		LuaStatistics::countOperations();
}

void LuaStateManager::init(ApiType apiType, StatisticsTracking statsTracking, StandardLibraries stdLibraries)
{
	if (stdLibraries == StandardLibraries::LOADED)
		luaL_openlibs(L_);
	managers_.pushBack(StateToManager(L_, this));

	if (statsTracking == StatisticsTracking::ENABLED)
	{
		LuaStatistics::registerState(this);
		lua_sethook(L_, luaCountHook, LUA_MASKCOUNT, LuaStatistics::OperationsCount);
	}

#ifdef WITH_TRACY
	tracy::LuaRegister(L_);
#endif

	apiType_ = apiType;
	statsTracking_ = statsTracking;
	stdLibraries_ = stdLibraries;

	exposeScriptApi();
}

void LuaStateManager::shutdown()
{
	unregisterState();

	if (statsTracking_ == StatisticsTracking::ENABLED)
		LuaStatistics::unregisterState(this);

	if (apiType_ == ApiType::FULL)
		releaseTrackedMemory();

	if (closeOnDestruction_)
		lua_close(L_);
}

void LuaStateManager::unregisterState()
{
	int index = -1;

	for (unsigned int i = 0; i < managers_.size(); i++)
	{
		if (managers_[i].luaState == L_ && managers_[i].stateManager == this)
		{
			index = static_cast<int>(i);
			break;
		}
	}

	ASSERT(index != -1);
	if (index != -1)
		managers_.unorderedRemoveAt(index);
}

void LuaStateManager::releaseTrackedMemory()
{
	if (trackedUserDatas_.isEmpty() == false)
		LOGW_X("Lua array of tracked userdata is not empty: %d elements", trackedUserDatas_.size());

	for (unsigned int i = 0; i < trackedUserDatas_.size(); i++)
	{
		UserDataWrapper &wrapper = trackedUserDatas_[i];
		ASSERT(wrapper.arrayIndex == i);

		switch (wrapper.type)
		{
			case LuaTypes::TEXTURE:
			{
				LuaTexture::release(wrapper.object);
				break;
			}
			case LuaTypes::FONT:
			{
				LuaFont::release(wrapper.object);
				break;
			}
			case LuaTypes::SCENENODE:
			{
				LuaSceneNode::release(wrapper.object);
				break;
			}
			case LuaTypes::SPRITE:
			{
				LuaSprite::release(wrapper.object);
				break;
			}
			case LuaTypes::MESH_SPRITE:
			{
				LuaMeshSprite::release(wrapper.object);
				break;
			}
			case LuaTypes::ANIMATED_SPRITE:
			{
				LuaAnimatedSprite::release(wrapper.object);
				break;
			}
			case LuaTypes::TEXTNODE:
			{
				LuaTextNode::release(wrapper.object);
				break;
			}
#ifdef WITH_AUDIO
			case LuaTypes::AUDIOBUFFER:
			{
				LuaAudioBuffer::release(wrapper.object);
				break;
			}
			case LuaTypes::AUDIOBUFFER_PLAYER:
			{
				LuaAudioBufferPlayer::release(wrapper.object);
				break;
			}
			case LuaTypes::AUDIOSTREAM_PLAYER:
			{
				LuaAudioStreamPlayer::release(wrapper.object);
				break;
			}
#endif
			case LuaTypes::PARTICLE_SYSTEM:
			{
				LuaParticleSystem::release(wrapper.object);
				break;
			}
			case LuaTypes::UNKNOWN:
			default:
				FATAL_MSG("Unsupported Lua wrapped userdata");
				break;
		}

		wrapper.object = nullptr;
	}
	trackedUserDatas_.clear();
}

void LuaStateManager::exposeScriptApi()
{
	if (apiType_ != ApiType::NONE)
	{
		exposeModuleApi();
		lua_setglobal(L_, LuaNames::ncine);
	}
}

void LuaStateManager::exposeModuleApi()
{
	if (apiType_ != ApiType::NONE)
	{
		lua_settop(L_, 0);
		lua_newtable(L_);
		exposeConstants();
		if (apiType_ != ApiType::CONSTANTS_ONLY)
			exposeApi();
	}
}

void LuaStateManager::exposeApi()
{
	const AppConfiguration &appCfg = theApplication().appConfiguration();

	LuaRect<float>::expose(L_);
	LuaVector2<float>::expose(L_);
	LuaVector3<float>::expose(L_);
	LuaColor::expose(L_);
	LuaILogger::expose(L_);
	LuaIInputManager::expose(L_);
	LuaKeyboardEvents::expose(L_);
	LuaJoystickEvents::expose(L_);

	LuaTimeStamp::expose(L_);
	LuaFileSystem::expose(L_);
	LuaApplication::expose(L_);
	if (appCfg.withScenegraph)
	{
		LuaTexture::expose(this);
		LuaSceneNode::expose(this);
		LuaSprite::expose(this);
		LuaMeshSprite::expose(this);
		LuaAnimatedSprite::expose(this);
		LuaFont::expose(this);
		LuaTextNode::expose(this);
		LuaParticleSystem::expose(this);
	}

#ifdef WITH_AUDIO
	if (appCfg.withAudio)
	{
		LuaIAudioDevice::expose(L_);
		LuaAudioBuffer::expose(this);
		LuaAudioBufferPlayer::expose(this);
		LuaAudioStreamPlayer::expose(this);
	}
#endif
}

void LuaStateManager::exposeConstants()
{
	const AppConfiguration &appCfg = theApplication().appConfiguration();

	LuaColor::exposeConstants(L_);
	LuaMouseEvents::exposeConstants(L_);
	LuaKeys::exposeConstants(L_);
	LuaJoystickEvents::exposeConstants(L_);
	LuaIInputManager::exposeConstants(L_);
	LuaILogger::exposeConstants(L_);

	LuaFileSystem::exposeConstants(L_);
	if (appCfg.withScenegraph)
	{
		LuaTexture::exposeConstants(L_);
		LuaDrawableNode::exposeConstants(L_);
		LuaRectAnimation::exposeConstants(L_);
		LuaMeshSprite::exposeConstants(L_);
		LuaTextNode::exposeConstants(L_);
	}

#ifdef WITH_GIT_VERSION
	lua_pushstring(L_, VersionStrings::Version);
	lua_setfield(L_, -2, LuaNames::Version);
	lua_pushstring(L_, VersionStrings::GitRevCount);
	lua_setfield(L_, -2, LuaNames::GitRevCount);
	lua_pushstring(L_, VersionStrings::GitShortHash);
	lua_setfield(L_, -2, LuaNames::GitShortHash);
	lua_pushstring(L_, VersionStrings::GitLastCommitDate);
	lua_setfield(L_, -2, LuaNames::GitLastCommitDate);
	lua_pushstring(L_, VersionStrings::GitBranch);
	lua_setfield(L_, -2, LuaNames::GitBranch);
	lua_pushstring(L_, VersionStrings::GitTag);
	lua_setfield(L_, -2, LuaNames::GitTag);
#endif

	lua_pushboolean(L_, true);
#if defined(_WIN32)
	#if defined(__MINGW32__)
	lua_setfield(L_, -2, LuaNames::MinGW);
	#else
	lua_setfield(L_, -2, LuaNames::Windows);
	#endif
#elif defined(__APPLE__)
	lua_setfield(L_, -2, LuaNames::macOS);
#elif defined(__ANDROID__)
	lua_setfield(L_, -2, LuaNames::Android);
#elif defined(__EMSCRIPTEN__)
	lua_setfield(L_, -2, LuaNames::Emscripten);
#else
	lua_setfield(L_, -2, LuaNames::Linux);
#endif

#ifdef NCINE_DEBUG
	lua_pushboolean(L_, true);
	lua_setfield(L_, -2, LuaNames::Debug);
#endif
}

}
