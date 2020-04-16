#include <ncine/common_macros.h>
#include <nctl/FreeListAllocator.h>
#include <nctl/PointerMath.h>
namespace nctl {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

FreeListAllocator::FreeListAllocator()
    : IAllocator(allocateImpl, reallocateImpl, deallocateImpl),
      freeBlocks_(nullptr), fitStrategy_(FitStrategy::BEST_FIT)
{
}

FreeListAllocator::FreeListAllocator(size_t size, void *base, bool defragOnDeallocation)
    : IAllocator(allocateImpl, reallocateImpl, deallocateImpl, size, base),
      freeBlocks_(reinterpret_cast<Block *>(base)), fitStrategy_(FitStrategy::BEST_FIT),
      defragOnDeallocation_(defragOnDeallocation)
{
	// Total size should be bigger than the size of a block header
	FATAL_ASSERT(size > sizeof(Block));
	freeBlocks_->size = size;
	freeBlocks_->next = nullptr;
}

FreeListAllocator::~FreeListAllocator()
{
	FATAL_ASSERT(usedMemory_ == 0 && numAllocations_ == 0);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void FreeListAllocator::init(size_t size, void *base)
{
	FATAL_ASSERT(usedMemory_ == 0 && numAllocations_ == 0);
	size_ = size;
	base_ = base;

	// Total size should be bigger than the size of a block header
	FATAL_ASSERT(size > sizeof(Block));
	freeBlocks_ = reinterpret_cast<Block *>(base);
	freeBlocks_->size = size;
	freeBlocks_->next = nullptr;
}

/*! \note It does not alter allocated pointers */
void FreeListAllocator::defrag()
{
	// Deallocation leaves blocks ordered by address, coalescing in one direction only
	Block *freeBlock = freeBlocks_;
	while (freeBlock != nullptr)
	{
		bool hasCoalesced = true;
		while (hasCoalesced)
		{
			Block *nextBlock = freeBlock->next;
			const size_t distance = PointerMath::subtract(nextBlock, freeBlock);
			hasCoalesced = (freeBlock->size == distance);
			if (hasCoalesced)
			{
				freeBlock->size += nextBlock->size;
				freeBlock->next = nextBlock->next;
			}
		}
		freeBlock = freeBlock->next;
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void *FreeListAllocator::allocateImpl(IAllocator *allocator, size_t bytes, uint8_t alignment)
{
	static_assert(sizeof(Header) >= sizeof(Block), "The header should be bigger than the block");

	FATAL_ASSERT(bytes > 0);
	FATAL_ASSERT_MSG((alignment & (alignment - 1)) == 0, "The alignment should be a power of two");
	FATAL_ASSERT_MSG(alignment >= 1 && alignment <= 128, "The alignment must be between 1 and 128");

	FATAL_ASSERT(allocator);
	FreeListAllocator *allocatorImpl = static_cast<FreeListAllocator *>(allocator);

	Block *previousFreeBlock = nullptr;
	Block *freeBlock = nullptr;

	Block *previousSearchBlock = nullptr;
	Block *searchBlock = allocatorImpl->freeBlocks_;
	while (searchBlock != nullptr)
	{
		// Calculate the adjustment needed to keep objects correctly aligned
		const uint8_t adjustment = PointerMath::alignWithHeader(searchBlock, alignment, sizeof(Header));
		const size_t totalSize = bytes + adjustment;

		if (searchBlock->size >= totalSize)
		{
			if ((allocatorImpl->fitStrategy_ == FitStrategy::BEST_FIT && freeBlock && searchBlock->size < freeBlock->size) ||
			    (allocatorImpl->fitStrategy_ == FitStrategy::WORST_FIT && freeBlock && searchBlock->size > freeBlock->size) ||
			    allocatorImpl->fitStrategy_ == FitStrategy::FIRST_FIT || freeBlock == nullptr)
			{
				previousFreeBlock = previousSearchBlock;
				freeBlock = searchBlock;

				if (allocatorImpl->fitStrategy_ == FitStrategy::FIRST_FIT)
					break;
			}
		}

		previousSearchBlock = searchBlock;
		searchBlock = searchBlock->next;
	}

	const uint8_t adjustment = PointerMath::alignWithHeader(freeBlock, alignment, sizeof(Header));
	size_t totalSize = bytes + adjustment;

	// Cannot find a free block large enough
	if (freeBlock == nullptr)
		return nullptr;

	// If allocating in the remaining memory is not possible
	if (freeBlock->size - totalSize <= sizeof(Header))
	{
		// Increase allocation size instead of creating a new block
		totalSize = freeBlock->size;

		if (previousFreeBlock != nullptr)
			previousFreeBlock->next = freeBlock->next;
		else
			allocatorImpl->freeBlocks_ = freeBlock->next;
	}
	else
	{
		// Otherwise create a new block containing the remaining memory
		Block *nextBlock = reinterpret_cast<Block *>(PointerMath::add(freeBlock, totalSize));

		nextBlock->size = freeBlock->size - totalSize;
		nextBlock->next = freeBlock->next;

		if (previousFreeBlock != nullptr)
			previousFreeBlock->next = nextBlock;
		else
			allocatorImpl->freeBlocks_ = nextBlock;
	}

	void *alignedAddress = PointerMath::add(freeBlock, adjustment);
	Header *header = reinterpret_cast<Header *>(PointerMath::subtract(alignedAddress, sizeof(Header)));
	header->size = totalSize;
	header->adjustment = adjustment;
	allocatorImpl->usedMemory_ += totalSize;
	allocatorImpl->numAllocations_++;
	FATAL_ASSERT(PointerMath::alignAdjustment(alignedAddress, alignment) == 0);

	return alignedAddress;
}

void *FreeListAllocator::reallocateImpl(IAllocator *allocator, void *ptr, size_t bytes, uint8_t alignment, size_t &oldSize)
{
	FATAL_ASSERT(ptr != nullptr);
	FATAL_ASSERT(bytes > 0);
	FATAL_ASSERT_MSG((alignment & (alignment - 1)) == 0, "The alignment should be a power of two");
	FATAL_ASSERT_MSG(alignment >= 1 && alignment <= 128, "The alignment must be between 1 and 128");

	FATAL_ASSERT(allocator);
	FreeListAllocator *allocatorImpl = static_cast<FreeListAllocator *>(allocator);

	Header *header = reinterpret_cast<Header *>(PointerMath::subtract(ptr, sizeof(Header)));
	void *blockStart = PointerMath::subtract(ptr, header->adjustment);
	const size_t blockSize = header->size;
	void *blockEnd = PointerMath::add(blockStart, blockSize);

	Block *previousFreeBlock = nullptr;
	Block *freeBlock = allocatorImpl->freeBlocks_;
	while (freeBlock != nullptr)
	{
		if (freeBlock == blockEnd)
			break;

		previousFreeBlock = freeBlock;
		freeBlock = freeBlock->next;
	}

	oldSize = PointerMath::subtract(blockEnd, ptr);
	const long int difference = bytes - oldSize;

	Block *nextBlock = nullptr;
	// Shrinking or extending the block following the allocation
	if ((difference < 0 && static_cast<size_t>(-difference) > sizeof(Header)) ||
	    (difference > 0 && freeBlock && freeBlock->size > difference + sizeof(Header)))
	{
		// Shrink the block following the allocation
		Block *temp = reinterpret_cast<Block *>(PointerMath::add(ptr, oldSize + difference));
		temp->size = -difference + (freeBlock ? freeBlock->size : 0);
		temp->next = freeBlock ? freeBlock->next : nullptr;
		nextBlock = temp;
	}
	// Swallow the entire block following the allocation
	else if (freeBlock && freeBlock->size == static_cast<size_t>(difference))
		nextBlock = freeBlock->next;
	else
		return nullptr;

	if (previousFreeBlock)
		previousFreeBlock->next = nextBlock;
	else
		allocatorImpl->freeBlocks_ = nextBlock;

	header->size += difference;
	FATAL_ASSERT(difference > 0 || allocatorImpl->usedMemory_ > static_cast<size_t>(-difference));
	allocatorImpl->usedMemory_ += difference;
	return ptr;
}

void FreeListAllocator::deallocateImpl(IAllocator *allocator, void *ptr)
{
	if (ptr == nullptr)
		return;

	FATAL_ASSERT(allocator);
	FreeListAllocator *allocatorImpl = static_cast<FreeListAllocator *>(allocator);

	Header *header = reinterpret_cast<Header *>(PointerMath::subtract(ptr, sizeof(Header)));
	void *blockStart = PointerMath::subtract(ptr, header->adjustment);
	const size_t blockSize = header->size;
	void *blockEnd = PointerMath::add(blockStart, blockSize);

	Block *beforePreviousFreeBlock = nullptr;
	Block *previousFreeBlock = nullptr;
	Block *freeBlock = allocatorImpl->freeBlocks_;
	while (freeBlock != nullptr)
	{
		if (freeBlock > blockEnd)
			break;

		if (beforePreviousFreeBlock)
			beforePreviousFreeBlock = previousFreeBlock;
		previousFreeBlock = freeBlock;
		freeBlock = freeBlock->next;

		if (allocatorImpl->freeBlocks_->next == previousFreeBlock)
			beforePreviousFreeBlock = allocatorImpl->freeBlocks_;
	}

	if (previousFreeBlock == nullptr)
	{
		previousFreeBlock = reinterpret_cast<Block *>(blockStart);
		previousFreeBlock->size = blockSize;
		previousFreeBlock->next = allocatorImpl->freeBlocks_;
		allocatorImpl->freeBlocks_ = previousFreeBlock;

		// Swallowing next adjacent block
		if (blockEnd == previousFreeBlock->next)
		{
			previousFreeBlock->size += previousFreeBlock->next->size;
			previousFreeBlock->next = previousFreeBlock->next->next;
		}
	}
	else if (PointerMath::add(previousFreeBlock, previousFreeBlock->size) == blockStart)
		previousFreeBlock->size += blockSize;
	else
	{
		Block *temp = reinterpret_cast<Block *>(blockStart);
		// Swallowing previous adjacent block
		if (blockEnd == previousFreeBlock)
		{
			temp->size = blockSize + previousFreeBlock->size;
			temp->next = previousFreeBlock->next;

			if (beforePreviousFreeBlock)
				beforePreviousFreeBlock->next = temp;
			else
				allocatorImpl->freeBlocks_ = temp;
		}
		else
		{
			temp->size = blockSize;
			temp->next = previousFreeBlock->next;
			previousFreeBlock->next = temp;
			previousFreeBlock = temp;
		}
	}

	FATAL_ASSERT(allocatorImpl->usedMemory_ >= blockSize);
	allocatorImpl->usedMemory_ -= blockSize;
	FATAL_ASSERT(allocatorImpl->numAllocations_ > 0);
	allocatorImpl->numAllocations_--;

	if (allocatorImpl->usedMemory_ == 0 && allocatorImpl->numAllocations_ == 0)
		allocatorImpl->init(allocatorImpl->size_, allocatorImpl->base_);
	else if (allocatorImpl->defragOnDeallocation_)
		allocatorImpl->defrag();
}

}
