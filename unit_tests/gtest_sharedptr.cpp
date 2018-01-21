#include "gtest_uniqueptr.h"
#include "test_thread_functions.h"

namespace {

const int NumThreads = 100;
const int NumIterations = 1000;

class SharedPtrTest : public ::testing::Test
{
  public:
	SharedPtrTest() : ptr_(new int(Value)), tr_(this) { }

	nctl::SharedPtr<int> ptr_;
	ThreadRunner<NumThreads> tr_;
};

TEST_F(SharedPtrTest, NullPtr)
{
	nctl::SharedPtr<int> newPtr;
	printf("Creating a new shared pointer not pointing to anything, "); printPointer(newPtr);

	ASSERT_EQ(newPtr, nullptr);
	ASSERT_EQ(newPtr.useCount(), 0);
}

TEST_F(SharedPtrTest, Dereferencing)
{
	printf("Dereferencing a shared pointer to int, "); printPointer(ptr_);

	ASSERT_EQ(*ptr_, Value);
}

TEST_F(SharedPtrTest, MemberAccess)
{
	struct St { int number; };
	nctl::SharedPtr<St> newPtr(new St);
	newPtr->number = Value;
	printf("Dereferencing a shared pointer to a struct: number = %d, ", newPtr->number);

	ASSERT_EQ(newPtr->number, Value);
}

TEST_F(SharedPtrTest, Reset)
{
	const int newValue = 3;
	int *raw = new int(newValue);
	int const * const oldRaw = raw;
	printf("Creating a raw pointer to int, "); printPointer(raw);

	ptr_.reset(raw);
	raw = nullptr;
	printf("Resetting the shared pointer to the raw one, "); printPointer(ptr_);

	ASSERT_EQ(*ptr_, newValue);
	ASSERT_EQ(ptr_.get(), oldRaw);
	ASSERT_EQ(ptr_.useCount(), 1);
}

TEST_F(SharedPtrTest, SelfReset)
{
	int * const raw = ptr_.get();
	printf("Creating a raw pointer to int, "); printPointer(raw);

	ptr_.reset(raw);
	printf("Resetting the shared pointer to itself, "); printPointer(ptr_);

	ASSERT_EQ(*ptr_, Value);
	ASSERT_EQ(ptr_.get(), raw);
}

TEST_F(SharedPtrTest, ResetNull)
{
	ptr_.reset(nullptr);
	printf("Resetting the shared pointer to `nullptr`, "); printPointer(ptr_);

	ASSERT_EQ(ptr_, nullptr);
	ASSERT_EQ(ptr_.useCount(), 0);
}

TEST_F(SharedPtrTest, MoveConstructor)
{
	int const * const oldPtr = ptr_.get();
	nctl::SharedPtr<int> newPtr(nctl::move(ptr_));
	printf("Creating a new shared pointer moving from the first one, "); printPointer(newPtr);

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
	int const * const oldPtr = ptr_.get();
	nctl::SharedPtr<int> newPtr;
	newPtr = nctl::move(ptr_);
	printf("Assigning to a new shared pointer moving from first, "); printPointer(newPtr);

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
	int const * const oldPtr = uniquePtr.get();
	printf("Creating a unique pointer to int, "); printPointer(uniquePtr);

	nctl::SharedPtr<int> newPtr(nctl::move(uniquePtr));
	printf("Assigning to a new shared pointer moving from the unique one, "); printPointer(newPtr);

	ASSERT_EQ(uniquePtr, nullptr);
	ASSERT_EQ(*newPtr, newValue);
	ASSERT_EQ(newPtr.get(), oldPtr);
	ASSERT_EQ(newPtr.useCount(), 1);
}

TEST_F(SharedPtrTest, ShareConstructor)
{
	nctl::SharedPtr<int> newPtr1(ptr_);
	printf("Creating a new shared pointer to int, "); printPointer(newPtr1);
	ASSERT_EQ(*newPtr1, Value);
	ASSERT_EQ(newPtr1.get(), ptr_.get());
	ASSERT_EQ(ptr_.useCount(), 2);
	ASSERT_EQ(newPtr1.useCount(), 2);

	nctl::SharedPtr<int> newPtr2(ptr_);
	printf("Creating a new shared pointer to int, "); printPointer(newPtr2);
	ASSERT_EQ(*newPtr2, Value);
	ASSERT_EQ(newPtr2.get(), ptr_.get());
	ASSERT_EQ(ptr_.useCount(), 3);
	ASSERT_EQ(newPtr2.useCount(), 3);

	{
		nctl::SharedPtr<int> newPtr3(ptr_);
		printf("Creating a new shared pointer to int, "); printPointer(newPtr3);
		ASSERT_EQ(*newPtr3, Value);
		ASSERT_EQ(newPtr3.get(), ptr_.get());
		ASSERT_EQ(ptr_.useCount(), 4);
		ASSERT_EQ(newPtr3.useCount(), 4);
	}

	printf("Letting last new shared pointer go out of scope, "); printPointer(ptr_);
	ASSERT_EQ(ptr_.useCount(), 3);
	ASSERT_EQ(newPtr1.useCount(), 3);
	ASSERT_EQ(newPtr2.useCount(), 3);
}

TEST_F(SharedPtrTest, ShareAssignment)
{
	{
		nctl::SharedPtr<int> ptrs[Size];

		for (unsigned int i =0; i < Size; i++)
		{
			ptrs[i] = ptr_;
			*ptrs[i] = static_cast<int>(i);
			printf("Creating a new shared pointer to int, "); printPointer(ptrs[i]);
			ASSERT_EQ(*ptrs[i], static_cast<int>(i));
			ASSERT_EQ(ptrs[i].get(), ptr_.get());
			ASSERT_EQ(ptr_.useCount(), i + 2);
		}
	}

	printf("Letting all new shared pointer go out of scope, "); printPointer(ptr_);
	ASSERT_EQ(ptr_.useCount(), 1);
}

TEST_F(SharedPtrTest, ShareAssignmentMultithread)
{
	tr_.runThreads([](void *arg) -> ThreadRunner<NumThreads>::threadFuncRet
	{
		SharedPtrTest *obj = static_cast<SharedPtrTest *>(arg);
		for (unsigned int i = 0; i < NumIterations; i++)
		{
			nctl::SharedPtr<int> newPtr(obj->ptr_);
		}
		return static_cast<SharedPtrTest *>(arg)->tr_.retFunc();
	});

	printf("Letting all threads join and new shared pointer go out of scope, "); printPointer(ptr_);
	ASSERT_EQ(ptr_.useCount(), 1);
}

TEST_F(SharedPtrTest, MakeShared)
{
	const int newValue = 3;
	auto newPtr = nctl::makeShared<int>(newValue);
	printf("Creating a shared pointer with `makeShared()`, "); printPointer(newPtr);

	ASSERT_EQ(*newPtr, newValue);
	ASSERT_EQ(newPtr.useCount(), 1);
}

TEST_F(SharedPtrTest, MakeSharedMoveConstructor)
{
	const int newValue = 3;
	auto newPtr = nctl::makeShared<int>(newValue);
	printf("Creating a shared pointer with `makeShared()`, "); printPointer(newPtr);
	int const * const oldPtr = newPtr.get();

	nctl::SharedPtr<int> newPtr2(nctl::move(newPtr));
	printf("Creating a second shared pointer moving from the first one, "); printPointer(newPtr2);

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
	printf("Creating a shared pointer with `makeShared()`, "); printPointer(newPtr);
	int const * const oldPtr = newPtr.get();

	nctl::SharedPtr<int> newPtr2;
	newPtr2 = nctl::move(newPtr);
	printf("Assigning to a new shared pointer moving from the first one, "); printPointer(newPtr2);

	const int num = *newPtr2;
	*newPtr2 = num + 1;
	printf("Incremented the value pointed by the new shared pointer: %d\n", *newPtr2);

	ASSERT_EQ(newPtr, nullptr);
	ASSERT_EQ(*newPtr2, newValue + 1);
	ASSERT_EQ(newPtr2.get(), oldPtr);
}

/*
TEST(SharedPtrDeathTest, MakeSharedReset)
{
	const int newValue = 3;
	auto newPtr = nctl::makeShared<int>(newValue);
	ASSERT_DEATH(newPtr.reset(new int(0)), "");
}

TEST(SharedPtrDeathTest, MakeSharedResetNull)
{
	const int newValue = 3;
	auto newPtr = nctl::makeShared<int>(newValue);
	ASSERT_DEATH(newPtr.reset(nullptr), "");
}
*/

}
