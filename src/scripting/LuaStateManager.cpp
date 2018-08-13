#define NCINE_INCLUDE_LUA
#include "common_headers.h"
#include "common_macros.h"

#include "LuaStateManager.h"
#include "LuaDebug.h"
#include "LuaStatistics.h"
#include "LuaNames.h"

#include "LuaVector2.h"
#include "LuaVector3.h"
#include "LuaColor.h"
#include "LuaILogger.h"

#include "LuaIInputManager.h"
#include "LuaMouseEvents.h"
#include "LuaKeyboardEvents.h"
#include "LuaKeys.h"
#include "LuaJoystickEvents.h"

#include "LuaApplication.h"
#include "LuaTexture.h"
#include "LuaSprite.h"
#include "LuaMeshSprite.h"
#include "LuaAnimatedSprite.h"
#include "LuaFont.h"
#include "LuaTextNode.h"
#include "LuaDrawableNode.h"
#include "LuaRectAnimation.h"
#include "LuaParticleSystem.h"

#ifdef WITH_AUDIO
#include "LuaAudioBuffer.h"
#include "LuaAudioBufferPlayer.h"
#include "LuaAudioStreamPlayer.h"
#endif

#include "Application.h"
#include <cstring> // for memchr()
#include "IFile.h"

#ifdef WITH_GIT_VERSION
	#include "version.h"
#endif

namespace ncine {

namespace LuaNames
{
	static const char *Version = "_VERSION";
	static const char *GitRevCount = "_GITREVCOUNT";
	static const char *GitShortHash = "_GITSHORTHASH";
	static const char *GitLastCommitDate = "_GITLATCOMMITDATE";
	static const char *GitBranch = "_GITBRANCH";

	static const char *WINDOWS = "WINDOWS";
	static const char *MACOS = "MACOS";
	static const char *ANDROID = "ANDROID";
	static const char *LINUX = "LINUX";

