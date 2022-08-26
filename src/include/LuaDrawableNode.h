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
	static int size(lua_State *L);
	static int anchorPoint(lua_State *L);
	static int setAnchorPoint(lua_State *L);

	static int isBlendingEnabled(lua_State *L);
	static int setBlendingEnabled(lua_State *L);
	static int srcBlendingFactor(lua_State *L);
	static int destBlendingFactor(lua_State *L);
	static int setBlendingPreset(lua_State *L);
	static int setBlendingFactors(lua_State *L);

	static int lastFrameRendered(lua_State *L);
	static int aabb(lua_State *L);

	friend class LuaBaseSprite;
	friend class LuaTextNode;
};

}

#endif
