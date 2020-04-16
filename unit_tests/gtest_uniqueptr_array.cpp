#include "gtest_uniqueptr.h"

namespace {

class UniquePtrArrayTest : public ::testing::Test
{
  public:
	UniquePtrArrayTest()
	    : ptr_(newArray<int>(Size)) {}

  protected:
	void SetUp() override { initPtrArray(ptr_.get(), Size); }

	nctl::UniquePtr<int[]> ptr_;
};

TEST_F(UniquePtrArrayTest, PointerSize)
{
	printf("Size of a unique pointer instance: %lu (raw poiner: %lu)\n", sizeof(ptr_), sizeof(int *));
	ASSERT_EQ(sizeof(ptr_), sizeof(int *));
}

TEST_F(UniquePtrArrayTest, Reset)
{
	const int newValue = 3;
	int *raw = newArray<int>(Size);
	int const *const oldRaw = raw;
	raw[0] = newValue;
	printf("Creating a raw pointer to an array of ints, address: 0x%p, first value: %d\n", static_cast<const int *>(raw), *raw);

	ptr_.reset(raw);
	raw = nullptr;
	printPointer("Resetting the unique pointer to the raw one, ", ptr_);

	ASSERT_EQ(ptr_[0], newValue);
	ASSERT_EQ(ptr_.get(), oldRaw);
}

TEST_F(UniquePtrArrayTest, SelfReset)
{
	int *const raw = ptr_.get();
	printPointer("Creating a raw pointer to an array of ints, ", raw);

	ptr_.reset(raw);
	printPointer("Resetting the unique pointer to itself, ", ptr_);

	ASSERT_EQ(ptr_[0], FirstElement);
	ASSERT_EQ(ptr_.get(), raw);
}

TEST_F(UniquePtrArrayTest, ResetNull)
{
	ptr_.reset(nullptr);
	printPointer("Resetting the unique pointer to `nullptr`, ", ptr_);

	ASSERT_EQ(ptr_, nullptr);
}

TEST_F(UniquePtrArrayTest, Release)
{
	int *raw = nullptr;
	int const *const oldPtr = ptr_.get();
	raw = ptr_.release();
	printPointer("Releasing the unique pointer to the raw one, ", raw);

	ASSERT_EQ(ptr_.get(), nullptr);
	ASSERT_EQ(raw, oldPtr);
	ASSERT_EQ(*raw, FirstElement);
	ASSERT_EQ(raw[0], FirstElement);
	deleteArray(raw);
}

TEST_F(UniquePtrArrayTest, MoveConstructor)
{
	int const *const oldPtr = ptr_.get();
	nctl::UniquePtr<int[]> newPtr(nctl::move(ptr_));
	printPointer("Creating a new unique pointer moving from the first one, ", newPtr);

	const int num = newPtr[0];
	newPtr[0] = num + 1;
	printf("Incrementing the value pointed by the new unique pointer: %d", newPtr[0]);

	ASSERT_EQ(ptr_.get(), nullptr);
	ASSERT_EQ(newPtr[0], FirstElement + 1);
	ASSERT_EQ(newPtr.get(), oldPtr);
}

TEST_F(UniquePtrArrayTest, MoveAssignment)
{
	int const *const oldPtr = ptr_.get();
	nctl::UniquePtr<int[]> newPtr;
	newPtr = nctl::move(ptr_);
	printPointer("Assigning to a new unique pointer moving from first, ", newPtr);

	const int num = newPtr[0];
	newPtr[0] = num + 1;
	printf("Incrementing the value pointed by the new unique pointer: %d", newPtr[0]);

	ASSERT_EQ(ptr_.get(), nullptr);
	ASSERT_EQ(newPtr[0], FirstElement + 1);
	ASSERT_EQ(newPtr.get(), oldPtr);
}

TEST_F(UniquePtrArrayTest, Subscript)
{
	nctl::UniquePtr<int[]> arrayPtr(newArray<int>(Size));
	printf("Creating a unique pointer to array of %u elements, address: %p\n", Size, static_cast<const int *>(arrayPtr.get()));
	initPtrArray(arrayPtr.get(), Size);

	printf("Elements in the array:");
	for (unsigned int i = 0; i < Size; i++)
	{
		printf(" %d", arrayPtr[i]);
		ASSERT_EQ(arrayPtr[i], static_cast<int>(i));
	}
	printf("\n");
}

TEST_F(UniquePtrArrayTest, MakeUnique)
{
	nctl::UniquePtr<int[]> arrayPtr = nctl::makeUnique<int[]>(Size);
	printf("Creating a unique pointer to array of %u elements with `makeUnique()`, address: %p\n", Size, static_cast<const int *>(arrayPtr.get()));
	initPtrArray(arrayPtr.get(), Size);

	printf("Elements in the array:");
	for (unsigned int i = 0; i < Size; i++)
	{
		printf(" %d", arrayPtr[i]);
		ASSERT_EQ(arrayPtr[i], static_cast<int>(i));
	}
	printf("\n");
}

#if NCINE_WITH_ALLOCATORS
namespace {

