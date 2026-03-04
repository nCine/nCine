#include "gtest_pair.h"

namespace {

class PairTest : public ::testing::Test
{
  public:
	PairTest()
	    : pair_(FirstElement, SecondElement) {}

  protected:
	void SetUp() override
	{
		pair_.first = FirstElement;
		pair_.second = SecondElement;
	}

	PairType pair_;
};

TEST_F(PairTest, Constructor)
{
	printf("Constructing a pair\n");
	PairType pair(FirstElement, SecondElement);
	printPair(pair);

	ASSERT_EQ(pair.first, FirstElement);
	ASSERT_EQ(pair.second, SecondElement);
}

TEST_F(PairTest, ConstructorFromDifferentTypes)
{
	printf("Constructing a pair from different types\n");
	PairType pair(static_cast<float>(FirstElement), static_cast<float>(SecondElement));
	printPair(pair);

	ASSERT_EQ(pair.first, FirstElement);
	ASSERT_EQ(pair.second, SecondElement);
}

TEST_F(PairTest, SwapObjects)
{
	PairType pair(FirstElement + 10, SecondElement + 10);
	printPair(pair_);
	printPair(pair);

	printf("Swapping two pairs\n");
	swap(pair_, pair);
	printPair(pair_);
	printPair(pair);

	ASSERT_EQ(pair_.first, FirstElement + 10);
	ASSERT_EQ(pair_.second, SecondElement + 10);
	ASSERT_EQ(pair.first, FirstElement);
	ASSERT_EQ(pair.second, SecondElement);
}

TEST_F(PairTest, EqualityOperator)
{
	PairType pair(FirstElement, SecondElement);
	printf("Comparing two pairs for equality: %d\n", pair_ == pair);
	printPair(pair_);
	printPair(pair);

	ASSERT_EQ(pair_, pair);
}

TEST_F(PairTest, InequalityOperator)
{
	PairType pair(FirstElement + 10, SecondElement + 10);
	printf("Comparing two pairs for inequality: %d\n", pair_ != pair);
	printPair(pair_);
	printPair(pair);

	ASSERT_NE(pair_, pair);
}

TEST_F(PairTest, LessThanOperator)
{
	PairType pair(FirstElement + 10, SecondElement + 10);
	printf("Comparing two pairs for less than: %d\n", pair_ < pair);
	printPair(pair_);
	printPair(pair);

	ASSERT_LT(pair_, pair);
}

TEST_F(PairTest, LessThanEqualOperator)
{
	PairType pair(FirstElement + 10, SecondElement + 10);
	printf("Comparing two pairs for less than or equal: %d\n", pair_ <= pair);
	printPair(pair_);
	printPair(pair);

	ASSERT_LE(pair_, pair);
}

TEST_F(PairTest, GreaterThanOperator)
{
	PairType pair(FirstElement + 10, SecondElement + 10);
	printf("Comparing two pairs for greater than: %d\n", pair_ > pair);
	printPair(pair_);
	printPair(pair);

	ASSERT_GT(pair, pair_);
}

TEST_F(PairTest, GreaterThanEqualOperator)
{
	nctl::Pair pair(FirstElement + 10, SecondElement + 10);
	printf("Comparing two pairs for greater than or equal: %d\n", pair_ >= pair);
	printPair(pair_);
	printPair(pair);

	ASSERT_GE(pair, pair_);
}

TEST_F(PairTest, MakePairScalarDecay)
{
	const int first = FirstElement;
	const int second = SecondElement;
	const int &secondRef = second;

	printf("Making a pair with makePair\n");
	PairType pair = nctl::makePair(first, secondRef);
	printPair(pair);

	ASSERT_EQ(pair.first, FirstElement);
	ASSERT_EQ(pair.second, SecondElement);
}

TEST_F(PairTest, MakePairArrayDecay)
{
	int array[4] = { FirstElement, 1, 2, 3 };
	const int second = SecondElement;

	printf("Making a pair with makePair\n");
	nctl::Pair pair = nctl::makePair(array, second);

	ASSERT_EQ(pair.first, array);
	ASSERT_EQ(*pair.first, FirstElement);
	ASSERT_EQ(pair.second, SecondElement);
}

}
