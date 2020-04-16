#include "gtest_allocators.h"

namespace {

class AllocatorFreeListTest : public ::testing::Test
{
  public:
	AllocatorFreeListTest()
	    : allocator_(BufferSize, &buffer_) {}

  protected:
	uint8_t buffer_[BufferSize];
	nctl::FreeListAllocator allocator_;
};

TEST(AllocatorFreeListDeathTest, AllocateZeroBytes)
{
	uint8_t buffer[BufferSize];
	nctl::FreeListAllocator freelistAllocator(BufferSize, &buffer);

	printf("Allocating zero bytes with the FreeListAllocator\n");
	ASSERT_DEATH(freelistAllocator.allocate(0, ElementSize), "");
}

TEST(AllocatorFreeListDeathTest, ZeroAlignment)
{
	uint8_t buffer[BufferSize];
	nctl::FreeListAllocator freelistAllocator(BufferSize, &buffer);

	printf("Allocating with zero alignment with the FreeListAllocator\n");
	ASSERT_DEATH(freelistAllocator.allocate(ElementSize, 0), "");
}

TEST_F(AllocatorFreeListTest, DefaultConstructor)
{
	nctl::FreeListAllocator allocator;
	allocator.init(BufferSize, &buffer_);

	printf("Allocating from a FreeListAllocator not initialized in the constructor\n");
	ElementType *ptr = reinterpret_cast<ElementType *>(allocator.allocate(ElementSize));
	ASSERT_NE(ptr, nullptr);
	ASSERT_EQ(allocator.numAllocations(), 1);

	printf("Dellocating from a FreeListAllocator not initialized in the constructor\n");
	allocator.deallocate(ptr);
	ASSERT_EQ(allocator.numAllocations(), 0);
}

TEST_F(AllocatorFreeListTest, AllocateDeallocate)
{
	const size_t Bytes = NumElements * ElementSize;
	printf("Allocating %lu bytes for %d elements with the FreeListAllocator\n", Bytes, NumElements);
	ElementType *ptr = static_cast<ElementType *>(allocator_.allocate(Bytes, 4));
	ASSERT_NE(ptr, nullptr);
	ASSERT_EQ(allocator_.numAllocations(), 1);
	ASSERT_GE(allocator_.usedMemory(), Bytes);
	ASSERT_LE(allocator_.usedMemory(), Bytes + ElementSize);

	printf("Filling the memory with %d integers\n", NumElements);
	fillElements(ptr, NumElements);

	printf("Deallocating %lu bytes of memory\n", Bytes);
	allocator_.deallocate(ptr);
	ASSERT_EQ(allocator_.numAllocations(), 0);
	ASSERT_EQ(allocator_.usedMemory(), 0);
}

TEST_F(AllocatorFreeListTest, FreeBlock)
{
	ASSERT_EQ(uintptr_t(allocator_.freeBlock()), uintptr_t(allocator_.base()));
	ASSERT_EQ(allocator_.freeBlock()->size, allocator_.size());
	const size_t Bytes = ElementSize;
	printf("Allocating %lu bytes with the FreeListAllocator\n", Bytes);
	ElementType *ptr = reinterpret_cast<ElementType *>(allocator_.allocate(Bytes));
	ASSERT_GE(uintptr_t(allocator_.freeBlock()), uintptr_t(allocator_.base()) + allocator_.usedMemory());
	ASSERT_LE(uintptr_t(allocator_.freeBlock()), uintptr_t(allocator_.base()) + allocator_.usedMemory() + nctl::IAllocator::DefaultAlignment);
	ASSERT_EQ(allocator_.freeBlock()->size, allocator_.size() - allocator_.usedMemory());

	printf("Deallocating %lu bytes from the PoolAllocator\n", Bytes);
	allocator_.deallocate(ptr);
}

TEST_F(AllocatorFreeListTest, AllocateTooMuch)
{
	const size_t Bytes = (Capacity + 1) * ElementSize;
	printf("Allocating too much for the FreeListAllocator\n");
	void *ptr = allocator_.allocate(Bytes);
	ASSERT_EQ(ptr, nullptr);
}

TEST_F(AllocatorFreeListTest, ReallocateShrink)
{
	const size_t Bytes = NumElements * ElementSize;
	printf("Allocating %lu bytes with the FreeListAllocator\n", Bytes);
	ElementType *ptr = reinterpret_cast<ElementType *>(allocator_.allocate(Bytes));
	ASSERT_NE(ptr, nullptr);
	ASSERT_EQ(allocator_.numAllocations(), 1);

	const size_t NewBytes = Bytes / 2;
	printf("Shrinking the allocation to %lu bytes\n", NewBytes);
	void *newPtr = reinterpret_cast<ElementType *>(allocator_.reallocate(ptr, NewBytes));
	ASSERT_NE(newPtr, nullptr);
	ASSERT_EQ(newPtr, ptr);
	ASSERT_EQ(allocator_.numAllocations(), 1);

	printf("Deallocating %lu bytes of memory\n", NewBytes);
	allocator_.deallocate(ptr);
	ASSERT_EQ(allocator_.numAllocations(), 0);
}

TEST_F(AllocatorFreeListTest, ReallocateGrow)
{
	const size_t Bytes = NumElements * ElementSize;
	printf("Allocating %lu bytes with the FreeListAllocator\n", Bytes);
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

TEST_F(AllocatorFreeListTest, ReallocateTooMuch)
{
	const size_t Bytes = NumElements * ElementSize;
	printf("Allocating %lu bytes with the FreeListAllocator\n", Bytes);
	ElementType *ptr = reinterpret_cast<ElementType *>(allocator_.allocate(Bytes));
	ASSERT_NE(ptr, nullptr);
	ASSERT_EQ(allocator_.numAllocations(), 1);

	const size_t NewBytes = (Capacity + 1) * ElementSize;
	printf("Reallocating too much for the FreeListAllocator (%lu bytes)\n", NewBytes);
	void *newPtr = reinterpret_cast<ElementType *>(allocator_.reallocate(ptr, NewBytes));
	ASSERT_EQ(newPtr, nullptr);
	ASSERT_EQ(allocator_.numAllocations(), 1);
	ASSERT_GE(allocator_.usedMemory(), Bytes);
	ASSERT_LE(allocator_.usedMemory(), Bytes + ElementSize);

	printf("Deallocating %lu bytes of memory\n", NewBytes);
	allocator_.deallocate(ptr);
	ASSERT_EQ(allocator_.numAllocations(), 0);
}

}
