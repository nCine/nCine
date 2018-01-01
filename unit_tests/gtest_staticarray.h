#ifndef GTEST_STATICARRAY_H
#define GTEST_STATICARRAY_H

#include "StaticArray.h"
#include "gtest/gtest.h"

namespace nc = ncine;

namespace {

const unsigned int Capacity = 10;
const int FirstElement = 0;

void printArray(const nc::StaticArray<int, Capacity> &array)
{
	printf("Size: %u, ", array.size());
	for (unsigned int i = 0; i < array.size(); i++)
		printf("[%u]=%d ", i, array[i]);
	printf("\n");
}

void initArray(nc::StaticArray<int, Capacity> &array)
{
	int value = FirstElement;

	for (unsigned int i = 0; i < Capacity; i++)
		array[i] = value++;
}

void initArrayRandom(nc::StaticArray<int, Capacity> &array)
{
	for (unsigned int i = 0; i < Capacity; i++)
		array[i] = rand() % 100;
}

bool isUnmodified(const nc::StaticArray<int, Capacity> &array)
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

bool isSorted(const nc::StaticArray<int, Capacity> &array)
{
	for (unsigned int i = 0; i < Capacity - 1; i++)
	{
		if (array[i] > array[i + 1])
			return false;
	}

		return true;
}

bool isReverseSorted(const nc::StaticArray<int, Capacity> &array)
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
