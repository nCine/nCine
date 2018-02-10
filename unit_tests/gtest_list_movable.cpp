#include "gtest_list.h"
#include "test_movable.h"

namespace {

class ListMovableTest : public ::testing::Test
{
  protected:
	nctl::List<Movable> list_;
};

TEST_F(ListMovableTest, PushBackLValue)
{
	Movable movable(Movable::Construction::INITIALIZED);
	printf("Inserting a complex object to the back\n");
	list_.pushBack(movable);

	list_.front().printAndAssert();
	ASSERT_EQ(list_.size(), 1);
	ASSERT_EQ(movable.size(), list_.front().size());
	ASSERT_NE(movable.data(), nullptr);
}

TEST_F(ListMovableTest, PushBackRValue)
{
	Movable movable(Movable::Construction::INITIALIZED);
	printf("Inserting a complex object to the back\n");
	list_.pushBack(nctl::move(movable));

	list_.front().printAndAssert();
	ASSERT_EQ(list_.size(), 1);
	ASSERT_EQ(movable.size(), 0);
	ASSERT_EQ(movable.data(), nullptr);
}

TEST_F(ListMovableTest, PushFrontLValue)
{
	Movable movable(Movable::Construction::INITIALIZED);
	printf("Inserting a complex object to the front\n");
	list_.pushFront(movable);

	list_.front().printAndAssert();
	ASSERT_EQ(list_.size(), 1);
	ASSERT_EQ(movable.size(), list_.front().size());
	ASSERT_NE(movable.data(), nullptr);
}

TEST_F(ListMovableTest, PushFrontRValue)
{
	Movable movable(Movable::Construction::INITIALIZED);
	printf("Inserting a complex object to the front\n");
	list_.pushFront(nctl::move(movable));

	list_.front().printAndAssert();
	ASSERT_EQ(list_.size(), 1);
	ASSERT_EQ(movable.size(), 0);
	ASSERT_EQ(movable.data(), nullptr);
}

TEST_F(ListMovableTest, InsertAfterLValue)
{
	Movable movable(Movable::Construction::INITIALIZED);
	printf("Inserting a complex object to the front\n");
	list_.insertAfter(list_.begin(), movable);

	list_.front().printAndAssert();
	ASSERT_EQ(list_.size(), 1);
	ASSERT_EQ(movable.size(), list_.front().size());
	ASSERT_NE(movable.data(), nullptr);
}

TEST_F(ListMovableTest, InsertAfterRValue)
{
	Movable movable(Movable::Construction::INITIALIZED);
	printf("Inserting a complex object to the front\n");
	list_.insertAfter(list_.begin(), nctl::move(movable));

	list_.front().printAndAssert();
	ASSERT_EQ(list_.size(), 1);
	ASSERT_EQ(movable.size(), 0);
	ASSERT_EQ(movable.data(), nullptr);
}

TEST_F(ListMovableTest, InsertBeforeLValue)
{
	Movable movable(Movable::Construction::INITIALIZED);
	printf("Inserting a complex object to the back\n");
	list_.insertBefore(list_.end(), movable);

	list_.front().printAndAssert();
	ASSERT_EQ(list_.size(), 1);
	ASSERT_EQ(movable.size(), list_.front().size());
	ASSERT_NE(movable.data(), nullptr);
}

TEST_F(ListMovableTest, InsertBeforeRValue)
{
	Movable movable(Movable::Construction::INITIALIZED);
	printf("Inserting a complex object to the back\n");
	list_.insertBefore(list_.end(), nctl::move(movable));

	list_.front().printAndAssert();
	ASSERT_EQ(list_.size(), 1);
	ASSERT_EQ(movable.size(), 0);
	ASSERT_EQ(movable.data(), nullptr);
}

}
