#include <ncine/config.h>

#if (defined(WITH_TRACY) || NCINE_WITH_TRACY) && NCINE_WITH_VULKAN

	#include "tracy/TracyVulkan.hpp"

#else

	#define TracyVkContext(x, y, z, w) nullptr
	#define TracyVkContextCalibrated(x, y, z, w, a, b) nullptr
	#if defined VK_EXT_host_query_reset
		#define TracyVkContextHostCalibrated(x, y, z, w, a) nullptr
	#endif
	#define TracyVkDestroy(x)
	#define TracyVkContextName(c, x, y)
	#define TracyVkNamedZone(c, x, y, z, w)
	#define TracyVkNamedZoneC(c, x, y, z, w, a)
	#define TracyVkZone(c, x, y)
	#define TracyVkZoneC(c, x, y, z)
	#define TracyVkZoneTransient(c, x, y, z, w)
	#define TracyVkCollect(c, x)
	#define TracyVkCollectHost(c)

	#define TracyVkNamedZoneS(c, x, y, z, w, a)
	#define TracyVkNamedZoneCS(c, x, y, z, w, v, a)
	#define TracyVkZoneS(c, x, y, z)
	#define TracyVkZoneCS(c, x, y, z, w)
	#define TracyVkZoneTransientS(c, x, y, z, w, a)

namespace tracy
{
	class VkCtxScope {};
}

using TracyVkCtx = void*;

#endif
