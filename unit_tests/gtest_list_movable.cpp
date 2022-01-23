#include "gtest_list.h"
#include "test_movable.h"

namespace {

class ListMovableTest : public ::testing::Test
{
  protected:
	nctl::List<Movable> list_;
};

#if !TEST_MOVABLE_ONLY
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
#endif

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

TEST_F(ListMovableTest, EmplaceBack)
{
	printf("Emplacing a complex object to the back\n");
	list_.emplaceBack(Movable::Construction::INITIALIZED);

	list_.front().printAndAssert();
	ASSERT_EQ(list_.size(), 1);
}

#if !TEST_MOVABLE_ONLY
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
#endif

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

TEST_F(ListMovableTest, EmplaceFront)
{
	printf("Emplacing a complex object to the front\n");
	list_.emplaceFront(Movable::Construction::INITIALIZED);

	list_.front().printAndAssert();
	ASSERT_EQ(list_.size(), 1);
}

#if !TEST_MOVABLE_ONLY
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
#endif

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

TEST_F(ListMovableTest, EmplaceAfter)
{
	printf("Emplacing a complex object to the front\n");
	list_.emplaceAfter(list_.begin(), Movable::Construction::INITIALIZED);

	list_.front().printAndAssert();
	ASSERT_EQ(list_.size(), 1);
}

#if !TEST_MOVABLE_ONLY
TEST_F(ListMovableTest, InsertBeforeLValue)
{
	Movable movable(Movable::Construction::INITIALIZED);
	printf("Inserting a complex object to the back\n");
	list_.insertBefore(list_.end(), movable);

	list_.back().printAndAssert();
	ASSERT_EQ(list_.size(), 1);
	ASSERT_EQ(movable.size(), list_.front().size());
	ASSERT_NE(movable.data(), nullptr);
}
#endif

TEST_F(ListMovableTest, InsertBeforeRValue)
{
	Movable movable(Movable::Construction::INITIALIZED);
	printf("Inserting a complex object to the back\n");
	list_.insertBefore(list_.end(), nctl::move(movable));

	list_.back().printAndAssert();
	ASSERT_EQ(list_.size(), 1);
	ASSERT_EQ(movable.size(), 0);
	ASSERT_EQ(movable.data(), nullptr);
}

TEST_F(ListMovableTest, EmplaceBefore)
{
	printf("Emplacing a complex object to the back\n");
	list_.emplaceBefore(list_.end(), Movable::Construction::INITIALIZED);

	list_.back().printAndAssert();
	ASSERT_EQ(list_.size(), 1);
}

TEST_F(ListMovableTest, MoveConstruction)
{
	Movable movable(Movable::Construction::INITIALIZED);
	list_.pushBack(nctl::move(movable));
	printf("Creating a new list with move construction\n");
	nctl::List<Movable> newList(nctl::move(list_));

	newList.front().printAndAssert();
	ASSERT_EQ(list_.size(), 0);
	ASSERT_EQ(newList.size(), 1);
}

TEST_F(ListMovableTest, MoveAssignmentOperator)
{
	Movable movable(Movable::Construction::INITIALIZED);
	list_.pushBack(nctl::move(movable));
	printf("Creating a new list with the move assignment operator\n");
	nctl::List<Movable> newList;
	newList = nctl::move(list_);

	newList.front().printAndAssert();
	ASSERT_EQ(list_.size(), 0);
	ASSERT_EQ(newList.size(), 1);
}

}
