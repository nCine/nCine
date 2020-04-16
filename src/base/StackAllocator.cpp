#include <ncine/common_macros.h>
#include <nctl/StackAllocator.h>
#include <nctl/PointerMath.h>

namespace nctl {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

StackAllocator::StackAllocator()
    : IAllocator(allocateImpl, reallocateImpl, deallocateImpl),
      current_(nullptr)
{
#if STACK_DEBUG
	previous_ = nullptr;
#endif
}

StackAllocator::StackAllocator(size_t size, void *base)
    : IAllocator(allocateImpl, reallocateImpl, deallocateImpl, size, base),
      current_(base)
{
#if STACK_DEBUG
	previous_ = nullptr;
#endif
}

StackAllocator::~StackAllocator()
{
	FATAL_ASSERT(usedMemory_ == 0 && numAllocations_ == 0);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void StackAllocator::init(size_t size, void *base)
{
	FATAL_ASSERT(usedMemory_ == 0 && numAllocations_ == 0);
	size_ = size;
	base_ = base;
	current_ = base;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void *StackAllocator::allocateImpl(IAllocator *allocator, size_t bytes, uint8_t alignment)
{
	FATAL_ASSERT(bytes > 0);
	FATAL_ASSERT_MSG((alignment & (alignment - 1)) == 0, "The alignment must be a power of two");
	FATAL_ASSERT_MSG(alignment >= 1 && alignment <= 128, "The alignment must be between 1 and 128");

	FATAL_ASSERT(allocator);
	StackAllocator *allocatorImpl = static_cast<StackAllocator *>(allocator);

	const uint8_t adjustment = PointerMath::alignWithHeader(allocatorImpl->current_, alignment, sizeof(Header));

	if (allocatorImpl->usedMemory_ + adjustment + bytes > allocatorImpl->size_)
		return nullptr;

	void *alignedAddress = PointerMath::add(allocatorImpl->current_, adjustment);

	// Add allocation header
	Header *header = reinterpret_cast<Header *>(PointerMath::subtract(alignedAddress, sizeof(Header)));
	header->adjustment = adjustment;

#if STACK_DEBUG
	header->previous = allocatorImpl->previous_;
	allocatorImpl->previous_ = alignedAddress;
#endif

	allocatorImpl->current_ = PointerMath::add(alignedAddress, bytes);
	allocatorImpl->usedMemory_ += bytes + adjustment;
	allocatorImpl->numAllocations_++;

	return alignedAddress;
}

void *StackAllocator::reallocateImpl(IAllocator *allocator, void *ptr, size_t bytes, uint8_t alignment, size_t &oldSize)
{
	FATAL_ASSERT(ptr != nullptr);
	FATAL_ASSERT(bytes > 0);
	FATAL_ASSERT_MSG((alignment & (alignment - 1)) == 0, "The alignment should be a power of two");
	FATAL_ASSERT_MSG(alignment >= 1 && alignment <= 128, "The alignment must be between 1 and 128");

	FATAL_ASSERT(allocator);
	StackAllocator *allocatorImpl = static_cast<StackAllocator *>(allocator);

	// Never try to allocte a new block and perform a copy of the data in `IAllocator`
	allocatorImpl->copyOnReallocation_ = false;

#if STACK_DEBUG
	FATAL_ASSERT(ptr == allocatorImpl->previous_);
#endif

	oldSize = PointerMath::subtract(allocatorImpl->current_, ptr);
	const long int diffBytes = bytes - oldSize;

	void *newCurrent = PointerMath::add(ptr, bytes);
	if (newCurrent > PointerMath::add(allocatorImpl->base_, allocatorImpl->size_))
		return nullptr;

	allocatorImpl->current_ = newCurrent;
	allocatorImpl->usedMemory_ += diffBytes;
	return ptr;
}

void StackAllocator::deallocateImpl(IAllocator *allocator, void *ptr)
{
	if (ptr == nullptr)
		return;

	FATAL_ASSERT(allocator);
	StackAllocator *allocatorImpl = static_cast<StackAllocator *>(allocator);

#if STACK_DEBUG
	FATAL_ASSERT(ptr == allocatorImpl->previous_);
#endif

	// Access the header in the bytes before `ptr`
	Header *header = reinterpret_cast<Header *>(PointerMath::subtract(ptr, sizeof(Header)));
	allocatorImpl->usedMemory_ -= PointerMath::subtract(allocatorImpl->current_, ptr) + header->adjustment;
	allocatorImpl->current_ = PointerMath::subtract(ptr, header->adjustment);

#if STACK_DEBUG
	allocatorImpl->previous_ = header->previous;
#endif

	FATAL_ASSERT(allocatorImpl->numAllocations_ > 0);
	allocatorImpl->numAllocations_--;
}

}
