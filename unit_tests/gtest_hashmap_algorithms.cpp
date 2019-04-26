#include "gtest_hashmap.h"
#include "test_functions.h"

namespace {

class HashMapAlgorithmsTest : public ::testing::Test
{
  public:
	HashMapAlgorithmsTest()
	    : hashmap_(Capacity) {}

  protected:
	void SetUp() override { initHashMap(hashmap_); }

	HashMapTestType hashmap_;
};

TEST_F(HashMapAlgorithmsTest, Minimum)
{
	const int minimum = *nctl::minElement(hashmap_.begin(), hashmap_.end());
	printf("Minimum element: %d\n", minimum);

	ASSERT_EQ(minimum, KeyValueDifference + 0);
}

TEST_F(HashMapAlgorithmsTest, Maximum)
{
	const int maximum = *nctl::maxElement(hashmap_.begin(), hashmap_.end());
	printf("Maximum element: %d\n", maximum);

	ASSERT_EQ(maximum, KeyValueDifference + Size - 1);
}

TEST_F(HashMapAlgorithmsTest, AllOfGreater)
{
	const bool allOf = nctl::allOf(hashmap_.begin(), hashmap_.end(), nctl::IsGreaterThan<int>(-1));
	printf("All bigger than -1: %d\n", allOf);
	ASSERT_EQ(allOf, true);
}

TEST_F(HashMapAlgorithmsTest, NoneOfGreater)
{
	const bool noneOf = nctl::noneOf(hashmap_.begin(), hashmap_.end(), nctl::IsGreaterThan<int>(5));
	printf("No one bigger than 5: %d\n", noneOf);
	ASSERT_EQ(noneOf, false);
}

TEST_F(HashMapAlgorithmsTest, AnyOfGreater)
{
	const bool anyOf = nctl::anyOf(hashmap_.begin(), hashmap_.end(), nctl::IsGreaterThan<int>(5));
	printf("Anyone bigger than 5: %d\n", anyOf);
	ASSERT_EQ(anyOf, true);
}

TEST_F(HashMapAlgorithmsTest, AddValueForEach)
{
	const int value = 10;
	printf("Adding %d to each element of the hashmap\n", value);
	nctl::forEach(hashmap_.begin(), hashmap_.end(), addValue<value>);
	printHashMap(hashmap_);

	unsigned int n = 0;
	for (HashMapTestType::ConstIterator i = hashmap_.begin(); i != hashmap_.end(); ++i)
	{
		printf("[%u] hash: %u, key: %d, value: %d\n", n, i.hash(), i.key(), i.value());
		ASSERT_EQ(i.key(), n);
		ASSERT_EQ(*i, KeyValueDifference + n + value);
		n++;
	}
}

TEST_F(HashMapAlgorithmsTest, CountEqual)
{
	const int counter = nctl::count(hashmap_.begin(), hashmap_.end(), 16);
	printf("Number of elements equal to 16: %d\n", counter);
	ASSERT_EQ(counter, 1);
}

TEST_F(HashMapAlgorithmsTest, CountElementsGreater)
{
	const int counter = nctl::countIf(hashmap_.begin(), hashmap_.end(), nctl::IsGreaterThan<int>(14));
	printf("Number of elements bigger than 14: %d\n", counter);
	ASSERT_EQ(counter, 5);
}

TEST_F(HashMapAlgorithmsTest, DistanceToFirstElementEqual)
{
	const int position = nctl::distance(hashmap_.begin(), nctl::find(hashmap_.begin(), hashmap_.end(), 13));
	printf("First element equal to 13 in position: %d\n", position);
	ASSERT_EQ(position, 3);
}

TEST_F(HashMapAlgorithmsTest, DistanceToFirstElementBigger)
{
	const int counter = nctl::distance(hashmap_.begin(), nctl::findIf(hashmap_.begin(), hashmap_.end(), nctl::IsGreaterThan<int>(13)));
	printf("First element bigger than 13 in position: %d\n", counter);
	ASSERT_EQ(counter, 4);
}

TEST_F(HashMapAlgorithmsTest, DistanceToFirstElementNotBigger)
{
	const int counter = nctl::distance(hashmap_.begin(), nctl::findIfNot(hashmap_.begin(), hashmap_.end(), nctl::IsGreaterThan<int>(13)));
	printf("First element not bigger than 13 in position: %d\n", counter);
	ASSERT_EQ(counter, 0);
}

TEST_F(HashMapAlgorithmsTest, CheckEqual)
{
	printf("Copying the hashmap to a second one and check they are equal\n");
	HashMapTestType newHashmap(Capacity);
	newHashmap = hashmap_;
	printHashMap(newHashmap);

	ASSERT_TRUE(nctl::equal(hashmap_.begin(), hashmap_.end(), newHashmap.begin()));
	assertHashMapsAreEqual(hashmap_, newHashmap);
}

TEST_F(HashMapAlgorithmsTest, FillNElements)
{
	printf("Filling half hashmap with zeroes\n");
	nctl::fillN(hashmap_.begin(), Size / 2, 0);
	printHashMap(hashmap_);

	HashMapTestType::ConstIterator halfIt = nctl::next(hashmap_.begin(), Size / 2);

	int n = 0;
	for (HashMapTestType::ConstIterator i = hashmap_.begin(); i != halfIt; ++i)
	{
		ASSERT_EQ(i.key(), n);
		ASSERT_EQ(*i, 0);
		n++;
	}
	for (HashMapTestType::ConstIterator i = halfIt; i != hashmap_.end(); ++i)
	{
		ASSERT_EQ(i.key(), n);
		ASSERT_EQ(*i, KeyValueDifference + n);
		n++;
	}
}

TEST_F(HashMapAlgorithmsTest, FillWithIterators)
{
	printf("Filling the whole hashmap with zeroes\n");
	nctl::fill(hashmap_.begin(), hashmap_.end(), 0);
	printHashMap(hashmap_);

	for (HashMapTestType::ConstIterator i = hashmap_.begin(); i != hashmap_.end(); ++i)
		ASSERT_EQ(*i, 0);
}

TEST_F(HashMapAlgorithmsTest, ClampElements)
{
	const int minValue = 13;
	const int maxValue = 16;

	printf("Clamping array elements between %d and %d\n", minValue, maxValue);
	nctl::clampElements(hashmap_.begin(), hashmap_.end(), minValue, maxValue);
	printHashMap(hashmap_);

	for (HashMapTestType::ConstIterator i = hashmap_.begin(); i != hashmap_.end(); ++i)
	{
		ASSERT_TRUE(*i >= minValue);
		ASSERT_TRUE(*i <= maxValue);
	}
}

TEST_F(HashMapAlgorithmsTest, Replace)
{
	const int oldValue = 15;
	const int newValue = 55;

	printf("Replacing all elements equal to %d with %d\n", oldValue, newValue);
	nctl::replace(hashmap_.begin(), hashmap_.end(), oldValue, newValue);
	printHashMap(hashmap_);

	for (HashMapTestType::ConstIterator i = hashmap_.begin(); i != hashmap_.end(); ++i)
		ASSERT_TRUE(*i != oldValue);
}

TEST_F(HashMapAlgorithmsTest, ReplaceIf)
{
	const int refValue = 15;
	const int newValue = 55;

	printf("Replacing all elements bigger than %d with %d\n", refValue, newValue);
	nctl::replaceIf(hashmap_.begin(), hashmap_.end(), nctl::IsEqualTo<int>(refValue), newValue);
	printHashMap(hashmap_);

	for (HashMapTestType::ConstIterator i = hashmap_.begin(); i != hashmap_.end(); ++i)
		ASSERT_TRUE(*i != refValue);
}

TEST_F(HashMapAlgorithmsTest, Generate)
{
	const int value = 1;
	printf("Generating a sequence starting at %d and store it into the hashmap\n", value);
	nctl::generate(hashmap_.begin(), hashmap_.end(), GenerateSequenceFrom(value));
	printHashMap(hashmap_);

	int n = 0;
	for (HashMapTestType::ConstIterator i = hashmap_.begin(); i != hashmap_.end(); ++i)
	{
		ASSERT_EQ(i.key(), n);
		ASSERT_EQ(*i, n + value);
		n++;
	}
}

TEST_F(HashMapAlgorithmsTest, GenerateN)
{
	const int value = -4;
	const int numElements = 5;
	printf("Generating a sequence of %d values starting at %d and store it into the hashmap\n", numElements, value);
	nctl::generateN(hashmap_.begin(), numElements, GenerateSequenceFrom(value));
	printHashMap(hashmap_);

	HashMapTestType::ConstIterator numElementsIt = nctl::next(hashmap_.begin(), numElements);
	int n = 0;
	for (HashMapTestType::ConstIterator i = hashmap_.begin(); i != numElementsIt; ++i)
	{
		ASSERT_EQ(i.key(), n);
		ASSERT_EQ(*i, n + value);
		n++;
	}
	for (HashMapTestType::ConstIterator i = numElementsIt; i != hashmap_.end(); ++i)
	{
		ASSERT_EQ(i.key(), n);
		ASSERT_EQ(*i, n + KeyValueDifference);
		n++;
	}
}

}
