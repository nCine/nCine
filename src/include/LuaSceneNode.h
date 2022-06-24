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
	static void exposeConstants(lua_State *L);
	static void release(void *object);

  private:
	static void exposeFunctions(lua_State *L);

	static int newObject(lua_State *L);
	static int cloneNode(lua_State *L);

	static int parent(lua_State *L);
	static int setParent(lua_State *L);
	static int addChildNode(lua_State *L);
	static int removeChildNode(lua_State *L);
	static int removeChildNodeAt(lua_State *L);
	static int removeAllChildrenNodes(lua_State *L);
	static int unlinkChildNode(lua_State *L);

	static int childOrderIndex(lua_State *L);
	static int swapChildrenNodes(lua_State *L);
	static int swapNodeForward(lua_State *L);
	static int swapNodeBack(lua_State *L);

	static int visitOrderState(lua_State *L);
	static int setVisitOrderState(lua_State *L);
	static int visitOrderIndex(lua_State *L);

	static int isEnabled(lua_State *L);
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

	static int layer(lua_State *L);
	static int setLayer(lua_State *L);

	static int lastFrameUpdated(lua_State *L);

	friend class LuaDrawableNode;
	friend class LuaParticleSystem;
};

}

#endif
