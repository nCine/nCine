#ifndef CLASS_NCINE_LUARECTANIMATION
#define CLASS_NCINE_LUARECTANIMATION

#include <nctl/UniquePtr.h>

struct lua_State;

namespace ncine {

class RectAnimation;

/// Lua bindings around the `RectAnimation` class
class LuaRectAnimation
{
  public:
	static void exposeConstants(lua_State *L);

	static nctl::UniquePtr<RectAnimation> retrieveTable(lua_State *L, int index);
};

}

#endif
