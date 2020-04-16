#ifndef CLASS_NCTL_POINTERMATH
#define CLASS_NCTL_POINTERMATH

#include <cstdint>
#include <cstddef>
#include <ncine/common_defines.h>

#ifdef _MSC_VER
	#define nc_inline
#else
	#define nc_inline inline
#endif

namespace nctl {

/// A class to perform pointer math operations
class DLL_PUBLIC PointerMath
{
  public:
	static void *add(void *ptr, size_t amount);
	static void *subtract(void *ptr, size_t amount);

	static uintptr_t add(void *first, void *second);
	static uintptr_t subtract(void *first, void *second);

	static void *align(void *ptr, uint8_t alignment);
	static uint8_t alignAdjustment(const void *ptr, uint8_t alignment);
	static uint8_t alignWithHeader(const void *ptr, uint8_t alignment, uint8_t headerSize);
};

nc_inline void *PointerMath::add(void *ptr, size_t amount)
{
	return reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(ptr) + amount);
}

nc_inline void *PointerMath::subtract(void *ptr, size_t amount)
{
	return reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(ptr) - amount);
}

nc_inline uintptr_t PointerMath::add(void *first, void *second)
{
	return reinterpret_cast<uintptr_t>(first) + reinterpret_cast<uintptr_t>(second);
}

nc_inline uintptr_t PointerMath::subtract(void *first, void *second)
{
	return reinterpret_cast<uintptr_t>(first) - reinterpret_cast<uintptr_t>(second);
}

nc_inline void *PointerMath::align(void *ptr, uint8_t alignment)
{
	return reinterpret_cast<void *>((reinterpret_cast<uintptr_t>(ptr) +
	                                 static_cast<uintptr_t>(alignment - 1)) &
	                                static_cast<uintptr_t>(~(alignment - 1)));
}

nc_inline uint8_t PointerMath::alignAdjustment(const void *ptr, uint8_t alignment)
{
	uint8_t amount = alignment - (reinterpret_cast<uintptr_t>(ptr) & (alignment - 1));

	if (amount == alignment)
		return 0;

	return amount;
}

nc_inline uint8_t PointerMath::alignWithHeader(const void *ptr, uint8_t alignment, uint8_t headerSize)
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

#endif
