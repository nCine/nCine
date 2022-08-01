#ifndef CLASS_NCINE_LUAPARTICLEAFFECTOR
#define CLASS_NCINE_LUAPARTICLEAFFECTOR

struct lua_State;

namespace ncine {

class LuaStateManager;

/// Lua bindings around the `ParticleAffector` classes
class LuaParticleAffector
{
  public:
	static void expose(LuaStateManager *stateManager);
	static void exposeConstants(lua_State *L);

  private:
	static int type(lua_State *L);
	static int isEnabled(lua_State *L);
	static int setEnabled(lua_State *L);
	static int numSteps(lua_State *L);
	static int removeStep(lua_State *L);
	static int clearSteps(lua_State *L);

	static int colorSteps(lua_State *L);
	static int sizeSteps(lua_State *L);
	static int rotationSteps(lua_State *L);
	static int positionSteps(lua_State *L);
	static int velocitySteps(lua_State *L);

	static int addColorStep(lua_State *L);
	static int setBaseScale(lua_State *L);
	static int addSizeStep(lua_State *L);
	static int addRotationStep(lua_State *L);
	static int addPositionStep(lua_State *L);
	static int addVelocityStep(lua_State *L);
};

}

#endif
