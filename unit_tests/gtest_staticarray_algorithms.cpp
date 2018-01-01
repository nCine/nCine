#include "gtest_staticarray.h"
#include "test_functions.h"
#include "algorithms.h"

namespace {

class StaticArrayAlgorithmsTest : public ::testing::Test
{
  protected:
	void SetUp() { initArray(array_); }

	nc::StaticArray<int, Capacity> array_;
};

TEST_F(StaticArrayAlgorithmsTest, Minimum)
{
	const int minimum = *nc::minElement(array_.begin(), array_.end());
	printf("Minimum element: %d\n", minimum);

	ASSERT_EQ(minimum, 0);
}

TEST_F(StaticArrayAlgorithmsTest, Maximum)
{
	const int maximum = *nc::maxElement(array_.begin(), array_.end());
	printf("Maximum element: %d\n", maximum);

	ASSERT_EQ(maximum, Capacity - 1);
}

TEST_F(StaticArrayAlgorithmsTest, AllOfGreater)
{
	const bool allOf = nc::allOf(array_.begin(), array_.end(), nc::IsGreaterThan<int>(-1));
	printf("All bigger than -1: %d\n", allOf);
	ASSERT_EQ(allOf, true);
}

TEST_F(StaticArrayAlgorithmsTest, NoneOfGreater)
{
	const bool noneOf = nc::noneOf(array_.begin(), array_.end(), nc::IsGreaterThan<int>(5));
	printf("No one bigger than 5: %d\n", noneOf);
	ASSERT_EQ(noneOf, false);
}

TEST_F(StaticArrayAlgorithmsTest, AnyOfGreater)
{
	const bool anyOf = nc::anyOf(array_.begin(), array_.end(), nc::IsGreaterThan<int>(5));
	printf("Anyone bigger than 5: %d\n", anyOf);
	ASSERT_EQ(anyOf, true);
}

TEST_F(StaticArrayAlgorithmsTest, AddValueForEach)
{
	const int value = 10;
	printf("Adding %d to each element of the array\n", value);
	nc::forEach(array_.begin(), array_.end(), addValue<value>);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	for(unsigned int i = 0; i < array_.size(); i++)
		ASSERT_EQ(array_[i], i + value);
}

TEST_F(StaticArrayAlgorithmsTest, CountEqual)
{
	const int counter = nc::count(array_.begin(), array_.end(), 6);
	printf("Number of elements equal to 6: %d\n", counter);
	ASSERT_EQ(counter, 1);
}

TEST_F(StaticArrayAlgorithmsTest, CountElementsGreater)
{
	const int counter = nc::countIf(array_.begin(), array_.end(), nc::IsGreaterThan<int>(4));
	printf("Number of elements bigger than 4: %d\n", counter);
	ASSERT_EQ(counter, 5);
}

TEST_F(StaticArrayAlgorithmsTest, DistanceToFirstElementEqual)
{
	const int position = nc::distance(array_.begin(), nc::find(array_.begin(), array_.end(), 3));
	printf("First element equal to 3 in position: %d\n", position);
	ASSERT_EQ(position, 3);
}

TEST_F(StaticArrayAlgorithmsTest, DistanceToFirstElementBigger)
{
	const int counter = nc::distance(array_.begin(), nc::findIf(array_.begin(), array_.end(), nc::IsGreaterThan<int>(3)));
	printf("First element bigger than 3 in position: %d\n", counter);
	ASSERT_EQ(counter, 4);
}

TEST_F(StaticArrayAlgorithmsTest, DistanceToFirstElementNotBigger)
{
	const int counter = nc::distance(array_.begin(), nc::findIfNot(array_.begin(), array_.end(), nc::IsGreaterThan<int>(3)));
	printf("First element not bigger than 3 in position: %d\n", counter);
	ASSERT_EQ(counter, 0);
}

TEST_F(StaticArrayAlgorithmsTest, CheckEqual)
{
	printf("Copying the array to a second one and check they are equal\n");
	nc::StaticArray<int, Capacity> newArray;
	newArray = array_;
	printArray(newArray);

	ASSERT_TRUE(nc::equal(array_.begin(), array_.end(), newArray.begin()));
	ASSERT_EQ(array_.size(), newArray.size());
	for(unsigned int i = 0; i < newArray.size(); i++)
		ASSERT_EQ(array_[i], newArray[i]);
}

TEST_F(StaticArrayAlgorithmsTest, FillNElements)
{
	printf("Filling half array with zeroes\n");
	nc::fillN(array_.begin(), Capacity / 2, 0);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	const unsigned int halfSize = array_.size() / 2;
	for(unsigned int i = 0; i < halfSize; i++)
		ASSERT_EQ(array_[i], 0);
	for(unsigned int i = halfSize + 1; i < array_.size(); i++)
		ASSERT_EQ(array_[i], i);
}

TEST_F(StaticArrayAlgorithmsTest, FillWithIterators)
{
	printf("Filling the whole array with zeroes\n");
	nc::fill(array_.begin(), array_.end(), 0);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	for(unsigned int i = 0; i < array_.size(); i++)
		ASSERT_EQ(array_[i], 0);
}

TEST_F(StaticArrayAlgorithmsTest, CopyToArray)
{
	printf("Copying the array to a new one with iterators\n");
	nc::StaticArray<int, Capacity> newArray;
	newArray.setSize(Capacity);
	nc::copy(array_.begin(), array_.end(), newArray.begin());
	printArray(newArray);

	ASSERT_EQ(array_.size(), newArray.size());
	for(unsigned int i = 0; i < newArray.size(); i++)
		ASSERT_EQ(array_[i], newArray[i]);
}

TEST_F(StaticArrayAlgorithmsTest, CopyNToArray)
{
	printf("Copying half the array to a new one with iterators\n");
	nc::StaticArray<int, Capacity> newArray;
	newArray.setSize(Capacity / 2);
	nc::copyN(array_.begin(), Capacity / 2, newArray.begin());
	printArray(newArray);

	ASSERT_EQ(newArray.size(), Capacity / 2);
	for(unsigned int i = 0; i < newArray.size(); i++)
		ASSERT_EQ(array_[i], newArray[i]);
}

TEST_F(StaticArrayAlgorithmsTest, CopyIfToArray)
{
	printf("Copying all elements smaller than 5 to a new array\n");
	nc::StaticArray<int, Capacity> newArray;
	newArray.setSize(Capacity / 2);
	nc::copyIf(array_.begin(), array_.end(), newArray.begin(), nc::IsLessThan<int>(5));
	printArray(newArray);

	ASSERT_EQ(newArray.size(), Capacity / 2);
	for(unsigned int i = 0; i < newArray.size(); i++)
		ASSERT_EQ(array_[i], newArray[i]);
}

TEST_F(StaticArrayAlgorithmsTest, CopyTransformed)
{
	printf("Filling a new array with random numbers\n");
	nc::StaticArray<int, Capacity> newArray;
	initArrayRandom(newArray);
	printArray(newArray);

	printf("Storing transformed random numbers back into the first array\n");
	nc::transform(newArray.begin(), newArray.end(), array_.begin(), copyAddValue<10>);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	for(unsigned int i = 0; i < array_.size(); i++)
		ASSERT_EQ(array_[i], newArray[i] + 10);
}

TEST_F(StaticArrayAlgorithmsTest, CopyBinaryTransformed)
{
	printf("Filling a second array with random numbers\n");
	nc::StaticArray<int, Capacity> secondArray;
	initArrayRandom(secondArray);
	printArray(secondArray);

	printf("Creating a third array to store transformed elements\n");
	nc::StaticArray<int, Capacity> thirdArray;
	thirdArray.setSize(Capacity);

	printf("Storing the sum of the first two arrays into the third one\n");
	nc::transform(array_.begin(), array_.end(), secondArray.begin(), thirdArray.begin(), nc::Plus<int>);
	printArray(thirdArray);

	ASSERT_EQ(thirdArray.size(), Capacity);
	for(unsigned int i = 0; i < thirdArray.size(); i++)
		ASSERT_EQ(thirdArray[i], array_[i] + secondArray[i]);
}

TEST_F(StaticArrayAlgorithmsTest, ClampElements)
{
	const int minValue = 3;
	const int maxValue = 6;

	printf("Clamping array elements between %d and %d\n", minValue, maxValue);
	nc::clampElements(array_.begin(), array_.end(), minValue, maxValue);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	for(unsigned int i = 0; i < array_.size(); i++)
	{
		ASSERT_TRUE(array_[i] >= minValue);
		ASSERT_TRUE(array_[i] <= maxValue);
	}
}

TEST_F(StaticArrayAlgorithmsTest, Replace)
{
	const int oldValue = 5;
	const int newValue = 55;

	printf("Replacing all elements equal to %d with %d\n", oldValue, newValue);
	nc::replace(array_.begin(), array_.end(), oldValue, newValue);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	ASSERT_EQ(array_[oldValue], newValue);
}

TEST_F(StaticArrayAlgorithmsTest, ReplaceIf)
{
	const int refValue = 5;
	const int newValue = 55;

	printf("Replacing all elements bigger than %d with %d\n", refValue, newValue);
	nc::replaceIf(array_.begin(), array_.end(), nc::IsEqualTo<int>(refValue), newValue);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	ASSERT_EQ(array_[refValue], newValue);
}

TEST_F(StaticArrayAlgorithmsTest, ReplaceCopy)
{
	const int oldValue = 5;
	const int newValue = 55;

	printf("Creating a new array\n");
	nc::StaticArray<int, Capacity> newArray;
	newArray.setSize(Capacity);

	printf("Replacing all elements equal to %d with %d when copying to the new array\n", oldValue, newValue);
	nc::replaceCopy(array_.begin(), array_.end(), newArray.begin(), oldValue, newValue);
	printArray(newArray);

	ASSERT_EQ(array_.size(), newArray.size());
	ASSERT_EQ(array_[oldValue], oldValue);
	ASSERT_EQ(newArray[oldValue], newValue);
}

TEST_F(StaticArrayAlgorithmsTest, ReplaceCopyIf)
{
	const int refValue = 5;
	const int newValue = 55;

	printf("Creating a new array\n");
	nc::StaticArray<int, Capacity> newArray;
	newArray.setSize(Capacity);

	printf("Replacing all elements bigger than %d with %d when copying to the new array\n", refValue, newValue);
	nc::replaceCopyIf(array_.begin(), array_.end(), newArray.begin(), nc::IsEqualTo<int>(refValue), newValue);
	printArray(newArray);

	ASSERT_EQ(array_.size(), newArray.size());
	ASSERT_EQ(array_[refValue], refValue);
	ASSERT_EQ(newArray[refValue], newValue);
}

TEST_F(StaticArrayAlgorithmsTest, Generate)
{
	const int value = 1;
	printf("Generating a sequence starting at %d and store it into the array\n", value);
	nc::generate(array_.begin(), array_.end(), GenerateSequenceFrom(value));
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	for(unsigned int i = 0; i < array_.size(); i++)
		ASSERT_EQ(array_[i], i + value);
}

TEST_F(StaticArrayAlgorithmsTest, GenerateN)
{
	const int value = -4;
	const int numElements = 5;
	printf("Generating a sequence of %d values starting at %d and store it into the array\n", numElements, value);
	nc::generateN(array_.begin(), numElements, GenerateSequenceFrom(value));
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	for(unsigned int i = 0; i < numElements; i++)
		ASSERT_EQ(array_[i], i + value);
	for(unsigned int i = numElements; i < array_.size(); i++)
		ASSERT_EQ(array_[i], i);
}

TEST_F(StaticArrayAlgorithmsTest, Sort)
{
	printf("Filling the array with random numbers\n");
	initArrayRandom(array_);
	printArray(array_);

	printf("Sorting the array\n");
	nc::quicksort(array_.begin(), array_.end());
	printArray(array_);
	const bool sorted = nc::isSorted(array_.begin(), array_.end());
	printf("The array is %s\n", sorted ? "sorted" : "not sorted");

	ASSERT_EQ(sorted, true);
	ASSERT_EQ(isSorted(array_), true);
}

TEST_F(StaticArrayAlgorithmsTest, SortedUntil)
{
	const unsigned int position = 5;
	const int element = -1;
	printf("Inserting %d at position %u to break sorting\n", element, position);
	array_[position] = element;
	printArray(array_);

	nc::StaticArray<int, Capacity>::ConstIterator unsorted = nc::isSortedUntil(array_.begin(), array_.end());
	printf("The array is sorted until %d\n", *unsorted);

	ASSERT_EQ(*unsorted, element);
}

TEST_F(StaticArrayAlgorithmsTest, ReverseSort)
{
	printf("Filling the array with random numbers\n");
	initArrayRandom(array_);
	printArray(array_);

	printf("Reverse sorting the array\n");
	nc::quicksortDesc(array_.begin(), array_.end());
	printArray(array_);
	const bool reverseSorted = nc::isSorted(array_.begin(), array_.end(), nc::IsGreater<int>);
	printf("The array is %s\n", reverseSorted ? "reverse sorted" : "not reverse sorted");

	ASSERT_EQ(reverseSorted, true);
	ASSERT_EQ(isReverseSorted(array_), true);
}

TEST_F(StaticArrayAlgorithmsTest, ReverseSortedUntil)
{
	printf("Reverse sorting the array\n");
	nc::quicksortDesc(array_.begin(), array_.end());
	printArray(array_);

	const unsigned int position = 5;
	const int element = 99;
	printf("Inserting %d at position %u to break sorting\n", element, position);
	array_[position] = element;
	printArray(array_);

	nc::StaticArray<int, Capacity>::ConstIterator unsorted = nc::isSortedUntil(array_.begin(), array_.end(), nc::IsGreater<int>);
	printf("The array is sorted until %d\n", *unsorted);

	ASSERT_EQ(*unsorted, element);
}

TEST_F(StaticArrayAlgorithmsTest, Reverse)
{
	printf("Reversing the array\n");
	nc::reverse(array_.begin(), array_.end());
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	for(unsigned int i = 0; i < array_.size(); i++)
		ASSERT_EQ(array_[i], array_.size() - 1 - i);
}

TEST_F(StaticArrayAlgorithmsTest, ReverseCopy)
{
	nc::StaticArray<int, Capacity> newArray;
	newArray.setSize(Capacity);

	printf("Reverse copying the array into a new one\n");
	nc::reverseCopy(array_.begin(), array_.end(), newArray.begin());
	printArray(newArray);

	ASSERT_EQ(newArray.size(), Capacity);
	for(unsigned int i = 0; i < newArray.size(); i++)
		ASSERT_EQ(newArray[i], newArray.size() - 1 - i);
}

}
