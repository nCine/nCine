#include "gtest_allocators.h"

namespace {

class AllocatorStackTest : public ::testing::Test
{
  public:
	AllocatorStackTest()
	    : allocator_(BufferSize, &buffer_) {}

  protected:
	uint8_t buffer_[BufferSize];
	nctl::StackAllocator allocator_;
};

#if NCINE_DEBUG
TEST(AllocatorStackDeathTest, DeallocateOutOfOrder)
{
	uint8_t buffer[BufferSize];
	nctl::StackAllocator allocator(BufferSize, &buffer);
	void *ptr1 = allocator.allocate(4);
	void *ptr2 = allocator.allocate(4);

	printf("Trying to deallocate out of order from a StackAllocator\n");
	ASSERT_DEATH(allocator.deallocate(ptr1), "");
	allocator.deallocate(ptr2);
	allocator.deallocate(ptr1);
}
#endif

TEST(AllocatorStackDeathTest, AllocateZeroBytes)
{
	uint8_t buffer[BufferSize];
	nctl::StackAllocator allocator(BufferSize, &buffer);

	printf("Allocating zero bytes with the StackAllocator\n");
	ASSERT_DEATH(allocator.allocate(0, ElementSize), "");
}

TEST(AllocatorStackDeathTest, ZeroAlignment)
{
	uint8_t buffer[BufferSize];
	nctl::StackAllocator allocator(BufferSize, &buffer);

	printf("Allocating with zero alignment with the StackAllocator\n");
	ASSERT_DEATH(allocator.allocate(ElementSize, 0), "");
}

TEST_F(AllocatorStackTest, DefaultConstructor)
{
	nctl::StackAllocator allocator;
	allocator.init(BufferSize, &buffer_);

	printf("Allocating from a StackAllocator not initialized in the constructor\n");
	ElementType *ptr = reinterpret_cast<ElementType *>(allocator.allocate(ElementSize));
	ASSERT_NE(ptr, nullptr);
	ASSERT_EQ(allocator.numAllocations(), 1);

	printf("Dellocating from a StackAllocator not initialized in the constructor\n");
	allocator.deallocate(ptr);
	ASSERT_EQ(allocator.numAllocations(), 0);
}

TEST_F(AllocatorStackTest, AllocateDeallocate)
{
	const size_t Bytes = NumElements * ElementSize;
	printf("Allocating %lu bytes for %d elements with the StackAllocator\n", Bytes, NumElements);
	ElementType *ptr = reinterpret_cast<ElementType *>(allocator_.allocate(Bytes));
	ASSERT_NE(ptr, nullptr);
	ASSERT_EQ(allocator_.numAllocations(), 1);
	ASSERT_GE(allocator_.usedMemory(), Bytes);
	ASSERT_LE(allocator_.usedMemory(), Bytes + nctl::IAllocator::DefaultAlignment);
	ASSERT_EQ(uintptr_t(allocator_.current()), uintptr_t(allocator_.base()) + allocator_.usedMemory());

	printf("Filling the memory with %d integers\n", NumElements);
	fillElements(ptr, NumElements);

	printf("Deallocating %lu bytes from the StackAllocator\n", Bytes);
	allocator_.deallocate(ptr);
	ASSERT_EQ(allocator_.numAllocations(), 0);
	ASSERT_EQ(allocator_.usedMemory(), 0);
	ASSERT_EQ(uintptr_t(allocator_.current()), uintptr_t(allocator_.base()) + allocator_.usedMemory());
}

TEST_F(AllocatorStackTest, CurrentPointer)
{
	const size_t Bytes = ElementSize;
	printf("Allocating %lu bytes with the StackAllocator\n", Bytes);
	ElementType *ptr = reinterpret_cast<ElementType *>(allocator_.allocate(Bytes));
	ASSERT_EQ(uintptr_t(allocator_.current()), uintptr_t(ptr) + Bytes);
#if NCINE_DEBUG
	ASSERT_EQ(allocator_.previous(), ptr);
#endif

	printf("Deallocating %lu bytes from the StackAllocator\n", Bytes);
	allocator_.deallocate(ptr);
}

TEST_F(AllocatorStackTest, AllocateTooMuch)
{
	const size_t Bytes = (Capacity + 1) * ElementSize;
	printf("Allocating too much for the StackAllocator\n");
	void *ptr = allocator_.allocate(Bytes);
	ASSERT_EQ(ptr, nullptr);
}

TEST_F(AllocatorStackTest, ReallocateShrink)
{
	const size_t Bytes = NumElements * ElementSize;
	printf("Allocating %lu bytes with the StackAllocator\n", Bytes);
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

TEST_F(AllocatorStackTest, ReallocateGrow)
{
	const size_t Bytes = NumElements * ElementSize;
	printf("Allocating %lu bytes with the StackAllocator\n", Bytes);
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

TEST_F(AllocatorStackTest, ReallocateTooMuch)
{
	const size_t Bytes = NumElements * ElementSize;
	printf("Allocating %lu bytes with the StackAllocator\n", Bytes);
	ElementType *ptr = reinterpret_cast<ElementType *>(allocator_.allocate(Bytes));
	ASSERT_NE(ptr, nullptr);
	ASSERT_EQ(allocator_.numAllocations(), 1);

	const size_t NewBytes = (Capacity + 1) * ElementSize;
	printf("Reallocating too much for the StackAllocator (%lu bytes)\n", NewBytes);
	void *newPtr = reinterpret_cast<ElementType *>(allocator_.reallocate(ptr, NewBytes));
	ASSERT_EQ(newPtr, nullptr);
	ASSERT_EQ(allocator_.numAllocations(), 1);
	ASSERT_GE(allocator_.usedMemory(), Bytes);
	ASSERT_LE(allocator_.usedMemory(), Bytes + nctl::IAllocator::DefaultAlignment);

	printf("Deallocating %lu bytes of memory\n", NewBytes);
	allocator_.deallocate(ptr);
	ASSERT_EQ(allocator_.numAllocations(), 0);
}

}
