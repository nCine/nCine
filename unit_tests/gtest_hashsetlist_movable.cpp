#include "gtest_hashsetlist.h"
#include "test_movable.h"

namespace {

class HashSetListMovableTest : public ::testing::Test
{
  public:
	HashSetListMovableTest()
	    : hashset_(Capacity) {}

  protected:
	nctl::HashSetList<Movable> hashset_;
};

#if !TEST_MOVABLE_ONLY
TEST_F(HashSetListMovableTest, SubscriptLValue)
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

TEST_F(HashSetListMovableTest, SubscriptRValue)
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
TEST_F(HashSetListMovableTest, InsertLValue)
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

TEST_F(HashSetListMovableTest, InsertRValue)
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

TEST_F(HashSetListMovableTest, MoveConstruction)
{
	Movable refMovable(Movable::Construction::INITIALIZED);
	Movable movable(Movable::Construction::INITIALIZED);
	const unsigned int newSize = movable.size();
	const int *newData = movable.data();

	hashset_.insert(nctl::move(movable));
	const Movable *m = hashset_.find(refMovable);
	m->printAndAssert();
	printf("Creating a new hashset with move construction\n");
	nctl::HashSetList<Movable> newHashset(nctl::move(hashset_));
	m = newHashset.find(refMovable);
	m->printAndAssert();

	ASSERT_EQ(m->size(), newSize);
	ASSERT_EQ(m->data(), newData);
}

TEST_F(HashSetListMovableTest, MoveAssignmentOperator)
{
	Movable refMovable(Movable::Construction::INITIALIZED);
	Movable movable(Movable::Construction::INITIALIZED);
	const unsigned int newSize = movable.size();
	const int *newData = movable.data();

	hashset_.insert(nctl::move(movable));
	const Movable *m = hashset_.find(refMovable);
	m->printAndAssert();
	printf("Creating a new hashset with the move assignment operator\n");
	nctl::HashSetList<Movable> newHashset(Capacity);
	newHashset = nctl::move(hashset_);
	m = newHashset.find(refMovable);
	m->printAndAssert();

	ASSERT_EQ(m->size(), newSize);
	ASSERT_EQ(m->data(), newData);
}

TEST_F(HashSetListMovableTest, Rehash)
{
	Movable refMovable(Movable::Construction::INITIALIZED);
	Movable movable(Movable::Construction::INITIALIZED);
	hashset_.insert(nctl::move(movable));
	const Movable *m = hashset_.find(refMovable);
	m->printAndAssert();
	const float loadFactor = hashset_.loadFactor();
	printf("Original size: %u, capacity: %u, load factor: %f\n", hashset_.size(), Capacity, hashset_.loadFactor());

	printf("Doubling capacity by rehashing\n");
	hashset_.rehash(Capacity * 2);
	printf("New size: %u, capacity: %u, load factor: %f\n", hashset_.size(), Capacity * 2, hashset_.loadFactor());

	ASSERT_EQ(hashset_.size(), 1);
	ASSERT_FLOAT_EQ(hashset_.loadFactor(), loadFactor * 0.5f);
}

}
