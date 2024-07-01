#define NCINE_INCLUDE_OPENAL_EXT
#include "common_headers.h"

namespace {

LPALGENFILTERS alGenFilters = nullptr;
LPALDELETEFILTERS alDeleteFilters = nullptr;
LPALISFILTER alIsFilter = nullptr;
LPALFILTERI alFilteri = nullptr;
LPALFILTERIV alFilteriv = nullptr;
LPALFILTERF alFilterf = nullptr;
LPALFILTERFV alFilterfv = nullptr;
LPALGETFILTERI alGetFilteri = nullptr;
LPALGETFILTERIV alGetFilteriv = nullptr;
LPALGETFILTERF alGetFilterf = nullptr;
LPALGETFILTERFV alGetFilterfv = nullptr;

/* Effect object functions */
LPALGENEFFECTS alGenEffects = nullptr;
LPALDELETEEFFECTS alDeleteEffects = nullptr;
LPALISEFFECT alIsEffect = nullptr;
LPALEFFECTI alEffecti = nullptr;
LPALEFFECTIV alEffectiv = nullptr;
LPALEFFECTF alEffectf = nullptr;
LPALEFFECTFV alEffectfv = nullptr;
LPALGETEFFECTI alGetEffecti = nullptr;
LPALGETEFFECTIV alGetEffectiv = nullptr;
LPALGETEFFECTF alGetEffectf = nullptr;
LPALGETEFFECTFV alGetEffectfv = nullptr;

/* Auxiliary Effect Slot object functions */
LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots = nullptr;
LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots = nullptr;
LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot = nullptr;
LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti = nullptr;
LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv = nullptr;
LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf = nullptr;
LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv = nullptr;
LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti = nullptr;
LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv = nullptr;
LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf = nullptr;
LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv = nullptr;

/* Pause device extension functions */
LPALCDEVICEPAUSESOFT alcDevicePauseSOFT = nullptr;
LPALCDEVICERESUMESOFT alcDeviceResumeSOFT = nullptr;

/* Deferred updates extension functions */
LPALDEFERUPDATESSOFT alDeferUpdatesSOFT = nullptr;
LPALPROCESSUPDATESSOFT alProcessUpdatesSOFT = nullptr;

#define LOAD_PROC(T, x) ((x) = reinterpret_cast<T>(alGetProcAddress(#x)))

void retrieveEfxFunctions()
{
	LOAD_PROC(LPALGENFILTERS, alGenFilters);
	LOAD_PROC(LPALDELETEFILTERS, alDeleteFilters);
	LOAD_PROC(LPALISFILTER, alIsFilter);
	LOAD_PROC(LPALFILTERI, alFilteri);
	LOAD_PROC(LPALFILTERIV, alFilteriv);
	LOAD_PROC(LPALFILTERF, alFilterf);
	LOAD_PROC(LPALFILTERFV, alFilterfv);
	LOAD_PROC(LPALGETFILTERI, alGetFilteri);
	LOAD_PROC(LPALGETFILTERIV, alGetFilteriv);
	LOAD_PROC(LPALGETFILTERF, alGetFilterf);
	LOAD_PROC(LPALGETFILTERFV, alGetFilterfv);

	LOAD_PROC(LPALGENEFFECTS, alGenEffects);
	LOAD_PROC(LPALDELETEEFFECTS, alDeleteEffects);
	LOAD_PROC(LPALISEFFECT, alIsEffect);
	LOAD_PROC(LPALEFFECTI, alEffecti);
	LOAD_PROC(LPALEFFECTIV, alEffectiv);
	LOAD_PROC(LPALEFFECTF, alEffectf);
	LOAD_PROC(LPALEFFECTFV, alEffectfv);
	LOAD_PROC(LPALGETEFFECTI, alGetEffecti);
	LOAD_PROC(LPALGETEFFECTIV, alGetEffectiv);
	LOAD_PROC(LPALGETEFFECTF, alGetEffectf);
	LOAD_PROC(LPALGETEFFECTFV, alGetEffectfv);

	LOAD_PROC(LPALGENAUXILIARYEFFECTSLOTS, alGenAuxiliaryEffectSlots);
	LOAD_PROC(LPALDELETEAUXILIARYEFFECTSLOTS, alDeleteAuxiliaryEffectSlots);
	LOAD_PROC(LPALISAUXILIARYEFFECTSLOT, alIsAuxiliaryEffectSlot);
	LOAD_PROC(LPALAUXILIARYEFFECTSLOTI, alAuxiliaryEffectSloti);
	LOAD_PROC(LPALAUXILIARYEFFECTSLOTIV, alAuxiliaryEffectSlotiv);
	LOAD_PROC(LPALAUXILIARYEFFECTSLOTF, alAuxiliaryEffectSlotf);
	LOAD_PROC(LPALAUXILIARYEFFECTSLOTFV, alAuxiliaryEffectSlotfv);
	LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTI, alGetAuxiliaryEffectSloti);
	LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTIV, alGetAuxiliaryEffectSlotiv);
	LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTF, alGetAuxiliaryEffectSlotf);
	LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTFV, alGetAuxiliaryEffectSlotfv);
}

void retrievePauseDeviceFunctions()
{
	LOAD_PROC(LPALCDEVICEPAUSESOFT, alcDevicePauseSOFT);
	LOAD_PROC(LPALCDEVICERESUMESOFT, alcDeviceResumeSOFT);
}

void retrieveDeferredUpdatesFunctions()
{
	LOAD_PROC(LPALDEFERUPDATESSOFT, alDeferUpdatesSOFT);
	LOAD_PROC(LPALPROCESSUPDATESSOFT, alProcessUpdatesSOFT);
}

#undef LOAD_PROC

}
