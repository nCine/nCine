#ifndef CLASS_NCINE_LUAAUDIOSTREAMPLAYER
#define CLASS_NCINE_LUAAUDIOSTREAMPLAYER

struct lua_State;

namespace ncine {

class LuaStateManager;

/// Lua bindings around the `AudioStreamPlayer` class
class LuaAudioStreamPlayer
{
  public:
	static void expose(LuaStateManager *stateManager);
	static void release(void *object);

  private:
	static int newObject(lua_State *L);

	static int numStreamSamples(lua_State *L);
	static int streamBufferSize(lua_State *L);
};

}

#endif
