#include "gtest_array.h"
#include "test_functions.h"

namespace {

class ArrayAlgorithmsTest : public ::testing::Test
{
  public:
	ArrayAlgorithmsTest() : array_(Capacity) { }

  protected:
	void SetUp() override { initArray(array_); }

	nc::Array<int> array_;
};

TEST_F(ArrayAlgorithmsTest, Minimum)
{
	const int minimum = *nc::minElement(array_.begin(), array_.end());
	printf("Minimum element: %d\n", minimum);

	ASSERT_EQ(minimum, 0);
}

TEST_F(ArrayAlgorithmsTest, Maximum)
{
	const int maximum = *nc::maxElement(array_.begin(), array_.end());
	printf("Maximum element: %d\n", maximum);

	ASSERT_EQ(maximum, Capacity - 1);
}

TEST_F(ArrayAlgorithmsTest, AllOfGreater)
{
	const bool allOf = nc::allOf(array_.begin(), array_.end(), nc::IsGreaterThan<int>(-1));
	printf("All bigger than -1: %d\n", allOf);
	ASSERT_EQ(allOf, true);
}

TEST_F(ArrayAlgorithmsTest, NoneOfGreater)
{
	const bool noneOf = nc::noneOf(array_.begin(), array_.end(), nc::IsGreaterThan<int>(5));
	printf("No one bigger than 5: %d\n", noneOf);
	ASSERT_EQ(noneOf, false);
}

TEST_F(ArrayAlgorithmsTest, AnyOfGreater)
{
	const bool anyOf = nc::anyOf(array_.begin(), array_.end(), nc::IsGreaterThan<int>(5));
	printf("Anyone bigger than 5: %d\n", anyOf);
	ASSERT_EQ(anyOf, true);
}

TEST_F(ArrayAlgorithmsTest, AddValueForEach)
{
	const int value = 10;
	printf("Adding %d to each element of the array\n", value);
	nc::forEach(array_.begin(), array_.end(), addValue<value>);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	for(unsigned int i = 0; i < array_.size(); i++)
		ASSERT_EQ(array_[i], i + value);
}

TEST_F(ArrayAlgorithmsTest, CountEqual)
{
	const int counter = nc::count(array_.begin(), array_.end(), 6);
	printf("Number of elements equal to 6: %d\n", counter);
	ASSERT_EQ(counter, 1);
}

TEST_F(ArrayAlgorithmsTest, CountElementsGreater)
{
	const int counter = nc::countIf(array_.begin(), array_.end(), nc::IsGreaterThan<int>(4));
	printf("Number of elements bigger than 4: %d\n", counter);
	ASSERT_EQ(counter, 5);
}

TEST_F(ArrayAlgorithmsTest, DistanceToFirstElementEqual)
{
	const int position = nc::distance(array_.begin(), nc::find(array_.begin(), array_.end(), 3));
	printf("First element equal to 3 in position: %d\n", position);
	ASSERT_EQ(position, 3);
}

TEST_F(ArrayAlgorithmsTest, DistanceToFirstElementBigger)
{
	const int counter = nc::distance(array_.begin(), nc::findIf(array_.begin(), array_.end(), nc::IsGreaterThan<int>(3)));
	printf("First element bigger than 3 in position: %d\n", counter);
	ASSERT_EQ(counter, 4);
}

TEST_F(ArrayAlgorithmsTest, DistanceToFirstElementNotBigger)
{
	const int counter = nc::distance(array_.begin(), nc::findIfNot(array_.begin(), array_.end(), nc::IsGreaterThan<int>(3)));
	printf("First element not bigger than 3 in position: %d\n", counter);
	ASSERT_EQ(counter, 0);
}

TEST_F(ArrayAlgorithmsTest, CheckEqual)
{
	printf("Copying the array to a second one and check they are equal\n");
	nc::Array<int> newArray(Capacity);
	newArray = array_;
	printArray(newArray);

	ASSERT_TRUE(nc::equal(array_.begin(), array_.end(), newArray.begin()));
	ASSERT_EQ(array_.size(), newArray.size());
	for(unsigned int i = 0; i < newArray.size(); i++)
		ASSERT_EQ(array_[i], newArray[i]);
}

TEST_F(ArrayAlgorithmsTest, FillNElements)
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

TEST_F(ArrayAlgorithmsTest, FillWithIterators)
{
	printf("Filling the whole array with zeroes\n");
	nc::fill(array_.begin(), array_.end(), 0);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	for(unsigned int i = 0; i < array_.size(); i++)
		ASSERT_EQ(array_[i], 0);
}

TEST_F(ArrayAlgorithmsTest, CopyToArray)
{
	printf("Copying the array to a new one with iterators\n");
	nc::Array<int> newArray(Capacity);
	newArray.setSize(Capacity);
	nc::copy(array_.begin(), array_.end(), newArray.begin());
	printArray(newArray);

	ASSERT_EQ(array_.size(), newArray.size());
	for(unsigned int i = 0; i < newArray.size(); i++)
		ASSERT_EQ(array_[i], newArray[i]);
}

TEST_F(ArrayAlgorithmsTest, CopyNToArray)
{
	printf("Copying half the array to a new one with iterators\n");
	nc::Array<int> newArray(Capacity);
	newArray.setSize(Capacity / 2);
	nc::copyN(array_.begin(), Capacity / 2, newArray.begin());
	printArray(newArray);

	ASSERT_EQ(newArray.size(), Capacity / 2);
	for(unsigned int i = 0; i < newArray.size(); i++)
		ASSERT_EQ(array_[i], newArray[i]);
}

TEST_F(ArrayAlgorithmsTest, CopyIfToArray)
{
	printf("Copying all elements smaller than 5 to a new array\n");
	nc::Array<int> newArray(Capacity);
	newArray.setSize(Capacity / 2);
	nc::copyIf(array_.begin(), array_.end(), newArray.begin(), nc::IsLessThan<int>(5));
	printArray(newArray);

	ASSERT_EQ(newArray.size(), Capacity / 2);
	for(unsigned int i = 0; i < newArray.size(); i++)
		ASSERT_EQ(array_[i], newArray[i]);
}

TEST_F(ArrayAlgorithmsTest, CopyTransformed)
{
	printf("Filling a new array with random numbers\n");
	nc::Array<int> newArray(Capacity);
	initArrayRandom(newArray);
	printArray(newArray);

	printf("Storing transformed random numbers back into the first array\n");
	nc::transform(newArray.begin(), newArray.end(), array_.begin(), copyAddValue<10>);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	for(unsigned int i = 0; i < array_.size(); i++)
		ASSERT_EQ(array_[i], newArray[i] + 10);
}

TEST_F(ArrayAlgorithmsTest, CopyBinaryTransformed)
{
	printf("Filling a second array with random numbers\n");
	nc::Array<int> secondArray(Capacity);
	initArrayRandom(secondArray);
	printArray(secondArray);

	printf("Creating a third array to store transformed elements\n");
	nc::Array<int> thirdArray(Capacity);
	thirdArray.setSize(Capacity);

	printf("Storing the sum of the first two arrays into the third one\n");
	nc::transform(array_.begin(), array_.end(), secondArray.begin(), thirdArray.begin(), nc::Plus<int>);
	printArray(thirdArray);

	ASSERT_EQ(thirdArray.size(), Capacity);
	for(unsigned int i = 0; i < thirdArray.size(); i++)
		ASSERT_EQ(thirdArray[i], array_[i] + secondArray[i]);
}

TEST_F(ArrayAlgorithmsTest, ClampElements)
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

TEST_F(ArrayAlgorithmsTest, Replace)
{
	const int oldValue = 5;
	const int newValue = 55;

	printf("Replacing all elements equal to %d with %d\n", oldValue, newValue);
	nc::replace(array_.begin(), array_.end(), oldValue, newValue);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	ASSERT_EQ(array_[oldValue], newValue);
}

TEST_F(ArrayAlgorithmsTest, ReplaceIf)
{
	const int refValue = 5;
	const int newValue = 55;

	printf("Replacing all elements bigger than %d with %d\n", refValue, newValue);
	nc::replaceIf(array_.begin(), array_.end(), nc::IsEqualTo<int>(refValue), newValue);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	ASSERT_EQ(array_[refValue], newValue);
}

TEST_F(ArrayAlgorithmsTest, ReplaceCopy)
{
	const int oldValue = 5;
	const int newValue = 55;

	printf("Creating a new array\n");
	nc::Array<int> newArray(Capacity);
	newArray.setSize(Capacity);

	printf("Replacing all elements equal to %d with %d when copying to the new array\n", oldValue, newValue);
	nc::replaceCopy(array_.begin(), array_.end(), newArray.begin(), oldValue, newValue);
	printArray(newArray);

	ASSERT_EQ(array_.size(), newArray.size());
	ASSERT_EQ(array_[oldValue], oldValue);
	ASSERT_EQ(newArray[oldValue], newValue);
}

TEST_F(ArrayAlgorithmsTest, ReplaceCopyIf)
{
	const int refValue = 5;
	const int newValue = 55;

	printf("Creating a new array\n");
	nc::Array<int> newArray(Capacity);
	newArray.setSize(Capacity);

	printf("Replacing all elements bigger than %d with %d when copying to the new array\n", refValue, newValue);
	nc::replaceCopyIf(array_.begin(), array_.end(), newArray.begin(), nc::IsEqualTo<int>(refValue), newValue);
	printArray(newArray);

	ASSERT_EQ(array_.size(), newArray.size());
	ASSERT_EQ(array_[refValue], refValue);
	ASSERT_EQ(newArray[refValue], newValue);
}

TEST_F(ArrayAlgorithmsTest, Generate)
{
	const int value = 1;
	printf("Generating a sequence starting at %d and store it into the array\n", value);
	nc::generate(array_.begin(), array_.end(), GenerateSequenceFrom(value));
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	for(unsigned int i = 0; i < array_.size(); i++)
		ASSERT_EQ(array_[i], i + value);
}

TEST_F(ArrayAlgorithmsTest, GenerateN)
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

TEST_F(ArrayAlgorithmsTest, Sort)
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

TEST_F(ArrayAlgorithmsTest, SortedUntil)
{
	const unsigned int position = 5;
	const int element = -1;
	printf("Inserting %d at position %u to break sorting\n", element, position);
	array_[position] = element;
	printArray(array_);

	nc::Array<int>::ConstIterator unsorted = nc::isSortedUntil(array_.begin(), array_.end());
	printf("The array is sorted until %d\n", *unsorted);

	ASSERT_EQ(*unsorted, element);
}

TEST_F(ArrayAlgorithmsTest, ReverseSort)
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

TEST_F(ArrayAlgorithmsTest, ReverseSortedUntil)
{
	printf("Reverse sorting the array\n");
	nc::quicksortDesc(array_.begin(), array_.end());
	printArray(array_);

	const unsigned int position = 5;
	const int element = 99;
	printf("Inserting %d at position %u to break sorting\n", element, position);
	array_[position] = element;
	printArray(array_);

	nc::Array<int>::ConstIterator unsorted = nc::isSortedUntil(array_.begin(), array_.end(), nc::IsGreater<int>);
	printf("The array is sorted until %d\n", *unsorted);

	ASSERT_EQ(*unsorted, element);
}

TEST_F(ArrayAlgorithmsTest, Reverse)
{
	printf("Reversing the array\n");
	nc::reverse(array_.begin(), array_.end());
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	for(unsigned int i = 0; i < array_.size(); i++)
		ASSERT_EQ(array_[i], array_.size() - 1 - i);
}

TEST_F(ArrayAlgorithmsTest, ReverseCopy)
{
	nc::Array<int> newArray(Capacity);
	newArray.setSize(Capacity);

	printf("Reverse copying the array into a new one\n");
	nc::reverseCopy(array_.begin(), array_.end(), newArray.begin());
	printArray(newArray);

	ASSERT_EQ(newArray.size(), Capacity);
	for(unsigned int i = 0; i < newArray.size(); i++)
		ASSERT_EQ(newArray[i], newArray.size() - 1 - i);
}

TEST_F(ArrayAlgorithmsTest, Remove)
{
	const int value = 4;
	printf("Removing all the elements equal to %d from the array\n", value);
	array_.erase(nc::remove(array_.begin(), array_.end(), value));
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity - 1);
	for(unsigned int i = 0; i < value; i++)
		ASSERT_EQ(array_[i], i);
	for(unsigned int i = value; i < array_.size(); i++)
		ASSERT_EQ(array_[i], i+1);
}

TEST_F(ArrayAlgorithmsTest, RemoveIf)
{
	printf("Removing all the elements greater than 4 from the array\n");
	array_.erase(nc::removeIf(array_.begin(), array_.end(), nc::IsGreaterThan<int>(4)), array_.end());
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity / 2);
	for(unsigned int i = 0; i < array_.size(); i++)
		ASSERT_EQ(array_[i], i);
}

}
