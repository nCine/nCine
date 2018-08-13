#ifndef CLASS_NCINE_LUAPARTICLESYSTEM
#define CLASS_NCINE_LUAPARTICLESYSTEM

struct lua_State;

namespace ncine {

class LuaStateManager;

/// Lua bindings around the `ParticleSystem` class
class LuaParticleSystem
{
  public:
	static void expose(LuaStateManager *stateManager);
	static void release(void *object);

  private:
	static int newObject(lua_State *L);

	static int addAccelerationAffector(lua_State *L);
	static int addColorAffector(lua_State *L);
	static int addSizeAffector(lua_State *L);
	static int clearAffectors(lua_State *L);

	static int emitParticles(lua_State *L);
	static int inLocalSpace(lua_State *L);
	static int setInLocalSpace(lua_State *L);
};

}

#endif
