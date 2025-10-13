#include "gtest_carray.h"
#include <nctl/algorithms.h>

namespace {

class CArraySortTest : public ::testing::Test
{
  protected:
	void SetUp() override { initArray(array_); }

	int array_[Capacity];
};

TEST_F(CArraySortTest, QuickSort)
{
	printf("Filling the array with random numbers\n");
	initArrayRandom(array_);
	printArray(array_);

	printf("Sorting the array with quicksort\n");
	nctl::quicksort(nctl::begin(array_), nctl::end(array_));
	printArray(array_);
	const bool sorted = nctl::isSorted(nctl::cBegin(array_), nctl::cEnd(array_));
	printf("The array is %s\n", sorted ? "sorted" : "not sorted");

	ASSERT_EQ(sorted, true);
	ASSERT_EQ(isSorted(array_), true);
}

TEST_F(CArraySortTest, QuickSortedUntil)
{
	const unsigned int position = 5;
	const int element = -1;
	printf("Inserting %d at position %u to break sorting\n", element, position);
	array_[position] = element;
	printArray(array_);

	auto unsorted = nctl::isSortedUntil(nctl::cBegin(array_), nctl::cEnd(array_));
	printf("The array is sorted until %d\n", *unsorted);

	ASSERT_EQ(*unsorted, element);
}

TEST_F(CArraySortTest, ReverseQuickSort)
{
	printf("Filling the array with random numbers\n");
	initArrayRandom(array_);
	printArray(array_);

	printf("Reverse sorting the array with quicksort\n");
	nctl::quicksortDesc(nctl::begin(array_), nctl::end(array_));
	printArray(array_);
	const bool reverseSorted = nctl::isSorted(nctl::cBegin(array_), nctl::cEnd(array_), nctl::IsGreater<int>);
	printf("The array is %s\n", reverseSorted ? "reverse sorted" : "not reverse sorted");

	ASSERT_EQ(reverseSorted, true);
	ASSERT_EQ(isReverseSorted(array_), true);
}

TEST_F(CArraySortTest, ReverseQuickSortedUntil)
{
	printf("Reverse sorting the array with quicksort\n");
	nctl::quicksortDesc(nctl::begin(array_), nctl::end(array_));
	printArray(array_);

	const unsigned int position = 5;
	const int element = 99;
	printf("Inserting %d at position %u to break sorting\n", element, position);
	array_[position] = element;
	printArray(array_);

	auto unsorted = nctl::isSortedUntil(nctl::cBegin(array_), nctl::cEnd(array_), nctl::IsGreater<int>);
	printf("The array is sorted until %d\n", *unsorted);

	ASSERT_EQ(*unsorted, element);
}

TEST_F(CArraySortTest, HeapSort)
{
	printf("Filling the array with random numbers\n");
	initArrayRandom(array_);
	printArray(array_);

	printf("Sorting the array with heapsort\n");
	nctl::heapsort(nctl::begin(array_), nctl::end(array_));
	printArray(array_);
	const bool sorted = nctl::isSorted(nctl::cBegin(array_), nctl::cEnd(array_));
	printf("The array is %s\n", sorted ? "sorted" : "not sorted");

	ASSERT_EQ(sorted, true);
	ASSERT_EQ(isSorted(array_), true);
}

TEST_F(CArraySortTest, HeapSortedUntil)
{
	const unsigned int position = 5;
	const int element = -1;
	printf("Inserting %d at position %u to break sorting\n", element, position);
	array_[position] = element;
	printArray(array_);

	auto unsorted = nctl::isSortedUntil(nctl::cBegin(array_), nctl::cEnd(array_));
	printf("The array is sorted until %d\n", *unsorted);

	ASSERT_EQ(*unsorted, element);
}

TEST_F(CArraySortTest, ReverseHeapSort)
{
	printf("Filling the array with random numbers\n");
	initArrayRandom(array_);
	printArray(array_);

	printf("Reverse sorting the array with heapsort\n");
	nctl::heapsortDesc(nctl::begin(array_), nctl::end(array_));
	printArray(array_);
	const bool reverseSorted = nctl::isSorted(nctl::cBegin(array_), nctl::cEnd(array_), nctl::IsGreater<int>);
	printf("The array is %s\n", reverseSorted ? "reverse sorted" : "not reverse sorted");

	ASSERT_EQ(reverseSorted, true);
	ASSERT_EQ(isReverseSorted(array_), true);
}

TEST_F(CArraySortTest, ReverseHeapSortedUntil)
{
	printf("Reverse sorting the array with heapsort\n");
	nctl::heapsortDesc(nctl::begin(array_), nctl::end(array_));
	printArray(array_);

	const unsigned int position = 5;
	const int element = 99;
	printf("Inserting %d at position %u to break sorting\n", element, position);
	array_[position] = element;
	printArray(array_);

	auto unsorted = nctl::isSortedUntil(nctl::cBegin(array_), nctl::cEnd(array_), nctl::IsGreater<int>);
	printf("The array is sorted until %d\n", *unsorted);

	ASSERT_EQ(*unsorted, element);
}

TEST_F(CArraySortTest, InsertionSort)
{
	printf("Filling the array with random numbers\n");
	initArrayRandom(array_);
	printArray(array_);

	printf("Sorting the array with insertion sort\n");
	nctl::insertionsort(nctl::begin(array_), nctl::end(array_));
	printArray(array_);
	const bool sorted = nctl::isSorted(nctl::cBegin(array_), nctl::cEnd(array_));
	printf("The array is %s\n", sorted ? "sorted" : "not sorted");

	ASSERT_EQ(sorted, true);
	ASSERT_EQ(isSorted(array_), true);
}

TEST_F(CArraySortTest, InsertionSortedUntil)
{
	const unsigned int position = 5;
	const int element = -1;
	printf("Inserting %d at position %u to break sorting\n", element, position);
	array_[position] = element;
	printArray(array_);

	auto unsorted = nctl::isSortedUntil(nctl::cBegin(array_), nctl::cEnd(array_));
	printf("The array is sorted until %d\n", *unsorted);

	ASSERT_EQ(*unsorted, element);
}

TEST_F(CArraySortTest, ReverseInsertionSort)
{
	printf("Filling the array with random numbers\n");
	initArrayRandom(array_);
	printArray(array_);

	printf("Reverse sorting the array with insertion sort\n");
	nctl::insertionsortDesc(nctl::begin(array_), nctl::end(array_));
	printArray(array_);
	const bool reverseSorted = nctl::isSorted(nctl::cBegin(array_), nctl::cEnd(array_), nctl::IsGreater<int>);
	printf("The array is %s\n", reverseSorted ? "reverse sorted" : "not reverse sorted");

	ASSERT_EQ(reverseSorted, true);
	ASSERT_EQ(isReverseSorted(array_), true);
}

TEST_F(CArraySortTest, ReverseInsertionSortedUntil)
{
	printf("Reverse sorting the array with insertion sort\n");
	nctl::insertionsortDesc(nctl::begin(array_), nctl::end(array_));
	printArray(array_);

	const unsigned int position = 5;
	const int element = 99;
	printf("Inserting %d at position %u to break sorting\n", element, position);
	array_[position] = element;
	printArray(array_);

	auto unsorted = nctl::isSortedUntil(nctl::cBegin(array_), nctl::cEnd(array_), nctl::IsGreater<int>);
	printf("The array is sorted until %d\n", *unsorted);

	ASSERT_EQ(*unsorted, element);
}

TEST_F(CArraySortTest, IntrospectiveSortSmallArray)
{
	const unsigned int Capacity = 15;
	int array[Capacity];

	printf("Filling the array with random numbers\n");
	for (unsigned int i = 0; i < Capacity; i++)
		array[i] = nc::random().integer(0, 100);
	printArray(array);

	printf("Sorting the array with introspective sort (insertion sort only)\n");
	nctl::sort(nctl::begin(array), nctl::end(array));
	printArray(array);
	const bool sorted = nctl::isSorted(nctl::cBegin(array), nctl::cEnd(array));
	printf("The array is %s\n", sorted ? "sorted" : "not sorted");

	ASSERT_EQ(sorted, true);
	ASSERT_EQ(isSorted(array), true);
}

TEST_F(CArraySortTest, IntrospectiveSortArray)
{
	const unsigned int Capacity = 16384;
	int array[Capacity];

	printf("Filling the array with random numbers\n");
	for (unsigned int i = 0; i < Capacity; i++)
		array[i] = nc::random().integer(0, 10000);

	printf("Sorting the array with introspective sort\n");
	nctl::sort(nctl::begin(array), nctl::end(array));
	const bool sorted = nctl::isSorted(nctl::cBegin(array), nctl::cEnd(array));
	printf("The array is %s\n", sorted ? "sorted" : "not sorted");

	ASSERT_EQ(sorted, true);
	ASSERT_EQ(isSorted(array), true);
}

TEST_F(CArraySortTest, IntrospectiveSortArraySorted)
{
	const unsigned int Capacity = 16384;
	int array[Capacity];

	printf("Filling the array with sorted numbers\n");
	for (unsigned int i = 0; i < Capacity; i++)
		array[i] = i;

	printf("Sorting the array with introspective sort\n");
	nctl::sort(nctl::begin(array), nctl::end(array));
	const bool sorted = nctl::isSorted(nctl::cBegin(array), nctl::cEnd(array));
	printf("The array is %s\n", sorted ? "sorted" : "not sorted");

	ASSERT_EQ(sorted, true);
	ASSERT_EQ(isSorted(array), true);
}

TEST_F(CArraySortTest, IntrospectiveSortArrayReverseSorted)
{
	const unsigned int Capacity = 16384;
	int array[Capacity];

	printf("Filling the array with reverse sorted numbers\n");
	for (unsigned int i = 0; i < Capacity; i++)
		array[i] = Capacity - i - 1;

	printf("Sorting the array with introspective sort\n");
	nctl::sort(nctl::begin(array), nctl::end(array));
	const bool sorted = nctl::isSorted(nctl::cBegin(array), nctl::cEnd(array));
	printf("The array is %s\n", sorted ? "sorted" : "not sorted");

	ASSERT_EQ(sorted, true);
	ASSERT_EQ(isSorted(array), true);
}

TEST_F(CArraySortTest, IntrospectiveSortArrayRepeatedValue)
{
	const unsigned int Capacity = 16384;
	int array[Capacity];

	const int RepeatedValue = 42;
	printf("Filling the array with a repeated number\n");
	for (unsigned int i = 0; i < Capacity; i++)
		array[i] = RepeatedValue;

	printf("Sorting the array with introspective sort\n");
	nctl::sort(nctl::begin(array), nctl::end(array));
	const bool sorted = nctl::isSorted(nctl::cBegin(array), nctl::cEnd(array));
	printf("The array is %s\n", sorted ? "sorted" : "not sorted");

	ASSERT_EQ(sorted, true);
	ASSERT_EQ(isSorted(array), true);
}

TEST_F(CArraySortTest, IntrospectiveSortArrayHalfSorted)
{
	const unsigned int Capacity = 16384;
	int array[Capacity];

	printf("Filling the array half with sorted and half with random numbers\n");
	for (unsigned int i = 0; i < Capacity / 2; i++)
		array[i] = i;
	for (unsigned int i = Capacity / 2; i < Capacity; i++)
		array[i] = nc::random().integer(0, 10000);

	printf("Sorting the array with introspective sort\n");
	nctl::sort(nctl::begin(array), nctl::end(array));
	const bool sorted = nctl::isSorted(nctl::cBegin(array), nctl::cEnd(array));
	printf("The array is %s\n", sorted ? "sorted" : "not sorted");

	ASSERT_EQ(sorted, true);
	ASSERT_EQ(isSorted(array), true);
}

}
