#ifndef CLASS_NCINE_LUAAUDIOBUFFERPLAYER
#define CLASS_NCINE_LUAAUDIOBUFFERPLAYER

struct lua_State;

namespace ncine {

class LuaStateManager;

/// Lua bindings around the `AudioBufferPlayer` class
class LuaAudioBufferPlayer
{
  public:
	static void expose(LuaStateManager *stateManager);
	static void release(void *object);

  private:
	static int newObject(lua_State *L);
};

}

#endif
