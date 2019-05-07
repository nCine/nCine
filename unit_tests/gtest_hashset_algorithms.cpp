#include "gtest_hashset.h"
#include "test_functions.h"

namespace {

class HashSetAlgorithmsTest : public ::testing::Test
{
  public:
	HashSetAlgorithmsTest()
	    : hashset_(Capacity) {}

  protected:
	void SetUp() override { initHashSet(hashset_); }

	HashSetTestType hashset_;
};

TEST_F(HashSetAlgorithmsTest, Minimum)
{
	const int minimum = *nctl::minElement(hashset_.begin(), hashset_.end());
	printf("Minimum element: %d\n", minimum);

	ASSERT_EQ(minimum, 0);
}

TEST_F(HashSetAlgorithmsTest, Maximum)
{
	const int maximum = *nctl::maxElement(hashset_.begin(), hashset_.end());
	printf("Maximum element: %d\n", maximum);

	ASSERT_EQ(maximum, Size - 1);
}

TEST_F(HashSetAlgorithmsTest, AllOfGreater)
{
	const bool allOf = nctl::allOf(hashset_.begin(), hashset_.end(), nctl::IsGreaterThan<int>(-1));
	printf("All bigger than -1: %d\n", allOf);
	ASSERT_EQ(allOf, true);
}

TEST_F(HashSetAlgorithmsTest, NoneOfGreater)
{
	const bool noneOf = nctl::noneOf(hashset_.begin(), hashset_.end(), nctl::IsGreaterThan<int>(5));
	printf("No one bigger than 5: %d\n", noneOf);
	ASSERT_EQ(noneOf, false);
}

TEST_F(HashSetAlgorithmsTest, AnyOfGreater)
{
	const bool anyOf = nctl::anyOf(hashset_.begin(), hashset_.end(), nctl::IsGreaterThan<int>(5));
	printf("Anyone bigger than 5: %d\n", anyOf);
	ASSERT_EQ(anyOf, true);
}

TEST_F(HashSetAlgorithmsTest, CountEqual)
{
	const int counter = nctl::count(hashset_.begin(), hashset_.end(), 6);
	printf("Number of elements equal to 6: %d\n", counter);
	ASSERT_EQ(counter, 1);
}

TEST_F(HashSetAlgorithmsTest, CountElementsGreater)
{
	const int counter = nctl::countIf(hashset_.begin(), hashset_.end(), nctl::IsGreaterThan<int>(4));
	printf("Number of elements bigger than 4: %d\n", counter);
	ASSERT_EQ(counter, 5);
}

TEST_F(HashSetAlgorithmsTest, DistanceToFirstElementEqual)
{
	const int position = nctl::distance(hashset_.begin(), nctl::find(hashset_.begin(), hashset_.end(), 3));
	printf("First element equal to 3 in position: %d\n", position);
	ASSERT_EQ(position, 3);
}

TEST_F(HashSetAlgorithmsTest, DistanceToFirstElementBigger)
{
	const int counter = nctl::distance(hashset_.begin(), nctl::findIf(hashset_.begin(), hashset_.end(), nctl::IsGreaterThan<int>(3)));
	printf("First element bigger than 3 in position: %d\n", counter);
	ASSERT_EQ(counter, 4);
}

TEST_F(HashSetAlgorithmsTest, DistanceToFirstElementNotBigger)
{
	const int counter = nctl::distance(hashset_.begin(), nctl::findIfNot(hashset_.begin(), hashset_.end(), nctl::IsGreaterThan<int>(3)));
	printf("First element not bigger than 3 in position: %d\n", counter);
	ASSERT_EQ(counter, 0);
}

TEST_F(HashSetAlgorithmsTest, CheckEqual)
{
	printf("Copying the hashset to a second one and check they are equal\n");
	HashSetTestType newHashset(Capacity);
	newHashset = hashset_;
	printHashSet(newHashset);

	ASSERT_TRUE(nctl::equal(hashset_.begin(), hashset_.end(), newHashset.begin()));
	assertHashSetsAreEqual(hashset_, newHashset);
}

}
