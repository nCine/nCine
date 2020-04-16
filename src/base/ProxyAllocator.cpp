#include <ncine/common_macros.h>
#include <nctl/ProxyAllocator.h>

namespace nctl {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ProxyAllocator::ProxyAllocator(IAllocator &allocator)
    : IAllocator(allocateImpl, reallocateImpl, deallocateImpl, 0, nullptr),
      allocator_(allocator)
{
}

ProxyAllocator::~ProxyAllocator()
{
	FATAL_ASSERT(usedMemory_ == 0 && numAllocations_ == 0);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void *ProxyAllocator::allocateImpl(IAllocator *allocator, size_t bytes, uint8_t alignment)
{
	FATAL_ASSERT(allocator);
	ProxyAllocator *allocatorImpl = static_cast<ProxyAllocator *>(allocator);
	IAllocator &subject = allocatorImpl->allocator_;

	const size_t memoryUsedBefore = subject.usedMemory();
	void *ptr = subject.allocateFunc_(&subject, bytes, alignment);
	if (ptr)
	{
		allocatorImpl->usedMemory_ += subject.usedMemory() - memoryUsedBefore;
		allocatorImpl->numAllocations_++;
	}

	return ptr;
}

void *ProxyAllocator::reallocateImpl(IAllocator *allocator, void *ptr, size_t bytes, uint8_t alignment, size_t &oldSize)
{
	FATAL_ASSERT(allocator);
	ProxyAllocator *allocatorImpl = static_cast<ProxyAllocator *>(allocator);
	IAllocator &subject = allocatorImpl->allocator_;

	const size_t memoryUsedBefore = subject.usedMemory();
	void *newPtr = subject.reallocateFunc_(&subject, ptr, bytes, alignment, oldSize);
	if (newPtr)
		allocatorImpl->usedMemory_ += subject.usedMemory() - memoryUsedBefore;

	return newPtr;
}

void ProxyAllocator::deallocateImpl(IAllocator *allocator, void *ptr)
{
	if (ptr == nullptr)
		return;

	FATAL_ASSERT(allocator);
	ProxyAllocator *allocatorImpl = static_cast<ProxyAllocator *>(allocator);
	IAllocator &subject = allocatorImpl->allocator_;

	const size_t memoryUsedBefore = subject.usedMemory();
	subject.deallocateFunc_(&subject, ptr);
	allocatorImpl->usedMemory_ -= memoryUsedBefore - subject.usedMemory();

	FATAL_ASSERT(allocatorImpl->numAllocations_ > 0);
	allocatorImpl->numAllocations_--;
}

}
