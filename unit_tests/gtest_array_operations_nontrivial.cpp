#include "gtest_array.h"
#include "test_nontrivial.h"

unsigned int NonTrivial::nextId_ = 0;
unsigned int NonTrivial::counter_ = 0;
unsigned int NonTrivial::constructions_ = 0;
unsigned int NonTrivial::destructions_ = 0;
unsigned int NonTrivial::assignments_ = 0;
nctl::HashMap<const void *, unsigned int> NonTrivial::liveInstances_(128);
nctl::HashSet<const void *> NonTrivial::allInstanceAddresses_(128);

namespace {

class ArrayNonTrivialOperationsTest : public ::testing::Test
{
  public:
	ArrayNonTrivialOperationsTest()
	    : array_(Capacity) {}

  protected:
	void SetUp() override { NonTrivial::resetCounters(); }

	nctl::Array<NonTrivial> array_;
};

TEST_F(ArrayNonTrivialOperationsTest, InsertRangeInTheMiddle)
{
	const unsigned int ArrayElements = 4;
	const unsigned int NewArrayElements = 3;
	const unsigned int InsertPos = ArrayElements / 2;
	static_assert(NewArrayElements > 1, "More than one element should be inserted by this test");
	static_assert(ArrayElements + NewArrayElements <= Capacity, "Don't trigger a reallocation with `setCapacity()`");

	for (unsigned int i = 0; i < ArrayElements; i++)
		array_.emplaceBack();
	nctl::Array<NonTrivial> newArray(Capacity);
	for (unsigned int i = 0; i < NewArrayElements; i++)
		newArray.emplaceBack();
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	const unsigned int expectedBeforeInsert = ArrayElements + NewArrayElements;
	ASSERT_EQ(NonTrivial::counter(), expectedBeforeInsert);
	ASSERT_EQ(NonTrivial::constructions(), expectedBeforeInsert);
	ASSERT_EQ(NonTrivial::destructions(), 0);

	printf("Inserting a range into the middle of the array\n");
	array_.insert(array_.begin() + InsertPos, newArray.begin(), newArray.end());
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	const unsigned int movedElements = ArrayElements - (ArrayElements - InsertPos); // Elements after `InsertPos` are the shifted ones
	const unsigned int expectedAfterInsert = ArrayElements + NewArrayElements * 2; // The array received `NewArrayElements` number of elements
	ASSERT_EQ(array_.size(), ArrayElements + NewArrayElements);
	ASSERT_EQ(NonTrivial::counter(), expectedAfterInsert) << "Number of alive objects must match after insertion";
	ASSERT_EQ(NonTrivial::constructions(), expectedAfterInsert + movedElements) << "All new elements must be constructed properly";
	ASSERT_LE(NonTrivial::destructions(), movedElements) << "If any elements were destroyed during shifting, they should not exceed the number of moved elements";
	ASSERT_EQ(NonTrivial::liveInstances(), NonTrivial::counter()) << "Internal live object map must match counter";
}

TEST_F(ArrayNonTrivialOperationsTest, InsertRangeInTheMiddleRealloc)
{
	const unsigned int ArrayElements = 8;
	const unsigned int NewArrayElements = 4;
	const unsigned int InsertPos = ArrayElements / 2;
	static_assert(ArrayElements <= Capacity, "Don't trigger a reallocation with `setCapacity()` before inserting");
	static_assert(NewArrayElements > 1, "More than one element should be inserted by this test");
	static_assert(ArrayElements + NewArrayElements > Capacity, "Trigger a reallocation with `setCapacity()`");

	for (unsigned int i = 0; i < ArrayElements; i++)
		array_.emplaceBack();
	nctl::Array<NonTrivial> newArray(Capacity);
	for (unsigned int i = 0; i < NewArrayElements; i++)
		newArray.emplaceBack();
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	const unsigned int expectedBeforeInsert = ArrayElements + NewArrayElements;
	ASSERT_EQ(NonTrivial::counter(), expectedBeforeInsert);
	ASSERT_EQ(NonTrivial::constructions(), expectedBeforeInsert);
	ASSERT_EQ(NonTrivial::destructions(), 0);

	printf("Inserting a range into the middle of the array\n");
	array_.insert(array_.begin() + InsertPos, newArray.begin(), newArray.end());
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	// Elements after `InsertPos` are the shifted ones, plus the ones from `setCapacity()`
	const unsigned int movedElements = ArrayElements - (ArrayElements - InsertPos) + ArrayElements;
	const unsigned int expectedAfterInsert = ArrayElements + NewArrayElements * 2; // The array received `NewArrayElements` number of elements
	ASSERT_EQ(array_.size(), ArrayElements + NewArrayElements);
	ASSERT_EQ(NonTrivial::counter(), expectedAfterInsert) << "Number of alive objects must match after insertion";
	ASSERT_EQ(NonTrivial::constructions(), expectedAfterInsert + movedElements) << "All new elements must be constructed properly";
	ASSERT_LE(NonTrivial::destructions(), movedElements) << "If any elements were destroyed during shifting, they should not exceed the number of moved elements";
	ASSERT_EQ(NonTrivial::liveInstances(), NonTrivial::counter()) << "Internal live object map must match counter";
}

TEST_F(ArrayNonTrivialOperationsTest, InsertRangeInTheMiddleZero)
{
	const unsigned int ArrayElements = 4;
	const unsigned int InsertPos = ArrayElements / 2;
	static_assert(ArrayElements <= Capacity, "Don't trigger a reallocation with `setCapacity()`");

	for (unsigned int i = 0; i < ArrayElements; i++)
		array_.emplaceBack();
	nctl::Array<NonTrivial> newArray(Capacity);
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	const unsigned int expectedBeforeInsert = ArrayElements;
	ASSERT_EQ(NonTrivial::counter(), expectedBeforeInsert);
	ASSERT_EQ(NonTrivial::constructions(), expectedBeforeInsert);
	ASSERT_EQ(NonTrivial::destructions(), 0);

	ASSERT_TRUE(newArray.isEmpty()) << "No elements should be inserted by this test";
	printf("Inserting an empty range into the middle of the array\n");
	array_.insert(array_.begin() + InsertPos, newArray.begin(), newArray.end());
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	const unsigned int movedElements = 0;
	const unsigned int expectedAfterInsert = ArrayElements; // The array received no elements
	const unsigned int expectedConstructions = ArrayElements;

	ASSERT_EQ(array_.size(), ArrayElements);
	ASSERT_EQ(NonTrivial::counter(), expectedAfterInsert) << "Number of alive objects must match after insertion";
	ASSERT_EQ(NonTrivial::constructions(), expectedConstructions) << "All new elements must be constructed properly";
	ASSERT_LE(NonTrivial::destructions(), movedElements) << "If any elements were destroyed during shifting, they should not exceed the number of moved elements";
	ASSERT_EQ(NonTrivial::liveInstances(), NonTrivial::counter()) << "Internal live object map must match counter";
}

TEST_F(ArrayNonTrivialOperationsTest, InsertRangeInTheMiddleOne)
{
	const unsigned int ArrayElements = 4;
	const unsigned int NewArrayElements = 1;
	static_assert(NewArrayElements == 1, "Only one element should be inserted by this test");
	const unsigned int InsertPos = ArrayElements / 2;
	static_assert(ArrayElements + NewArrayElements <= Capacity, "Don't trigger a reallocation with `setCapacity()`");

	for (unsigned int i = 0; i < ArrayElements; i++)
		array_.emplaceBack();
	nctl::Array<NonTrivial> newArray(Capacity);
	for (unsigned int i = 0; i < NewArrayElements; i++)
		newArray.emplaceBack();
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	const unsigned int expectedBeforeInsert = ArrayElements + NewArrayElements;
	ASSERT_EQ(NonTrivial::counter(), expectedBeforeInsert);
	ASSERT_EQ(NonTrivial::constructions(), expectedBeforeInsert);
	ASSERT_EQ(NonTrivial::destructions(), 0);

	ASSERT_EQ(newArray.size(), 1) << "Only one element should be inserted by this test";
	printf("Inserting a range into the middle of the array\n");
	array_.insert(array_.begin() + InsertPos, newArray.begin(), newArray.end());
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	const unsigned int movedElements = ArrayElements - (ArrayElements - InsertPos); // Elements after `InsertPos` are the shifted ones
	const unsigned int expectedAfterInsert = ArrayElements + NewArrayElements * 2; // The array received `NewArrayElements` number of elements
	const unsigned int expectedConstructions = ArrayElements + NewArrayElements + movedElements + NewArrayElements; // array_, newArray, move constructed, inserted range

	ASSERT_EQ(array_.size(), ArrayElements + NewArrayElements);
	ASSERT_EQ(NonTrivial::counter(), expectedAfterInsert) << "Number of alive objects must match after insertion";
	ASSERT_EQ(NonTrivial::constructions(), expectedConstructions) << "All new elements must be constructed properly";
	ASSERT_LE(NonTrivial::destructions(), movedElements) << "If any elements were destroyed during shifting, they should not exceed the number of moved elements";
	ASSERT_EQ(NonTrivial::liveInstances(), NonTrivial::counter()) << "Internal live object map must match counter";
}

TEST_F(ArrayNonTrivialOperationsTest, InsertRangeAtTheEnd)
{
	const unsigned int ArrayElements = 4;
	const unsigned int NewArrayElements = 3;
	const unsigned int InsertPos = ArrayElements;
	static_assert(NewArrayElements > 1, "More than one element should be inserted by this test");
	static_assert(ArrayElements + NewArrayElements <= Capacity, "Don't trigger a reallocation with `setCapacity()`");

	for (unsigned int i = 0; i < ArrayElements; i++)
		array_.emplaceBack();
	nctl::Array<NonTrivial> newArray(Capacity);
	for (unsigned int i = 0; i < NewArrayElements; i++)
		newArray.emplaceBack();
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	const unsigned int expectedBeforeInsert = ArrayElements + NewArrayElements;
	ASSERT_EQ(NonTrivial::counter(), expectedBeforeInsert);
	ASSERT_EQ(NonTrivial::constructions(), expectedBeforeInsert);
	ASSERT_EQ(NonTrivial::destructions(), 0);

	printf("Inserting a range at the end of the array\n");
	array_.insert(array_.begin() + InsertPos, newArray.begin(), newArray.end());
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	const unsigned int movedElements = 0; // No elements are moved when appending at the end
	const unsigned int expectedAfterInsert = ArrayElements + NewArrayElements * 2; // `array_` received `NewArrayElements` number of elements
	ASSERT_EQ(array_.size(), ArrayElements + NewArrayElements);
	ASSERT_EQ(NonTrivial::counter(), expectedAfterInsert) << "Number of alive objects must match after insertion";
	ASSERT_EQ(NonTrivial::constructions(), expectedAfterInsert + movedElements) << "All new elements must be constructed properly";
	ASSERT_LE(NonTrivial::destructions(), movedElements) << "If any elements were destroyed during shifting, they should not exceed the number of moved elements";
	ASSERT_EQ(NonTrivial::liveInstances(), NonTrivial::counter()) << "Internal live object map must match counter";
}

TEST_F(ArrayNonTrivialOperationsTest, InsertRangeAtTheEndRealloc)
{
	const unsigned int ArrayElements = 8;
	const unsigned int NewArrayElements = 4;
	const unsigned int InsertPos = ArrayElements;
	static_assert(NewArrayElements > 1, "More than one element should be inserted by this test");
	static_assert(ArrayElements <= Capacity, "Don't trigger a reallocation with `setCapacity()` before inserting");
	static_assert(ArrayElements + NewArrayElements > Capacity, "Trigger a reallocation with `setCapacity()`");

	for (unsigned int i = 0; i < ArrayElements; i++)
		array_.emplaceBack();
	nctl::Array<NonTrivial> newArray(Capacity);
	for (unsigned int i = 0; i < NewArrayElements; i++)
		newArray.emplaceBack();
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	const unsigned int expectedBeforeInsert = ArrayElements + NewArrayElements;
	ASSERT_EQ(NonTrivial::counter(), expectedBeforeInsert);
	ASSERT_EQ(NonTrivial::constructions(), expectedBeforeInsert);
	ASSERT_EQ(NonTrivial::destructions(), 0);

	printf("Inserting a range at the end of the array\n");
	array_.insert(array_.begin() + InsertPos, newArray.begin(), newArray.end());
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	const unsigned int movedElements = ArrayElements; // All array elements are moved by `setCapacity()`
	const unsigned int expectedAfterInsert = ArrayElements + NewArrayElements * 2; // The array received `NewArrayElements` number of elements
	ASSERT_EQ(array_.size(), ArrayElements + NewArrayElements);
	ASSERT_EQ(NonTrivial::counter(), expectedAfterInsert) << "Number of alive objects must match after insertion";
	ASSERT_EQ(NonTrivial::constructions(), expectedAfterInsert + movedElements) << "All new elements must be constructed properly";
	ASSERT_LE(NonTrivial::destructions(), movedElements) << "If any elements were destroyed during shifting, they should not exceed the number of moved elements";
	ASSERT_EQ(NonTrivial::liveInstances(), NonTrivial::counter()) << "Internal live object map must match counter";
}

TEST_F(ArrayNonTrivialOperationsTest, InsertAtInTheMiddleCopy)
{
	const unsigned int ArrayElements = 4;
	const unsigned int InsertPos = ArrayElements / 2;
	static_assert(ArrayElements <= Capacity, "Don't trigger a reallocation with `setCapacity()`");

	for (unsigned int i = 0; i < ArrayElements; i++)
		array_.emplaceBack();
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	ASSERT_EQ(NonTrivial::counter(), ArrayElements);
	ASSERT_EQ(NonTrivial::constructions(), ArrayElements);
	ASSERT_EQ(NonTrivial::destructions(), 0);

	// The `newElement` object will be destructed when out of scope
	{
		printf("Inserting an element in the middle of the array\n");
		NonTrivial newElement;
		array_.insertAt(InsertPos, newElement);
	}
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	const unsigned int expectedAfterInsert = ArrayElements + 1;
	ASSERT_EQ(array_.size(), ArrayElements + 1);
	ASSERT_EQ(NonTrivial::counter(), expectedAfterInsert) << "Should have exactly one more live object";
	ASSERT_EQ(NonTrivial::liveInstances(), NonTrivial::counter()) << "Internal live object map must match counter";
}

TEST_F(ArrayNonTrivialOperationsTest, InsertAtTheEndCopy)
{
	const unsigned int ArrayElements = 4;
	const unsigned int InsertPos = ArrayElements;
	static_assert(ArrayElements <= Capacity, "Don't trigger a reallocation with `setCapacity()`");

	for (unsigned int i = 0; i < ArrayElements; i++)
		array_.emplaceBack();
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	ASSERT_EQ(NonTrivial::counter(), ArrayElements);
	ASSERT_EQ(NonTrivial::constructions(), ArrayElements);
	ASSERT_EQ(NonTrivial::destructions(), 0);

	// The `newElement` object will be destructed when out of scope
	{
		printf("Inserting an element at the end of the array\n");
		NonTrivial newElement;
		array_.insertAt(InsertPos, newElement);
	}
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	const unsigned int expectedAfterInsert = ArrayElements + 1;
	ASSERT_EQ(array_.size(), ArrayElements + 1);
	ASSERT_EQ(NonTrivial::counter(), expectedAfterInsert) << "Should have exactly one more live object";
	ASSERT_EQ(NonTrivial::liveInstances(), NonTrivial::counter()) << "Internal live object map must match counter";
}

TEST_F(ArrayNonTrivialOperationsTest, InsertAtInTheMiddleMove)
{
	const unsigned int ArrayElements = 4;
	const unsigned int InsertPos = ArrayElements / 2;
	static_assert(ArrayElements <= Capacity, "Don't trigger a reallocation with `setCapacity()`");

	for (unsigned int i = 0; i < ArrayElements; i++)
		array_.emplaceBack();
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	ASSERT_EQ(NonTrivial::counter(), ArrayElements);
	ASSERT_EQ(NonTrivial::constructions(), ArrayElements);
	ASSERT_EQ(NonTrivial::destructions(), 0);

	// The `newElement` object will be destructed when out of scope
	{
		printf("Inserting an element in the middle of the array\n");
		NonTrivial newElement;
		array_.insertAt(InsertPos, newElement);
	}
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	const unsigned int expectedAfterInsert = ArrayElements + 1;
	ASSERT_EQ(array_.size(), ArrayElements + 1);
	ASSERT_EQ(NonTrivial::counter(), expectedAfterInsert) << "Should have exactly one more live object";
	ASSERT_EQ(NonTrivial::liveInstances(), NonTrivial::counter()) << "Internal live object map must match counter";
}

TEST_F(ArrayNonTrivialOperationsTest, InsertAtTheEndMove)
{
	const unsigned int ArrayElements = 4;
	const unsigned int InsertPos = ArrayElements;
	static_assert(ArrayElements <= Capacity, "Don't trigger a reallocation with `setCapacity()`");

	for (unsigned int i = 0; i < ArrayElements; i++)
		array_.emplaceBack();
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	ASSERT_EQ(NonTrivial::counter(), ArrayElements);
	ASSERT_EQ(NonTrivial::constructions(), ArrayElements);
	ASSERT_EQ(NonTrivial::destructions(), 0);

	// The `newElement` object will be destructed when out of scope
	{
		printf("Inserting an element at the end of the array\n");
		NonTrivial newElement;
		array_.insertAt(InsertPos, newElement);
	}
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	const unsigned int expectedAfterInsert = ArrayElements + 1;
	ASSERT_EQ(array_.size(), ArrayElements + 1);
	ASSERT_EQ(NonTrivial::counter(), expectedAfterInsert) << "Should have exactly one more live object";
	ASSERT_EQ(NonTrivial::liveInstances(), NonTrivial::counter()) << "Internal live object map must match counter";
}

TEST_F(ArrayNonTrivialOperationsTest, EmplaceAtInTheMiddle)
{
	const unsigned int ArrayElements = 4;
	const unsigned int EmplacePos = ArrayElements / 2;
	static_assert(ArrayElements <= Capacity, "Don't trigger a reallocation with `setCapacity()`");

	for (unsigned int i = 0; i < ArrayElements; i++)
		array_.emplaceBack();
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	ASSERT_EQ(NonTrivial::counter(), ArrayElements);
	ASSERT_EQ(NonTrivial::constructions(), ArrayElements);
	ASSERT_EQ(NonTrivial::destructions(), 0);

	printf("Emplacing an element in the middle of the array\n");
	array_.emplaceAt(EmplacePos);
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	const unsigned int expectedAfterEmplace = ArrayElements + 1;
	ASSERT_EQ(array_.size(), ArrayElements + 1);
	ASSERT_EQ(NonTrivial::counter(), expectedAfterEmplace) << "Should have exactly one more live object";
	ASSERT_EQ(NonTrivial::liveInstances(), NonTrivial::counter()) << "Internal live object map must match counter";
}

TEST_F(ArrayNonTrivialOperationsTest, EmplaceAtTheEnd)
{
	const unsigned int ArrayElements = 4;
	const unsigned int EmplacePos = ArrayElements;
	static_assert(ArrayElements <= Capacity, "Don't trigger a reallocation with `setCapacity()`");

	for (unsigned int i = 0; i < ArrayElements; i++)
		array_.emplaceBack();
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	ASSERT_EQ(NonTrivial::counter(), ArrayElements);
	ASSERT_EQ(NonTrivial::constructions(), ArrayElements);
	ASSERT_EQ(NonTrivial::destructions(), 0);

	printf("Emplacing an element at the end of the array\n");
	array_.emplaceAt(EmplacePos);
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	const unsigned int expectedAfterEmplace = ArrayElements + 1;
	ASSERT_EQ(array_.size(), ArrayElements + 1);
	ASSERT_EQ(NonTrivial::counter(), expectedAfterEmplace) << "Should have exactly one more live object";
	ASSERT_EQ(NonTrivial::liveInstances(), NonTrivial::counter()) << "Internal live object map must match counter";
}

TEST_F(ArrayNonTrivialOperationsTest, RemoveRangeInTheMiddle)
{
	const unsigned int ArrayElements = 6;
	const unsigned int RemovePos = ArrayElements / 2;
	const unsigned int NumRemoved = 2;
	static_assert(ArrayElements <= Capacity, "Don't trigger a reallocation with `setCapacity()`");

	for (unsigned int i = 0; i < ArrayElements; i++)
		array_.emplaceBack();
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	ASSERT_EQ(NonTrivial::counter(), ArrayElements);
	ASSERT_EQ(NonTrivial::constructions(), ArrayElements);
	ASSERT_EQ(NonTrivial::destructions(), 0);

	printf("Removing a range from the middle of the array\n");
	array_.removeRange(RemovePos, RemovePos + NumRemoved);
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	const unsigned int expectedAfterRemove = ArrayElements - NumRemoved;
	ASSERT_EQ(array_.size(), ArrayElements - NumRemoved);
	ASSERT_EQ(NonTrivial::counter(), expectedAfterRemove) << "Should have exactly one more live object";
	ASSERT_EQ(NonTrivial::liveInstances(), NonTrivial::counter()) << "Internal live object map must match counter";
}

TEST_F(ArrayNonTrivialOperationsTest, UnorderedRemoveRangeInTheMiddle)
{
	const unsigned int ArrayElements = 6;
	const unsigned int RemovePos = ArrayElements / 2;
	const unsigned int NumRemoved = 2;
	static_assert(ArrayElements <= Capacity, "Don't trigger a reallocation with `setCapacity()`");

	for (unsigned int i = 0; i < ArrayElements; i++)
		array_.emplaceBack();
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	ASSERT_EQ(NonTrivial::counter(), ArrayElements);
	ASSERT_EQ(NonTrivial::constructions(), ArrayElements);
	ASSERT_EQ(NonTrivial::destructions(), 0);

	printf("Removing a range from the middle of the array\n");
	array_.unorderedRemoveRange(RemovePos, RemovePos + NumRemoved);
	printf("NonTrivial class counters - alive: %u, constructed: %u, destructed: %u, assigned: %u\n",
	       NonTrivial::counter(), NonTrivial::constructions(), NonTrivial::destructions(), NonTrivial::assignments());

	const unsigned int expectedAfterRemove = ArrayElements - NumRemoved;
	ASSERT_EQ(array_.size(), ArrayElements - NumRemoved);
	ASSERT_EQ(NonTrivial::counter(), expectedAfterRemove) << "Should have exactly one more live object";
	ASSERT_EQ(NonTrivial::liveInstances(), NonTrivial::counter()) << "Internal live object map must match counter";
}

}
