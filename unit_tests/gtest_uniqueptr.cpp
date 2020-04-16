#include "gtest_uniqueptr.h"

namespace {

class UniquePtrTest : public ::testing::Test
{
  public:
	UniquePtrTest()
	    : ptr_(newObject<int>(Value)) {}

  protected:
	nctl::UniquePtr<int> ptr_;
};

TEST_F(UniquePtrTest, PointerSize)
{
	printf("Size of a unique pointer instance: %lu (raw poiner: %lu)\n", sizeof(ptr_), sizeof(int *));
	ASSERT_EQ(sizeof(ptr_), sizeof(int *));
}

TEST_F(UniquePtrTest, Dereferencing)
{
	printPointer("Dereferencing a unique pointer to int, ", ptr_);

	ASSERT_EQ(*ptr_, Value);
}

TEST_F(UniquePtrTest, MemberAccess)
{
	struct St
	{
		int number;
	};

	nctl::UniquePtr<St> newPtr(newObject<St>());
	newPtr->number = Value;
	printf("Dereferencing a unique pointer to a struct number: %d\n", newPtr->number);

	ASSERT_EQ(newPtr->number, Value);
}

TEST_F(UniquePtrTest, Reset)
{
	const int newValue = 3;
	int *raw = newObject<int>(newValue);
	int const *const oldRaw = raw;
	printPointer("Creating a raw pointer to int, ", raw);

	ptr_.reset(raw);
	raw = nullptr;
	printPointer("Resetting the unique pointer to the raw one, ", ptr_);

	ASSERT_EQ(*ptr_, newValue);
	ASSERT_EQ(ptr_.get(), oldRaw);
}

TEST_F(UniquePtrTest, SelfReset)
{
	int *const raw = ptr_.get();
	printPointer("Creating a raw pointer to int, ", raw);

	ptr_.reset(raw);
	printPointer("Resetting the unique pointer to itself, ", ptr_);

	ASSERT_EQ(*ptr_, Value);
	ASSERT_EQ(ptr_.get(), raw);
}

TEST_F(UniquePtrTest, ResetNull)
{
	ptr_.reset(nullptr);
	printPointer("Resetting the unique pointer to `nullptr`, ", ptr_);

	ASSERT_EQ(ptr_, nullptr);
}

TEST_F(UniquePtrTest, Release)
{
	int *raw = nullptr;
	int const *const oldPtr = ptr_.get();
	raw = ptr_.release();
	printPointer("Releasing the unique pointer to the raw one, ", raw);

	ASSERT_EQ(ptr_, nullptr);
	ASSERT_EQ(raw, oldPtr);
	ASSERT_EQ(*raw, Value);
	deleteObject(raw);
}

TEST_F(UniquePtrTest, MoveConstructor)
{
	int const *const oldPtr = ptr_.get();
	nctl::UniquePtr<int> newPtr(nctl::move(ptr_));
	printPointer("Creating a new unique pointer moving from the first one, ", newPtr);

	const int num = *newPtr;
	*newPtr = num + 1;
	printf("Incrementing the value pointed by the new unique pointer: %d\n", *newPtr);

	ASSERT_EQ(ptr_, nullptr);
	ASSERT_EQ(*newPtr, Value + 1);
	ASSERT_EQ(newPtr.get(), oldPtr);
}

TEST_F(UniquePtrTest, MoveConstructorDerived)
{
	nctl::UniquePtr<Derived> derivedPtr(newObject<Derived>());
	Derived const *const oldPtr = derivedPtr.get();
	nctl::UniquePtr<Base> newPtr(nctl::move(derivedPtr));
	printf("Creating a new unique pointer moving from the derived one: %s\n", newPtr->name());

	ASSERT_EQ(derivedPtr, nullptr);
	ASSERT_FALSE(newPtr->isBase());
	ASSERT_EQ(newPtr.get(), oldPtr);
}

TEST_F(UniquePtrTest, MoveAssignment)
{
	int const *const oldPtr = ptr_.get();
	nctl::UniquePtr<int> newPtr;
	newPtr = nctl::move(ptr_);
	printPointer("Assigning to a new unique pointer moving from the first one, ", newPtr);

	const int num = *newPtr;
	*newPtr = num + 1;
	printf("Incrementing the value pointed by the new unique pointer: %d\n", *newPtr);

	ASSERT_EQ(ptr_, nullptr);
	ASSERT_EQ(*newPtr, Value + 1);
	ASSERT_EQ(newPtr.get(), oldPtr);
}

TEST_F(UniquePtrTest, MoveAssignmentDerived)
{
	nctl::UniquePtr<Derived> derivedPtr(newObject<Derived>());
	Derived const *const oldPtr = derivedPtr.get();
	nctl::UniquePtr<Base> newPtr;
	newPtr = nctl::move(derivedPtr);
	printf("Assigning to a new unique pointer moving from the derived one: %s\n", newPtr->name());

	ASSERT_EQ(derivedPtr, nullptr);
	ASSERT_FALSE(newPtr->isBase());
	ASSERT_EQ(newPtr.get(), oldPtr);
}

TEST_F(UniquePtrTest, MakeUnique)
{
	nctl::UniquePtr<int> newPtr = nctl::makeUnique<int>(Value);
	printPointer("Creating a unique pointer with `makeUnique()`, ", newPtr);

	ASSERT_EQ(*newPtr, Value);
}

TEST_F(UniquePtrTest, MakeUniqueDerived)
{
	nctl::UniquePtr<Base> newPtr = nctl::makeUnique<Derived>();
	printf("Creating a unique pointer to a derived class with `makeUnique()`: %s\n", newPtr->name());

	ASSERT_FALSE(newPtr->isBase());
}

#if NCINE_WITH_ALLOCATORS
namespace {

