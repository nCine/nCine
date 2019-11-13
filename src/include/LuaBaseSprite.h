#ifndef CLASS_NCINE_LUABASESPRITE
#define CLASS_NCINE_LUABASESPRITE

struct lua_State;

namespace ncine {

/// Lua bindings around the `BaseSprite` class
class LuaBaseSprite
{
  private:
	static void exposeFunctions(lua_State *L);

	static int setWidth(lua_State *L);
	static int setHeight(lua_State *L);
	static int setSize(lua_State *L);

	static int texture(lua_State *L);
	static int setTexture(lua_State *L);
	static int setOpaqueTexture(lua_State *L);

	static int texRect(lua_State *L);
	static int setTexRect(lua_State *L);

	static int anchorPoint(lua_State *L);
	static int setAnchorPoint(lua_State *L);

	static int isFlippedX(lua_State *L);
	static int setFlippedX(lua_State *L);
	static int isFlippedY(lua_State *L);
	static int setFlippedY(lua_State *L);

	friend class LuaSprite;
	friend class LuaMeshSprite;
};

}

#endif
