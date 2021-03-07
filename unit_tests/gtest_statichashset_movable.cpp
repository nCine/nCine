#include "gtest_statichashset.h"
#include "test_movable.h"

namespace {

class StaticHashSetMovableTest : public ::testing::Test
{
  protected:
	nctl::StaticHashSet<Movable, Capacity> hashset_;
};

#if !TEST_MOVABLE_ONLY
TEST_F(StaticHashSetMovableTest, SubscriptLValue)
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

TEST_F(StaticHashSetMovableTest, SubscriptRValue)
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
TEST_F(StaticHashSetMovableTest, InsertLValue)
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

TEST_F(StaticHashSetMovableTest, InsertRValue)
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

TEST_F(StaticHashSetMovableTest, MoveConstruction)
{
	Movable refMovable(Movable::Construction::INITIALIZED);
	Movable movable(Movable::Construction::INITIALIZED);
	const unsigned int newSize = movable.size();
	const int *newData = movable.data();

	hashset_.insert(nctl::move(movable));
	const Movable *m = hashset_.find(refMovable);
	m->printAndAssert();
	printf("Creating a new hashset with move construction\n");
	nctl::StaticHashSet<Movable, Capacity> newHashset(nctl::move(hashset_));
	m = newHashset.find(refMovable);
	m->printAndAssert();

	ASSERT_EQ(m->size(), newSize);
	ASSERT_EQ(m->data(), newData);
}

TEST_F(StaticHashSetMovableTest, MoveAssignmentOperator)
{
	Movable refMovable(Movable::Construction::INITIALIZED);
	Movable movable(Movable::Construction::INITIALIZED);
	const unsigned int newSize = movable.size();
	const int *newData = movable.data();

	hashset_.insert(nctl::move(movable));
	const Movable *m = hashset_.find(refMovable);
	m->printAndAssert();
	printf("Creating a new hashset with the move assignment operator\n");
	nctl::StaticHashSet<Movable, Capacity> newHashset;
	newHashset = nctl::move(hashset_);
	m = newHashset.find(refMovable);
	m->printAndAssert();

	ASSERT_EQ(m->size(), newSize);
	ASSERT_EQ(m->data(), newData);
}

}
