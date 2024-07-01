#include "LuaAudioEffect.h"
#include "LuaUntrackedUserData.h"
#include "LuaClassTracker.h"
#include "LuaUtils.h"
#include "AudioEffect.h"

namespace ncine {

namespace LuaNames {
namespace AudioEffect {
	static const char *AudioEffect = "audio_effect";

	static const char *type = "get_type";
	static const char *effectId = "get_effect_id";
	static const char *applyProperties = "apply_properties";

	static const char *NULL_EFFECT = "NULL_EFFECT";
	static const char *REVERB = "REVERB";
	static const char *EAXREVERB = "EAXREVERB";
	static const char *CHORUS = "CHORUS";
	static const char *DISTORTION = "DISTORTION";
	static const char *ECHO = "ECHO";
	static const char *FLANGER = "FLANGER";
	static const char *FREQUENCY_SHIFTER = "FREQUENCY_SHIFTER";
	static const char *VOCAL_MORPHER = "VOCAL_MORPHER";
	static const char *PITCH_SHIFTER = "PITCH_SHIFTER";
	static const char *RING_MODULATOR = "RING_MODULATOR";
	static const char *AUTOWAH = "AUTOWAH";
	static const char *COMPRESSOR = "COMPRESSOR";
	static const char *EQUALIZER = "EQUALIZER";
	static const char *Type = "audio_effect_type";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaAudioEffect::expose(LuaStateManager *stateManager)
{
	lua_State *L = stateManager->state();
	lua_createtable(L, 0, 5);

	if (stateManager->apiType() == LuaStateManager::ApiType::FULL)
	{
		LuaClassTracker<AudioEffect>::exposeDelete(L);
		LuaUtils::addFunction(L, LuaNames::newObject, newObject);
	}

	LuaUtils::addFunction(L, LuaNames::AudioEffect::type, type);
	LuaUtils::addFunction(L, LuaNames::AudioEffect::effectId, effectId);
	LuaUtils::addFunction(L, LuaNames::AudioEffect::applyProperties, applyProperties);

	lua_setfield(L, -2, LuaNames::AudioEffect::AudioEffect);
}

void LuaAudioEffect::exposeConstants(lua_State *L)
{
	lua_createtable(L, 0, 14);

	LuaUtils::pushField(L, LuaNames::AudioEffect::NULL_EFFECT, static_cast<int64_t>(AudioEffect::Type::NULL_EFFECT));
	LuaUtils::pushField(L, LuaNames::AudioEffect::REVERB, static_cast<int64_t>(AudioEffect::Type::REVERB));
	LuaUtils::pushField(L, LuaNames::AudioEffect::EAXREVERB, static_cast<int64_t>(AudioEffect::Type::EAXREVERB));
	LuaUtils::pushField(L, LuaNames::AudioEffect::CHORUS, static_cast<int64_t>(AudioEffect::Type::CHORUS));
	LuaUtils::pushField(L, LuaNames::AudioEffect::DISTORTION, static_cast<int64_t>(AudioEffect::Type::DISTORTION));
	LuaUtils::pushField(L, LuaNames::AudioEffect::ECHO, static_cast<int64_t>(AudioEffect::Type::ECHO));
	LuaUtils::pushField(L, LuaNames::AudioEffect::FLANGER, static_cast<int64_t>(AudioEffect::Type::FLANGER));
	LuaUtils::pushField(L, LuaNames::AudioEffect::FREQUENCY_SHIFTER, static_cast<int64_t>(AudioEffect::Type::FREQUENCY_SHIFTER));
	LuaUtils::pushField(L, LuaNames::AudioEffect::VOCAL_MORPHER, static_cast<int64_t>(AudioEffect::Type::VOCAL_MORPHER));
	LuaUtils::pushField(L, LuaNames::AudioEffect::PITCH_SHIFTER, static_cast<int64_t>(AudioEffect::Type::PITCH_SHIFTER));
	LuaUtils::pushField(L, LuaNames::AudioEffect::RING_MODULATOR, static_cast<int64_t>(AudioEffect::Type::RING_MODULATOR));
	LuaUtils::pushField(L, LuaNames::AudioEffect::AUTOWAH, static_cast<int64_t>(AudioEffect::Type::AUTOWAH));
	LuaUtils::pushField(L, LuaNames::AudioEffect::COMPRESSOR, static_cast<int64_t>(AudioEffect::Type::COMPRESSOR));
	LuaUtils::pushField(L, LuaNames::AudioEffect::EQUALIZER, static_cast<int64_t>(AudioEffect::Type::EQUALIZER));

	lua_setfield(L, -2, LuaNames::AudioEffect::Type);
}

void LuaAudioEffect::release(void *object)
{
	AudioEffect *audioEffect = reinterpret_cast<AudioEffect *>(object);
	delete audioEffect;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

int LuaAudioEffect::newObject(lua_State *L)
{
	LuaClassTracker<AudioEffect>::newObject(L);

	return 1;
}

int LuaAudioEffect::type(lua_State *L)
{
	AudioEffect *audioEffect = LuaUntrackedUserData<AudioEffect>::retrieve(L, -1);

	if (audioEffect)
		LuaUtils::push(L, static_cast<int64_t>(audioEffect->type()));
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaAudioEffect::effectId(lua_State *L)
{
	AudioEffect *audioEffect = LuaUntrackedUserData<AudioEffect>::retrieve(L, -1);

	if (audioEffect)
		LuaUtils::push(L, audioEffect->effectId());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaAudioEffect::applyProperties(lua_State *L)
{
	AudioEffect *audioEffect = LuaUntrackedUserData<AudioEffect>::retrieve(L, -2);
	AudioEffectProperties *properties = LuaUntrackedUserData<AudioEffectProperties>::retrieve(L, -1);

	if (audioEffect && properties)
		audioEffect->applyProperties(*properties);

	return 0;
}

}
