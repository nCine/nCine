#ifndef CLASS_NCINE_LUATEXTNODE
#define CLASS_NCINE_LUATEXTNODE

struct lua_State;

namespace ncine {

class LuaStateManager;

/// Lua bindings around the `TextNode` class
class LuaTextNode
{
  public:
	static void expose(LuaStateManager *stateManager);
	static void exposeConstants(lua_State *L);
	static void release(void *object);

  private:
	static int newObject(lua_State *L);
	static int cloneNode(lua_State *L);

	static int width(lua_State *L);
	static int height(lua_State *L);
	static int absWidth(lua_State *L);
	static int absHeight(lua_State *L);

	static int font(lua_State *L);
	static int setFont(lua_State *L);

	static int renderMode(lua_State *L);
	static int setRenderMode(lua_State *L);

	static int withKerning(lua_State *L);
	static int enableKerning(lua_State *L);

	static int alignment(lua_State *L);
	static int setAlignment(lua_State *L);

	static int lineHeight(lua_State *L);
	static int absLineHeight(lua_State *L);
	static int setLineHeight(lua_State *L);

	static int tabSize(lua_State *L);
	static int setTabSize(lua_State *L);

	static int string(lua_State *L);
	static int setString(lua_State *L);

	static int calculateBoundaries(lua_State *L);
};

}

#endif
