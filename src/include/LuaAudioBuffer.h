#ifndef CLASS_NCINE_LUAAUDIOBUFFER
#define CLASS_NCINE_LUAAUDIOBUFFER

struct lua_State;

namespace ncine {

class LuaStateManager;

/// Lua bindings around the `AudioBuffer` class
class LuaAudioBuffer
{
  public:
	static void expose(LuaStateManager *stateManager);
	static void release(void *object);

  private:
	static int newObject(lua_State *L);

	static int bufferId(lua_State *L);

	static int bytesPerSample(lua_State *L);
	static int numChannels(lua_State *L);
	static int frequency(lua_State *L);

	static int numSamples(lua_State *L);
	static int duration(lua_State *L);
	static int bufferSize(lua_State *L);
};

}

#endif
