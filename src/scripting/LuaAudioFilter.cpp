#include "LuaAudioFilter.h"
#include "LuaUntrackedUserData.h"
#include "LuaClassTracker.h"
#include "LuaUtils.h"
#include "AudioFilter.h"

namespace ncine {

namespace LuaNames {
namespace AudioFilter {
	static const char *AudioFilter = "audio_filter";

	static const char *type = "get_type";
	static const char *filterId = "get_filter_id";
	static const char *applyProperties = "apply_properties";
	static const char *defaultProperties = "get_default_properties";

	static const char *NULL_FILTER = "NULL_FILTER";
	static const char *LOWPASS = "LOWPASS";
	static const char *HIGHPASS = "HIGHPASS";
	static const char *BANDPASS = "BANDPASS";
	static const char *Type = "audio_filter_type";

	namespace Properties {
		static const char *type = "type";
		static const char *gain = "gain";
		static const char *gainLF = "gain_lf";
		static const char *gainHF = "gain_hf";
	}
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaAudioFilter::expose(LuaStateManager *stateManager)
{
	lua_State *L = stateManager->state();
	lua_createtable(L, 0, 6);

	if (stateManager->apiType() == LuaStateManager::ApiType::FULL)
	{
		LuaClassTracker<AudioFilter>::exposeDelete(L);
		LuaUtils::addFunction(L, LuaNames::newObject, newObject);
	}

	LuaUtils::addFunction(L, LuaNames::AudioFilter::type, type);
	LuaUtils::addFunction(L, LuaNames::AudioFilter::filterId, filterId);
	LuaUtils::addFunction(L, LuaNames::AudioFilter::applyProperties, applyProperties);

	LuaUtils::addFunction(L, LuaNames::AudioFilter::defaultProperties, defaultProperties);

	lua_setfield(L, -2, LuaNames::AudioFilter::AudioFilter);
}

void LuaAudioFilter::exposeConstants(lua_State *L)
{
	lua_createtable(L, 0, 4);

	LuaUtils::pushField(L, LuaNames::AudioFilter::NULL_FILTER, static_cast<int64_t>(AudioFilter::Type::NULL_FILTER));
	LuaUtils::pushField(L, LuaNames::AudioFilter::LOWPASS, static_cast<int64_t>(AudioFilter::Type::LOWPASS));
	LuaUtils::pushField(L, LuaNames::AudioFilter::HIGHPASS, static_cast<int64_t>(AudioFilter::Type::HIGHPASS));
	LuaUtils::pushField(L, LuaNames::AudioFilter::BANDPASS, static_cast<int64_t>(AudioFilter::Type::BANDPASS));

	lua_setfield(L, -2, LuaNames::AudioFilter::Type);
}

void LuaAudioFilter::release(void *object)
{
	AudioFilter *audioFilter = reinterpret_cast<AudioFilter *>(object);
	delete audioFilter;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

namespace {

	AudioFilter::Properties retrieveProperties(lua_State *L, int index)
	{
		AudioFilter::Properties props;

		props.type = AudioFilter::Type(LuaUtils::retrieveField<int64_t>(L, index, LuaNames::AudioFilter::Properties::type));
		props.gain = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioFilter::Properties::gain);
		props.gainLF = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioFilter::Properties::gainLF);
		props.gainHF = LuaUtils::retrieveField<float>(L, index, LuaNames::AudioFilter::Properties::gainHF);

		return props;
	}

	void pushProperties(lua_State *L, const AudioFilter::Properties &props)
	{
		lua_createtable(L, 0, 4);

		LuaUtils::pushField(L, LuaNames::AudioFilter::Properties::type, static_cast<int64_t>(props.type));
		LuaUtils::pushField(L, LuaNames::AudioFilter::Properties::gain, props.gain);
		LuaUtils::pushField(L, LuaNames::AudioFilter::Properties::gainLF, props.gainLF);
		LuaUtils::pushField(L, LuaNames::AudioFilter::Properties::gainHF, props.gainHF);
	}
}

int LuaAudioFilter::newObject(lua_State *L)
{
	LuaClassTracker<AudioFilter>::newObject(L);

	return 1;
}

int LuaAudioFilter::type(lua_State *L)
{
	AudioFilter *audioFilter = LuaUntrackedUserData<AudioFilter>::retrieve(L, -1);

	if (audioFilter)
		LuaUtils::push(L, static_cast<int64_t>(audioFilter->type()));
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaAudioFilter::filterId(lua_State *L)
{
	AudioFilter *audioFilter = LuaUntrackedUserData<AudioFilter>::retrieve(L, -1);

	if (audioFilter)
		LuaUtils::push(L, audioFilter->filterId());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaAudioFilter::applyProperties(lua_State *L)
{
	if (lua_istable(L, -1) == false)
	{
		LOGW("Expecting a table at index -1");
		return 0;
	}

	AudioFilter *audioFilter = LuaUntrackedUserData<AudioFilter>::retrieve(L, -2);

	if (audioFilter)
	{
		const AudioFilter::Properties props = retrieveProperties(L, -1);
		audioFilter->applyProperties(props);
	}
	else
		lua_pop(L, 1);

	return 0;
}

int LuaAudioFilter::defaultProperties(lua_State *L)
{
	const AudioFilter::Properties props;

	pushProperties(L, props);

	return 1;
}

}
