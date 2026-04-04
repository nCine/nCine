#include "grail/private/GenerationalPool.h"

namespace ncine {
namespace grail {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GenerationalPool::GenerationalPool(uint32_t capacity)
    : generations_(capacity), nextFree_(capacity)
{
	// Initialize the free list
	for (uint32_t i = 0; i < capacity - 1; ++i)
		nextFree_.emplaceBack(i + 1);
	// Last slot (at capacity - 1) has an invalid next free index
	nextFree_.emplaceBack(Invalid);

	// Initialize the generations array
	for (uint32_t i = 0; i < capacity; ++i)
		generations_.emplaceBack(1);

	// The head of the list is the first element from the generations array
	freeListHead_.store(0, nctl::MemoryModel::RELAXED);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

uint32_t GenerationalPool::allocate()
{
	uint32_t head;
	uint32_t next;

	do {
		head = freeListHead_.load(nctl::MemoryModel::ACQUIRE);
		if (head == Invalid)
			return Invalid;

		next = nextFree_[head].load(nctl::MemoryModel::RELAXED);
	} while (!freeListHead_.cmpExchange(head, next, nctl::MemoryModel::RELEASE));

	allocated_.fetchAdd(1, nctl::MemoryModel::RELAXED);
	return head;
}

void GenerationalPool::free(uint32_t index)
{
	uint32_t head;
	do {
		head = freeListHead_.load(nctl::MemoryModel::ACQUIRE);
		nextFree_[index].store(head, nctl::MemoryModel::RELAXED);
	} while (!freeListHead_.cmpExchange(head, index, nctl::MemoryModel::RELEASE));

	allocated_.fetchSub(1, nctl::MemoryModel::RELAXED);
}

} // namespace grail
} // namespace ncine
