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

	static const char *bytesPerSample = "bytes_per_sample";
	static const char *numChannels = "num_channels";
	static const char *frequency = "frequency";

	static const char *numSamples = "num_samples";
	static const char *duration = "duration";
	static const char *bufferSize = "buffer_size";
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

	LuaUtils::addFunction(L, LuaNames::AudioBuffer::bytesPerSample, bytesPerSample);
	LuaUtils::addFunction(L, LuaNames::AudioBuffer::numChannels, numChannels);
	LuaUtils::addFunction(L, LuaNames::AudioBuffer::frequency, frequency);

	LuaUtils::addFunction(L, LuaNames::AudioBuffer::numSamples, numSamples);
	LuaUtils::addFunction(L, LuaNames::AudioBuffer::duration, duration);
	LuaUtils::addFunction(L, LuaNames::AudioBuffer::bufferSize, bufferSize);

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

	LuaClassTracker<AudioBuffer>::newObject(L, filename);

	return 1;
}

int LuaAudioBuffer::bufferId(lua_State *L)
{
	AudioBuffer *audioBuffer = LuaClassWrapper<AudioBuffer>::unwrapUserData(L, -1);
	LuaUtils::push(L, audioBuffer->bufferId());
	return 1;
}

int LuaAudioBuffer::bytesPerSample(lua_State *L)
{
	AudioBuffer *audioBuffer = LuaClassWrapper<AudioBuffer>::unwrapUserData(L, -1);
	LuaUtils::push(L, audioBuffer->bytesPerSample());
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

int LuaAudioBuffer::numSamples(lua_State *L)
{
	AudioBuffer *audioBuffer = LuaClassWrapper<AudioBuffer>::unwrapUserData(L, -1);
	LuaUtils::push(L, static_cast<uint64_t>(audioBuffer->numSamples()));
	return 1;
}

int LuaAudioBuffer::duration(lua_State *L)
{
	AudioBuffer *audioBuffer = LuaClassWrapper<AudioBuffer>::unwrapUserData(L, -1);
	LuaUtils::push(L, audioBuffer->duration());
	return 1;
}

int LuaAudioBuffer::bufferSize(lua_State *L)
{
	AudioBuffer *audioBuffer = LuaClassWrapper<AudioBuffer>::unwrapUserData(L, -1);
	LuaUtils::push(L, static_cast<uint64_t>(audioBuffer->bufferSize()));
	return 1;
}

}
