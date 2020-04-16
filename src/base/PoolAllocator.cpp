#include <ncine/common_macros.h>
#include <nctl/PoolAllocator.h>
#include <nctl/PointerMath.h>

namespace nctl {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

PoolAllocator::PoolAllocator()
    : IAllocator(allocateImpl, reallocateImpl, deallocateImpl),
      elementSize_(0), elementAlignment_(0), freeList_(nullptr)
{
}

PoolAllocator::PoolAllocator(size_t elementSize, uint8_t elementAlignment, size_t size, void *base)
    : IAllocator(allocateImpl, reallocateImpl, deallocateImpl, size, base),
      elementSize_(elementSize), elementAlignment_(elementAlignment), freeList_(nullptr)
{
	internalInit();
}

PoolAllocator::~PoolAllocator()
{
	FATAL_ASSERT(usedMemory_ == 0 && numAllocations_ == 0);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void PoolAllocator::init(size_t elementSize, uint8_t elementAlignment, size_t size, void *base)
{
	FATAL_ASSERT(usedMemory_ == 0 && numAllocations_ == 0);
	size_ = size;
	base_ = base;
	elementSize_ = elementSize;
	elementAlignment_ = elementAlignment;

	internalInit();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void PoolAllocator::internalInit()
{
	// The element should be bigger than a pointer in the free list
	ASSERT(elementSize_ >= sizeof(void *));

	const uint8_t adjustment = PointerMath::alignAdjustment(base_, elementAlignment_);

	freeList_ = reinterpret_cast<void **>(PointerMath::add(base_, adjustment));
	const size_t numElements = (size_ - adjustment) / elementSize_;
	void **ptr = freeList_;

	// Initialize the free blocks list
	for (size_t i = 0; i < numElements - 1; i++)
	{
		*ptr = PointerMath::add(ptr, elementSize_);
		ptr = reinterpret_cast<void **>(*ptr);
	}

	*ptr = nullptr;
}

void *PoolAllocator::allocateImpl(IAllocator *allocator, size_t bytes, uint8_t alignment)
{
	FATAL_ASSERT(bytes > 0);
	FATAL_ASSERT_MSG((alignment & (alignment - 1)) == 0, "The alignment should be a power of two");
	FATAL_ASSERT_MSG(alignment >= 1 && alignment <= 128, "The alignment must be between 1 and 128");

	FATAL_ASSERT(allocator);
	PoolAllocator *allocatorImpl = static_cast<PoolAllocator *>(allocator);

	FATAL_ASSERT(bytes == allocatorImpl->elementSize_);
	FATAL_ASSERT(alignment == allocatorImpl->elementAlignment_);

	if (allocatorImpl->freeList_ == nullptr)
		return nullptr;

	void *ptr = allocatorImpl->freeList_;
	allocatorImpl->freeList_ = reinterpret_cast<void **>(*allocatorImpl->freeList_);

	allocatorImpl->usedMemory_ += bytes;
	allocatorImpl->numAllocations_++;
	return ptr;
}

void *PoolAllocator::reallocateImpl(IAllocator *allocator, void *ptr, size_t bytes, uint8_t alignment, size_t &oldSize)
{
	ASSERT_MSG(false, "PoolAllocator cannot reallocate");

	FATAL_ASSERT(allocator);
	PoolAllocator *allocatorImpl = static_cast<PoolAllocator *>(allocator);

	// Never try to allocte a new block and perform a copy of the data in `IAllocator`
	allocatorImpl->copyOnReallocation_ = false;
	oldSize = 0;

	return nullptr;
}

void PoolAllocator::deallocateImpl(IAllocator *allocator, void *ptr)
{
	if (ptr == nullptr)
		return;

	FATAL_ASSERT(allocator);
	PoolAllocator *allocatorImpl = static_cast<PoolAllocator *>(allocator);

	*reinterpret_cast<void **>(ptr) = allocatorImpl->freeList_;
	allocatorImpl->freeList_ = reinterpret_cast<void **>(ptr);
	allocatorImpl->usedMemory_ -= allocatorImpl->elementSize_;

	FATAL_ASSERT(allocatorImpl->numAllocations_ > 0);
	allocatorImpl->numAllocations_--;
}

}
