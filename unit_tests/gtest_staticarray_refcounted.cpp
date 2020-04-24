#include "gtest_staticarray.h"
#define PRINT_ALL_COUNTERS (0)
#include "test_refcounted.h"

int RefCounted::counter_ = 0;
int RefCounted::constructions_ = 0;
int RefCounted::destructions_ = 0;
int RefCounted::copyConstructions_ = 0;
int RefCounted::moveConstructions_ = 0;
int RefCounted::assignments_ = 0;
int RefCounted::moveAssignments_ = 0;

namespace {

class StaticArrayRefCountedTest : public ::testing::Test
{
  public:
	StaticArrayRefCountedTest() {}

  protected:
	nctl::StaticArray<RefCounted, Capacity> array_;
};

TEST_F(StaticArrayRefCountedTest, PushBackLValue)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting a new RefCounted object at the back\n");
	{
		const RefCounted rc;
		array_.pushBack(rc);
	}

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(StaticArrayRefCountedTest, PushBackRValue)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Move inserting a new RefCounted object at the back\n");
	array_.pushBack(RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(StaticArrayRefCountedTest, PushAndPopBack)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting a new RefCounted object at the back\n");
	array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Removing the RefCounted object from the back\n");
	array_.popBack();
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 0);
}

TEST_F(StaticArrayRefCountedTest, DoublePushBackClear)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting two new RefCounted objects at the back\n");
	array_.pushBack(RefCounted());
	array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Clearing the array\n");
	array_.clear();
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 0);
}

TEST_F(StaticArrayRefCountedTest, EmplaceBack)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Emplacing a RefCounted object at the back\n");
	array_.emplaceBack(RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(StaticArrayRefCountedTest, InsertLValueAtBackWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting a RefCounted object at the back\n");
	{
		const RefCounted rc;
		array_.insert(array_.end(), rc);
	}

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(StaticArrayRefCountedTest, InsertRValueAtBackWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Move inserting a RefCounted object at the back\n");
	array_.insert(array_.end(), RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(StaticArrayRefCountedTest, InsertLValueAtFrontWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	array_.pushBack(RefCounted());
	printf("Inserting a RefCounted object at the beginning\n");
	{
		const RefCounted rc;
		array_.insert(array_.begin(), rc);
	}

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(StaticArrayRefCountedTest, InsertRValueAtFrontWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	array_.pushBack(RefCounted());
	printf("Move inserting a RefCounted object at the beginning\n");
	array_.insert(array_.begin(), RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(StaticArrayRefCountedTest, EmplaceAtBackWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Emplacing a RefCounted object at the back\n");
	array_.emplace(array_.end(), RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(StaticArrayRefCountedTest, EmplaceAtFrontWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	array_.pushBack(RefCounted());
	printf("Emplacing a RefCounted object at the beginning\n");
	array_.emplace(array_.begin(), RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(StaticArrayRefCountedTest, CopyConstruction)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new array with copy construction\n");
	nctl::StaticArray<RefCounted, Capacity> newArray(array_);
	printRefCounters();

	ASSERT_EQ(array_.size(), 1);
	ASSERT_EQ(newArray.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(StaticArrayRefCountedTest, MoveConstruction)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new array with move construction\n");
	nctl::StaticArray<RefCounted, Capacity> newArray(nctl::move(array_));
	printRefCounters();

	ASSERT_EQ(array_.size(), 0);
	ASSERT_EQ(newArray.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(StaticArrayRefCountedTest, CopyAssignmentOperator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new array with the copy assignment operator\n");
	nctl::StaticArray<RefCounted, Capacity> newArray;
	newArray = array_;
	printRefCounters();

	ASSERT_EQ(array_.size(), 1);
	ASSERT_EQ(newArray.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(StaticArrayRefCountedTest, CopyAssignmentOperatorFromBigger)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	array_.pushBack(RefCounted());
	array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Creating a new array with the copy assignment operator\n");
	nctl::StaticArray<RefCounted, Capacity> newArray;
	newArray = array_;
	printRefCounters();

	ASSERT_EQ(array_.size(), 2);
	ASSERT_EQ(newArray.size(), 2);
	ASSERT_EQ(RefCounted::counter(), 4);
}

TEST_F(StaticArrayRefCountedTest, CopyAssignmentOperatorFromSmaller)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new array with the copy assignment operator\n");
	nctl::StaticArray<RefCounted, Capacity> newArray;
	newArray.pushBack(RefCounted());
	newArray.pushBack(RefCounted());
	ASSERT_EQ(RefCounted::counter(), 3);
	newArray = array_;
	printRefCounters();

	ASSERT_EQ(array_.size(), 1);
	ASSERT_EQ(newArray.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(StaticArrayRefCountedTest, MoveAssignmentOperator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new array with the move assignment operator\n");
	nctl::StaticArray<RefCounted, Capacity> newArray;
	newArray = nctl::move(array_);
	printRefCounters();

	ASSERT_EQ(array_.size(), 0);
	ASSERT_EQ(newArray.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(StaticArrayRefCountedTest, MoveAssignmentOperatorFromBigger)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	array_.pushBack(RefCounted());
	array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Creating a new array with the move assignment operator\n");
	nctl::StaticArray<RefCounted, Capacity> newArray;
	newArray = nctl::move(array_);
	printRefCounters();

	ASSERT_EQ(array_.size(), 0);
	ASSERT_EQ(newArray.size(), 2);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(StaticArrayRefCountedTest, MoveAssignmentOperatorFromSmaller)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new array with the move assignment operator\n");
	nctl::StaticArray<RefCounted, Capacity> newArray;
	newArray.pushBack(RefCounted());
	newArray.pushBack(RefCounted());
	ASSERT_EQ(RefCounted::counter(), 3);
	newArray = nctl::move(array_);
	printRefCounters();

	ASSERT_EQ(array_.size(), 0);
	ASSERT_EQ(newArray.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(StaticArrayRefCountedTest, RemoveLastWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Insert three elements)\n");
	array_.pushBack(RefCounted());
	array_.pushBack(RefCounted());
	array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 3);

	printf("Removing last element\n");
	array_.erase(array_.end() - 1);
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(StaticArrayRefCountedTest, RemoveSecondHalfWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Fill the whole array up to capacity (%u elements)\n", Capacity);
	for (unsigned int i = 0; i < Capacity; i++)
		array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity);

	printf("Removing the second half of the array\n");
	array_.erase(array_.begin() + array_.size() / 2, array_.end());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity / 2);
}

TEST_F(StaticArrayRefCountedTest, UnorderedRemoveSecondHalfWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Fill the whole array up to capacity (%u elements)\n", Capacity);
	for (unsigned int i = 0; i < Capacity; i++)
		array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity);

	printf("Removing the first half of the array (unordered)\n");
	array_.unorderedErase(array_.begin(), array_.begin() + array_.size() / 2);
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity / 2);
}

TEST_F(StaticArrayRefCountedTest, SetHalfSize)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Fill the whole array up to capacity (%u elements)\n", Capacity);
	for (unsigned int i = 0; i < Capacity; i++)
		array_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity);

	printf("Shrink the array to half its size\n");
	array_.setSize(Capacity / 2);
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Capacity / 2);
}

}
