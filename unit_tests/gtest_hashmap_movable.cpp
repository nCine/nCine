#include "gtest_hashmap.h"
#include "test_movable.h"

namespace {

class HashMapMovableTest : public ::testing::Test
{
  public:
	HashMapMovableTest()
	    : hashmap_(Capacity) {}

  protected:
	nctl::HashMap<int, Movable, nctl::FixedHashFunc<int>> hashmap_;
};

TEST_F(HashMapMovableTest, SubscriptLValue)
{
	Movable movable(Movable::Construction::INITIALIZED);

	ASSERT_EQ(hashmap_.find(0), nullptr);
	hashmap_[0] = movable;
	hashmap_[0].printAndAssert();

	ASSERT_NE(hashmap_.find(0), nullptr);
	ASSERT_EQ(movable.size(), hashmap_[0].size());
	ASSERT_NE(movable.data(), nullptr);
}

TEST_F(HashMapMovableTest, SubscriptRValue)
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

TEST_F(HashMapMovableTest, MoveConstruction)
{
	Movable movable(Movable::Construction::INITIALIZED);
	const unsigned int newSize = movable.size();
	const int *newData = movable.data();

	hashmap_[0] = nctl::move(movable);
	hashmap_[0].printAndAssert();
	printf("Creating a new hashmap with move construction\n");
	nctl::HashMap<int, Movable, nctl::FixedHashFunc<int>> newHashmap(nctl::move(hashmap_));
	newHashmap[0].printAndAssert();

	ASSERT_EQ(newHashmap[0].size(), newSize);
	ASSERT_EQ(newHashmap[0].data(), newData);
}

TEST_F(HashMapMovableTest, MoveAssignmentOperator)
{
	Movable movable(Movable::Construction::INITIALIZED);
	const unsigned int newSize = movable.size();
	const int *newData = movable.data();

	hashmap_[0] = nctl::move(movable);
	hashmap_[0].printAndAssert();
	printf("Creating a new hashmap with the move assignment operator\n");
	nctl::HashMap<int, Movable, nctl::FixedHashFunc<int>> newHashmap(Capacity);
	newHashmap = nctl::move(hashmap_);
	newHashmap[0].printAndAssert();

	ASSERT_EQ(newHashmap[0].size(), newSize);
	ASSERT_EQ(newHashmap[0].data(), newData);
}

}
