#include "gtest_uniqueptr.h"

namespace {

class UniquePtrArrayTest : public ::testing::Test
{
  public:
	UniquePtrArrayTest()
	    : ptr_(new int[Size]) {}

  protected:
	void SetUp() override { initPtrArray(ptr_.get(), Size); }

	nctl::UniquePtr<int[]> ptr_;
};

TEST_F(UniquePtrArrayTest, Reset)
{
	const int newValue = 3;
	int *raw = new int[Size];
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
	const int *raw = nullptr;
	int const *const oldPtr = ptr_.get();
	raw = ptr_.release();
	printPointer("Releasing the unique pointer to the raw one, ", raw);

	ASSERT_EQ(ptr_.get(), nullptr);
	ASSERT_EQ(raw, oldPtr);
	ASSERT_EQ(*raw, FirstElement);
	ASSERT_EQ(raw[0], FirstElement);
	delete[] raw;
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
	nctl::UniquePtr<int[]> arrayPtr(new int[Size]);
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
	auto arrayPtr = nctl::makeUnique<int[]>(Size);
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

}
