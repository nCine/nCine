#ifndef CLASS_NCINE_LUASHADER
#define CLASS_NCINE_LUASHADER

struct lua_State;

namespace ncine {

class LuaStateManager;

/// Lua bindings around the `Shader` class
class LuaShader
{
  public:
	static void exposeConstants(lua_State *L);
	static void expose(LuaStateManager *stateManager);
	static void release(void *object);

  private:
	static int newObject(lua_State *L);

	static int loadFromMemory(lua_State *L);
	static int loadFromMemoryDefaultVertex(lua_State *L);
	static int loadFromMemoryDefaultFragment(lua_State *L);

	static int loadFromFile(lua_State *L);
	static int loadFromFileDefaultVertex(lua_State *L);
	static int loadFromFileDefaultFragment(lua_State *L);

	static int setAttribute(lua_State *L);

	static int isLinked(lua_State *L);

	static int retrieveInfoLogLength(lua_State *L);
	static int retrieveInfoLog(lua_State *L);

	static int logOnErrors(lua_State *L);
	static int setLogOnErrors(lua_State *L);

	static int setGLShaderProgramLabel(lua_State *L);

	static int registerBatchedShader(lua_State *L);
};

}

#endif
