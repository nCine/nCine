#ifndef GTEST_STATICARRAY_H
#define GTEST_STATICARRAY_H

#include <nctl/StaticArray.h>
#include <ncine/Random.h>
#include "gtest/gtest.h"

namespace nc = ncine;

namespace {

const unsigned int Capacity = 10;
const int FirstElement = 0;

void printArray(const nctl::StaticArray<int, Capacity> &array)
{
	printf("Size: %u, ", array.size());
	for (unsigned int i = 0; i < array.size(); i++)
		printf("[%u]=%d ", i, array[i]);
	printf("\n");
}

void initArray(nctl::StaticArray<int, Capacity> &array)
{
	int value = FirstElement;

	for (unsigned int i = 0; i < Capacity; i++)
		array[i] = value++;
}

void initArrayRandom(nctl::StaticArray<int, Capacity> &array)
{
	for (unsigned int i = 0; i < Capacity; i++)
		array[i] = nc::random().integer(0, 100);
}

bool isUnmodified(const nctl::StaticArray<int, Capacity> &array)
{
	int value = FirstElement;

	for (unsigned int i = 0; i < Capacity; i++)
	{
		if (array[i] != value)
			return false;

		value++;
	}

	return true;
}

bool isSorted(const nctl::StaticArray<int, Capacity> &array)
{
	for (unsigned int i = 0; i < Capacity - 1; i++)
	{
		if (array[i] > array[i + 1])
			return false;
	}

	return true;
}

bool isReverseSorted(const nctl::StaticArray<int, Capacity> &array)
{
	for (unsigned int i = 0; i < Capacity - 1; i++)
	{
		if (array[i] < array[i + 1])
			return false;
	}

	return true;
}

}

#endif
