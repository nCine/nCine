#ifndef CLASS_NCINE_LUADRAWABLENODE
#define CLASS_NCINE_LUADRAWABLENODE

struct lua_State;

namespace ncine {

class LuaStateManager;

/// Lua bindings around the `DrawableNode` class
class LuaDrawableNode
{
  public:
	static void exposeConstants(lua_State *L);

  private:
	static void exposeFunctions(lua_State *L);

	static int width(lua_State *L);
	static int height(lua_State *L);
	static int layer(lua_State *L);
	static int setLayer(lua_State *L);

	friend class LuaBaseSprite;
	friend class LuaTextNode;
};

}

#endif
