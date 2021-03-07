#include "gtest_hashset.h"
#include "test_movable.h"

namespace {

class HashSetMovableTest : public ::testing::Test
{
  public:
	HashSetMovableTest()
	    : hashset_(Capacity) {}

  protected:
	nctl::HashSet<Movable> hashset_;
};

#if !TEST_MOVABLE_ONLY
TEST_F(HashSetMovableTest, SubscriptLValue)
{
	Movable movable(Movable::Construction::INITIALIZED);

	ASSERT_EQ(hashset_.find(movable), nullptr);
	hashset_.insert(movable);
	const Movable *m = hashset_.find(movable);
	m->printAndAssert();

	ASSERT_NE(hashset_.find(movable), nullptr);
	ASSERT_EQ(movable.size(), m->size());
	ASSERT_NE(movable.data(), nullptr);
}
#endif

TEST_F(HashSetMovableTest, SubscriptRValue)
{
	Movable refMovable(Movable::Construction::INITIALIZED);
	Movable movable(Movable::Construction::INITIALIZED);
	const unsigned int newSize = movable.size();
	const int *newData = movable.data();

	ASSERT_EQ(hashset_.find(refMovable), nullptr);
	hashset_.insert(nctl::move(movable));
	const Movable *m = hashset_.find(refMovable);
	m->printAndAssert();

	ASSERT_NE(hashset_.find(refMovable), nullptr);
	ASSERT_EQ(m->size(), newSize);
	ASSERT_EQ(m->data(), newData);
	ASSERT_EQ(movable.size(), 0);
	ASSERT_EQ(movable.data(), nullptr);
}

#if !TEST_MOVABLE_ONLY
TEST_F(HashSetMovableTest, InsertLValue)
{
	Movable movable(Movable::Construction::INITIALIZED);

	ASSERT_EQ(hashset_.find(movable), nullptr);
	hashset_.insert(movable);
	const Movable *m = hashset_.find(movable);
	m->printAndAssert();

	ASSERT_NE(hashset_.find(movable), nullptr);
	ASSERT_EQ(movable.size(), m->size());
	ASSERT_NE(movable.data(), nullptr);
}
#endif

TEST_F(HashSetMovableTest, InsertRValue)
{
	Movable refMovable(Movable::Construction::INITIALIZED);
	Movable movable(Movable::Construction::INITIALIZED);
	const unsigned int newSize = movable.size();
	const int *newData = movable.data();

	ASSERT_EQ(hashset_.find(refMovable), nullptr);
	hashset_.insert(nctl::move(movable));
	const Movable *m = hashset_.find(refMovable);
	m->printAndAssert();

	ASSERT_NE(hashset_.find(refMovable), nullptr);
	ASSERT_EQ(m->size(), newSize);
	ASSERT_EQ(m->data(), newData);
	ASSERT_EQ(movable.size(), 0);
	ASSERT_EQ(movable.data(), nullptr);
}

TEST_F(HashSetMovableTest, MoveConstruction)
{
	Movable refMovable(Movable::Construction::INITIALIZED);
	Movable movable(Movable::Construction::INITIALIZED);
	const unsigned int newSize = movable.size();
	const int *newData = movable.data();

	hashset_.insert(nctl::move(movable));
	const Movable *m = hashset_.find(refMovable);
	m->printAndAssert();
	printf("Creating a new hashset with move construction\n");
	nctl::HashSet<Movable> newHashset(nctl::move(hashset_));
	m = newHashset.find(refMovable);
	m->printAndAssert();

	ASSERT_EQ(m->size(), newSize);
	ASSERT_EQ(m->data(), newData);
}

TEST_F(HashSetMovableTest, MoveAssignmentOperator)
{
	Movable refMovable(Movable::Construction::INITIALIZED);
	Movable movable(Movable::Construction::INITIALIZED);
	const unsigned int newSize = movable.size();
	const int *newData = movable.data();

	hashset_.insert(nctl::move(movable));
	const Movable *m = hashset_.find(refMovable);
	m->printAndAssert();
	printf("Creating a new hashset with the move assignment operator\n");
	nctl::HashSet<Movable> newHashset(Capacity);
	newHashset = nctl::move(hashset_);
	m = newHashset.find(refMovable);
	m->printAndAssert();

	ASSERT_EQ(m->size(), newSize);
	ASSERT_EQ(m->data(), newData);
}

}
