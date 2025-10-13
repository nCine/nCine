#include "gtest_carray.h"
#include "test_functions.h"
#include <nctl/algorithms.h>

namespace {

class CArrayAlgorithmsTest : public ::testing::Test
{
  protected:
	void SetUp() override { initArray(array_); }

	int array_[Capacity];
};

TEST_F(CArrayAlgorithmsTest, Minimum)
{
	const int minimum = *nctl::minElement(nctl::cBegin(array_), nctl::cEnd(array_));
	printf("Minimum element: %d\n", minimum);

	ASSERT_EQ(minimum, 0);
}

TEST_F(CArrayAlgorithmsTest, Maximum)
{
	const int maximum = *nctl::maxElement(nctl::cBegin(array_), nctl::cEnd(array_));
	printf("Maximum element: %d\n", maximum);

	ASSERT_EQ(maximum, Capacity - 1);
}

TEST_F(CArrayAlgorithmsTest, AllOfGreater)
{
	const bool allOf = nctl::allOf(nctl::cBegin(array_), nctl::cEnd(array_), nctl::IsGreaterThan<int>(-1));
	printf("All bigger than -1: %d\n", allOf);
	ASSERT_EQ(allOf, true);
}

TEST_F(CArrayAlgorithmsTest, NoneOfGreater)
{
	const bool noneOf = nctl::noneOf(nctl::cBegin(array_), nctl::cEnd(array_), nctl::IsGreaterThan<int>(5));
	printf("No one bigger than 5: %d\n", noneOf);
	ASSERT_EQ(noneOf, false);
}

TEST_F(CArrayAlgorithmsTest, AnyOfGreater)
{
	const bool anyOf = nctl::anyOf(nctl::cBegin(array_), nctl::cEnd(array_), nctl::IsGreaterThan<int>(5));
	printf("Anyone bigger than 5: %d\n", anyOf);
	ASSERT_EQ(anyOf, true);
}

TEST_F(CArrayAlgorithmsTest, AddValueForEach)
{
	const int value = 10;
	printf("Adding %d to each element of the array\n", value);
	nctl::forEach(nctl::begin(array_), nctl::end(array_), addValue<value>);
	printArray(array_);

	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_EQ(array_[i], i + value);
}

TEST_F(CArrayAlgorithmsTest, CountEqual)
{
	const int counter = nctl::count(nctl::cBegin(array_), nctl::cEnd(array_), 6);
	printf("Number of elements equal to 6: %d\n", counter);
	ASSERT_EQ(counter, 1);
}

TEST_F(CArrayAlgorithmsTest, CountElementsGreater)
{
	const int counter = nctl::countIf(nctl::cBegin(array_), nctl::cEnd(array_), nctl::IsGreaterThan<int>(4));
	printf("Number of elements bigger than 4: %d\n", counter);
	ASSERT_EQ(counter, 5);
}

TEST_F(CArrayAlgorithmsTest, DistanceToFirstElementEqual)
{
	const int position = nctl::distance(nctl::cBegin(array_), nctl::find(nctl::cBegin(array_), nctl::cEnd(array_), 3));
	printf("First element equal to 3 in position: %d\n", position);
	ASSERT_EQ(position, 3);
}

TEST_F(CArrayAlgorithmsTest, DistanceToFirstElementBigger)
{
	const int counter = nctl::distance(nctl::cBegin(array_), nctl::findIf(nctl::cBegin(array_), nctl::cEnd(array_), nctl::IsGreaterThan<int>(3)));
	printf("First element bigger than 3 in position: %d\n", counter);
	ASSERT_EQ(counter, 4);
}

TEST_F(CArrayAlgorithmsTest, DistanceToFirstElementNotBigger)
{
	const int counter = nctl::distance(nctl::cBegin(array_), nctl::findIfNot(nctl::cBegin(array_), nctl::cEnd(array_), nctl::IsGreaterThan<int>(3)));
	printf("First element not bigger than 3 in position: %d\n", counter);
	ASSERT_EQ(counter, 0);
}

TEST_F(CArrayAlgorithmsTest, CheckEqual)
{
	printf("Copying the array to a second one and check they are equal\n");
	int newArray[Capacity];
	for (unsigned int i = 0; i < Capacity; i++)
		newArray[i] = array_[i];
	printArray(newArray);

	ASSERT_TRUE(nctl::equal(nctl::cBegin(array_), nctl::cEnd(array_), nctl::cBegin(newArray)));
	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_EQ(array_[i], newArray[i]);
}

TEST_F(CArrayAlgorithmsTest, FillNElements)
{
	printf("Filling half array with zeroes\n");
	nctl::fillN(nctl::begin(array_), Capacity / 2, 0);
	printArray(array_);

	const unsigned int halfSize = Capacity / 2;
	for (unsigned int i = 0; i < halfSize; i++)
		ASSERT_EQ(array_[i], 0);
	for (unsigned int i = halfSize + 1; i < Capacity; i++)
		ASSERT_EQ(array_[i], i);
}

TEST_F(CArrayAlgorithmsTest, FillWithIterators)
{
	printf("Filling the whole array with zeroes\n");
	nctl::fill(nctl::begin(array_), nctl::end(array_), 0);
	printArray(array_);

	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_EQ(array_[i], 0);
}

TEST_F(CArrayAlgorithmsTest, CopyToArray)
{
	printf("Copying the array to a new one with iterators\n");
	int newArray[Capacity];
	nctl::copy(nctl::cBegin(array_), nctl::cEnd(array_), nctl::begin(newArray));
	printArray(newArray);

	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_EQ(array_[i], newArray[i]);
}

TEST_F(CArrayAlgorithmsTest, CopyNToArray)
{
	printf("Copying half the array to a new one with iterators\n");
	int newArray[Capacity];
	nctl::copyN(nctl::cBegin(array_), Capacity / 2, nctl::begin(newArray));
	printArray(newArray);

	for (unsigned int i = 0; i < Capacity / 2; i++)
		ASSERT_EQ(array_[i], newArray[i]);
}

TEST_F(CArrayAlgorithmsTest, CopyIfToArray)
{
	printf("Copying all elements smaller than 5 to a new array\n");
	int newArray[Capacity];
	nctl::copyIf(nctl::cBegin(array_), nctl::cEnd(array_), nctl::begin(newArray), nctl::IsLessThan<int>(5));
	printArray(newArray);

	for (unsigned int i = 0; i < Capacity / 2; i++)
		ASSERT_EQ(array_[i], newArray[i]);
}

TEST_F(CArrayAlgorithmsTest, CopyTransformed)
{
	printf("Filling a new array with random numbers\n");
	int newArray[Capacity];
	initArrayRandom(newArray);
	printArray(newArray);

	printf("Storing transformed random numbers back into the first array\n");
	nctl::transform(nctl::cBegin(newArray), nctl::cEnd(newArray), nctl::begin(array_), copyAddValue<10>);
	printArray(array_);

	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_EQ(array_[i], newArray[i] + 10);
}

TEST_F(CArrayAlgorithmsTest, CopyBinaryTransformed)
{
	printf("Filling a second array with random numbers\n");
	int secondArray[Capacity];
	initArrayRandom(secondArray);
	printArray(secondArray);

	printf("Creating a third array to store transformed elements\n");
	int thirdArray[Capacity];

	printf("Storing the sum of the first two arrays into the third one\n");
	nctl::transform(nctl::cBegin(array_), nctl::cEnd(array_), nctl::cBegin(secondArray), nctl::begin(thirdArray), nctl::Plus<int>);
	printArray(thirdArray);

	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_EQ(thirdArray[i], array_[i] + secondArray[i]);
}

TEST_F(CArrayAlgorithmsTest, ClampElements)
{
	const int minValue = 3;
	const int maxValue = 6;

	printf("Clamping array elements between %d and %d\n", minValue, maxValue);
	nctl::clampElements(nctl::begin(array_), nctl::end(array_), minValue, maxValue);
	printArray(array_);

	for (unsigned int i = 0; i < Capacity; i++)
	{
		ASSERT_TRUE(array_[i] >= minValue);
		ASSERT_TRUE(array_[i] <= maxValue);
	}
}

TEST_F(CArrayAlgorithmsTest, Replace)
{
	const int oldValue = 5;
	const int newValue = 55;

	printf("Replacing all elements equal to %d with %d\n", oldValue, newValue);
	nctl::replace(nctl::begin(array_), nctl::end(array_), oldValue, newValue);
	printArray(array_);

	ASSERT_EQ(array_[oldValue], newValue);
}

TEST_F(CArrayAlgorithmsTest, ReplaceIf)
{
	const int refValue = 5;
	const int newValue = 55;

	printf("Replacing all elements bigger than %d with %d\n", refValue, newValue);
	nctl::replaceIf(nctl::begin(array_), nctl::end(array_), nctl::IsEqualTo<int>(refValue), newValue);
	printArray(array_);
	ASSERT_EQ(array_[refValue], newValue);
}

TEST_F(CArrayAlgorithmsTest, ReplaceCopy)
{
	const int oldValue = 5;
	const int newValue = 55;

	printf("Creating a new array\n");
	int newArray[Capacity];

	printf("Replacing all elements equal to %d with %d when copying to the new array\n", oldValue, newValue);
	nctl::replaceCopy(nctl::cBegin(array_), nctl::cEnd(array_), nctl::begin(newArray), oldValue, newValue);
	printArray(newArray);

	ASSERT_EQ(array_[oldValue], oldValue);
	ASSERT_EQ(newArray[oldValue], newValue);
}

TEST_F(CArrayAlgorithmsTest, ReplaceCopyIf)
{
	const int refValue = 5;
	const int newValue = 55;

	printf("Creating a new array\n");
	int newArray[Capacity];

	printf("Replacing all elements bigger than %d with %d when copying to the new array\n", refValue, newValue);
	nctl::replaceCopyIf(nctl::cBegin(array_), nctl::cEnd(array_), nctl::begin(newArray), nctl::IsEqualTo<int>(refValue), newValue);
	printArray(newArray);

	ASSERT_EQ(array_[refValue], refValue);
	ASSERT_EQ(newArray[refValue], newValue);
}

TEST_F(CArrayAlgorithmsTest, Generate)
{
	const int value = 1;
	printf("Generating a sequence starting at %d and store it into the array\n", value);
	nctl::generate(nctl::begin(array_), nctl::end(array_), GenerateSequenceFrom(value));
	printArray(array_);

	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_EQ(array_[i], i + value);
}

TEST_F(CArrayAlgorithmsTest, GenerateN)
{
	const int value = -4;
	const int numElements = 5;
	printf("Generating a sequence of %d values starting at %d and store it into the array\n", numElements, value);
	nctl::generateN(nctl::begin(array_), numElements, GenerateSequenceFrom(value));
	printArray(array_);

	for (unsigned int i = 0; i < numElements; i++)
		ASSERT_EQ(array_[i], i + value);
	for (unsigned int i = numElements; i < Capacity; i++)
		ASSERT_EQ(array_[i], i);
}

TEST_F(CArrayAlgorithmsTest, Reverse)
{
	printf("Reversing the array\n");
	nctl::reverse(nctl::begin(array_), nctl::end(array_));
	printArray(array_);

	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_EQ(array_[i], Capacity - 1 - i);
}

TEST_F(CArrayAlgorithmsTest, ReverseCopy)
{
	int newArray[Capacity];

	printf("Reverse copying the array into a new one\n");
	nctl::reverseCopy(nctl::cBegin(array_), nctl::cEnd(array_), nctl::begin(newArray));
	printArray(newArray);

	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_EQ(newArray[i], Capacity - 1 - i);
}

}
