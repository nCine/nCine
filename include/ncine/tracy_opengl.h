#if defined(WITH_TRACY) && defined(WITH_TRACY_OPENGL)

	#ifdef __ANDROID__
		#define GL_TIMESTAMP GL_TIMESTAMP_EXT
		#define GL_QUERY_COUNTER_BITS GL_QUERY_COUNTER_BITS_EXT
		#define GL_QUERY_RESULT GL_QUERY_RESULT_EXT
		#define GL_QUERY_RESULT_AVAILABLE GL_QUERY_RESULT_AVAILABLE_EXT
		#define glQueryCounter glQueryCounterEXT
		#define glGetQueryiv glGetQueryivEXT
		#define glGetQueryObjectiv glGetQueryObjectivEXT
		#define glGetQueryObjectui64v glGetQueryObjectui64vEXT
	#endif

	#include "TracyOpenGL.hpp"

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
