#ifndef CLASS_NCINE_LUAVIEWPORT
#define CLASS_NCINE_LUAVIEWPORT

struct lua_State;

namespace ncine {

class LuaStateManager;

/// Lua bindings around the `Viewport` class
class LuaViewport
{
  public:
	static void exposeConstants(lua_State *L);
	static void expose(LuaStateManager *stateManager);
	static void release(void *object);

  private:
	static int newObject(lua_State *L);

	static int type(lua_State *L);

	static int texture(lua_State *L);
	static int setTexture(lua_State *L);

	static int depthStencilFormat(lua_State *L);
	static int setDepthStencilFormat(lua_State *L);

	static int removeAllTextures(lua_State *L);

	static int width(lua_State *L);
	static int height(lua_State *L);
	static int numColorAttachments(lua_State *L);

	static int viewportRect(lua_State *L);
	static int setViewportRect(lua_State *L);
	static int scissorRect(lua_State *L);
	static int setScissorRect(lua_State *L);
	static int cullingRect(lua_State *L);

	static int lastFrameCleared(lua_State *L);

	static int clearMode(lua_State *L);
	static int setClearMode(lua_State *L);

	static int clearColor(lua_State *L);
	static int setClearColor(lua_State *L);

	static int rootNode(lua_State *L);
	static int setRootNode(lua_State *L);

	static int clearChain(lua_State *L);
	static int pushBackChain(lua_State *L);
	static int pushFrontChain(lua_State *L);

	static int camera(lua_State *L);
	static int setCamera(lua_State *L);

	static int setGLFramebufferLabel(lua_State *L);
};

}

#endif
