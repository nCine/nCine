#include <nctl/IAllocator.h>
#include <ncine/common_macros.h>

#ifdef RECORD_ALLOCATIONS
	#include <cstdio>
#endif

namespace nctl {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

IAllocator::IAllocator(AllocateFunction allocFunc, ReallocateFunction reallocFunc, DeallocateFunction deallocFunc, size_t size, void *base)
#ifndef RECORD_ALLOCATIONS
    : allocateFunc_(allocFunc), reallocateFunc_(reallocFunc), deallocateFunc_(deallocFunc),
      copyOnReallocation_(true), size_(size), base_(base), usedMemory_(0), numAllocations_(0)
#else
    : allocateFunc_(recordAllocate), reallocateFunc_(recordReallocate), deallocateFunc_(recordDeallocate), recordAllocations_(true),
      copyOnReallocation_(true), size_(size), base_(base), usedMemory_(0), numAllocations_(0),
      realAllocateFunc_(allocFunc), realReallocateFunc_(reallocFunc), realDeallocateFunc_(deallocFunc), numEntries_(0)
#endif
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

#ifdef RECORD_ALLOCATIONS

int IAllocator::findAllocation(void *ptr)
{
	int index = -1;
	for (size_t i = 0; i < numEntries_; i++)
	{
		const IAllocator::Entry &e = entries_[i];
		if (e.alignment > 0 && e.ptr == ptr)
		{
			index = static_cast<int>(i);
			break;
		}
	}

	return index;
}

unsigned int IAllocator::findDeallocation(unsigned int index)
{
	if (entries_[index].alignment == 0)
		return 0;

	// Entry zero cannot be a deallocation
	unsigned int deallocationIndex = 0;

	const void *ptr = entries_[index].ptr;
	for (size_t i = index + 1; i < numEntries_; i++)
	{
		const nctl::IAllocator::Entry &e = entries_[i];
		if (e.alignment == 0 && e.ptr == ptr)
		{
			deallocationIndex = i;
			break;
		}
	}

	return deallocationIndex;
}

void IAllocator::printEntriesCSV()
{
	for (size_t i = 0; i < numEntries_; i++)
	{
		const IAllocator::Entry &e = entries_[i];
		printf("%lu,%lu,0x%lx,%lu,%u,%lu,%lu\n", i, e.timestamp.ticks(), uintptr_t(e.ptr), e.bytes, e.alignment, e.usedMemory, e.numAllocations);
	}
}

void IAllocator::countNotFreed()
{
	unsigned int notFreed = 0;
	for (size_t i = 0; i < numEntries_; i++)
	{
		const IAllocator::Entry &e = entries_[i];
		if (e.alignment > 0)
		{
			printf("- #%lu 0x%lx, %lu bytes, %u align", i, uintptr_t(e.ptr), e.bytes, e.alignment);

			const unsigned int deallocateEntry = findDeallocation(i);
			if (deallocateEntry > 0)
				printf(" - freed #%u\n", deallocateEntry);
			else
			{
				printf(" - NOT FREED\n");
				notFreed++;
			}
		}
	}
	printf("-> Not freed allocations: %u\n", notFreed);
}

struct Counters
{
	void *ptr = nullptr;
	size_t countAlloc;
	size_t countDealloc;
};

void IAllocator::printPointerCounters()
{
	Counters counters[MaxEntries];
	size_t numCounterEntries = 0;

	for (size_t i = 0; i < numEntries_; i++)
	{
		const IAllocator::Entry &e = entries_[i];
		bool found = false;
		for (size_t j = 0; j < numCounterEntries; j++)
		{
			if (counters[j].ptr == e.ptr)
			{
				if (e.alignment > 0)
					counters[j].countAlloc++;
				else
					counters[j].countDealloc++;
				found = true;
				break;
			}
		}

		if (found == false)
		{
			counters[numCounterEntries].ptr = e.ptr;
			if (e.bytes > 0)
				counters[numCounterEntries].countAlloc++;
			else
				counters[numCounterEntries].countDealloc++;
			numCounterEntries++;
		}
	}

	for (size_t i = 0; i < numCounterEntries; i++)
	{
		const Counters &c = counters[i];
		if (c.countAlloc != c.countDealloc)
			printf("#%lu,0x%lx,+%lu,-%lu\n", i, uintptr_t(c.ptr), c.countAlloc, c.countDealloc);
	}
}

#endif

void *IAllocator::reallocate(void *ptr, size_t bytes, uint8_t alignment)
{
	if (bytes == 0)
	{
		deallocate(ptr);
		return nullptr;
	}
	else if (ptr == nullptr)
		return allocate(bytes, alignment);

	size_t oldSize = 0;
	void *newPtr = (*reallocateFunc_)(this, ptr, bytes, alignment, oldSize);
	if (newPtr == nullptr && copyOnReallocation_)
	{
		newPtr = allocate(bytes, alignment);
		if (newPtr)
		{
			memcpy(newPtr, ptr, (bytes < oldSize) ? bytes : oldSize);
			(*deallocateFunc_)(this, ptr);
		}
	}

	return newPtr;
}

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

#ifdef RECORD_ALLOCATIONS

void *IAllocator::recordAllocate(IAllocator *allocator, size_t bytes, uint8_t alignment)
{
	void *ptr = (*allocator->realAllocateFunc_)(allocator, bytes, alignment);

	if (allocator->recordAllocations_ && allocator->numEntries_ < MaxEntries && ptr != nullptr && bytes > 0)
	{
		Entry &entry = allocator->entries_[allocator->numEntries_];
		entry.timestamp = ncine::TimeStamp::now();
		entry.ptr = ptr;
		entry.bytes = bytes;
		entry.alignment = alignment;
		entry.usedMemory = allocator->usedMemory_;
		entry.numAllocations = allocator->numAllocations_;
		allocator->numEntries_++;
	}

	return ptr;
}

void *IAllocator::recordReallocate(IAllocator *allocator, void *ptr, size_t bytes, uint8_t alignment, size_t &oldSize)
{
	void *newPtr = (*allocator->realReallocateFunc_)(allocator, ptr, bytes, alignment, oldSize);
	if (allocator->recordAllocations_ && newPtr != nullptr)
	{
		for (size_t i = 0; i < allocator->numEntries_; i++)
		{
			if (allocator->entry(i).ptr == ptr)
			{
				Entry &entry = allocator->entries_[i];
				entry.timestamp = ncine::TimeStamp::now();
				entry.ptr = newPtr;
				entry.bytes = bytes;
				entry.alignment = alignment;
				entry.usedMemory = allocator->usedMemory_;
				entry.numAllocations = allocator->numAllocations_;
				break;
			}
		}
	}

	return newPtr;
}

void IAllocator::recordDeallocate(IAllocator *allocator, void *ptr)
{
	const size_t memoryUsedBefore = allocator->usedMemory();
	(*allocator->realDeallocateFunc_)(allocator, ptr);

	if (allocator->recordAllocations_ && allocator->numEntries_ < MaxEntries && ptr != nullptr)
	{
		Entry &entry = allocator->entries_[allocator->numEntries_];
		entry.timestamp = ncine::TimeStamp::now();
		entry.ptr = ptr;
		entry.bytes = memoryUsedBefore - allocator->usedMemory_;
		entry.alignment = 0;
		entry.usedMemory = allocator->usedMemory_;
		entry.numAllocations = allocator->numAllocations_;
		allocator->numEntries_++;
	}
}

#endif

}