	const size_t BufferSize = 24;
	uint8_t buffer1[BufferSize];
	uint8_t buffer2[BufferSize];
	nctl::StackAllocator stackAllocator1(BufferSize, &buffer1);
	nctl::StackAllocator stackAllocator2(BufferSize, &buffer2);

}

TEST_F(UniquePtrTest, PointerSizeWithAllocator)
{
	auto newPtr = nctl::allocateUnique<int>(nctl::theDefaultAllocator(), Value);
	printf("Size of a unique pointer instance with a custom allocator deleter: %lu (raw poiner: %lu)\n", sizeof(newPtr), sizeof(int *));
	ASSERT_EQ(sizeof(newPtr), sizeof(int *) * 2);
}

TEST_F(UniquePtrTest, AllocateUnique)
{
	auto newPtr = nctl::allocateUnique<int>(stackAllocator1, Value);
	printf("Creating a unique pointer with `allocateUnique()`, address: %p, value: %d\n",
	       static_cast<const int *>(newPtr.get()), newPtr ? *newPtr : 0);
	ASSERT_EQ(*newPtr, Value);

	printf("Releasing the unique pointer to free memory from the custom allocator\n");
	newPtr.reset(nullptr);
	ASSERT_EQ(stackAllocator1.numAllocations(), 0);
}

TEST_F(UniquePtrTest, MoveConstructorWithAllocator)
{
	printf("Creating a unique pointer with a custom allocator\n");
	auto ptr1 = nctl::allocateUnique<int>(stackAllocator1, Value);
	ASSERT_EQ(*ptr1, Value);

	printf("Creating a second unique pointer with move construction\n");
	auto ptr2(nctl::move(ptr1));
	ASSERT_EQ(*ptr2, Value);
	ASSERT_EQ(ptr1.get(), nullptr);
	ASSERT_EQ(stackAllocator1.numAllocations(), 1);

	printf("Releasing the unique pointer to free memory from the custom allocator\n");
	ptr2.reset(nullptr);
	ASSERT_EQ(stackAllocator1.numAllocations(), 0);
}

TEST_F(UniquePtrTest, MoveAssignmentWithAllocator)
{
	printf("Creating a unique pointer with a custom allocator\n");
	auto ptr1 = nctl::allocateUnique<int>(stackAllocator1, Value);
	ASSERT_EQ(*ptr1, Value);

	printf("Creating a second unique pointer with a different custom allocator\n");
	const int newValue = 3;
	auto ptr2 = nctl::allocateUnique<int>(stackAllocator2, newValue);
	ASSERT_EQ(*ptr2, newValue);

	printf("Move assign the first unique pointer to the second\n");
	ptr2 = nctl::move(ptr1);
	ASSERT_EQ(*ptr2, Value);
	ASSERT_EQ(ptr1.get(), nullptr);
	ASSERT_EQ(stackAllocator2.numAllocations(), 0);

	printf("Releasing the second unique pointer to free memory from the custom allocator\n");
	ptr2.reset(nullptr);
	ASSERT_EQ(stackAllocator1.numAllocations(), 0);
}

TEST_F(UniquePtrTest, MoveAssignmentDerivedWithAllocator)
{
	printf("Creating a unique pointer to a derived class with a custom allocator\n");
	nctl::AllocDelete<Derived> deleter1(&stackAllocator1);
	nctl::UniquePtr<Derived, nctl::AllocDelete<Derived>> derivedPtr(stackAllocator1.newObject<Derived>(), deleter1);
	Derived const *const oldPtr = derivedPtr.get();

	printf("Creating a second unique pointer to a base class with a different custom allocator\n");
	nctl::AllocDelete<Base> deleter2(&stackAllocator2);
	nctl::UniquePtr<Base, nctl::AllocDelete<Base>> newPtr(stackAllocator2.newObject<Base>(), deleter2);
	newPtr = nctl::move(derivedPtr);
	printf("Assigning to a new unique pointer moving from the derived one: %s\n", newPtr->name());
	ASSERT_EQ(stackAllocator2.numAllocations(), 0);

	ASSERT_EQ(derivedPtr, nullptr);
	ASSERT_FALSE(newPtr->isBase());
	ASSERT_EQ(newPtr.get(), oldPtr);

	printf("Releasing the second unique pointer to free memory from the custom allocator\n");
	newPtr.reset(nullptr);
	ASSERT_EQ(stackAllocator1.numAllocations(), 0);
}

#endif

}
