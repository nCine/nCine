#include "gtest_allocators.h"

namespace {

class AllocatorLinearTest : public ::testing::Test
{
  public:
	AllocatorLinearTest()
	    : allocator_(BufferSize, &buffer_) {}

  protected:
	uint8_t buffer_[BufferSize];
	nctl::LinearAllocator allocator_;
};

#if NCINE_DEBUG
TEST(AllocatorLinearDeathTest, Deallocate)
{
	uint8_t buffer[BufferSize];
	nctl::LinearAllocator allocator(BufferSize, &buffer);
	void *ptr = allocator.allocate(4);

	printf("Trying to deallocate from a LinearAllocator\n");
	ASSERT_DEATH(allocator.deallocate(ptr), "");
	allocator.clear();
}
#endif

TEST(AllocatorLinearDeathTest, AllocateZeroBytes)
{
	uint8_t buffer[BufferSize];
	nctl::LinearAllocator allocator(BufferSize, &buffer);

	printf("Allocating zero bytes with the LinearAllocator\n");
	ASSERT_DEATH(allocator.allocate(0), "");
}

TEST(AllocatorLinearDeathTest, ZeroAlignment)
{
	uint8_t buffer[BufferSize];
	nctl::LinearAllocator allocator(BufferSize, &buffer);

	printf("Allocating with zero alignment with the LinearAllocator\n");
	ASSERT_DEATH(allocator.allocate(ElementSize, 0), "");
}

TEST_F(AllocatorLinearTest, DefaultConstructor)
{
	nctl::LinearAllocator allocator;
	allocator.init(BufferSize, &buffer_);

	printf("Allocating from a LinearAllocator not initialized in the constructor\n");
	ElementType *ptr = reinterpret_cast<ElementType *>(allocator.allocate(ElementSize));
	ASSERT_NE(ptr, nullptr);
	ASSERT_EQ(allocator.numAllocations(), 1);

	printf("Clearing the LinearAllocator\n");
	allocator.clear();
	ASSERT_EQ(allocator.numAllocations(), 0);
	ASSERT_EQ(allocator.usedMemory(), 0);
}

TEST_F(AllocatorLinearTest, AllocateClearDeallocate)
{
	const size_t Bytes = NumElements * ElementSize;
	printf("Allocating %lu bytes for %d elements with the LinearAllocator\n", Bytes, NumElements);
	ElementType *ptr = reinterpret_cast<ElementType *>(allocator_.allocate(Bytes));
	ASSERT_NE(ptr, nullptr);
	ASSERT_EQ(allocator_.numAllocations(), 1);
	ASSERT_GE(allocator_.usedMemory(), Bytes);
	ASSERT_LE(allocator_.usedMemory(), Bytes + nctl::IAllocator::DefaultAlignment);
	ASSERT_EQ(uintptr_t(allocator_.current()), uintptr_t(allocator_.base()) + allocator_.usedMemory());

	printf("Filling the memory with %d integers\n", NumElements);
	fillElements(ptr, NumElements);

	printf("Clearing the LinearAllocator\n");
	allocator_.clear();
	ASSERT_EQ(allocator_.numAllocations(), 0);
	ASSERT_EQ(allocator_.usedMemory(), 0);
	ASSERT_EQ(uintptr_t(allocator_.current()), uintptr_t(allocator_.base()) + allocator_.usedMemory());

	printf("Allocating %lu bytes again after clearing\n", Bytes);
	ptr = static_cast<ElementType *>(allocator_.allocate(Bytes));
	ASSERT_NE(ptr, nullptr);
	ASSERT_EQ(allocator_.numAllocations(), 1);
	ASSERT_EQ(allocator_.usedMemory(), Bytes);

	printf("Filling the memory with %d integers\n", NumElements);
	fillElements(ptr, NumElements);

	printf("Clearing the LinearAllocator\n");
	allocator_.clear();
}

TEST_F(AllocatorLinearTest, CurrentPointer)
{
	const size_t Bytes = ElementSize;
	printf("Allocating %lu bytes with the LinearAllocator\n", Bytes);
	ElementType *ptr = reinterpret_cast<ElementType *>(allocator_.allocate(Bytes));
	ASSERT_EQ(allocator_.current(), reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(ptr) + Bytes));

	printf("Clearing the LinearAllocator\n");
	allocator_.clear();
}

TEST_F(AllocatorLinearTest, AllocateTooMuch)
{
	const size_t Bytes = (Capacity + 1) * ElementSize;
	printf("Allocating too much for the LinearAllocator\n");
	void *ptr = allocator_.allocate(Bytes);
	ASSERT_EQ(ptr, nullptr);
}

/*
TEST_F(AllocatorLinearTest, ReallocateShrink)
{
	const size_t Bytes = NumElements * ElementSize;
	printf("Allocating %lu bytes with the LinearAllocator\n", Bytes);
	ElementType *ptr = reinterpret_cast<ElementType *>(allocator_.allocate(Bytes));
	ASSERT_NE(ptr, nullptr);
	ASSERT_EQ(allocator_.numAllocations(), 1);

	const size_t NewBytes = Bytes / 2;
	printf("Shrinking the allocation to %lu bytes\n", NewBytes);
	void *newPtr = reinterpret_cast<ElementType *>(allocator_.reallocate(ptr, NewBytes));
	ASSERT_NE(newPtr, nullptr);
	ASSERT_EQ(newPtr, ptr);
	ASSERT_EQ(allocator_.numAllocations(), 1);

	printf("Clearing the LinearAllocator\n");
	allocator_.clear();
}

TEST_F(AllocatorLinearTest, ReallocateGrow)
{
	const size_t Bytes = NumElements * ElementSize;
	printf("Allocating %lu bytes with the LinearAllocator\n", Bytes);
	ElementType *ptr = reinterpret_cast<ElementType *>(allocator_.allocate(Bytes));
	ASSERT_NE(ptr, nullptr);
	ASSERT_EQ(allocator_.numAllocations(), 1);

	const size_t NewBytes = Bytes * 2;
	printf("Growing the allocation to %lu bytes\n", NewBytes);
	void *newPtr = reinterpret_cast<ElementType *>(allocator_.reallocate(ptr, NewBytes));
	ASSERT_NE(newPtr, nullptr);
	ASSERT_EQ(newPtr, ptr);
	ASSERT_EQ(allocator_.numAllocations(), 1);

	printf("Clearing the LinearAllocator\n");
	allocator_.clear();
}

TEST_F(AllocatorLinearTest, ReallocateTooMuch)
{
	const size_t Bytes = NumElements * ElementSize;
	printf("Allocating %lu bytes with the LinearAllocator\n", Bytes);
	ElementType *ptr = reinterpret_cast<ElementType *>(allocator_.allocate(Bytes));
	ASSERT_NE(ptr, nullptr);
	ASSERT_EQ(allocator_.numAllocations(), 1);

	const size_t NewBytes = (Capacity + 1) * ElementSize;
	printf("Reallocating too much for the LinearAllocator (%lu bytes)\n", NewBytes);
	void *newPtr = reinterpret_cast<ElementType *>(allocator_.reallocate(ptr, NewBytes));
	ASSERT_EQ(newPtr, nullptr);
	ASSERT_EQ(allocator_.numAllocations(), 1);
	ASSERT_GE(allocator_.usedMemory(), Bytes);
	ASSERT_LE(allocator_.usedMemory(), Bytes + nctl::IAllocator::DefaultAlignment);

	printf("Clearing the LinearAllocator\n");
	allocator_.clear();
}
*/

}
