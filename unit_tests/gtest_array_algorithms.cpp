#include "gtest_array.h"
#include "test_functions.h"
#include <nctl/algorithms.h>

namespace {

class ArrayAlgorithmsTest : public ::testing::Test
{
  public:
	ArrayAlgorithmsTest()
	    : array_(Capacity) {}

  protected:
	void SetUp() override { initArray(array_); }

	nctl::Array<int> array_;
};

TEST_F(ArrayAlgorithmsTest, Minimum)
{
	const int minimum = *nctl::minElement(array_.begin(), array_.end());
	printf("Minimum element: %d\n", minimum);

	ASSERT_EQ(minimum, 0);
}

TEST_F(ArrayAlgorithmsTest, Maximum)
{
	const int maximum = *nctl::maxElement(array_.begin(), array_.end());
	printf("Maximum element: %d\n", maximum);

	ASSERT_EQ(maximum, Capacity - 1);
}

TEST_F(ArrayAlgorithmsTest, AllOfGreater)
{
	const bool allOf = nctl::allOf(array_.begin(), array_.end(), nctl::IsGreaterThan<int>(-1));
	printf("All bigger than -1: %d\n", allOf);
	ASSERT_EQ(allOf, true);
}

TEST_F(ArrayAlgorithmsTest, NoneOfGreater)
{
	const bool noneOf = nctl::noneOf(array_.begin(), array_.end(), nctl::IsGreaterThan<int>(5));
	printf("No one bigger than 5: %d\n", noneOf);
	ASSERT_EQ(noneOf, false);
}

TEST_F(ArrayAlgorithmsTest, AnyOfGreater)
{
	const bool anyOf = nctl::anyOf(array_.begin(), array_.end(), nctl::IsGreaterThan<int>(5));
	printf("Anyone bigger than 5: %d\n", anyOf);
	ASSERT_EQ(anyOf, true);
}

TEST_F(ArrayAlgorithmsTest, AddValueForEach)
{
	const int value = 10;
	printf("Adding %d to each element of the array\n", value);
	nctl::forEach(array_.begin(), array_.end(), addValue<value>);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	for (unsigned int i = 0; i < array_.size(); i++)
		ASSERT_EQ(array_[i], i + value);
}

TEST_F(ArrayAlgorithmsTest, CountEqual)
{
	const int counter = nctl::count(array_.begin(), array_.end(), 6);
	printf("Number of elements equal to 6: %d\n", counter);
	ASSERT_EQ(counter, 1);
}

TEST_F(ArrayAlgorithmsTest, CountElementsGreater)
{
	const int counter = nctl::countIf(array_.begin(), array_.end(), nctl::IsGreaterThan<int>(4));
	printf("Number of elements bigger than 4: %d\n", counter);
	ASSERT_EQ(counter, 5);
}

TEST_F(ArrayAlgorithmsTest, DistanceToFirstElementEqual)
{
	const int position = nctl::distance(array_.begin(), nctl::find(array_.begin(), array_.end(), 3));
	printf("First element equal to 3 in position: %d\n", position);
	ASSERT_EQ(position, 3);
}

TEST_F(ArrayAlgorithmsTest, DistanceToFirstElementBigger)
{
	const int counter = nctl::distance(array_.begin(), nctl::findIf(array_.begin(), array_.end(), nctl::IsGreaterThan<int>(3)));
	printf("First element bigger than 3 in position: %d\n", counter);
	ASSERT_EQ(counter, 4);
}

TEST_F(ArrayAlgorithmsTest, DistanceToFirstElementNotBigger)
{
	const int counter = nctl::distance(array_.begin(), nctl::findIfNot(array_.begin(), array_.end(), nctl::IsGreaterThan<int>(3)));
	printf("First element not bigger than 3 in position: %d\n", counter);
	ASSERT_EQ(counter, 0);
}

TEST_F(ArrayAlgorithmsTest, CheckEqual)
{
	printf("Copying the array to a second one and check they are equal\n");
	nctl::Array<int> newArray(Capacity);
	newArray = array_;
	printArray(newArray);

	ASSERT_TRUE(nctl::equal(array_.begin(), array_.end(), newArray.begin()));
	ASSERT_EQ(array_.size(), newArray.size());
	for (unsigned int i = 0; i < newArray.size(); i++)
		ASSERT_EQ(array_[i], newArray[i]);
}

TEST_F(ArrayAlgorithmsTest, FillNElements)
{
	printf("Filling half array with zeroes\n");
	nctl::fillN(array_.begin(), Capacity / 2, 0);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	const unsigned int halfSize = array_.size() / 2;
	for (unsigned int i = 0; i < halfSize; i++)
		ASSERT_EQ(array_[i], 0);
	for (unsigned int i = halfSize + 1; i < array_.size(); i++)
		ASSERT_EQ(array_[i], i);
}

TEST_F(ArrayAlgorithmsTest, FillWithIterators)
{
	printf("Filling the whole array with zeroes\n");
	nctl::fill(array_.begin(), array_.end(), 0);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	for (unsigned int i = 0; i < array_.size(); i++)
		ASSERT_EQ(array_[i], 0);
}

TEST_F(ArrayAlgorithmsTest, CopyToArray)
{
	printf("Copying the array to a new one with iterators\n");
	nctl::Array<int> newArray(Capacity);
	newArray.setSize(Capacity);
	nctl::copy(array_.begin(), array_.end(), newArray.begin());
	printArray(newArray);

	ASSERT_EQ(array_.size(), newArray.size());
	for (unsigned int i = 0; i < newArray.size(); i++)
		ASSERT_EQ(array_[i], newArray[i]);
}

TEST_F(ArrayAlgorithmsTest, CopyNToArray)
{
	printf("Copying half the array to a new one with iterators\n");
	nctl::Array<int> newArray(Capacity);
	newArray.setSize(Capacity / 2);
	nctl::copyN(array_.begin(), Capacity / 2, newArray.begin());
	printArray(newArray);

	ASSERT_EQ(newArray.size(), Capacity / 2);
	for (unsigned int i = 0; i < newArray.size(); i++)
		ASSERT_EQ(array_[i], newArray[i]);
}

TEST_F(ArrayAlgorithmsTest, CopyIfToArray)
{
	printf("Copying all elements smaller than 5 to a new array\n");
	nctl::Array<int> newArray(Capacity);
	newArray.setSize(Capacity / 2);
	nctl::copyIf(array_.begin(), array_.end(), newArray.begin(), nctl::IsLessThan<int>(5));
	printArray(newArray);

	ASSERT_EQ(newArray.size(), Capacity / 2);
	for (unsigned int i = 0; i < newArray.size(); i++)
		ASSERT_EQ(array_[i], newArray[i]);
}

TEST_F(ArrayAlgorithmsTest, CopyTransformed)
{
	printf("Filling a new array with random numbers\n");
	nctl::Array<int> newArray(Capacity);
	initArrayRandom(newArray);
	printArray(newArray);

	printf("Storing transformed random numbers back into the first array\n");
	nctl::transform(newArray.begin(), newArray.end(), array_.begin(), copyAddValue<10>);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	for (unsigned int i = 0; i < array_.size(); i++)
		ASSERT_EQ(array_[i], newArray[i] + 10);
}

TEST_F(ArrayAlgorithmsTest, CopyBinaryTransformed)
{
	printf("Filling a second array with random numbers\n");
	nctl::Array<int> secondArray(Capacity);
	initArrayRandom(secondArray);
	printArray(secondArray);

	printf("Creating a third array to store transformed elements\n");
	nctl::Array<int> thirdArray(Capacity);
	thirdArray.setSize(Capacity);

	printf("Storing the sum of the first two arrays into the third one\n");
	nctl::transform(array_.begin(), array_.end(), secondArray.begin(), thirdArray.begin(), nctl::Plus<int>);
	printArray(thirdArray);

	ASSERT_EQ(thirdArray.size(), Capacity);
	for (unsigned int i = 0; i < thirdArray.size(); i++)
		ASSERT_EQ(thirdArray[i], array_[i] + secondArray[i]);
}

TEST_F(ArrayAlgorithmsTest, ClampElements)
{
	const int minValue = 3;
	const int maxValue = 6;

	printf("Clamping array elements between %d and %d\n", minValue, maxValue);
	nctl::clampElements(array_.begin(), array_.end(), minValue, maxValue);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	for (unsigned int i = 0; i < array_.size(); i++)
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
	nctl::replace(array_.begin(), array_.end(), oldValue, newValue);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	ASSERT_EQ(array_[oldValue], newValue);
}

TEST_F(ArrayAlgorithmsTest, ReplaceIf)
{
	const int refValue = 5;
	const int newValue = 55;

	printf("Replacing all elements bigger than %d with %d\n", refValue, newValue);
	nctl::replaceIf(array_.begin(), array_.end(), nctl::IsEqualTo<int>(refValue), newValue);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	ASSERT_EQ(array_[refValue], newValue);
}

TEST_F(ArrayAlgorithmsTest, ReplaceCopy)
{
	const int oldValue = 5;
	const int newValue = 55;

	printf("Creating a new array\n");
	nctl::Array<int> newArray(Capacity);
	newArray.setSize(Capacity);

	printf("Replacing all elements equal to %d with %d when copying to the new array\n", oldValue, newValue);
	nctl::replaceCopy(array_.begin(), array_.end(), newArray.begin(), oldValue, newValue);
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
	nctl::Array<int> newArray(Capacity);
	newArray.setSize(Capacity);

	printf("Replacing all elements bigger than %d with %d when copying to the new array\n", refValue, newValue);
	nctl::replaceCopyIf(array_.begin(), array_.end(), newArray.begin(), nctl::IsEqualTo<int>(refValue), newValue);
	printArray(newArray);

	ASSERT_EQ(array_.size(), newArray.size());
	ASSERT_EQ(array_[refValue], refValue);
	ASSERT_EQ(newArray[refValue], newValue);
}

TEST_F(ArrayAlgorithmsTest, Generate)
{
	const int value = 1;
	printf("Generating a sequence starting at %d and store it into the array\n", value);
	nctl::generate(array_.begin(), array_.end(), GenerateSequenceFrom(value));
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	for (unsigned int i = 0; i < array_.size(); i++)
		ASSERT_EQ(array_[i], i + value);
}

TEST_F(ArrayAlgorithmsTest, GenerateN)
{
	const int value = -4;
	const int numElements = 5;
	printf("Generating a sequence of %d values starting at %d and store it into the array\n", numElements, value);
	nctl::generateN(array_.begin(), numElements, GenerateSequenceFrom(value));
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	for (unsigned int i = 0; i < numElements; i++)
		ASSERT_EQ(array_[i], i + value);
	for (unsigned int i = numElements; i < array_.size(); i++)
		ASSERT_EQ(array_[i], i);
}

TEST_F(ArrayAlgorithmsTest, Reverse)
{
	printf("Reversing the array\n");
	nctl::reverse(array_.begin(), array_.end());
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	for (unsigned int i = 0; i < array_.size(); i++)
		ASSERT_EQ(array_[i], array_.size() - 1 - i);
}

TEST_F(ArrayAlgorithmsTest, ReverseCopy)
{
	nctl::Array<int> newArray(Capacity);
	newArray.setSize(Capacity);

	printf("Reverse copying the array into a new one\n");
	nctl::reverseCopy(array_.begin(), array_.end(), newArray.begin());
	printArray(newArray);

	ASSERT_EQ(newArray.size(), Capacity);
	for (unsigned int i = 0; i < newArray.size(); i++)
		ASSERT_EQ(newArray[i], newArray.size() - 1 - i);
}

TEST_F(ArrayAlgorithmsTest, Remove)
{
	const int value = 4;
	printf("Removing all the elements equal to %d from the array\n", value);
	array_.erase(nctl::remove(array_.begin(), array_.end(), value));
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity - 1);
	for (unsigned int i = 0; i < value; i++)
		ASSERT_EQ(array_[i], i);
	for (unsigned int i = value; i < array_.size(); i++)
		ASSERT_EQ(array_[i], i + 1);
}

TEST_F(ArrayAlgorithmsTest, RemoveIf)
{
	printf("Removing all the elements greater than 4 from the array\n");
	array_.erase(nctl::removeIf(array_.begin(), array_.end(), nctl::IsGreaterThan<int>(4)), array_.end());
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity / 2);
	for (unsigned int i = 0; i < array_.size(); i++)
		ASSERT_EQ(array_[i], i);
}

}
