#ifndef CLASS_NCINE_LUAAUDIOEFFECTPROPERTIES
#define CLASS_NCINE_LUAAUDIOEFFECTPROPERTIES

struct lua_State;

namespace ncine {

class LuaStateManager;

/// Lua bindings around the `AudioEffectProperties` class
class LuaAudioEffectProperties
{
  public:
	static void expose(LuaStateManager *stateManager);
	static void exposeConstants(lua_State *L);
	static void release(void *object);

  private:
	static int newObject(lua_State *L);

	static int type(lua_State *L);
	static int setType(lua_State *L);

	static int minReverbProperties(lua_State *L);
	static int maxReverbProperties(lua_State *L);
	static int reverbProperties(lua_State *L);
	static int setReverbProperties(lua_State *L);
	static int resetReverbProperties(lua_State *L);

	static int minEaxReverbProperties(lua_State *L);
	static int maxEaxReverbProperties(lua_State *L);
	static int eaxReverbProperties(lua_State *L);
	static int setEaxReverbProperties(lua_State *L);
	static int resetEaxReverbProperties(lua_State *L);
	static int loadEfxReverbPreset(lua_State *L);

	static int minChorusProperties(lua_State *L);
	static int maxChorusProperties(lua_State *L);
	static int chorusProperties(lua_State *L);
	static int setChorusProperties(lua_State *L);
	static int resetChorusProperties(lua_State *L);

	static int minDistortionProperties(lua_State *L);
	static int maxDistortionProperties(lua_State *L);
	static int distortionProperties(lua_State *L);
	static int setDistortionProperties(lua_State *L);
	static int resetDistortionProperties(lua_State *L);

	static int minEchoProperties(lua_State *L);
	static int maxEchoProperties(lua_State *L);
	static int echoProperties(lua_State *L);
	static int setEchoProperties(lua_State *L);
	static int resetEchoProperties(lua_State *L);

	static int minFlangerProperties(lua_State *L);
	static int maxFlangerProperties(lua_State *L);
	static int flangerProperties(lua_State *L);
	static int setFlangerProperties(lua_State *L);
	static int resetFlangerProperties(lua_State *L);

	static int minFrequencyShifterProperties(lua_State *L);
	static int maxFrequencyShifterProperties(lua_State *L);
	static int frequencyShifterProperties(lua_State *L);
	static int setFrequencyShifterProperties(lua_State *L);
	static int resetFrequencyShifterProperties(lua_State *L);

	static int minVocalMorpherProperties(lua_State *L);
	static int maxVocalMorpherProperties(lua_State *L);
	static int vocalMorpherProperties(lua_State *L);
	static int setVocalMorpherProperties(lua_State *L);
	static int resetVocalMorpherProperties(lua_State *L);

	static int minPitchShifterProperties(lua_State *L);
	static int maxPitchShifterProperties(lua_State *L);
	static int pitchShifterProperties(lua_State *L);
	static int setPitchShifterProperties(lua_State *L);
	static int resetPitchShifterProperties(lua_State *L);

	static int minRingModulatorProperties(lua_State *L);
	static int maxRingModulatorProperties(lua_State *L);
	static int ringModulatorProperties(lua_State *L);
	static int setRingModulatorProperties(lua_State *L);
	static int resetRingModulatorProperties(lua_State *L);

	static int minAutoWahProperties(lua_State *L);
	static int maxAutoWahProperties(lua_State *L);
	static int autoWahProperties(lua_State *L);
	static int setAutoWahProperties(lua_State *L);
	static int resetAutoWahProperties(lua_State *L);

	static int minCompressorProperties(lua_State *L);
	static int maxCompressorProperties(lua_State *L);
	static int compressorProperties(lua_State *L);
	static int setCompressorProperties(lua_State *L);
	static int resetCompressorProperties(lua_State *L);

	static int minEqualizerProperties(lua_State *L);
	static int maxEqualizerProperties(lua_State *L);
	static int equalizerProperties(lua_State *L);
	static int setEqualizerProperties(lua_State *L);
	static int resetEqualizerProperties(lua_State *L);
};

}

#endif
