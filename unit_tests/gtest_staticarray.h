#ifndef GTEST_STATICARRAY_H
#define GTEST_STATICARRAY_H

#include <nctl/StaticArray.h>
#include <ncine/Random.h>
#include "gtest/gtest.h"

namespace nc = ncine;

namespace {

const unsigned int Capacity = 10;
const int FirstElement = 0;

template <unsigned int C>
void printArray(const nctl::StaticArray<int, C> &array)
{
	printf("Size: %u, ", array.size());
	for (unsigned int i = 0; i < array.size(); i++)
		printf("[%u]=%d ", i, array[i]);
	printf("\n");
}

template <unsigned int C>
void initArray(nctl::StaticArray<int, C> &array)
{
	int value = FirstElement;

	for (unsigned int i = 0; i < C; i++)
		array.pushBack(value++);
}

template <unsigned int C>
void initArrayRandom(nctl::StaticArray<int, C> &array)
{
	for (unsigned int i = 0; i < C; i++)
		array.pushBack(nc::random().integer(0, 100));
}

template <unsigned int C>
bool isUnmodified(const nctl::StaticArray<int, C> &array)
{
	int value = FirstElement;

	for (unsigned int i = 0; i < C; i++)
	{
		if (array[i] != value)
			return false;

		value++;
	}

	return true;
}

template <unsigned int C>
bool isSorted(const nctl::StaticArray<int, C> &array)
{
	for (unsigned int i = 0; i < C - 1; i++)
	{
		if (array[i] > array[i + 1])
			return false;
	}

	return true;
}

template <unsigned int C>
bool isReverseSorted(const nctl::StaticArray<int, C> &array)
{
	for (unsigned int i = 0; i < C - 1; i++)
	{
		if (array[i] < array[i + 1])
			return false;
	}

	return true;
}

}

#endif
