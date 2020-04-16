#ifndef CLASS_NCTL_LINEARALLOCATOR
#define CLASS_NCTL_LINEARALLOCATOR

#include <nctl/IAllocator.h>

namespace nctl {

/// A linear allocator
class DLL_PUBLIC LinearAllocator : public IAllocator
{
  public:
	LinearAllocator();
	LinearAllocator(size_t size, void *base);
	~LinearAllocator();

	void init(size_t size, void *base);
	/// Clears and loses all active allocations in costant time
	void clear();
	inline const void *current() const { return current_; }

  private:
	void *current_;

	LinearAllocator(const LinearAllocator &) = delete;
	LinearAllocator &operator=(const LinearAllocator &) = delete;

	static void *allocateImpl(IAllocator *allocator, size_t size, uint8_t alignment);
	static void *reallocateImpl(IAllocator *allocator, void *ptr, size_t size, uint8_t alignment, size_t &oldSize);
	static void deallocateImpl(IAllocator *allocator, void *ptr);
};

}

#endif
