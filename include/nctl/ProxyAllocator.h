#ifndef CLASS_NCTL_PROXYALLOCATOR
#define CLASS_NCTL_PROXYALLOCATOR

#include <nctl/IAllocator.h>

namespace nctl {

/// A proxy allocator
class DLL_PUBLIC ProxyAllocator : public IAllocator
{
  public:
	explicit ProxyAllocator(IAllocator &allocator)
	    : ProxyAllocator("Proxy", allocator) {}
	ProxyAllocator(const char *name, IAllocator &allocator);
	~ProxyAllocator();

  private:
	IAllocator &allocator_;

	ProxyAllocator(const ProxyAllocator &) = delete;
	ProxyAllocator &operator=(const ProxyAllocator &) = delete;

	static void *allocateImpl(IAllocator *allocator, size_t size, uint8_t alignment);
	static void *reallocateImpl(IAllocator *allocator, void *ptr, size_t size, uint8_t alignment, size_t &oldSize);
	static void deallocateImpl(IAllocator *allocator, void *ptr);
};

}

#endif
