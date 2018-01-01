#include "gtest_hashmap.h"
#include "test_functions.h"

namespace {

class HashMapAlgorithmsTest : public ::testing::Test
{
  public:
	HashMapAlgorithmsTest() : hashmap_(Capacity) { }

  protected:
	void SetUp() { initHashMap(hashmap_); }

	nc::HashMap<int, int, nc::FixedHashFunc<int> > hashmap_;
};

TEST_F(HashMapAlgorithmsTest, Minimum)
{
	const int minimum = *nc::minElement(hashmap_.begin(), hashmap_.end());
	printf("Minimum element: %d\n", minimum);

	ASSERT_EQ(minimum, KeyValueDifference + 0);
}

TEST_F(HashMapAlgorithmsTest, Maximum)
{
	const int maximum = *nc::maxElement(hashmap_.begin(), hashmap_.end());
	printf("Maximum element: %d\n", maximum);

	ASSERT_EQ(maximum, KeyValueDifference + Size - 1);
}

TEST_F(HashMapAlgorithmsTest, AllOfGreater)
{
	const bool allOf = nc::allOf(hashmap_.begin(), hashmap_.end(), nc::IsGreaterThan<int>(-1));
	printf("All bigger than -1: %d\n", allOf);
	ASSERT_EQ(allOf, true);
}

TEST_F(HashMapAlgorithmsTest, NoneOfGreater)
{
	const bool noneOf = nc::noneOf(hashmap_.begin(), hashmap_.end(), nc::IsGreaterThan<int>(5));
	printf("No one bigger than 5: %d\n", noneOf);
	ASSERT_EQ(noneOf, false);
}

TEST_F(HashMapAlgorithmsTest, AnyOfGreater)
{
	const bool anyOf = nc::anyOf(hashmap_.begin(), hashmap_.end(), nc::IsGreaterThan<int>(5));
	printf("Anyone bigger than 5: %d\n", anyOf);
	ASSERT_EQ(anyOf, true);
}

TEST_F(HashMapAlgorithmsTest, AddValueForEach)
{
	const int value = 10;
	printf("Adding %d to each element of the hashmap\n", value);
	nc::forEach(hashmap_.begin(), hashmap_.end(), addValue<value>);
	printHashMap(hashmap_);

	unsigned int n = 0;
	for (nc::HashMap<int, int, nc::FixedHashFunc<int> >::ConstIterator i = hashmap_.begin(); i != hashmap_.end(); ++i)
	{
		printf("[%u] hash: %lu, key: %d, value: %d\n", n, i.hash(), i.key(), i.value());
		ASSERT_EQ(i.key(), n);
		ASSERT_EQ(*i, KeyValueDifference + n + value);
		n++;
	}
}

TEST_F(HashMapAlgorithmsTest, CountEqual)
{
	const int counter = nc::count(hashmap_.begin(), hashmap_.end(), 16);
	printf("Number of elements equal to 16: %d\n", counter);
	ASSERT_EQ(counter, 1);
}

TEST_F(HashMapAlgorithmsTest, CountElementsGreater)
{
	const int counter = nc::countIf(hashmap_.begin(), hashmap_.end(), nc::IsGreaterThan<int>(14));
	printf("Number of elements bigger than 14: %d\n", counter);
	ASSERT_EQ(counter, 5);
}

TEST_F(HashMapAlgorithmsTest, DistanceToFirstElementEqual)
{
	const int position = nc::distance(hashmap_.begin(), nc::find(hashmap_.begin(), hashmap_.end(), 13));
	printf("First element equal to 13 in position: %d\n", position);
	ASSERT_EQ(position, 3);
}

TEST_F(HashMapAlgorithmsTest, DistanceToFirstElementBigger)
{
	const int counter = nc::distance(hashmap_.begin(), nc::findIf(hashmap_.begin(), hashmap_.end(), nc::IsGreaterThan<int>(13)));
	printf("First element bigger than 13 in position: %d\n", counter);
	ASSERT_EQ(counter, 4);
}

TEST_F(HashMapAlgorithmsTest, DistanceToFirstElementNotBigger)
{
	const int counter = nc::distance(hashmap_.begin(), nc::findIfNot(hashmap_.begin(), hashmap_.end(), nc::IsGreaterThan<int>(13)));
	printf("First element not bigger than 13 in position: %d\n", counter);
	ASSERT_EQ(counter, 0);
}

TEST_F(HashMapAlgorithmsTest, CheckEqual)
{
	printf("Copying the hashmap to a second one and check they are equal\n");
	nc::HashMap<int, int, nc::FixedHashFunc<int> > newHashmap(Capacity);
	newHashmap = hashmap_;
	printHashMap(newHashmap);

	ASSERT_TRUE(nc::equal(hashmap_.begin(), hashmap_.end(), newHashmap.begin()));
	assertHashMapsAreEqual(hashmap_, newHashmap);
}

TEST_F(HashMapAlgorithmsTest, FillNElements)
{
	printf("Filling half hashmap with zeroes\n");
	nc::fillN(hashmap_.begin(), Size / 2, 0);
	printHashMap(hashmap_);

	nc::HashMap<int, int, nc::FixedHashFunc<int> >::ConstIterator halfIt = nc::next(hashmap_.begin(), Size / 2);

	int n = 0;
	for (nc::HashMap<int, int, nc::FixedHashFunc<int> >::ConstIterator i = hashmap_.begin(); i != halfIt; ++i)
	{
		ASSERT_EQ(i.key(), n);
		ASSERT_EQ(*i, 0);
		n++;
	}
	for (nc::HashMap<int, int, nc::FixedHashFunc<int> >::ConstIterator i = halfIt; i != hashmap_.end(); ++i)
	{
		ASSERT_EQ(i.key(), n);
		ASSERT_EQ(*i, KeyValueDifference + n);
		n++;
	}
}

TEST_F(HashMapAlgorithmsTest, FillWithIterators)
{
	printf("Filling the whole hashmap with zeroes\n");
	nc::fill(hashmap_.begin(), hashmap_.end(), 0);
	printHashMap(hashmap_);

	for (nc::HashMap<int, int, nc::FixedHashFunc<int> >::ConstIterator i = hashmap_.begin(); i != hashmap_.end(); ++i)
		ASSERT_EQ(*i, 0);
}

TEST_F(HashMapAlgorithmsTest, ClampElements)
{
	const int minValue = 13;
	const int maxValue = 16;

	printf("Clamping array elements between %d and %d\n", minValue, maxValue);
	nc::clampElements(hashmap_.begin(), hashmap_.end(), minValue, maxValue);
	printHashMap(hashmap_);

	for (nc::HashMap<int, int, nc::FixedHashFunc<int> >::ConstIterator i = hashmap_.begin(); i != hashmap_.end(); ++i)
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
	nc::replace(hashmap_.begin(), hashmap_.end(), oldValue, newValue);
	printHashMap(hashmap_);

	for (nc::HashMap<int, int, nc::FixedHashFunc<int> >::ConstIterator i = hashmap_.begin(); i != hashmap_.end(); ++i)
		ASSERT_TRUE(*i != oldValue);
}

TEST_F(HashMapAlgorithmsTest, ReplaceIf)
{
	const int refValue = 15;
	const int newValue = 55;

	printf("Replacing all elements bigger than %d with %d\n", refValue, newValue);
	nc::replaceIf(hashmap_.begin(), hashmap_.end(), nc::IsEqualTo<int>(refValue), newValue);
	printHashMap(hashmap_);

	for (nc::HashMap<int, int, nc::FixedHashFunc<int> >::ConstIterator i = hashmap_.begin(); i != hashmap_.end(); ++i)
		ASSERT_TRUE(*i != refValue);
}

TEST_F(HashMapAlgorithmsTest, Generate)
{
	const int value = 1;
	printf("Generating a sequence starting at %d and store it into the hashmap\n", value);
	nc::generate(hashmap_.begin(), hashmap_.end(), GenerateSequenceFrom(value));
	printHashMap(hashmap_);

	int n = 0;
	for (nc::HashMap<int, int, nc::FixedHashFunc<int> >::ConstIterator i = hashmap_.begin(); i != hashmap_.end(); ++i)
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
	nc::generateN(hashmap_.begin(), numElements, GenerateSequenceFrom(value));
	printHashMap(hashmap_);

	nc::HashMap<int, int, nc::FixedHashFunc<int> >::ConstIterator numElementsIt = nc::next(hashmap_.begin(), numElements);
	int n = 0;
	for (nc::HashMap<int, int, nc::FixedHashFunc<int> >::ConstIterator i = hashmap_.begin(); i != numElementsIt; ++i)
	{
		ASSERT_EQ(i.key(), n);
		ASSERT_EQ(*i, n + value);
		n++;
	}
	for (nc::HashMap<int, int, nc::FixedHashFunc<int> >::ConstIterator i = numElementsIt; i != hashmap_.end(); ++i)
	{
		ASSERT_EQ(i.key(), n);
		ASSERT_EQ(*i, n + KeyValueDifference);
		n++;
	}
}

}
