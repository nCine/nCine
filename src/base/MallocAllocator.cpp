#include <ncine/common_macros.h>
#include <nctl/MallocAllocator.h>
#include <ncine/tracy.h>

namespace nctl {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

MallocAllocator::MallocAllocator()
    : IAllocator(allocateImpl, reallocateImpl, deallocateImpl, 1, nullptr)
{
}

MallocAllocator::~MallocAllocator()
{
	FATAL_ASSERT(usedMemory_ == 0 && numAllocations_ == 0);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void *MallocAllocator::allocateImpl(IAllocator *allocator, size_t bytes, uint8_t alignment)
{
	FATAL_ASSERT(bytes > 0);
	FATAL_ASSERT_MSG((alignment & (alignment - 1)) == 0, "The alignment should be a power of two");
	FATAL_ASSERT_MSG(alignment >= 1 && alignment <= 128, "The alignment must be between 1 and 128");

	FATAL_ASSERT(allocator);
	MallocAllocator *allocatorImpl = static_cast<MallocAllocator *>(allocator);

	void *ptr = malloc(bytes);
	TracyAllocS(ptr, bytes, 5);

	if (ptr != nullptr)
		allocatorImpl->numAllocations_++;

	return ptr;
}

void *MallocAllocator::reallocateImpl(IAllocator *allocator, void *ptr, size_t bytes, uint8_t alignment, size_t &oldSize)
{
	FATAL_ASSERT(ptr != nullptr);
	FATAL_ASSERT(bytes > 0);
	FATAL_ASSERT_MSG((alignment & (alignment - 1)) == 0, "The alignment should be a power of two");
	FATAL_ASSERT_MSG(alignment >= 1 && alignment <= 128, "The alignment must be between 1 and 128");

	FATAL_ASSERT(allocator);
	MallocAllocator *allocatorImpl = static_cast<MallocAllocator *>(allocator);

	// Never perform a second copy of the data in `IAllocator`
	allocatorImpl->copyOnReallocation_ = false;
	// Previous allocation size is unknown
	oldSize = 0;

	// There is no way to know how many bytes were allocated previously
	return realloc(ptr, bytes);
}

void MallocAllocator::deallocateImpl(IAllocator *allocator, void *ptr)
{
	if (ptr == nullptr)
		return;

	FATAL_ASSERT(allocator);
	MallocAllocator *allocatorImpl = static_cast<MallocAllocator *>(allocator);

	FATAL_ASSERT(allocatorImpl->numAllocations_ > 0);
	allocatorImpl->numAllocations_--;

	TracyFreeS(ptr, 5);
	free(ptr);
}

}
