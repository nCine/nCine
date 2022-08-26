#ifndef CLASS_NCINE_LUAMESHSPRITE
#define CLASS_NCINE_LUAMESHSPRITE

struct lua_State;

namespace ncine {

class LuaStateManager;

/// Lua bindings around the `MeshSprite` class
class LuaMeshSprite
{
  public:
	static void expose(LuaStateManager *stateManager);
	static void exposeConstants(lua_State *L);
	static void release(void *object);

  private:
	static int newObject(lua_State *L);
	static int cloneNode(lua_State *L);

	static int bytesPerVertex(lua_State *L);
	static int numVertices(lua_State *L);
	static int numBytes(lua_State *L);
	static int vertices(lua_State *L);
	static int uniqueVertices(lua_State *L);

	static int copyVertices(lua_State *L);
	static int setVertices(lua_State *L);
	static int createVerticesFromTexels(lua_State *L);
	static int emplaceVertices(lua_State *L);

	static int numIndices(lua_State *L);
	static int indices(lua_State *L);
	static int uniqueIndices(lua_State *L);

	static int copyIndices(lua_State *L);
	static int setIndices(lua_State *L);
	static int emplaceIndices(lua_State *L);
};

}

#endif
