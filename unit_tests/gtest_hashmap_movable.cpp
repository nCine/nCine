#include "gtest_hashmap.h"
#include "test_movable.h"

namespace {

class HashMapMovableTest : public ::testing::Test
{
  public:
	HashMapMovableTest() : hashmap_(Capacity) { }

  protected:
	nctl::HashMap<int, Movable, nctl::FixedHashFunc<int> > hashmap_;
};

TEST_F(HashMapMovableTest, SubscriptLValue)
{
	Movable movable(Movable::Construction::INITIALIZED);
	hashmap_[0] = movable;

	hashmap_[0].printAndAssert();
	ASSERT_NE(hashmap_.find(0), nullptr);
	ASSERT_EQ(movable.size(), hashmap_[0].size());
	ASSERT_NE(movable.data(), nullptr);
}

TEST_F(HashMapMovableTest, SubscriptRValue)
{
	Movable movable(Movable::Construction::INITIALIZED);
	hashmap_[0] = nctl::move(movable);

	hashmap_[0].printAndAssert();
	ASSERT_NE(hashmap_.find(0), nullptr);
	ASSERT_EQ(movable.size(), 0);
	ASSERT_EQ(movable.data(), nullptr);
}

}
