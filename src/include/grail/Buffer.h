#ifndef CLASS_GRAIL_BUFFER
#define CLASS_GRAIL_BUFFER

#include <cstdint>
#include "grail/handle_types.h"

namespace ncine {
namespace grail {

namespace Buffer {

enum class MemoryType : uint8_t
{
	DEFAULT, ///< CPU no access, GPU read/write
	UPLOAD, ///< CPU write, GPU read
	READBACK ///< CPU read, GPU write
};

enum class Usage : uint8_t
{
	NONE = 0,
	VERTEX = 1 << 0,
	INDEX = 1 << 1,
	UNIFORM = 1 << 2,
	STORAGE = 1 << 3,
	TRANSFER_SRC = 1 << 4,
	TRANSFER_DST = 1 << 5
};

inline Usage operator|(Usage a, Usage b)
{
	return static_cast<Usage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline bool hasFlag(Usage value, Usage flag)
{
	return (static_cast<uint32_t>(value) & static_cast<uint32_t>(flag)) != 0;
}

struct Desc
{
	uint64_t size = 0;
	MemoryType memoryType = MemoryType::DEFAULT;
	Usage usage = Usage::STORAGE;
};

/// A structure with the information returned by `Device::allocateDynamicBuffer()`
struct DynamicAlloc
{
	/// The handle of the dynamic buffer
	Handle handle;
	/// The offset to use for dynamic binding
	uint32_t offset = 0;
	/// CPU-accessible mapped pointer
	uint8_t *mapPtr = nullptr;

	inline bool isValid() const { return handle.isValid() && mapPtr != nullptr; }
};

}

} // namespace grail
} // namespace ncine

#endif
