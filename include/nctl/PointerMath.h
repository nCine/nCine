#ifndef CLASS_NCTL_POINTERMATH
#define CLASS_NCTL_POINTERMATH

#include <cstdint>
#include <cstddef>
#include <ncine/common_defines.h>
#include <ncine/common_macros.h>

namespace nctl {

/// A group of functions to perform pointer math operations
namespace PointerMath
{
	inline uintptr_t add(uintptr_t ptr, size_t amount)
	{
		return ptr + amount;
	}

	inline uintptr_t subtract(uintptr_t ptr, size_t amount)
	{
		return ptr - amount;
	}

	inline ptrdiff_t distance(uintptr_t a, uintptr_t b)
	{
		return static_cast<ptrdiff_t>(a - b);
	}

	inline uintptr_t align(uintptr_t ptr, size_t alignment)
	{
		ASSERT((alignment & (alignment - 1)) == 0);
		return (ptr + (alignment - 1)) & ~(alignment - 1);
	}

	inline size_t alignAdjustment(uintptr_t ptr, size_t alignment)
	{
		ASSERT((alignment & (alignment - 1)) == 0);

		const size_t mask = alignment - 1;
		const size_t misalignment = ptr & mask;

		return misalignment ? (alignment - misalignment) : 0;
	}

	inline size_t alignWithHeader(uintptr_t ptr, size_t alignment, size_t headerSize)
	{
		size_t adjustment = alignAdjustment(ptr, alignment);

		if (adjustment < headerSize)
		{
			size_t needed = headerSize - adjustment;

			adjustment += alignment * (needed / alignment);
			if (needed % alignment)
				adjustment += alignment;
		}

		return adjustment;
	}

	inline void *add(void *ptr, size_t amount)
	{
		return reinterpret_cast<void *>(add(reinterpret_cast<uintptr_t>(ptr), amount));
	}

	inline const void *add(const void *ptr, size_t amount)
	{
		return reinterpret_cast<const void *>(add(reinterpret_cast<uintptr_t>(ptr), amount));
	}

	inline void *subtract(void *ptr, size_t amount)
	{
		return reinterpret_cast<void *>(subtract(reinterpret_cast<uintptr_t>(ptr), amount));
	}

	inline const void *subtract(const void *ptr, size_t amount)
	{
		return reinterpret_cast<const void *>(subtract(reinterpret_cast<uintptr_t>(ptr), amount));
	}

	inline ptrdiff_t distance(const void *a, const void *b)
	{
		return distance(reinterpret_cast<uintptr_t>(a), reinterpret_cast<uintptr_t>(b));
	}

	inline void *align(void *ptr, size_t alignment)
	{
		return reinterpret_cast<void *>(align(reinterpret_cast<uintptr_t>(ptr), alignment));
	}

	inline const void *align(const void *ptr, size_t alignment)
	{
		return reinterpret_cast<const void *>(align(reinterpret_cast<uintptr_t>(ptr), alignment));
	}

	inline size_t alignAdjustment(const void *ptr, size_t alignment)
	{
		return alignAdjustment(reinterpret_cast<uintptr_t>(ptr), alignment);
	}

	inline size_t alignWithHeader(const void *ptr, size_t alignment, size_t headerSize)
	{
		return alignWithHeader(reinterpret_cast<uintptr_t>(ptr), alignment, headerSize);
	}
}

}

#endif
