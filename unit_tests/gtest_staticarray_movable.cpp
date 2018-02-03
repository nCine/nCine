#include "gtest_staticarray.h"
#include "test_movable.h"

namespace {

class StaticArrayMovableTest : public ::testing::Test
{
  protected:
	nctl::StaticArray<Movable, Capacity> array_;
};

TEST_F(StaticArrayMovableTest, CopyConstruction)
{
	printf("Inserting a complex object at the back\n");
	array_.insertBack(Movable(Movable::Construction::INITIALIZED));
	printf("Creating a new array with copy construction\n");
	nctl::StaticArray<Movable, Capacity> newArray(array_);

	array_[0].printAndAssert();
	newArray[0].printAndAssert();
	ASSERT_EQ(array_.size(), 1);
	ASSERT_EQ(newArray.size(), 1);
	ASSERT_EQ(array_[0].size(), 10);
	ASSERT_NE(array_[0].data(), nullptr);
	ASSERT_EQ(newArray[0].size(), 10);
	ASSERT_NE(newArray[0].data(), nullptr);
	ASSERT_NE(array_[0].data(), newArray[0].data());
}

TEST_F(StaticArrayMovableTest, MoveConstruction)
{
	printf("Inserting a complex object at the back\n");
	array_.insertBack(Movable(Movable::Construction::INITIALIZED));
	printf("Creating a new array with move construction\n");
	nctl::StaticArray<Movable, Capacity> newArray(nctl::move(array_));

	newArray[0].printAndAssert();
	ASSERT_EQ(array_.size(), 0);
	ASSERT_EQ(newArray.size(), 1);
}

TEST_F(StaticArrayMovableTest, InsertBackLValue)
{
	Movable movable(Movable::Construction::INITIALIZED);
	printf("Inserting a complex object at the back\n");
	array_.insertBack(movable);

	array_[0].printAndAssert();
	ASSERT_EQ(array_.size(), 1);
	ASSERT_EQ(movable.size(), array_[0].size());
	ASSERT_NE(movable.data(), nullptr);
}

TEST_F(StaticArrayMovableTest, InsertBackRValue)
{
	Movable movable(Movable::Construction::INITIALIZED);
	printf("Move inserting a complex object at the back\n");
	array_.insertBack(nctl::move(movable));

	array_[0].printAndAssert();
	ASSERT_EQ(array_.size(), 1);
	ASSERT_EQ(movable.size(), 0);
	ASSERT_EQ(movable.data(), nullptr);
}

}
