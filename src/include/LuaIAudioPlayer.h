#ifndef CLASS_NCINE_LUAIAUDIOPLAYER
#define CLASS_NCINE_LUAIAUDIOPLAYER

struct lua_State;

namespace ncine {

/// Lua bindings around the `IAudioPlayer` class
class LuaIAudioPlayer
{
  private:
	static void exposeFunctions(lua_State *L);

	static int sourceId(lua_State *L);
	static int bufferId(lua_State *L);

	static int bytesPerSample(lua_State *L);
	static int numChannels(lua_State *L);
	static int frequency(lua_State *L);

	static int numSamples(lua_State *L);
	static int duration(lua_State *L);

	static int bufferSize(lua_State *L);

	static int sampleOffset(lua_State *L);
	static int setSampleOffset(lua_State *L);

	static int play(lua_State *L);
	static int pause(lua_State *L);
	static int stop(lua_State *L);

	static int isPlaying(lua_State *L);
	static int isPaused(lua_State *L);
	static int isStopped(lua_State *L);

	static int isLooping(lua_State *L);
	static int setLooping(lua_State *L);

	static int gain(lua_State *L);
	static int setGain(lua_State *L);
	static int pitch(lua_State *L);
	static int setPitch(lua_State *L);
	static int position(lua_State *L);
	static int setPosition(lua_State *L);

	friend class LuaAudioBufferPlayer;
	friend class LuaAudioStreamPlayer;
};

}

#endif
