#include "gtest_array.h"
#include "test_movable.h"

namespace {

class ArrayMovableTest : public ::testing::Test
{
  public:
	ArrayMovableTest()
	    : array_(Capacity) {}

  protected:
	nctl::Array<Movable> array_;
};

#if !TEST_MOVABLE_ONLY
TEST_F(ArrayMovableTest, PushBackLValue)
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

TEST_F(ArrayMovableTest, PushBackRValue)
{
	Movable movable(Movable::Construction::INITIALIZED);
	printf("Move inserting a complex object at the back\n");
	array_.pushBack(nctl::move(movable));

	array_[0].printAndAssert();
	ASSERT_EQ(array_.size(), 1);
	ASSERT_EQ(movable.size(), 0);
	ASSERT_EQ(movable.data(), nullptr);
}

TEST_F(ArrayMovableTest, EmplaceBack)
{
	printf("Emplacing a complex object at the back\n");
	array_.emplaceBack(Movable::Construction::INITIALIZED);

	array_[0].printAndAssert();
	ASSERT_EQ(array_.size(), 1);
}

#if !TEST_MOVABLE_ONLY
TEST_F(ArrayMovableTest, InsertLValue)
{
	Movable movable(Movable::Construction::INITIALIZED);
	printf("Inserting a complex object at the back\n");
	array_.insertAt(0, movable);

	array_[0].printAndAssert();
	ASSERT_EQ(array_.size(), 1);
	ASSERT_EQ(movable.size(), array_[0].size());
	ASSERT_NE(movable.data(), nullptr);
}
#endif

TEST_F(ArrayMovableTest, InsertRValue)
{
	Movable movable(Movable::Construction::INITIALIZED);
	printf("Move inserting a complex object at the back\n");
	array_.insertAt(0, nctl::move(movable));

	array_[0].printAndAssert();
	ASSERT_EQ(array_.size(), 1);
	ASSERT_EQ(movable.size(), 0);
	ASSERT_EQ(movable.data(), nullptr);
}

TEST_F(ArrayMovableTest, EmplaceAt)
{
	Movable movable(Movable::Construction::INITIALIZED);
	printf("Emplacing a complex object at the back\n");
	array_.emplaceAt(0, Movable::Construction::INITIALIZED);

	array_[0].printAndAssert();
	ASSERT_EQ(array_.size(), 1);
	ASSERT_EQ(movable.size(), array_[0].size());
	ASSERT_NE(movable.data(), nullptr);
}

#if !TEST_MOVABLE_ONLY
TEST_F(ArrayMovableTest, InsertLValueAtBackWithIterator)
{
	Movable movable(Movable::Construction::INITIALIZED);
	printf("Inserting a complex object at the back\n");
	array_.insert(array_.end(), movable);

	array_[0].printAndAssert();
	ASSERT_EQ(array_.size(), 1);
	ASSERT_EQ(movable.size(), array_[0].size());
	ASSERT_NE(movable.data(), nullptr);
}
#endif

TEST_F(ArrayMovableTest, InsertRValueAtBackWithIterator)
{
	Movable movable(Movable::Construction::INITIALIZED);
	printf("Move inserting a complex object at the back\n");
	array_.insert(array_.end(), nctl::move(movable));

	array_[0].printAndAssert();
	ASSERT_EQ(array_.size(), 1);
	ASSERT_EQ(movable.size(), 0);
	ASSERT_EQ(movable.data(), nullptr);
}

TEST_F(ArrayMovableTest, EmplaceAtBackWithIterator)
{
	Movable movable(Movable::Construction::INITIALIZED);
	printf("Emplacing a complex object at the back\n");
	array_.emplace(array_.end(), Movable::Construction::INITIALIZED);

	array_[0].printAndAssert();
	ASSERT_EQ(array_.size(), 1);
	ASSERT_EQ(movable.size(), array_[0].size());
	ASSERT_NE(movable.data(), nullptr);
}

TEST_F(ArrayMovableTest, MoveConstruction)
{
	Movable movable(Movable::Construction::INITIALIZED);
	array_.pushBack(nctl::move(movable));
	printf("Creating a new array with move construction\n");
	nctl::Array<Movable> newArray(nctl::move(array_));

	newArray[0].printAndAssert();
	ASSERT_EQ(array_.size(), 0);
	ASSERT_EQ(newArray.size(), 1);
}

TEST_F(ArrayMovableTest, MoveAssignmentOperator)
{
	Movable movable(Movable::Construction::INITIALIZED);
	array_.pushBack(nctl::move(movable));
	printf("Creating a new array with the move assignment operator\n");
	nctl::Array<Movable> newArray;
	newArray = nctl::move(array_);

	newArray[0].printAndAssert();
	ASSERT_EQ(array_.size(), 0);
	ASSERT_EQ(newArray.size(), 1);
}

}
