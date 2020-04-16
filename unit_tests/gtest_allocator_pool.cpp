#include "gtest_allocators.h"

namespace {

class AllocatorPoolTest : public ::testing::Test
{
  public:
	AllocatorPoolTest()
	    : allocator_(ElementSize, BufferSize, &buffer_) {}

  protected:
	uint8_t buffer_[BufferSize];
	nctl::PoolAllocator allocator_;
};

TEST(AllocatorPoolDeathTest, AllocateZeroBytes)
{
	uint8_t buffer[BufferSize];
	nctl::PoolAllocator allocator(ElementSize, BufferSize, &buffer);

	printf("Allocating zero bytes with the PoolAllocator\n");
	ASSERT_DEATH(allocator.allocate(0, ElementSize), "");
}

TEST(AllocatorPoolDeathTest, ZeroAlignment)
{
	uint8_t buffer[BufferSize];
	nctl::PoolAllocator allocator(ElementSize, BufferSize, &buffer);

	printf("Allocating with zero alignment with the PoolAllocator\n");
	ASSERT_DEATH(allocator.allocate(ElementSize, 0), "");
}

#if NCINE_DEBUG
TEST(AllocatorPoolDeathTest, Reallocate)
{
	uint8_t buffer[BufferSize];
	nctl::PoolAllocator allocator(ElementSize, BufferSize, &buffer);

	printf("Allocating %lu bytes with the PoolAllocator\n", ElementSize);
	ElementType *ptr = reinterpret_cast<ElementType *>(allocator.allocate(ElementSize));
	ASSERT_NE(ptr, nullptr);
	ASSERT_EQ(allocator.numAllocations(), 1);

	const size_t NewBytes = ElementSize / 2;
	printf("Shrinking the allocation to %lu bytes\n", NewBytes);
	ASSERT_DEATH(allocator.reallocate(ptr, NewBytes), "");
	ASSERT_EQ(allocator.numAllocations(), 1);

	printf("Deallocating %lu bytes from the PoolAllocator\n", ElementSize);
	allocator.deallocate(ptr);
}
#endif

TEST_F(AllocatorPoolTest, DefaultConstructor)
{
	nctl::PoolAllocator allocator;
	allocator.init(ElementSize, BufferSize, &buffer_);

	printf("Allocating from a PoolAllocator not initialized in the constructor\n");
	ElementType *ptr = reinterpret_cast<ElementType *>(allocator.allocate(ElementSize));
	ASSERT_NE(ptr, nullptr);
	ASSERT_EQ(allocator.numAllocations(), 1);

	printf("Dellocating from a PoolAllocator not initialized in the constructor\n");
	allocator.deallocate(ptr);
	ASSERT_EQ(allocator.numAllocations(), 0);
}

TEST_F(AllocatorPoolTest, AllocateDeallocate)
{
	const size_t Bytes = NumElements * ElementSize;
	ElementType *ptrs[ElementSize];
	printf("Allocating %lu bytes for %d elements with the PoolAllocator\n", Bytes, NumElements);
	for (unsigned int i = 0; i < NumElements; i++)
	{
		ptrs[i] = reinterpret_cast<ElementType *>(allocator_.allocate(ElementSize));
		ASSERT_NE(ptrs[i], nullptr);
		ASSERT_EQ(allocator_.numAllocations(), i + 1);
		ASSERT_GE(allocator_.usedMemory(), ElementSize * (i + 1));
		ASSERT_LE(allocator_.usedMemory(), ElementSize * (i + 1) + nctl::IAllocator::DefaultAlignment);
	}

	printf("Filling the memory with %d integers\n", NumElements);
	for (unsigned int i = 0; i < NumElements; i++)
		fillElements(ptrs[i], 1);

	printf("Deallocating %lu bytes for %d elements with the PoolAllocator\n", Bytes, NumElements);
	for (int i = NumElements - 1; i >= 0; i--)
	{
		allocator_.deallocate(ptrs[i]);
		ptrs[i] = nullptr;
		ASSERT_EQ(allocator_.numAllocations(), i);
		ASSERT_GE(allocator_.usedMemory(), ElementSize * i);
		ASSERT_LE(allocator_.usedMemory(), ElementSize * i + nctl::IAllocator::DefaultAlignment);
	}
}

TEST_F(AllocatorPoolTest, FreeList)
{
	ASSERT_GE(uintptr_t(allocator_.freeList()), uintptr_t(allocator_.base()));
	ASSERT_LE(uintptr_t(allocator_.freeList()), uintptr_t(allocator_.base()) + nctl::IAllocator::DefaultAlignment);
	const size_t Bytes = ElementSize;
	printf("Allocating %lu bytes with the PoolAllocator\n", Bytes);
	ElementType *ptr = reinterpret_cast<ElementType *>(allocator_.allocate(Bytes));
	ASSERT_GE(uintptr_t(allocator_.freeList()), uintptr_t(allocator_.base()) + allocator_.usedMemory());
	ASSERT_LE(uintptr_t(allocator_.freeList()), uintptr_t(allocator_.base()) + allocator_.usedMemory() + nctl::IAllocator::DefaultAlignment);

	printf("Deallocating %lu bytes from the PoolAllocator\n", Bytes);
	allocator_.deallocate(ptr);
}

TEST_F(AllocatorPoolTest, AllocateTooMuch)
{
	printf("Allocating too much for the PoolAllocator\n");
	ElementType *ptrs[Capacity];
	for (unsigned int i = 0; i < Capacity; i++)
	{
		ptrs[i] = reinterpret_cast<ElementType *>(allocator_.allocate(ElementSize));
		ASSERT_NE(ptrs[i], nullptr);
	}
	ElementType *ptr = reinterpret_cast<ElementType *>(allocator_.allocate(ElementSize));
	ASSERT_EQ(ptr, nullptr);

	for (int i = Capacity - 1; i >= 0; i--)
		allocator_.deallocate(ptrs[i]);
}

}
