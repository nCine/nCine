#ifndef CLASS_NCTL_MALLOCALLOCATOR
#define CLASS_NCTL_MALLOCALLOCATOR

#include <nctl/IAllocator.h>

namespace nctl {

/// The allocator class based on `malloc` and `free`
class DLL_PUBLIC MallocAllocator : public IAllocator
{
  public:
	MallocAllocator();
	~MallocAllocator();

  private:
	MallocAllocator(const MallocAllocator &) = delete;
	MallocAllocator &operator=(const MallocAllocator &) = delete;

	static void *allocateImpl(IAllocator *allocator, size_t size, uint8_t alignment);
	static void *reallocateImpl(IAllocator *allocator, void *ptr, size_t size, uint8_t alignment, size_t &oldSize);
	static void deallocateImpl(IAllocator *allocator, void *ptr);
};

}

#endif
