#ifndef CLASS_NCTL_POOLALLOCATOR
#define CLASS_NCTL_POOLALLOCATOR

#include <nctl/IAllocator.h>

namespace nctl {

/// A pool allocator
class DLL_PUBLIC PoolAllocator : public IAllocator
{
  public:
	PoolAllocator();
	PoolAllocator(size_t elementSize, size_t size, void *base)
	    : PoolAllocator(elementSize, DefaultAlignment, size, base) {}
	PoolAllocator(size_t elementSize, uint8_t elementAlignment, size_t size, void *base);
	~PoolAllocator();

	inline void init(size_t elementSize, size_t size, void *base) { init(elementSize, DefaultAlignment, size, base); }
	void init(size_t elementSize, uint8_t elementAlignment, size_t size, void *base);
	inline size_t elementSize() const { return elementSize_; }
	inline uint8_t elementAlignment() const { return elementAlignment_; }
	void **freeList() const { return freeList_; }

  private:
	size_t elementSize_;
	uint8_t elementAlignment_;
	void **freeList_;

	PoolAllocator(const PoolAllocator &) = delete;
	PoolAllocator &operator=(const PoolAllocator &) = delete;

	void internalInit();

	static void *allocateImpl(IAllocator *allocator, size_t size, uint8_t alignment);
	static void *reallocateImpl(IAllocator *allocator, void *ptr, size_t size, uint8_t alignment, size_t &oldSize);
	static void deallocateImpl(IAllocator *allocator, void *ptr);
};

}

#endif
