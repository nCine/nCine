#include "gtest_staticarray.h"
#include <nctl/algorithms.h>

namespace {

class StaticArraySortTest : public ::testing::Test
{
  protected:
	void SetUp() override { initArray(array_); }

	nctl::StaticArray<int, Capacity> array_;
};

TEST_F(StaticArraySortTest, QuickSort)
{
	printf("Filling the array with random numbers\n");
	array_.clear();
	initArrayRandom(array_);
	printArray(array_);

	printf("Sorting the array with quicksort\n");
	nctl::quicksort(array_.begin(), array_.end());
	printArray(array_);
	const bool sorted = nctl::isSorted(array_.begin(), array_.end());
	printf("The array is %s\n", sorted ? "sorted" : "not sorted");

	ASSERT_EQ(sorted, true);
	ASSERT_EQ(isSorted(array_), true);
}

TEST_F(StaticArraySortTest, QuickSortedUntil)
{
	const unsigned int position = 5;
	const int element = -1;
	printf("Inserting %d at position %u to break sorting\n", element, position);
	array_[position] = element;
	printArray(array_);

	nctl::StaticArray<int, Capacity>::ConstIterator unsorted = nctl::isSortedUntil(array_.begin(), array_.end());
	printf("The array is sorted until %d\n", *unsorted);

	ASSERT_EQ(*unsorted, element);
}

TEST_F(StaticArraySortTest, ReverseQuickSort)
{
	printf("Filling the array with random numbers\n");
	array_.clear();
	initArrayRandom(array_);
	printArray(array_);

	printf("Reverse sorting the array with quicksort\n");
	nctl::quicksortDesc(array_.begin(), array_.end());
	printArray(array_);
	const bool reverseSorted = nctl::isSorted(array_.begin(), array_.end(), nctl::IsGreater<int>);
	printf("The array is %s\n", reverseSorted ? "reverse sorted" : "not reverse sorted");

	ASSERT_EQ(reverseSorted, true);
	ASSERT_EQ(isReverseSorted(array_), true);
}

TEST_F(StaticArraySortTest, ReverseQuickSortedUntil)
{
	printf("Reverse sorting the array with quicksort\n");
	nctl::quicksortDesc(array_.begin(), array_.end());
	printArray(array_);

	const unsigned int position = 5;
	const int element = 99;
	printf("Inserting %d at position %u to break sorting\n", element, position);
	array_[position] = element;
	printArray(array_);

	nctl::StaticArray<int, Capacity>::ConstIterator unsorted = nctl::isSortedUntil(array_.begin(), array_.end(), nctl::IsGreater<int>);
	printf("The array is sorted until %d\n", *unsorted);

	ASSERT_EQ(*unsorted, element);
}

TEST_F(StaticArraySortTest, HeapSort)
{
	printf("Filling the array with random numbers\n");
	array_.clear();
	initArrayRandom(array_);
	printArray(array_);

	printf("Sorting the array with heapsort\n");
	nctl::heapsort(array_.begin(), array_.end());
	printArray(array_);
	const bool sorted = nctl::isSorted(array_.begin(), array_.end());
	printf("The array is %s\n", sorted ? "sorted" : "not sorted");

	ASSERT_EQ(sorted, true);
	ASSERT_EQ(isSorted(array_), true);
}

TEST_F(StaticArraySortTest, HeapSortedUntil)
{
	const unsigned int position = 5;
	const int element = -1;
	printf("Inserting %d at position %u to break sorting\n", element, position);
	array_[position] = element;
	printArray(array_);

	nctl::StaticArray<int, Capacity>::ConstIterator unsorted = nctl::isSortedUntil(array_.begin(), array_.end());
	printf("The array is sorted until %d\n", *unsorted);

	ASSERT_EQ(*unsorted, element);
}

TEST_F(StaticArraySortTest, ReverseHeapSort)
{
	printf("Filling the array with random numbers\n");
	array_.clear();
	initArrayRandom(array_);
	printArray(array_);

	printf("Reverse sorting the array with heapsort\n");
	nctl::heapsortDesc(array_.begin(), array_.end());
	printArray(array_);
	const bool reverseSorted = nctl::isSorted(array_.begin(), array_.end(), nctl::IsGreater<int>);
	printf("The array is %s\n", reverseSorted ? "reverse sorted" : "not reverse sorted");

	ASSERT_EQ(reverseSorted, true);
	ASSERT_EQ(isReverseSorted(array_), true);
}

TEST_F(StaticArraySortTest, ReverseHeapSortedUntil)
{
	printf("Reverse sorting the array with heapsort\n");
	nctl::heapsortDesc(array_.begin(), array_.end());
	printArray(array_);

	const unsigned int position = 5;
	const int element = 99;
	printf("Inserting %d at position %u to break sorting\n", element, position);
	array_[position] = element;
	printArray(array_);

	nctl::StaticArray<int, Capacity>::ConstIterator unsorted = nctl::isSortedUntil(array_.begin(), array_.end(), nctl::IsGreater<int>);
	printf("The array is sorted until %d\n", *unsorted);

	ASSERT_EQ(*unsorted, element);
}

TEST_F(StaticArraySortTest, InsertionSort)
{
	printf("Filling the array with random numbers\n");
	array_.clear();
	initArrayRandom(array_);
	printArray(array_);

	printf("Sorting the array with insertion sort\n");
	nctl::insertionsort(array_.begin(), array_.end());
	printArray(array_);
	const bool sorted = nctl::isSorted(array_.begin(), array_.end());
	printf("The array is %s\n", sorted ? "sorted" : "not sorted");

	ASSERT_EQ(sorted, true);
	ASSERT_EQ(isSorted(array_), true);
}

TEST_F(StaticArraySortTest, InsertionSortedUntil)
{
	const unsigned int position = 5;
	const int element = -1;
	printf("Inserting %d at position %u to break sorting\n", element, position);
	array_[position] = element;
	printArray(array_);

	nctl::StaticArray<int, Capacity>::ConstIterator unsorted = nctl::isSortedUntil(array_.begin(), array_.end());
	printf("The array is sorted until %d\n", *unsorted);

	ASSERT_EQ(*unsorted, element);
}

TEST_F(StaticArraySortTest, ReverseInsertionSort)
{
	printf("Filling the array with random numbers\n");
	array_.clear();
	initArrayRandom(array_);
	printArray(array_);

	printf("Reverse sorting the array with insertion sort\n");
	nctl::insertionsortDesc(array_.begin(), array_.end());
	printArray(array_);
	const bool reverseSorted = nctl::isSorted(array_.begin(), array_.end(), nctl::IsGreater<int>);
	printf("The array is %s\n", reverseSorted ? "reverse sorted" : "not reverse sorted");

	ASSERT_EQ(reverseSorted, true);
	ASSERT_EQ(isReverseSorted(array_), true);
}

TEST_F(StaticArraySortTest, ReverseInsertionSortedUntil)
{
	printf("Reverse sorting the array with insertion sort\n");
	nctl::insertionsortDesc(array_.begin(), array_.end());
	printArray(array_);

	const unsigned int position = 5;
	const int element = 99;
	printf("Inserting %d at position %u to break sorting\n", element, position);
	array_[position] = element;
	printArray(array_);

	nctl::StaticArray<int, Capacity>::ConstIterator unsorted = nctl::isSortedUntil(array_.begin(), array_.end(), nctl::IsGreater<int>);
	printf("The array is sorted until %d\n", *unsorted);

	ASSERT_EQ(*unsorted, element);
}

TEST_F(StaticArraySortTest, IntrospectiveSortSmallArray)
{
	const unsigned int Capacity = 15;
	nctl::StaticArray<int, Capacity> array;

	printf("Filling the array with random numbers\n");
	for (unsigned int i = 0; i < Capacity; i++)
		array.pushBack(nc::random().integer(0, 100));
	printArray(array);

	printf("Sorting the array with introspective sort (insertion sort only)\n");
	nctl::sort(array.begin(), array.end());
	printArray(array);
	const bool sorted = nctl::isSorted(array.begin(), array.end());
	printf("The array is %s\n", sorted ? "sorted" : "not sorted");

	ASSERT_EQ(sorted, true);
	ASSERT_EQ(isSorted(array), true);
}

TEST_F(StaticArraySortTest, IntrospectiveSortArray)
{
	const unsigned int Capacity = 16384;
	nctl::StaticArray<int, Capacity> array;

	printf("Filling the array with random numbers\n");
	for (unsigned int i = 0; i < Capacity; i++)
		array.pushBack(nc::random().integer(0, 10000));

	printf("Sorting the array with introspective sort\n");
	nctl::sort(array.begin(), array.end());
	const bool sorted = nctl::isSorted(array.begin(), array.end());
	printf("The array is %s\n", sorted ? "sorted" : "not sorted");

	ASSERT_EQ(sorted, true);
	ASSERT_EQ(isSorted(array), true);
}

TEST_F(StaticArraySortTest, IntrospectiveSortArraySorted)
{
	const unsigned int Capacity = 16384;
	nctl::StaticArray<int, Capacity> array;

	printf("Filling the array with sorted numbers\n");
	for (unsigned int i = 0; i < Capacity; i++)
		array.pushBack(i);

	printf("Sorting the array with introspective sort\n");
	nctl::sort(array.begin(), array.end());
	const bool sorted = nctl::isSorted(array.begin(), array.end());
	printf("The array is %s\n", sorted ? "sorted" : "not sorted");

	ASSERT_EQ(sorted, true);
	ASSERT_EQ(isSorted(array), true);
}

TEST_F(StaticArraySortTest, IntrospectiveSortArrayReverseSorted)
{
	const unsigned int Capacity = 16384;
	nctl::StaticArray<int, Capacity> array;

	printf("Filling the array with reverse sorted numbers\n");
	for (unsigned int i = 0; i < Capacity; i++)
		array.pushBack(Capacity - i - 1);

	printf("Sorting the array with introspective sort\n");
	nctl::sort(array.begin(), array.end());
	const bool sorted = nctl::isSorted(array.begin(), array.end());
	printf("The array is %s\n", sorted ? "sorted" : "not sorted");

	ASSERT_EQ(sorted, true);
	ASSERT_EQ(isSorted(array), true);
}

TEST_F(StaticArraySortTest, IntrospectiveSortArrayRepeatedValue)
{
	const unsigned int Capacity = 16384;
	nctl::StaticArray<int, Capacity> array;

	const int RepeatedValue = 42;
	printf("Filling the array with a repeated number\n");
	for (unsigned int i = 0; i < Capacity; i++)
		array.pushBack(RepeatedValue);

	printf("Sorting the array with introspective sort\n");
	nctl::sort(array.begin(), array.end());
	const bool sorted = nctl::isSorted(array.begin(), array.end());
	printf("The array is %s\n", sorted ? "sorted" : "not sorted");

	ASSERT_EQ(sorted, true);
	ASSERT_EQ(isSorted(array), true);
}

TEST_F(StaticArraySortTest, IntrospectiveSortArrayHalfSorted)
{
	const unsigned int Capacity = 16384;
	nctl::StaticArray<int, Capacity> array;

	printf("Filling the array half with sorted and half with random numbers\n");
	for (unsigned int i = 0; i < Capacity / 2; i++)
		array.pushBack(i);
	for (unsigned int i = Capacity / 2; i < Capacity; i++)
		array.pushBack(nc::random().integer(0, 10000));

	printf("Sorting the array with introspective sort\n");
	nctl::sort(array.begin(), array.end());
	const bool sorted = nctl::isSorted(array.begin(), array.end());
	printf("The array is %s\n", sorted ? "sorted" : "not sorted");

	ASSERT_EQ(sorted, true);
	ASSERT_EQ(isSorted(array), true);
}

}
