#include "gtest_staticarray.h"
#include "test_movable.h"

namespace {

class StaticArrayMovableTest : public ::testing::Test
{
  protected:
	nctl::StaticArray<Movable, Capacity> array_;
};

#if !TEST_MOVABLE_ONLY
TEST_F(StaticArrayMovableTest, PushBackLValue)
{
	Movable movable(Movable::Construction::INITIALIZED);
	printf("Inserting a complex object at the back\n");
	array_.pushBack(movable);

	array_[0].printAndAssert();
	ASSERT_EQ(array_.size(), 1);
	ASSERT_EQ(movable.size(), array_[0].size());
	ASSERT_NE(movable.data(), nullptr);
}
#endif

TEST_F(StaticArrayMovableTest, PushBackRValue)
{
	Movable movable(Movable::Construction::INITIALIZED);
	printf("Move inserting a complex object at the back\n");
	array_.pushBack(nctl::move(movable));

	array_[0].printAndAssert();
	ASSERT_EQ(array_.size(), 1);
	ASSERT_EQ(movable.size(), 0);
	ASSERT_EQ(movable.data(), nullptr);
}

TEST_F(StaticArrayMovableTest, MoveConstruction)
{
	printf("Inserting a complex object at the back\n");
	array_.pushBack(Movable(Movable::Construction::INITIALIZED));
	printf("Creating a new array with move construction\n");
	nctl::StaticArray<Movable, Capacity> newArray(nctl::move(array_));

	newArray[0].printAndAssert();
	ASSERT_EQ(array_.size(), 0);
	ASSERT_EQ(newArray.size(), 1);
}

TEST_F(StaticArrayMovableTest, MoveAssignmentOperator)
{
	printf("Inserting a complex object at the back\n");
	array_.pushBack(Movable(Movable::Construction::INITIALIZED));
	printf("Creating a new array with the move assignment operator\n");
	nctl::StaticArray<Movable, Capacity> newArray;
	newArray = nctl::move(array_);

	newArray[0].printAndAssert();
	ASSERT_EQ(array_.size(), 0);
	ASSERT_EQ(newArray.size(), 1);
}

}
