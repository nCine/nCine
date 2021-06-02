#ifndef CLASS_NCINE_LUATOUCHEVENTS
#define CLASS_NCINE_LUATOUCHEVENTS

struct lua_State;

namespace ncine {

class TouchEvent;
class AccelerometerEvent;

/// Wrapper around the `TouchEvent` and the `AccelerometerEvent` classes
class LuaTouchEvents
{
  public:
	static void pushTouchEvent(lua_State *L, const TouchEvent &event);
	static void pushAccelerometerEvent(lua_State *L, const AccelerometerEvent &event);
};

}

#endif
