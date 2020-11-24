#ifndef CLASS_NCINE_LUATEXTURE
#define CLASS_NCINE_LUATEXTURE

struct lua_State;

namespace ncine {

class LuaStateManager;

/// Lua bindings around the `Texture` class
class LuaTexture
{
  public:
	static void expose(LuaStateManager *stateManager);
	static void exposeConstants(lua_State *L);
	static void release(void *object);

  private:
	static int newObject(lua_State *L);

	static int width(lua_State *L);
	static int height(lua_State *L);
	static int mipMapLevels(lua_State *L);
	static int isCompressed(lua_State *L);
	static int numChannels(lua_State *L);
	static int dataSize(lua_State *L);

	static int minFiltering(lua_State *L);
	static int magFiltering(lua_State *L);
	static int wrap(lua_State *L);
	static int setMinFiltering(lua_State *L);
	static int setMagFiltering(lua_State *L);
	static int setWrap(lua_State *L);

	static int isChromaKeyEnabled(lua_State *L);
	static int chromaKeyColor(lua_State *L);
	static int setChromaKeyEnabled(lua_State *L);
	static int setChromaKeyColor(lua_State *L);
};

}

#endif
