#ifndef CLASS_NCTL_STACKALLOCATOR
#define CLASS_NCTL_STACKALLOCATOR

#define STACK_DEBUG (1)
#include <nctl/IAllocator.h>

namespace nctl {

/// A stack allocator
class DLL_PUBLIC StackAllocator : public IAllocator
{
  public:
	StackAllocator();
	StackAllocator(size_t size, void *base);
	~StackAllocator();

	void init(size_t size, void *base);
#if STACK_DEBUG
	inline const void *previous() const { return previous_; }
#endif
	inline const void *current() const { return current_; }

  private:
	struct Header
	{
#if STACK_DEBUG
		void *previous;
#endif
		uint8_t adjustment;
	};

#if STACK_DEBUG
	void *previous_;
#endif
	void *current_;

	StackAllocator(const StackAllocator &) = delete;
	StackAllocator &operator=(const StackAllocator &) = delete;

	static void *allocateImpl(IAllocator *allocator, size_t size, uint8_t alignment);
	static void *reallocateImpl(IAllocator *allocator, void *ptr, size_t size, uint8_t alignment, size_t &oldSize);
	static void deallocateImpl(IAllocator *allocator, void *ptr);
};

}

#endif
