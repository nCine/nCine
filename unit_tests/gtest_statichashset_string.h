#ifndef GTEST_STATICHASHSET_STRING_H
#define GTEST_STATICHASHSET_STRING_H

#include <nctl/StaticHashSet.h>
#include <nctl/StaticHashSetIterator.h>
#include <nctl/String.h>
#include "gtest/gtest.h"

namespace {

const unsigned int Capacity = 32;
const unsigned int Size = 6;
const char *Keys[Size] = { "A", "a", "B", "C", "AB", "BA" };

void initHashSet(nctl::StaticHashSet<nctl::String, Capacity> &strHashset)
{
	for (unsigned int i = 0; i < Size; i++)
		strHashset.insert(Keys[i]);
}

void printHashSet(nctl::StaticHashSet<nctl::String, Capacity> &strHashset)
{
	unsigned int n = 0;

	for (nctl::StaticHashSet<nctl::String, Capacity>::ConstIterator i = strHashset.begin(); i != strHashset.end(); ++i)
		printf("[%u] hash: %u, key: %s\n", n++, i.hash(), i.key().data());
	printf("\n");
}

unsigned int calcSize(const nctl::StaticHashSet<nctl::String, Capacity> &strHashset)
{
	unsigned int length = 0;

	for (typename nctl::StaticHashSet<nctl::String, Capacity>::ConstIterator i = strHashset.begin(); i != strHashset.end(); ++i)
		length++;

	return length;
}

void assertHashSetsAreEqual(const nctl::StaticHashSet<nctl::String, Capacity> &strHashset1, const nctl::StaticHashSet<nctl::String, Capacity> &strHashset2)
{
	nctl::StaticHashSet<nctl::String, Capacity>::ConstIterator strHashset1It = strHashset1.begin();
	nctl::StaticHashSet<nctl::String, Capacity>::ConstIterator strHashset2It = strHashset2.begin();
	while (strHashset1It != strHashset1.end())
	{
		ASSERT_EQ(strHashset1It.key(), strHashset2It.key());
		ASSERT_EQ(*strHashset1It, *strHashset2It);

		strHashset1It++;
		strHashset2It++;
	}
}

}

#endif
