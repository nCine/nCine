#ifndef CLASS_NCTL_IALLOCATOR
#define CLASS_NCTL_IALLOCATOR

#include <cstdint>
#include <new>
#include <ncine/common_defines.h>
#include <ncine/allocators_config.h>
#include "utility.h"

#ifdef RECORD_ALLOCATIONS
	#include <ncine/TimeStamp.h>
#endif

namespace nctl {

/// The allocator interface
class DLL_PUBLIC IAllocator
{
  public:
	/// 16 bytes default alignment on 64bit and 8 bytes on 32bit
	static const uint8_t DefaultAlignment = 2 * sizeof(void *);

#ifdef RECORD_ALLOCATIONS
	struct Entry
	{
		ncine::TimeStamp timestamp;
		void *ptr = nullptr;
		size_t bytes = 0;
		/// Deallocations are entries with an alignment value of zero
		uint8_t alignment = DefaultAlignment;
		size_t usedMemory;
		size_t numAllocations;
	};

	inline bool recordAllocations() const { return recordAllocations_; }
	inline void setRecordAllocations(bool recordAllocations) { recordAllocations_ = recordAllocations; }

	inline const Entry &entry(unsigned int index) const { return entries_[index]; }
	inline const size_t numEntries() const { return numEntries_; }

	int findAllocation(void *ptr);
	unsigned int findDeallocation(unsigned int index);
	void printEntriesCSV();
	void countNotFreed();
	void printPointerCounters();
#endif

	using AllocateFunction = void *(*)(IAllocator *allocator, size_t, uint8_t);
	using ReallocateFunction = void *(*)(IAllocator *allocator, void *, size_t, uint8_t, size_t &);
	using DeallocateFunction = void (*)(IAllocator *allocator, void *);

	IAllocator(AllocateFunction allocFunc, ReallocateFunction reallocFunc, DeallocateFunction deallocFunc)
	    : IAllocator(allocFunc, reallocFunc, deallocFunc, 0, nullptr) {}
	IAllocator(AllocateFunction allocFunc, ReallocateFunction reallocFunc, DeallocateFunction deallocFunc, size_t size, void *base);

	/// Tries to allocate the specified amount of memory with the specified alignment requirement
	inline void *allocate(size_t bytes, uint8_t alignment) { return (*allocateFunc_)(this, bytes, alignment); }
	inline void *allocate(size_t bytes) { return (*allocateFunc_)(this, bytes, DefaultAlignment); }
	/// Tries to reallocate the allocation at the specified pointer with a different size
	void *reallocate(void *ptr, size_t bytes, uint8_t alignment);
	inline void *reallocate(void *ptr, size_t bytes) { return reallocate(ptr, bytes, DefaultAlignment); }
	/// Deallocates the allocation at the specified pointer
	inline void deallocate(void *ptr) { (*deallocateFunc_)(this, ptr); }

	/// Returns the size of the buffer used for allocations
	inline size_t size() const { return size_; }
	/// Returns the address of the buffer used for allocations
	inline void *base() const { return base_; }
	/// Returns the amount of memory in use
	//! \note It can be more than the memory used by allocations due to overhead */
	inline size_t usedMemory() const { return usedMemory_; }
	/// Returns the amount of memory available in the buffer
	//! It can be less than the memory availble to allocations due to overhead or fragmentation */
	inline size_t freeMemory() const { return size_ - usedMemory_; }
	/// Returns the number of active allocations
	inline size_t numAllocations() const { return numAllocations_; }

	/// Returns the state of the copy on reallocation flag
	inline bool copyOnReallocation() const { return copyOnReallocation_; }
	/// Sets the state of the copy on reallocation flag
	/*! \note When the flag is true a growing reallocation might move the data in a new place */
	inline void setCopyOnReallocation(bool value) { copyOnReallocation_ = value; }

	template <class T, typename... Args> T *newObject(Args &&... args);
	template <class T> void deleteObject(T *ptr);
	template <class T> T *newArray(size_t numElements);
	template <class T> void deleteArray(T *ptr);

  protected:
	AllocateFunction allocateFunc_;
	ReallocateFunction reallocateFunc_;
	DeallocateFunction deallocateFunc_;
	bool copyOnReallocation_;
	size_t size_;
	void *base_;
	size_t usedMemory_;
	size_t numAllocations_;

#ifdef RECORD_ALLOCATIONS
	AllocateFunction realAllocateFunc_;
	ReallocateFunction realReallocateFunc_;
	DeallocateFunction realDeallocateFunc_;
	bool recordAllocations_;

