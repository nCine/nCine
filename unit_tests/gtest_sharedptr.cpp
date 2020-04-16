#include "gtest_uniqueptr.h"

namespace {

class SharedPtrTest : public ::testing::Test
{
  public:
	SharedPtrTest()
	    : ptr_(newObject<int>(Value)) {}

	nctl::SharedPtr<int> ptr_;
};

TEST_F(SharedPtrTest, NullPtr)
{
	nctl::SharedPtr<int> newPtr;
	printPointer("Creating a new shared pointer not pointing to anything, ", newPtr);

	ASSERT_EQ(newPtr, nullptr);
	ASSERT_EQ(newPtr.useCount(), 0);
}

TEST_F(SharedPtrTest, Dereferencing)
{
	printPointer("Dereferencing a shared pointer to int, ", ptr_);

	ASSERT_EQ(*ptr_, Value);
}

TEST_F(SharedPtrTest, MemberAccess)
{
	struct St
	{
		int number;
	};

	nctl::SharedPtr<St> newPtr(newObject<St>());
	newPtr->number = Value;
	printf("Dereferencing a shared pointer to a struct: number: %d\n", newPtr->number);

	ASSERT_EQ(newPtr->number, Value);
}

TEST_F(SharedPtrTest, Reset)
{
	const int newValue = 3;
	int *raw = newObject<int>(newValue);
	int const *const oldRaw = raw;
	printPointer("Creating a raw pointer to int, ", raw);

	ptr_.reset(raw);
	raw = nullptr;
	printPointer("Resetting the shared pointer to the raw one, ", ptr_);

	ASSERT_EQ(*ptr_, newValue);
	ASSERT_EQ(ptr_.get(), oldRaw);
	ASSERT_EQ(ptr_.useCount(), 1);
}

TEST_F(SharedPtrTest, SelfReset)
{
	int *const raw = ptr_.get();
	printPointer("Creating a raw pointer to int, ", raw);

	ptr_.reset(raw);
	printPointer("Resetting the shared pointer to itself, ", ptr_);

	ASSERT_EQ(*ptr_, Value);
	ASSERT_EQ(ptr_.get(), raw);
}

TEST_F(SharedPtrTest, ResetNull)
{
	ptr_.reset(nullptr);
	printPointer("Resetting the shared pointer to `nullptr`, ", ptr_);

	ASSERT_EQ(ptr_, nullptr);
	ASSERT_EQ(ptr_.useCount(), 0);
}

TEST_F(SharedPtrTest, MoveConstructor)
{
	int const *const oldPtr = ptr_.get();
	nctl::SharedPtr<int> newPtr(nctl::move(ptr_));
	printPointer("Creating a new shared pointer moving from the first one, ", newPtr);

	if (newPtr)
	{
		int num = *newPtr;
		*newPtr = num + 1;
	}
	printf("Incrementing the value pointed by the new shared pointer: %d\n", *newPtr);

	ASSERT_EQ(ptr_, nullptr);
	ASSERT_EQ(*newPtr, Value + 1);
	ASSERT_EQ(newPtr.get(), oldPtr);
}

TEST_F(SharedPtrTest, MoveAssignment)
{
	int const *const oldPtr = ptr_.get();
	nctl::SharedPtr<int> newPtr;
	newPtr = nctl::move(ptr_);
	printPointer("Assigning to a new shared pointer moving from first, ", newPtr);

	if (newPtr)
	{
		int num = *newPtr;
		*newPtr = num + 1;
	}
	printf("Incremented the value pointed by the new shared pointer: %d\n", *newPtr);

	ASSERT_EQ(ptr_, nullptr);
	ASSERT_EQ(*newPtr, Value + 1);
	ASSERT_EQ(newPtr.get(), oldPtr);
}

TEST_F(SharedPtrTest, ConstructFromUnique)
{
	const int newValue = 3;
	auto uniquePtr = nctl::makeUnique<int>(newValue);
	int const *const oldPtr = uniquePtr.get();
	printPointer("Creating a unique pointer to int, ", uniquePtr);

	nctl::SharedPtr<int> newPtr(nctl::move(uniquePtr));
	printPointer("Assigning to a new shared pointer moving from the unique one, ", newPtr);

	ASSERT_EQ(uniquePtr, nullptr);
	ASSERT_EQ(*newPtr, newValue);
	ASSERT_EQ(newPtr.get(), oldPtr);
	ASSERT_EQ(newPtr.useCount(), 1);
}

TEST_F(SharedPtrTest, ShareConstructor)
{
	nctl::SharedPtr<int> newPtr1(ptr_);
	printPointer("Creating a new shared pointer to int, ", newPtr1);
	ASSERT_EQ(*newPtr1, Value);
	ASSERT_EQ(newPtr1.get(), ptr_.get());
	ASSERT_EQ(ptr_.useCount(), 2);
	ASSERT_EQ(newPtr1.useCount(), 2);

	nctl::SharedPtr<int> newPtr2(ptr_);
	printPointer("Creating a new shared pointer to int, ", newPtr2);
	ASSERT_EQ(*newPtr2, Value);
	ASSERT_EQ(newPtr2.get(), ptr_.get());
	ASSERT_EQ(ptr_.useCount(), 3);
	ASSERT_EQ(newPtr2.useCount(), 3);

	{
		nctl::SharedPtr<int> newPtr3(ptr_);
		printPointer("Creating a new shared pointer to int, ", newPtr3);
		ASSERT_EQ(*newPtr3, Value);
		ASSERT_EQ(newPtr3.get(), ptr_.get());
		ASSERT_EQ(ptr_.useCount(), 4);
		ASSERT_EQ(newPtr3.useCount(), 4);
	}

	printPointer("Letting last new shared pointer go out of scope, ", ptr_);
	ASSERT_EQ(ptr_.useCount(), 3);
	ASSERT_EQ(newPtr1.useCount(), 3);
	ASSERT_EQ(newPtr2.useCount(), 3);
}

TEST_F(SharedPtrTest, ShareAssignment)
{
	{
		nctl::SharedPtr<int> ptrs[Size];

		for (unsigned int i = 0; i < Size; i++)
		{
			ptrs[i] = ptr_;
			*ptrs[i] = static_cast<int>(i);
			printPointer("Creating a new shared pointer to int, ", ptrs[i]);
			ASSERT_EQ(*ptrs[i], static_cast<int>(i));
			ASSERT_EQ(ptrs[i].get(), ptr_.get());
			ASSERT_EQ(ptr_.useCount(), i + 2);
		}
	}

	printPointer("Letting all new shared pointer go out of scope, ", ptr_);
	ASSERT_EQ(ptr_.useCount(), 1);
}

TEST_F(SharedPtrTest, MakeShared)
{
	const int newValue = 3;
	auto newPtr = nctl::makeShared<int>(newValue);
	printPointer("Creating a shared pointer with `makeShared()`, ", newPtr);

	ASSERT_EQ(*newPtr, newValue);
	ASSERT_EQ(newPtr.useCount(), 1);
}

TEST_F(SharedPtrTest, MakeSharedMoveConstructor)
{
	const int newValue = 3;
	auto newPtr = nctl::makeShared<int>(newValue);
	printPointer("Creating a shared pointer with `makeShared()`, ", newPtr);
	int const *const oldPtr = newPtr.get();

	nctl::SharedPtr<int> newPtr2(nctl::move(newPtr));
	printPointer("Creating a second shared pointer moving from the first one, ", newPtr2);

	const int num = *newPtr2;
	*newPtr2 = num + 1;
	printf("Incremented the value pointed by the new shared pointer: %d\n", *newPtr2);

	ASSERT_EQ(newPtr, nullptr);
	ASSERT_EQ(*newPtr2, newValue + 1);
	ASSERT_EQ(newPtr2.get(), oldPtr);
}

TEST_F(SharedPtrTest, MakeSharedMoveAssignment)
{
	const int newValue = 3;
	auto newPtr = nctl::makeShared<int>(newValue);
	printPointer("Creating a shared pointer with `makeShared()`, ", newPtr);
	int const *const oldPtr = newPtr.get();

	nctl::SharedPtr<int> newPtr2;
	newPtr2 = nctl::move(newPtr);
	printPointer("Assigning to a new shared pointer moving from the first one, ", newPtr2);

	const int num = *newPtr2;
	*newPtr2 = num + 1;
	printf("Incremented the value pointed by the new shared pointer: %d\n", *newPtr2);

	ASSERT_EQ(newPtr, nullptr);
	ASSERT_EQ(*newPtr2, newValue + 1);
	ASSERT_EQ(newPtr2.get(), oldPtr);
}

#ifndef __EMSCRIPTEN__
TEST(SharedPtrDeathTest, MakeSharedReset)
{
	const int newValue = 3;
	auto newPtr = nctl::makeShared<int>(newValue);
	ASSERT_DEATH(newPtr.reset(newObject<int>(0)), "");
}

TEST(SharedPtrDeathTest, MakeSharedResetNull)
{
	const int newValue = 3;
	auto newPtr = nctl::makeShared<int>(newValue);
	ASSERT_DEATH(newPtr.reset(nullptr), "");
}
#endif

#if NCINE_WITH_ALLOCATORS
namespace {

	const size_t BufferSize = 128;
	uint8_t buffer[BufferSize];
	nctl::StackAllocator stackAllocator(BufferSize, &buffer);
	nctl::FreeListAllocator freelistAllocator(BufferSize, &buffer);

}

TEST_F(SharedPtrTest, ConstructWithAllocator)
{
	int *newValue = freelistAllocator.newObject<int>(Value);
	auto newPtr = nctl::SharedPtr<int>(newValue, freelistAllocator);
	printPointer("Creating a shared pointer with a custom allocator, ", newPtr);

	ASSERT_EQ(*newPtr, Value);
	ASSERT_EQ(newPtr.useCount(), 1);
}

TEST_F(SharedPtrTest, AllocateShared)
{
	auto newPtr = nctl::allocateShared<int>(stackAllocator, Value);
	printPointer("Creating a shared pointer with `allocateShared()`, ", newPtr);

	ASSERT_EQ(*newPtr, Value);
	ASSERT_EQ(newPtr.useCount(), 1);
}
#endif

}
