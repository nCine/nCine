#ifndef GTEST_SPARSESET_H
#define GTEST_SPARSESET_H

#include <nctl/algorithms.h>
#include <nctl/SparseSet.h>
#include <nctl/SparseSetIterator.h>
#include "gtest/gtest.h"

namespace {

const unsigned int Capacity = 16;
const unsigned int MaxValue = 32;
const unsigned int Size = 8;
using SparseSetTestType = nctl::SparseSet<int>;

void initSparseSet(SparseSetTestType &sparseset)
{
	for (unsigned int i = 0; i < Size; i++)
		sparseset.insert(i);
}

void printSparseSet(const SparseSetTestType &sparseset)
{
	unsigned int n = 0;

	for (typename SparseSetTestType::ConstIterator i = sparseset.begin(); i != sparseset.end(); ++i)
		printf("[%u] value: %d\n", n++, *i);
	printf("\n");
}

unsigned int calcSize(const SparseSetTestType &sparseset)
{
	unsigned int length = 0;

	for (typename SparseSetTestType::ConstIterator i = sparseset.begin(); i != sparseset.end(); ++i)
		length++;

	return length;
}

void assertSparseSetsAreEqual(const SparseSetTestType &sparseset1, const SparseSetTestType &sparseset2)
{
	typename SparseSetTestType::ConstIterator sparseset1It = sparseset1.begin();
	typename SparseSetTestType::ConstIterator sparseset2It = sparseset2.begin();
	while (sparseset1It != sparseset1.end())
	{
		ASSERT_EQ(*sparseset1It, *sparseset2It);

		sparseset1It++;
		sparseset2It++;
	}
}

}

#endif
