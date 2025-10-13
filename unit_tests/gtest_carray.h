#ifndef GTEST_CARRAY_H
#define GTEST_CARRAY_H

#include <nctl/ArrayIterator.h>
#include <ncine/Random.h>
#include "gtest/gtest.h"

namespace nc = ncine;

namespace {

const unsigned int Capacity = 10;
const int FirstElement = 0;

void printArray(const int *array)
{
	printf("Size: %u, ", Capacity);
	for (unsigned int i = 0; i < Capacity; i++)
		printf("[%u]=%d ", i, array[i]);
	printf("\n");
}

void initArray(int *array)
{
	int value = FirstElement;

	for (unsigned int i = 0; i < Capacity; i++)
		array[i] = value++;
}

void initArrayRandom(int *array)
{
	for (unsigned int i = 0; i < Capacity; i++)
		array[i] = nc::random().integer(0, 100);
}

bool isSorted(const int *array)
{
	for (unsigned int i = 0; i < Capacity - 1; i++)
	{
		if (array[i] > array[i + 1])
			return false;
	}

	return true;
}

bool isReverseSorted(const int *array)
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
