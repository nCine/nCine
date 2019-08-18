#ifdef WITH_TRACY

	#include "Tracy.hpp"

#else

	#define ZoneNamed(x, y)
	#define ZoneNamedN(x, y, z)
	#define ZoneNamedC(x, y, z)
	#define ZoneNamedNC(x, y, z, w)

	#define ZoneScoped
	#define ZoneScopedN(x)
	#define ZoneScopedC(x)
	#define ZoneScopedNC(x, y)

	#define ZoneText(x, y)
	#define ZoneName(x, y)

	#define FrameMark
	#define FrameMarkNamed(x)
	#define FrameMarkStart(x)
	#define FrameMarkEnd(x)

	#define FrameImage(x, y, z, w, a)

	#define TracyLockable(type, varname) type varname;
	#define TracyLockableN(type, varname, desc) type varname;
	#define TracySharedLockable(type, varname) type varname;
	#define TracySharedLockableN(type, varname, desc) type varname;
	#define LockableBase(type) type
	#define SharedLockableBase(type) type
	#define LockMark(x) (void)x;

	#define TracyPlot(x, y)

	#define TracyMessage(x, y)
	#define TracyMessageL(x)
	#define TracyMessageC(x, y, z)
	#define TracyMessageLC(x, y)
	#define TracyAppInfo(x, y)

	#define TracyAlloc(x, y)
	#define TracyFree(x)

	#define ZoneNamedS(x, y, z)
	#define ZoneNamedNS(x, y, z, w)
	#define ZoneNamedCS(x, y, z, w)
	#define ZoneNamedNCS(x, y, z, w, a)

	#define ZoneScopedS(x)
	#define ZoneScopedNS(x, y)
	#define ZoneScopedCS(x, y)
	#define ZoneScopedNCS(x, y, z)

	#define TracyAllocS(x, y, z)
	#define TracyFreeS(x, y)

#endif
