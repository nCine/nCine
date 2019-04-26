#include "LuaAudioBuffer.h"
#include "LuaClassWrapper.h"
#include "LuaClassTracker.h"
#include "LuaUtils.h"
#include "AudioBuffer.h"

namespace ncine {

namespace LuaNames {
namespace AudioBuffer {
	static const char *AudioBuffer = "audiobuffer";

	static const char *bufferId = "buffer_id";
	static const char *numChannels = "num_channels";
	static const char *frequency = "frequency";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaAudioBuffer::expose(LuaStateManager *stateManager)
{
	lua_State *L = stateManager->state();
	lua_newtable(L);

	if (stateManager->apiType() == LuaStateManager::ApiType::FULL)
	{
		LuaClassTracker<AudioBuffer>::exposeDelete(L);
		LuaUtils::addFunction(L, LuaNames::newObject, newObject);
	}

	LuaUtils::addFunction(L, LuaNames::AudioBuffer::bufferId, bufferId);
	LuaUtils::addFunction(L, LuaNames::AudioBuffer::numChannels, numChannels);
	LuaUtils::addFunction(L, LuaNames::AudioBuffer::frequency, frequency);

	lua_setfield(L, -2, LuaNames::AudioBuffer::AudioBuffer);
}

void LuaAudioBuffer::release(void *object)
{
	AudioBuffer *audioBuffer = reinterpret_cast<AudioBuffer *>(object);
	delete audioBuffer;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

int LuaAudioBuffer::newObject(lua_State *L)
{
	const char *filename = LuaUtils::retrieve<const char *>(L, -1);

	AudioBuffer *audioBuffer = new AudioBuffer(filename);
	LuaClassTracker<AudioBuffer>::wrapTrackedUserData(L, audioBuffer);

	return 1;
}

int LuaAudioBuffer::bufferId(lua_State *L)
{
	AudioBuffer *audioBuffer = LuaClassWrapper<AudioBuffer>::unwrapUserData(L, -1);
	LuaUtils::push(L, audioBuffer->bufferId());
	return 1;
}

int LuaAudioBuffer::numChannels(lua_State *L)
{
	AudioBuffer *audioBuffer = LuaClassWrapper<AudioBuffer>::unwrapUserData(L, -1);
	LuaUtils::push(L, audioBuffer->numChannels());
	return 1;
}

int LuaAudioBuffer::frequency(lua_State *L)
{
	AudioBuffer *audioBuffer = LuaClassWrapper<AudioBuffer>::unwrapUserData(L, -1);
	LuaUtils::push(L, audioBuffer->frequency());
	return 1;
}

}
