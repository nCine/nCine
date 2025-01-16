#ifndef GTEST_ARRAY_H
#define GTEST_ARRAY_H

#include <nctl/Array.h>
#include <ncine/Random.h>
#include "gtest/gtest.h"

namespace nc = ncine;

namespace {

const unsigned int Capacity = 10;
const int FirstElement = 0;

void printArray(const nctl::Array<int> &array)
{
	printf("Size %u: ", array.size());
	for (unsigned int i = 0; i < array.size(); i++)
		printf("[%u]=%d ", i, array[i]);
	printf("\n");
}

void initArray(nctl::Array<int> &array)
{
	int value = FirstElement;

	for (unsigned int i = 0; i < Capacity; i++)
		array.pushBack(value++);
}

void initArrayRandom(nctl::Array<int> &array)
{
	for (unsigned int i = 0; i < Capacity; i++)
		array.pushBack(nc::random().integer(0, 100));
}

bool isUnmodified(const nctl::Array<int> &array)
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

bool isSorted(const nctl::Array<int> &array)
{
	for (unsigned int i = 0; i < Capacity - 1; i++)
	{
		if (array[i] > array[i + 1])
			return false;
	}

	return true;
}

bool isReverseSorted(const nctl::Array<int> &array)
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