	static const char *DEBUG = "DEBUG";
}

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

nctl::Array<LuaStateManager::StateToManager> LuaStateManager::managers_(2);

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

LuaStateManager::LuaStateManager(ApiType apiType, StatisticsTracking statsTracking, StandardLibraries stdLibraries)
	: LuaStateManager(lua_newstate(statsTracking == StatisticsTracking::ENABLED ?
	                  luaAllocatorWithStatistics : luaAllocator, nullptr),
	  apiType, statsTracking, stdLibraries)
{
	closeOnDestruction_ = true;
}

LuaStateManager::LuaStateManager(lua_State *L, ApiType apiType, StatisticsTracking statsTracking, StandardLibraries stdLibraries)
	: L_(L), apiType_(apiType), statsTracking_(statsTracking), stdLibraries_(stdLibraries),
	  trackedUserDatas_(apiType == ApiType::FULL ? 16 : 1),
	  untrackedUserDatas_(16), closeOnDestruction_(false)
{
	ASSERT(L_);
	if (stdLibraries == StandardLibraries::LOADED)
		luaL_openlibs(L_);
	managers_.pushBack(StateToManager(L_, this));

	if (statsTracking == StatisticsTracking::ENABLED)
	{
		LuaStatistics::registerState(this);
		lua_sethook(L_, luaCountHook, LUA_MASKCOUNT, LuaStatistics::OperationsCount);
	}
}

LuaStateManager::~LuaStateManager()
{
	if (statsTracking_ == StatisticsTracking::ENABLED)
		LuaStatistics::unregisterState(this);

	if (apiType_ == ApiType::FULL)
		releaseTrackedMemory();

	if (closeOnDestruction_)
		lua_close(L_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

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

void LuaStateManager::exposeScriptApi()
{
	if (apiType_ != ApiType::NONE)
	{
		exposeModuleApi();
		lua_setglobal(L_, LuaNames::ncine);
	}
}

bool LuaStateManager::run(const char *filename)
{
	releaseTrackedMemory();
	untrackedUserDatas_.clear();

	nctl::UniquePtr<IFile> fileHandle = IFile::createFileHandle(filename);
	LOGI_X("Loading file: \"%s\"", fileHandle->filename());

	fileHandle->open(IFile::OpenMode::READ | IFile::OpenMode::BINARY);
	unsigned long fileSize = fileHandle->size();
	nctl::UniquePtr<char []> buffer = nctl::makeUnique<char []>(fileSize);
	fileHandle->read(buffer.get(), fileSize);
	const char *bufferRead = buffer.get();

	// Skip shebang as `luaL_loadfile` does
	if (bufferRead[0] == '#')
	{
		bufferRead = static_cast<const char *>(memchr(bufferRead, '\n', fileSize)) + 1;
		fileSize -= bufferRead - buffer.get();
	}

	const int loadError = luaL_loadbufferx(L_, bufferRead, fileSize, filename, "bt");
	if (loadError != LUA_OK)
	{
		LOGE_X("Cannot load \"%s\" script: %s", filename, LuaDebug::errorToSting(loadError));
		return false;
	}

	const int callError = lua_pcall(L_, 0, LUA_MULTRET, 0);
	if (callError != LUA_OK)
	{
		LOGE_X("Cannot run \"%s\" script: %s", filename, LuaDebug::errorToSting(callError));
		return false;
	}

	return true;
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
		free(ptr);
		return nullptr;
	}
	else
		return realloc(ptr, nsize);
}

void *LuaStateManager::luaAllocatorWithStatistics(void *ud, void *ptr, size_t osize, size_t nsize)
{
	if (nsize == 0)
	{
		LuaStatistics::freeMemory(osize);
		free(ptr);
		return nullptr;
	}
	else
	{
		LuaStatistics::allocMemory(nsize - osize);
		return realloc(ptr, nsize);
	}
}

void LuaStateManager::luaCountHook(lua_State *L, lua_Debug *ar)
{
	if (ar->event == LUA_HOOKCOUNT)
		LuaStatistics::countOperations();
}

void LuaStateManager::exposeApi()
{
	const AppConfiguration &appCfg = theApplication().appConfiguration();

	LuaVector2<float>::expose(L_);
	LuaVector3<float>::expose(L_);
	LuaColor::expose(L_);
	LuaILogger::expose(L_);
	LuaIInputManager::expose(L_);
	LuaKeyboardEvents::expose(L_);
	LuaJoystickEvents::expose(L_);

	LuaApplication::expose(L_);
	if (appCfg.withScenegraph())
	{
		LuaTexture::expose(this);
		LuaSprite::expose(this);
		LuaMeshSprite::expose(this);
		LuaAnimatedSprite::expose(this);
		LuaFont::expose(this);
		LuaTextNode::expose(this);
		LuaParticleSystem::expose(this);
	}

#ifdef WITH_AUDIO
	if (appCfg.withAudio())
	{
		LuaAudioBuffer::expose(this);
		LuaAudioBufferPlayer::expose(this);
		LuaAudioStreamPlayer::expose(this);
	}
#endif
}

void LuaStateManager::exposeConstants()
{
	const AppConfiguration &appCfg = theApplication().appConfiguration();

	LuaMouseEvents::exposeConstants(L_);
	LuaKeys::exposeConstants(L_);
	LuaJoystickEvents::exposeConstants(L_);
	LuaIInputManager::exposeConstants(L_);
	LuaILogger::exposeConstants(L_);

	if (appCfg.withScenegraph())
	{
		LuaDrawableNode::exposeConstants(L_);
		LuaRectAnimation::exposeConstants(L_);
		LuaMeshSprite::exposeConstants(L_);
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
#endif

	lua_pushboolean(L_, true);
#if defined(_WIN32)
	lua_setfield(L_, -2, LuaNames::WINDOWS);
#elif defined(__APPLE__)
	lua_setfield(L_, -2, LuaNames::MACOS);
#elif defined(__ANDROID__)
	lua_setfield(L_, -2, LuaNames::ANDROID);
#else
	lua_setfield(L_, -2, LuaNames::LINUX);
#endif

#if NCINE_DEBUG
	lua_pushboolean(L_, true);
	lua_setfield(L_, -2, LuaNames::DEBUG);
#endif
}

}
