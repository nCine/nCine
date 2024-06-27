#ifndef CLASS_NCINE_LUAAUDIOEFFECTSLOT
#define CLASS_NCINE_LUAAUDIOEFFECTSLOT

struct lua_State;

namespace ncine {

class LuaStateManager;

/// Lua bindings around the `AudioEffectSlot` class
class LuaAudioEffectSlot
{
  public:
	static void expose(LuaStateManager *stateManager);
	static void release(void *object);

  private:
	static int newObject(lua_State *L);

	static int effectSlotId(lua_State *L);

	static int gain(lua_State *L);
	static int setGain(lua_State *L);

	static int auxSendAuto(lua_State *L);
	static int setAuxSendAuto(lua_State *L);

	static int applyEffect(lua_State *L);
};

}

#endif
