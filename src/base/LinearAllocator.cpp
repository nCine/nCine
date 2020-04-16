#include <ncine/common_macros.h>
#include <nctl/LinearAllocator.h>
#include <nctl/PointerMath.h>

namespace nctl {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

LinearAllocator::LinearAllocator()
    : IAllocator(allocateImpl, reallocateImpl, deallocateImpl),
      current_(nullptr)
{
}

LinearAllocator::LinearAllocator(size_t size, void *base)
    : IAllocator(allocateImpl, reallocateImpl, deallocateImpl, size, base),
      current_(base)
{
}

LinearAllocator::~LinearAllocator()
{
	FATAL_ASSERT(usedMemory_ == 0 && numAllocations_ == 0);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LinearAllocator::init(size_t size, void *base)
{
	FATAL_ASSERT(usedMemory_ == 0 && numAllocations_ == 0);
	size_ = size;
	base_ = base;
	current_ = base_;
}

void LinearAllocator::clear()
{
	current_ = base_;
	usedMemory_ = 0;
	numAllocations_ = 0;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void *LinearAllocator::allocateImpl(IAllocator *allocator, size_t bytes, uint8_t alignment)
{
	FATAL_ASSERT(bytes > 0);
	FATAL_ASSERT_MSG((alignment & (alignment - 1)) == 0, "The alignment should be a power of two");
	FATAL_ASSERT_MSG(alignment >= 1 && alignment <= 128, "The alignment must be between 1 and 128");

	FATAL_ASSERT(allocator);
	LinearAllocator *allocatorImpl = static_cast<LinearAllocator *>(allocator);

	const uint8_t adjustment = PointerMath::alignAdjustment(allocatorImpl->current_, alignment);

	if (PointerMath::add(allocatorImpl->current_, bytes + adjustment) >
	    PointerMath::add(allocatorImpl->base_, allocatorImpl->size_))
	{
		return nullptr;
	}
	else
	{
		void *alignedAddress = PointerMath::add(allocatorImpl->current_, adjustment);
		allocatorImpl->usedMemory_ += bytes + adjustment;
		allocatorImpl->numAllocations_++;
		allocatorImpl->current_ = PointerMath::add(allocatorImpl->current_, bytes + adjustment);
		return alignedAddress;
	}
}

void *LinearAllocator::reallocateImpl(IAllocator *allocator, void *ptr, size_t bytes, uint8_t alignment, size_t &oldSize)
{
	ASSERT_MSG(false, "LinearAllocator cannot reallocate");

	FATAL_ASSERT(allocator);
	LinearAllocator *allocatorImpl = static_cast<LinearAllocator *>(allocator);

	// Never try to allocte a new block and perform a copy of the data in `IAllocator`
	allocatorImpl->copyOnReallocation_ = false;
	oldSize = 0;

	return nullptr;
}

void LinearAllocator::deallocateImpl(IAllocator *allocator, void *ptr)
{
	if (ptr == nullptr)
		return;

	ASSERT_MSG(false, "LinearAllocator cannot deallocate");
}

}
