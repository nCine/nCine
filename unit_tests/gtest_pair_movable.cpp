#include "gtest_pair.h"
#include "test_movable.h"

namespace {

using PairMovableType = nctl::Pair<Movable, Movable>;

class PairMovableTest : public ::testing::Test
{
  public:
	PairMovableTest()
	    : pair_(nctl::move(Movable(Movable::Construction::INITIALIZED)),
	            nctl::move(Movable(Movable::Construction::INITIALIZED))) {}

  protected:
	void SetUp() override
	{
		pair_.first = nctl::move(Movable(Movable::Construction::INITIALIZED));
		pair_.second = nctl::move(Movable(Movable::Construction::INITIALIZED));
	}

	PairMovableType pair_;
};

TEST_F(PairMovableTest, Constructor)
{
	printf("Constructing a pair\n");
	PairMovableType pair(Movable::Construction::INITIALIZED,
	                     Movable::Construction::INITIALIZED);

	pair.first.printAndAssert();
	pair.second.printAndAssert();
	ASSERT_EQ(pair_.first.size(), pair.first.size());
	ASSERT_EQ(pair_.second.size(), pair.second.size());
}

TEST_F(PairMovableTest, SwapObjects)
{
	PairMovableType pair(nctl::move(Movable()),
	                     nctl::move(Movable()));
	pair.first.printAndAssert();

	printf("Swapping two pairs\n");
	swap(pair_, pair);

	pair.first.printAndAssert();
	pair.second.printAndAssert();
	ASSERT_EQ(pair.first.size(), Movable::Size);
	ASSERT_EQ(pair.second.size(), Movable::Size);
	ASSERT_EQ(pair_.first.size(), 0);
	ASSERT_EQ(pair_.second.size(), 0);
}

TEST_F(PairMovableTest, EqualityOperator)
{
	PairMovableType pair(Movable::Construction::INITIALIZED,
	                     Movable::Construction::INITIALIZED);
	printf("Comparing two pairs for equality: %d\n", pair_ == pair);

	ASSERT_EQ(pair_, pair);
}

TEST_F(PairMovableTest, InequalityOperator)
{
	PairMovableType pair(nctl::move(Movable()),
	                     nctl::move(Movable()));
	printf("Comparing two pairs for inequality: %d\n", pair_ != pair);

	ASSERT_NE(pair_, pair);
}

}
