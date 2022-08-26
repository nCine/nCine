#ifndef CLASS_NCINE_LUASHADERSTATE
#define CLASS_NCINE_LUASHADERSTATE

struct lua_State;

namespace ncine {

class LuaStateManager;

/// Lua bindings around the `ShaderState` class
class LuaShaderState
{
  public:
	static void expose(LuaStateManager *stateManager);
	static void release(void *object);

  private:
	static int newObject(lua_State *L);

	static int node(lua_State *L);
	static int setNode(lua_State *L);

	static int shader(lua_State *L);
	static int setShader(lua_State *L);
	static int resetShader(lua_State *L);

	static int setTexture(lua_State *L);

	static int setUniformIntVector(lua_State *L);
	static int setUniformIntValue1(lua_State *L);
	static int setUniformIntValue2(lua_State *L);
	static int setUniformIntValue3(lua_State *L);
	static int setUniformIntValue4(lua_State *L);

	static int setUniformIntVector2(lua_State *L);
	static int setUniformIntVector3(lua_State *L);
	static int setUniformIntVector4(lua_State *L);

	static int setUniformFloatVector(lua_State *L);
	static int setUniformFloatValue1(lua_State *L);
	static int setUniformFloatValue2(lua_State *L);
	static int setUniformFloatValue3(lua_State *L);
	static int setUniformFloatValue4(lua_State *L);

	static int setUniformFloatVector2(lua_State *L);
	static int setUniformFloatVector3(lua_State *L);
	static int setUniformFloatVector4(lua_State *L);
	static int setUniformFloatColor(lua_State *L);
};

}

#endif
