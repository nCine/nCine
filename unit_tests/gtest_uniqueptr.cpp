#include "gtest_uniqueptr.h"

namespace {

class UniquePtrTest : public ::testing::Test
{
  public:
	UniquePtrTest() : ptr_(new int(Value)) { }

  protected:
	nctl::UniquePtr<int> ptr_;
};

TEST_F(UniquePtrTest, Dereferencing)
{
	printf("Dereferencing a unique pointer to int, "); printPointer(ptr_);

	ASSERT_EQ(*ptr_, Value);
}

TEST_F(UniquePtrTest, MemberAccess)
{
	struct St { int number; };
	nctl::UniquePtr<St> newPtr(new St);
	newPtr->number = Value;
	printf("Dereferencing a unique pointer to a struct number = %d), ", newPtr->number);

	ASSERT_EQ(newPtr->number, Value);
}

TEST_F(UniquePtrTest, Reset)
{
	const int newValue = 3;
	int *raw = new int(newValue);
	int const * const oldRaw = raw;
	printf("Creating a raw pointer to int, "); printPointer(raw);

	ptr_.reset(raw);
	raw = nullptr;
	printf("Resetting the unique pointer to the raw one, "); printPointer(ptr_);

	ASSERT_EQ(*ptr_, newValue);
	ASSERT_EQ(ptr_.get(), oldRaw);
}

TEST_F(UniquePtrTest, SelfReset)
{
	int * const raw = ptr_.get();
	printf("Creating a raw pointer to int, "); printPointer(raw);

	ptr_.reset(raw);
	printf("Resetting the unique pointer to itself, "); printPointer(ptr_);

	ASSERT_EQ(*ptr_, Value);
	ASSERT_EQ(ptr_.get(), raw);
}

TEST_F(UniquePtrTest, ResetNull)
{
	ptr_.reset(nullptr);
	printf("Resetting the unique pointer to `nullptr`, "); printPointer(ptr_);

	ASSERT_EQ(ptr_, nullptr);
}

TEST_F(UniquePtrTest, Release)
{
	const int *raw = nullptr;
	int const * const oldPtr = ptr_.get();
	raw = ptr_.release();
	printf("Releasing the unique pointer to the raw one, "); printPointer(raw);

	ASSERT_EQ(ptr_, nullptr);
	ASSERT_EQ(raw, oldPtr);
	ASSERT_EQ(*raw, Value);
	delete raw;
}

TEST_F(UniquePtrTest, MoveConstructor)
{
	int const * const oldPtr = ptr_.get();
	nctl::UniquePtr<int> newPtr(nctl::move(ptr_));
	printf("Creating a new unique pointer moving from the first one, "); printPointer(newPtr);

	const int num = *newPtr;
	*newPtr = num + 1;
	printf("Incrementing the value pointed by the new unique pointer: %d\n", *newPtr);

	ASSERT_EQ(ptr_, nullptr);
	ASSERT_EQ(*newPtr, Value + 1);
	ASSERT_EQ(newPtr.get(), oldPtr);
}

TEST_F(UniquePtrTest, MoveConstructorDerived)
{
	nctl::UniquePtr<Derived> derivedPtr(new Derived);
	Derived const * const oldPtr = derivedPtr.get();
	nctl::UniquePtr<Base> newPtr(nctl::move(derivedPtr));
	printf("Creating a new unique pointer moving from the derived one: %s\n", newPtr->name());

	ASSERT_EQ(derivedPtr, nullptr);
	ASSERT_FALSE(newPtr->isBase());
	ASSERT_EQ(newPtr.get(), oldPtr);
}

TEST_F(UniquePtrTest, MoveAssignment)
{
	int const * const oldPtr = ptr_.get();
	nctl::UniquePtr<int> newPtr;
	newPtr = nctl::move(ptr_);
	printf("Assigning to a new unique pointer moving from the first one, "); printPointer(newPtr);

	const int num = *newPtr;
	*newPtr = num + 1;
	printf("Incrementing the value pointed by the new unique pointer: %d\n", *newPtr);

	ASSERT_EQ(ptr_, nullptr);
	ASSERT_EQ(*newPtr, Value + 1);
	ASSERT_EQ(newPtr.get(), oldPtr);
}

TEST_F(UniquePtrTest, MoveAssignmentDerived)
{
	nctl::UniquePtr<Derived> derivedPtr(new Derived);
	Derived const * const oldPtr = derivedPtr.get();
	nctl::UniquePtr<Base> newPtr;
	newPtr = nctl::move(derivedPtr);
	printf("Assigning to a new unique pointer moving from the derived one: %s\n", newPtr->name());

	ASSERT_EQ(derivedPtr, nullptr);
	ASSERT_FALSE(newPtr->isBase());
	ASSERT_EQ(newPtr.get(), oldPtr);
}

TEST_F(UniquePtrTest, MakeUnique)
{
	const int newValue = 3;
	auto newPtr = nctl::makeUnique<int>(newValue);
	printf("Creating a unique pointer with `makeUnique()`, "); printPointer(newPtr);

	ASSERT_EQ(*newPtr, newValue);
}

TEST_F(UniquePtrTest, MakeUniqueDerived)
{
	nctl::UniquePtr<Base> newPtr = nctl::makeUnique<Derived>();
	printf("Creating a unique pointer to a derived class with `makeUnique()`: %s\n", newPtr->name());

	ASSERT_FALSE(newPtr->isBase());
}

}
