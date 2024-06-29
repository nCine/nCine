#ifndef CLASS_NCTL_POINTERMATH
#define CLASS_NCTL_POINTERMATH

#include <cstdint>
#include <cstddef>
#include <ncine/common_defines.h>

namespace nctl {

/// A group of functions to perform pointer math operations
namespace PointerMath
{
	inline void *add(void *ptr, size_t amount) { return reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(ptr) + amount); }
	inline void *subtract(void *ptr, size_t amount) { return reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(ptr) - amount); }

	inline uintptr_t add(void *first, void *second) { return reinterpret_cast<uintptr_t>(first) + reinterpret_cast<uintptr_t>(second); }
	inline uintptr_t subtract(void *first, void *second) { return reinterpret_cast<uintptr_t>(first) - reinterpret_cast<uintptr_t>(second); }

	inline void *align(void *ptr, uint8_t alignment)
	{
		return reinterpret_cast<void *>((reinterpret_cast<uintptr_t>(ptr) +
		                                 static_cast<uintptr_t>(alignment - 1)) &
		                                 static_cast<uintptr_t>(~(alignment - 1)));
	}

	inline uint8_t alignAdjustment(const void *ptr, uint8_t alignment)
	{
		const uint8_t amount = alignment - (reinterpret_cast<uintptr_t>(ptr) & (alignment - 1));

		if (amount == alignment)
			return 0;

		return amount;
	}

	inline uint8_t alignWithHeader(const void *ptr, uint8_t alignment, uint8_t headerSize)
	{
		uint8_t amount = alignAdjustment(ptr, alignment);
		uint8_t spaceNeeded = headerSize;

		if (amount < spaceNeeded)
		{
			spaceNeeded -= amount;

			// Increase the amount to fit the header
			amount += alignment * (spaceNeeded / alignment);

			if (spaceNeeded % alignment > 0)
				amount += alignment;
		}

		return amount;
	}
}

}

#endif
