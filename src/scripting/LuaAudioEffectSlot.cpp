#include "LuaAudioEffectSlot.h"
#include "LuaUntrackedUserData.h"
#include "LuaClassTracker.h"
#include "LuaUtils.h"
#include "AudioEffectSlot.h"

namespace ncine {

namespace LuaNames {
namespace AudioEffectSlot {
	static const char *AudioEffectSlot = "audio_effect_slot";

	static const char *effectSlotId = "get_effect_slot_id";

	static const char *gain = "get_gain";
	static const char *setGain = "set_gain";

	static const char *auxSendAuto = "get_aux_send_auto";
	static const char *setAuxSendAuto = "set_aux_send_auto";

	static const char *applyEffect = "apply_effect";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaAudioEffectSlot::expose(LuaStateManager *stateManager)
{
	lua_State *L = stateManager->state();
	lua_createtable(L, 0, 8);

	if (stateManager->apiType() == LuaStateManager::ApiType::FULL)
	{
		LuaClassTracker<AudioEffectSlot>::exposeDelete(L);
		LuaUtils::addFunction(L, LuaNames::newObject, newObject);
	}

	LuaUtils::addFunction(L, LuaNames::AudioEffectSlot::effectSlotId, effectSlotId);

	LuaUtils::addFunction(L, LuaNames::AudioEffectSlot::gain, gain);
	LuaUtils::addFunction(L, LuaNames::AudioEffectSlot::setGain, setGain);

	LuaUtils::addFunction(L, LuaNames::AudioEffectSlot::auxSendAuto, auxSendAuto);
	LuaUtils::addFunction(L, LuaNames::AudioEffectSlot::setAuxSendAuto, setAuxSendAuto);

	LuaUtils::addFunction(L, LuaNames::AudioEffectSlot::applyEffect, applyEffect);

	lua_setfield(L, -2, LuaNames::AudioEffectSlot::AudioEffectSlot);
}

void LuaAudioEffectSlot::release(void *object)
{
	AudioEffectSlot *audioEffectSlot = reinterpret_cast<AudioEffectSlot *>(object);
	delete audioEffectSlot;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

int LuaAudioEffectSlot::newObject(lua_State *L)
{
	LuaClassTracker<AudioEffectSlot>::newObject(L);

	return 1;
}

int LuaAudioEffectSlot::effectSlotId(lua_State *L)
{
	AudioEffectSlot *audioEffectSlot = LuaUntrackedUserData<AudioEffectSlot>::retrieve(L, -1);

	if (audioEffectSlot)
		LuaUtils::push(L, audioEffectSlot->effectSlotId());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaAudioEffectSlot::gain(lua_State *L)
{
	AudioEffectSlot *audioEffectSlot = LuaUntrackedUserData<AudioEffectSlot>::retrieve(L, -1);

	if (audioEffectSlot)
		LuaUtils::push(L, audioEffectSlot->gain());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaAudioEffectSlot::setGain(lua_State *L)
{
	AudioEffectSlot *audioEffectSlot = LuaUntrackedUserData<AudioEffectSlot>::retrieve(L, -2);
	const float gain = LuaUtils::retrieve<float>(L, -1);

	if (audioEffectSlot)
		audioEffectSlot->setGain(gain);

	return 0;
}

int LuaAudioEffectSlot::auxSendAuto(lua_State *L)
{
	AudioEffectSlot *audioEffectSlot = LuaUntrackedUserData<AudioEffectSlot>::retrieve(L, -1);

	if (audioEffectSlot)
		LuaUtils::push(L, audioEffectSlot->auxSendAuto());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaAudioEffectSlot::setAuxSendAuto(lua_State *L)
{
	AudioEffectSlot *audioEffectSlot = LuaUntrackedUserData<AudioEffectSlot>::retrieve(L, -2);
	const bool auxSendAuto = LuaUtils::retrieve<bool>(L, -1);

	if (audioEffectSlot)
		audioEffectSlot->setAuxSendAuto(auxSendAuto);

	return 0;
}

int LuaAudioEffectSlot::applyEffect(lua_State *L)
{
	AudioEffectSlot *audioEffectSlot = LuaUntrackedUserData<AudioEffectSlot>::retrieve(L, -2);
	AudioEffect *audioEffect = LuaUntrackedUserData<AudioEffect>::retrieve(L, -1);

	if (audioEffectSlot && audioEffect)
		audioEffectSlot->applyEffect(*audioEffect);

	return 0;
}

}