	const size_t BufferSize = 32;
	uint8_t buffer1[BufferSize];
	uint8_t buffer2[BufferSize];
	nctl::StackAllocator stackAllocator1(BufferSize, &buffer1);
	nctl::StackAllocator stackAllocator2(BufferSize, &buffer2);

}

TEST_F(UniquePtrArrayTest, PointerSizeWithAllocator)
{
	auto arrayPtr = nctl::allocateUnique<int[]>(nctl::theDefaultAllocator(), Value);
	printf("Size of a unique pointer instance with a custom allocator deleter: %lu (raw poiner: %lu)\n", sizeof(arrayPtr), sizeof(int *));
	ASSERT_EQ(sizeof(arrayPtr), sizeof(int *) * 2);
}

TEST_F(UniquePtrArrayTest, AllocateUnique)
{
	auto arrayPtr = nctl::allocateUnique<int[]>(stackAllocator1, 2);
	printf("Creating a unique pointer to array of %u elements with `allocateUnique()`, address: %p\n", 2, static_cast<const int *>(arrayPtr.get()));
	initPtrArray(arrayPtr.get(), 2);

	printf("Elements in the array:");
	for (unsigned int i = 0; i < 2; i++)
	{
		printf(" %d", arrayPtr[i]);
		ASSERT_EQ(arrayPtr[i], static_cast<int>(i));
	}
	printf("\n");

	printf("Releasing the unique pointer to free memory from the custom allocator\n");
	arrayPtr.reset(nullptr);
	ASSERT_EQ(stackAllocator1.numAllocations(), 0);
}

TEST_F(UniquePtrArrayTest, MoveConstructorWithAllocator)
{
	printf("Creating a unique pointer with a custom allocator\n");
	auto arrayPtr1 = nctl::allocateUnique<int[]>(stackAllocator1, 2);
	arrayPtr1[0] = Value;
	ASSERT_EQ(arrayPtr1[0], Value);

	printf("Creating a second unique pointer with move construction\n");
	auto arrayPtr2(nctl::move(arrayPtr1));
	ASSERT_EQ(arrayPtr2[0], Value);
	ASSERT_EQ(arrayPtr1.get(), nullptr);
	ASSERT_EQ(stackAllocator1.numAllocations(), 1);

	printf("Releasing the unique pointer to free memory from the custom allocator\n");
	arrayPtr2.reset(nullptr);
	ASSERT_EQ(stackAllocator1.numAllocations(), 0);
}

TEST_F(UniquePtrArrayTest, MoveAssignmentWithAllocator)
{
	printf("Creating a unique pointer with a custom allocator\n");
	auto arrayPtr1 = nctl::allocateUnique<int[]>(stackAllocator1, 2);
	arrayPtr1[0] = Value;
	ASSERT_EQ(arrayPtr1[0], Value);

	printf("Creating a second unique pointer with a different custom allocator\n");
	auto arrayPtr2 = nctl::allocateUnique<int[]>(stackAllocator2, 2);
	arrayPtr2[0] = Value + 1;
	ASSERT_EQ(arrayPtr2[0], Value + 1);

	printf("Move assign the first unique pointer to the second\n");
	arrayPtr2 = nctl::move(arrayPtr1);
	ASSERT_EQ(arrayPtr2[0], Value);
	ASSERT_EQ(arrayPtr1.get(), nullptr);
	ASSERT_EQ(stackAllocator2.numAllocations(), 0);

	printf("Releasing the second unique pointer to free memory from the custom allocator\n");
	arrayPtr2.reset(nullptr);
	ASSERT_EQ(stackAllocator1.numAllocations(), 0);
}
#endif

}
