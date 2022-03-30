#ifdef WITH_TRACY

	#include "Tracy.hpp"
	#include "TracyC.h"

#else

	// From Tracy.hpp
	#define ZoneNamed(x, y)
	#define ZoneNamedN(x, y, z)
	#define ZoneNamedC(x, y, z)
	#define ZoneNamedNC(x, y, z, w)

	#define ZoneTransient(x, y)
	#define ZoneTransientN(x, y, z)

	#define ZoneScoped
	#define ZoneScopedN(x)
	#define ZoneScopedC(x)
	#define ZoneScopedNC(x, y)

	#define ZoneText(x, y)
	#define ZoneTextV(x, y, z)
	#define ZoneName(x, y)
	#define ZoneNameV(x, y, z)
	#define ZoneColor(x)
	#define ZoneColorV(x, y)
	#define ZoneValue(x)
	#define ZoneValueV(x, y)
	#define ZoneIsActive false
	#define ZoneIsActiveV(x) false

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
	#define LockableName(x, y, z);

	#define TracyPlot(x, y)
	#define TracyPlotConfig(x, y)

	#define TracyMessage(x, y)
	#define TracyMessageL(x)
	#define TracyMessageC(x, y, z)
	#define TracyMessageLC(x, y)
	#define TracyAppInfo(x, y)

	#define TracyAlloc(x, y)
	#define TracyFree(x)
	#define TracySecureAlloc(x, y)
	#define TracySecureFree(x)

	#define TracyAllocN(x, y, z)
	#define TracyFreeN(x, y)
	#define TracySecureAllocN(x, y, z)
	#define TracySecureFreeN(x, y)

	#define ZoneNamedS(x, y, z)
	#define ZoneNamedNS(x, y, z, w)
	#define ZoneNamedCS(x, y, z, w)
	#define ZoneNamedNCS(x, y, z, w, a)

	#define ZoneTransientS(x, y, z)
	#define ZoneTransientNS(x, y, z, w)

	#define ZoneScopedS(x)
	#define ZoneScopedNS(x, y)
	#define ZoneScopedCS(x, y)
	#define ZoneScopedNCS(x, y, z)

	#define TracyAllocS(x, y, z)
	#define TracyFreeS(x, y)
	#define TracySecureAllocS(x, y, z)
	#define TracySecureFreeS(x, y)

	#define TracyAllocNS(x, y, z, w)
	#define TracyFreeNS(x, y, z)
	#define TracySecureAllocNS(x, y, z, w)
	#define TracySecureFreeNS(x, y, z)

	#define TracyMessageS(x, y, z)
	#define TracyMessageLS(x, y)
	#define TracyMessageCS(x, y, z, w)
	#define TracyMessageLCS(x, y, z)

	#define TracyParameterRegister(x)
	#define TracyParameterSetup(x, y, z, w)
	#define TracyIsConnected false

	#define TracyFiberEnter(x)
	#define TracyFiberLeave

	// From TracyC.h
	typedef const void *TracyCZoneCtx;

	#define TracyCZone(c, x)
	#define TracyCZoneN(c, x, y)
	#define TracyCZoneC(c, x, y)
	#define TracyCZoneNC(c, x, y, z)
	#define TracyCZoneEnd(c)
	#define TracyCZoneText(c, x, y)
	#define TracyCZoneName(c, x, y)
	#define TracyCZoneColor(c, x)
	#define TracyCZoneValue(c, x)

	#define TracyCAlloc(x, y)
	#define TracyCFree(x)
	#define TracyCSecureAlloc(x, y)
	#define TracyCSecureFree(x)

	#define TracyCAllocN(x, y, z)
	#define TracyCFreeN(x, y)
	#define TracyCSecureAllocN(x, y, z)
	#define TracyCSecureFreeN(x, y)

	#define TracyCFrameMark
	#define TracyCFrameMarkNamed(x)
	#define TracyCFrameMarkStart(x)
	#define TracyCFrameMarkEnd(x)
	#define TracyCFrameImage(x, y, z, w, a)

	#define TracyCPlot(x, y)
	#define TracyCMessage(x, y)
	#define TracyCMessageL(x)
	#define TracyCMessageC(x, y, z)
	#define TracyCMessageLC(x, y)
	#define TracyCAppInfo(x, y)

	#define TracyCZoneS(x, y, z)
	#define TracyCZoneNS(x, y, z, w)
	#define TracyCZoneCS(x, y, z, w)
	#define TracyCZoneNCS(x, y, z, w, a)

	#define TracyCAllocS(x, y, z)
	#define TracyCFreeS(x, y)
	#define TracyCSecureAllocS(x, y, z)
	#define TracyCSecureFreeS(x, y)

	#define TracyCAllocNS(x, y, z, w)
	#define TracyCFreeNS(x, y, z)
	#define TracyCSecureAllocNS(x, y, z, w)
	#define TracyCSecureFreeNS(x, y, z)

	#define TracyCMessageS(x, y, z)
	#define TracyCMessageLS(x, y)
	#define TracyCMessageCS(x, y, z, w)
	#define TracyCMessageLCS(x, y, z)

	#define TracyCIsConnected 0

	#ifdef TRACY_FIBERS
		#define TracyCFiberEnter(fiber)
		#define TracyCFiberLeave
	#endif

#endif
