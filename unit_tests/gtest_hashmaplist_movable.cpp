#include "gtest_hashmaplist.h"
#include "test_movable.h"

namespace {

class HashMapListMovableTest : public ::testing::Test
{
  public:
	HashMapListMovableTest() : hashmap_(Capacity) { }

  protected:
	nctl::HashMapList<int, Movable, nctl::FixedHashFunc<int> > hashmap_;
};

TEST_F(HashMapListMovableTest, SubscriptLValue)
{
	Movable movable(Movable::Construction::INITIALIZED);

	ASSERT_EQ(hashmap_.find(0), nullptr);
	hashmap_[0] = movable;
	hashmap_[0].printAndAssert();

	ASSERT_NE(hashmap_.find(0), nullptr);
	ASSERT_EQ(movable.size(), hashmap_[0].size());
	ASSERT_NE(movable.data(), nullptr);
}

TEST_F(HashMapListMovableTest, SubscriptRValue)
{
	Movable movable(Movable::Construction::INITIALIZED);
	const unsigned int newSize = movable.size();
	const int *newData = movable.data();

	ASSERT_EQ(hashmap_.find(0), nullptr);
	hashmap_[0] = nctl::move(movable);
	hashmap_[0].printAndAssert();

	ASSERT_NE(hashmap_.find(0), nullptr);
	ASSERT_EQ(hashmap_[0].size(), newSize);
	ASSERT_EQ(hashmap_[0].data(), newData);
	ASSERT_EQ(movable.size(), 0);
	ASSERT_EQ(movable.data(), nullptr);
}

TEST_F(HashMapListMovableTest, MoveConstruction)
{
	Movable movable(Movable::Construction::INITIALIZED);
	const unsigned int newSize = movable.size();
	const int *newData = movable.data();

	hashmap_[0] = nctl::move(movable);
	hashmap_[0].printAndAssert();
	printf("Creating a new hashmap with move construction\n");
	nctl::HashMapList<int, Movable, nctl::FixedHashFunc<int> > newHashmap(nctl::move(hashmap_));
	newHashmap[0].printAndAssert();

	ASSERT_EQ(newHashmap[0].size(), newSize);
	ASSERT_EQ(newHashmap[0].data(), newData);
}

TEST_F(HashMapListMovableTest, MoveAssignmentOperator)
{
	Movable movable(Movable::Construction::INITIALIZED);
	const unsigned int newSize = movable.size();
	const int *newData = movable.data();

	hashmap_[0] = nctl::move(movable);
	hashmap_[0].printAndAssert();
	printf("Creating a new hashmap with the move assignment operator\n");
	nctl::HashMapList<int, Movable, nctl::FixedHashFunc<int> > newHashmap(Capacity);
	newHashmap = nctl::move(hashmap_);
	newHashmap[0].printAndAssert();

	ASSERT_EQ(newHashmap[0].size(), newSize);
	ASSERT_EQ(newHashmap[0].data(), newData);
}

}
