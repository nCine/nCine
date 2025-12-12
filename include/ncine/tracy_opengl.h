#if (defined(WITH_TRACY) || NCINE_WITH_TRACY) && \
    !defined(__ANDROID__) && !defined(__APPLE__) && !defined(__EMSCRIPTEN__)

	#include "tracy/TracyOpenGL.hpp"

#else

	#include "tracy.h"

	#define TracyGpuContext
	#define TracyGpuContextName(x, y)
	#define TracyGpuNamedZone(x, y, z)
	#define TracyGpuNamedZoneC(x, y, z, w)
	#define TracyGpuZone(x)
	#define TracyGpuZoneC(x, y)
	#define TracyGpuZoneTransient(x, y, z)
	#define TracyGpuCollect

	#define TracyGpuNamedZoneS(x, y, z, w)
	#define TracyGpuNamedZoneCS(x, y, z, w, a)
	#define TracyGpuZoneS(x, y)
	#define TracyGpuZoneCS(x, y, z)
	#define TracyGpuZoneTransientS(x, y, z, w)

namespace tracy {

struct SourceLocationData;
class GpuCtxScope
{
  public:
	GpuCtxScope(const SourceLocationData *, bool) {}
	GpuCtxScope(const SourceLocationData *, int, bool) {}
};

}

#endif
