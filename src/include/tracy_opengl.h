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
#define TracyGpuNamedZone(x,y)
#define TracyGpuNamedZoneC(x,y,z)
#define TracyGpuZone(x)
#define TracyGpuZoneC(x,y)
#define TracyGpuCollect

#define TracyGpuNamedZoneS(x,y,z)
#define TracyGpuNamedZoneCS(x,y,z,w)
#define TracyGpuZoneS(x,y)
#define TracyGpuZoneCS(x,y,z)

#endif
