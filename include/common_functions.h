#ifndef NCINE_COMMON_FUNCTIONS
#define NCINE_COMMON_FUNCTIONS

#include <cstdlib> // for rand()

namespace ncine {

/// Swaps the content of two objects of the same type
template <class T>
inline void swap(T& a, T& b)
{
	T temp = a;
	a = b;
	b = temp;
}

/// Returns the minimum between two objects that support operator<
template <class T>
inline const T& min(const T& a, const T& b)
{
	return !(b < a) ? a : b;
}

/// Returns the maximum between two objects that support operator<
template<class T>
inline const T& max(const T& a, const T& b)
{
	return (a < b) ? b : a;
}

/// Returns a random float between x0 and x1
inline float randBetween(float x0, float x1)
{
	return x0 + (x1 - x0) * (rand() / float(RAND_MAX));
}

/// Quicksort implementation
template <class T>
void qSort(T& array, int start, int end)
{
	if (start < end)
	{
		int div = qSortPartition(array, start, end);
		qSort(array, start, div);
		qSort(array, div + 1, end);
	}
}

/// Partition function for quicksort
template <class T>
int qSortPartition(T& array, int start, int end)
{
	bool shouldQuit;
	int i, j;
	unsigned long int pivot;

	pivot = array[start]->sortKey();
	i = start - 1;
	j = end + 1;

	shouldQuit = false;

	do
	{
		do
		{
			j--;
		} while (array[j]->sortKey() > pivot);

		do
		{
			i++;
		} while (array[i]->sortKey() < pivot);

		if (i < j)
		{
			swap(array[i], array[j]);
		}
		else
		{
			shouldQuit = true;
		}
	} while (!shouldQuit);

	return j;
}

}

namespace nc = ncine;

#endif
