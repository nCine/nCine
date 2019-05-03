#include "gtest_statichashmap.h"
#include "test_movable.h"

namespace {

class StaticHashMapMovableTest : public ::testing::Test
{
  protected:
	nctl::StaticHashMap<int, Movable, Capacity, nctl::FixedHashFunc<int>> hashmap_;
};

#if !TEST_MOVABLE_ONLY
TEST_F(StaticHashMapMovableTest, SubscriptLValue)
{
	Movable movable(Movable::Construction::INITIALIZED);

	ASSERT_EQ(hashmap_.find(0), nullptr);
	hashmap_[0] = movable;
	hashmap_[0].printAndAssert();

	ASSERT_NE(hashmap_.find(0), nullptr);
	ASSERT_EQ(movable.size(), hashmap_[0].size());
	ASSERT_NE(movable.data(), nullptr);
}
#endif

TEST_F(StaticHashMapMovableTest, SubscriptRValue)
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

#if !TEST_MOVABLE_ONLY
TEST_F(StaticHashMapMovableTest, InsertLValue)
{
	Movable movable(Movable::Construction::INITIALIZED);

	ASSERT_EQ(hashmap_.find(0), nullptr);
	hashmap_.insert(0, movable);
	hashmap_[0].printAndAssert();

	ASSERT_NE(hashmap_.find(0), nullptr);
	ASSERT_EQ(movable.size(), hashmap_[0].size());
	ASSERT_NE(movable.data(), nullptr);
}
#endif

TEST_F(StaticHashMapMovableTest, InsertRValue)
{
	Movable movable(Movable::Construction::INITIALIZED);
	const unsigned int newSize = movable.size();
	const int *newData = movable.data();

	ASSERT_EQ(hashmap_.find(0), nullptr);
	hashmap_.insert(0, nctl::move(movable));
	hashmap_[0].printAndAssert();

	ASSERT_NE(hashmap_.find(0), nullptr);
	ASSERT_EQ(hashmap_[0].size(), newSize);
	ASSERT_EQ(hashmap_[0].data(), newData);
	ASSERT_EQ(movable.size(), 0);
	ASSERT_EQ(movable.data(), nullptr);
}

TEST_F(StaticHashMapMovableTest, Emplace)
{
	ASSERT_EQ(hashmap_.find(0), nullptr);
	hashmap_.emplace(0, Movable::Construction::INITIALIZED);
	hashmap_[0].printAndAssert();

	ASSERT_NE(hashmap_.find(0), nullptr);
}

TEST_F(StaticHashMapMovableTest, MoveConstruction)
{
	Movable movable(Movable::Construction::INITIALIZED);
	const unsigned int newSize = movable.size();
	const int *newData = movable.data();

	hashmap_[0] = nctl::move(movable);
	hashmap_[0].printAndAssert();
	printf("Creating a new hashmap with move construction\n");
	nctl::StaticHashMap<int, Movable, Capacity, nctl::FixedHashFunc<int>> newHashmap(nctl::move(hashmap_));
	newHashmap[0].printAndAssert();

	ASSERT_EQ(newHashmap[0].size(), newSize);
	ASSERT_EQ(newHashmap[0].data(), newData);
}

TEST_F(StaticHashMapMovableTest, MoveAssignmentOperator)
{
	Movable movable(Movable::Construction::INITIALIZED);
	const unsigned int newSize = movable.size();
	const int *newData = movable.data();

	hashmap_[0] = nctl::move(movable);
	hashmap_[0].printAndAssert();
	printf("Creating a new hashmap with the move assignment operator\n");
	nctl::StaticHashMap<int, Movable, Capacity, nctl::FixedHashFunc<int>> newHashmap;
	newHashmap = nctl::move(hashmap_);
	newHashmap[0].printAndAssert();

	ASSERT_EQ(newHashmap[0].size(), newSize);
	ASSERT_EQ(newHashmap[0].data(), newData);
}

}
