#ifndef CLASS_NCINE_LUAANDROIDEVENTS
#define CLASS_NCINE_LUAANDROIDEVENTS

struct lua_State;

namespace ncine {

class TouchEvent;
class AccelerometerEvent;

/// Wrapper around the `TouchEvent` class
class LuaAndroidEvents
{
  public:
	static void pushTouchEvent(lua_State *L, const TouchEvent &event);
	static void pushAccelerometerEvent(lua_State *L, const AccelerometerEvent &event);
};

}

#endif
