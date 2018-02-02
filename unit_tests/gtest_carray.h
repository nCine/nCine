#ifndef GTEST_CARRAY_H
#define GTEST_CARRAY_H

#include "nctl/ArrayIterator.h"
#include "nctl/ReverseIterator.h"
#include "gtest/gtest.h"

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

}

#endif
