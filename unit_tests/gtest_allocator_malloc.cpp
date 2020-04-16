#include "gtest_allocators.h"

namespace {

class AllocatorMallocTest : public ::testing::Test
{
  protected:
	nctl::MallocAllocator allocator_;
};

#if 0
TEST(AllocatorMallocDeathTest, AllocateZeroBytes)
{
	nctl::MallocAllocator mallocAllocator;

	printf("Allocating zero bytes with the MallocAllocator\n");
	ASSERT_DEATH(mallocAllocator.allocate(0, ElementSize), "");
}

TEST(AllocatorMallocDeathTest, ZeroAlignment)
{
	nctl::MallocAllocator mallocAllocator;

	printf("Allocating with zero alignment with the MallocAllocator\n");
	ASSERT_DEATH(mallocAllocator.allocate(ElementSize, 0), "");
}
#endif

TEST_F(AllocatorMallocTest, AllocateDeallocate)
{
	const size_t Bytes = NumElements * ElementSize;
	printf("Allocating %lu bytes for %d elements with the MallocAllocator\n", Bytes, NumElements);
	ElementType *ptr = static_cast<ElementType *>(allocator_.allocate(Bytes, 4));
	ASSERT_NE(ptr, nullptr);
	ASSERT_EQ(allocator_.numAllocations(), 1);

	printf("Filling the memory with %d integers\n", NumElements);
	fillElements(ptr, NumElements);

	printf("Deallocating %lu bytes of memory\n", Bytes);
	allocator_.deallocate(ptr);
	ASSERT_EQ(allocator_.numAllocations(), 0);
}

TEST_F(AllocatorMallocTest, ReallocateShrink)
{
	const size_t Bytes = NumElements * ElementSize;
	printf("Allocating %lu bytes with the MallocAllocator\n", Bytes);
	ElementType *ptr = reinterpret_cast<ElementType *>(allocator_.allocate(Bytes));
	ASSERT_NE(ptr, nullptr);
	ASSERT_EQ(allocator_.numAllocations(), 1);

	const size_t NewBytes = Bytes / 2;
	printf("Shrinking the allocation to %lu bytes\n", NewBytes);
	void *newPtr = reinterpret_cast<ElementType *>(allocator_.reallocate(ptr, NewBytes));
	ASSERT_EQ(newPtr, ptr);
	ASSERT_EQ(allocator_.numAllocations(), 1);

	printf("Deallocating %lu bytes of memory\n", NewBytes);
	allocator_.deallocate(ptr);
	ASSERT_EQ(allocator_.numAllocations(), 0);
}

TEST_F(AllocatorMallocTest, ReallocateGrow)
{
	const size_t Bytes = NumElements * ElementSize;
	printf("Allocating %lu bytes with the MallocAllocator\n", Bytes);
	ElementType *ptr = reinterpret_cast<ElementType *>(allocator_.allocate(Bytes));
	ASSERT_NE(ptr, nullptr);
	ASSERT_EQ(allocator_.numAllocations(), 1);

	const size_t NewBytes = Bytes * 2;
	printf("Growing the allocation to %lu bytes\n", NewBytes);
	void *newPtr = reinterpret_cast<ElementType *>(allocator_.reallocate(ptr, NewBytes));
	ASSERT_NE(newPtr, nullptr);
	ASSERT_EQ(newPtr, ptr);
	ASSERT_EQ(allocator_.numAllocations(), 1);

	printf("Deallocating %lu bytes of memory\n", NewBytes);
	allocator_.deallocate(ptr);
	ASSERT_EQ(allocator_.numAllocations(), 0);
}

}