	static const unsigned int MaxEntries = 100 * 1000;
	Entry entries_[MaxEntries];
	size_t numEntries_;

	static void *recordAllocate(IAllocator *allocator, size_t bytes, uint8_t alignment);
	static void *recordReallocate(IAllocator *allocator, void *ptr, size_t bytes, uint8_t alignment, size_t &oldSize);
	static void recordDeallocate(IAllocator *allocator, void *ptr);
#endif

	friend class ProxyAllocator;
};

namespace detail {

	/// A container for functions to allocate and construct objects and arrays of objects
	template <bool value>
	struct allocHelpers
	{
		template <class T, typename... Args>
		inline static T *newObject(IAllocator &alloc, Args &&... args)
		{
			return new (alloc.allocate(sizeof(T))) T(nctl::forward<Args>(args)...);
		}

		template <class T>
		inline static void deleteObject(IAllocator &alloc, T *ptr)
		{
			ptr->~T();
			alloc.deallocate(ptr);
		}

		template <class T>
		inline static T *newArray(IAllocator &alloc, size_t numElements)
		{
			uint8_t headerSize = sizeof(size_t) / sizeof(T);

			if (sizeof(size_t) % sizeof(T) > 0)
				headerSize += 1;

			// Allocate extra space to store array length in the bytes before the array
			T *ptr = static_cast<T *>(alloc.allocate(sizeof(T) * (numElements + headerSize))) + headerSize;
			*reinterpret_cast<size_t *>(uintptr_t(ptr) - sizeof(size_t)) = numElements;

			for (size_t i = 0; i < numElements; i++)
				new (ptr + i) T();

			return ptr;
		}

		template <class T>
		inline static void deleteArray(IAllocator &alloc, T *ptr)
		{
			const size_t numElements = *reinterpret_cast<size_t *>(uintptr_t(ptr) - sizeof(size_t));

			for (size_t i = numElements; i > 0; i--)
				(ptr + i - 1)->~T();

			// Calculate how much extra memory was allocated to store the length before the array
			uint8_t headerSize = sizeof(size_t) / sizeof(T);
			if (sizeof(size_t) % sizeof(T) > 0)
				headerSize += 1;
			alloc.deallocate(ptr - headerSize);
		}
	};

	/// Specialization for trivially constructible and destructible types
	template <>
	struct allocHelpers<true>
	{
		template <class T, typename... Args>
		inline static T *newObject(IAllocator &alloc, Args &&... args)
		{
			// Trivially constructible types should still got their arguments forwarded
			return new (alloc.allocate(sizeof(T))) T(nctl::forward<Args>(args)...);
		}

		template <class T>
		inline static void deleteObject(IAllocator &alloc, T *ptr)
		{
			alloc.deallocate(ptr);
		}

		template <class T>
		inline static T *newArray(IAllocator &alloc, size_t numElements)
		{
			uint8_t headerSize = sizeof(size_t) / sizeof(T);

			if (sizeof(size_t) % sizeof(T) > 0)
				headerSize += 1;

			// Allocate extra space to store array length in the bytes before the array
			T *ptr = static_cast<T *>(alloc.allocate(sizeof(T) * (numElements + headerSize))) + headerSize;
			*reinterpret_cast<size_t *>(uintptr_t(ptr) - sizeof(size_t)) = numElements;

			return ptr;
		}

		template <class T>
		inline static void deleteArray(IAllocator &alloc, T *ptr)
		{
			// Calculate how much extra memory was allocated to store the length before the array
			uint8_t headerSize = sizeof(size_t) / sizeof(T);
			if (sizeof(size_t) % sizeof(T) > 0)
				headerSize += 1;
			alloc.deallocate(ptr - headerSize);
		}
	};

}

template <class T, typename... Args>
T *IAllocator::newObject(Args &&... args)
{
	return detail::allocHelpers<isTriviallyConstructible<T>::value>::template newObject<T>(*this, nctl::forward<Args>(args)...);
}

template <class T>
void IAllocator::deleteObject(T *ptr)
{
	if (ptr)
		detail::allocHelpers<isTriviallyDestructible<T>::value>::deleteObject(*this, ptr);
}

template <class T>
T *IAllocator::newArray(size_t numElements)
{
	if (numElements == 0)
		return nullptr;

	return detail::allocHelpers<isTriviallyConstructible<T>::value>::template newArray<T>(*this, numElements);
}

template <class T>
void IAllocator::deleteArray(T *ptr)
{
	if (ptr)
		detail::allocHelpers<isTriviallyDestructible<T>::value>::deleteArray(*this, ptr);
}

}

#endif
