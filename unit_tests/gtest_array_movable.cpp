#include "gtest_array.h"
#include "test_movable.h"

namespace {

class ArrayMovableTest : public ::testing::Test
{
  public:
	ArrayMovableTest() : array_(Capacity) { }

  protected:
	nctl::Array<Movable> array_;
};

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

}
