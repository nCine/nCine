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

	static int initTexture(lua_State *L);

	static int width(lua_State *L);
	static int height(lua_State *L);

	static int viewportRect(lua_State *L);
	static int setViewportRect(lua_State *L);
	static int scissorRect(lua_State *L);
	static int setScissorRect(lua_State *L);
	static int cullingRect(lua_State *L);

	static int colorFormat(lua_State *L);
	static int depthStencilFormat(lua_State *L);

	static int clearMode(lua_State *L);
	static int setClearMode(lua_State *L);

	static int clearColor(lua_State *L);
	static int setClearColor(lua_State *L);

	static int texture(lua_State *L);

	static int rootNode(lua_State *L);
	static int setRootNode(lua_State *L);

	static int nextViewport(lua_State *L);
	static int setNextViewport(lua_State *L);

	static int camera(lua_State *L);
	static int setCamera(lua_State *L);

	static int setGLLabels(lua_State *L);
	static int setGLFramebufferLabel(lua_State *L);
	static int setGLTextureLabel(lua_State *L);
};

}

#endif
