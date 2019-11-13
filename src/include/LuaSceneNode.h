#ifndef CLASS_NCINE_LUASCENENODE
#define CLASS_NCINE_LUASCENENODE

struct lua_State;

namespace ncine {

class LuaStateManager;

/// Lua bindings around the `SceneNode` class
class LuaSceneNode
{
  public:
	static void expose(LuaStateManager *stateManager);
	static void release(void *object);

  private:
	static void exposeFunctions(lua_State *L);

	static int newObject(lua_State *L);

	static int parent(lua_State *L);
	static int setParent(lua_State *L);
	static int addChildNode(lua_State *L);
	static int removeChildNode(lua_State *L);
	static int unlinkChildNode(lua_State *L);

	static int enabled(lua_State *L);
	static int setEnabled(lua_State *L);

	static int position(lua_State *L);
	static int setPosition(lua_State *L);
	static int absAnchorPoint(lua_State *L);
	static int setAbsAnchorPoint(lua_State *L);
	static int scale(lua_State *L);
	static int setScaleX(lua_State *L);
	static int setScaleY(lua_State *L);
	static int setScale(lua_State *L);
	static int rotation(lua_State *L);
	static int setRotation(lua_State *L);

	static int color(lua_State *L);
	static int setColor(lua_State *L);
	static int alpha(lua_State *L);
	static int setAlpha(lua_State *L);

	friend class LuaDrawableNode;
	friend class LuaParticleSystem;
};

}

#endif
