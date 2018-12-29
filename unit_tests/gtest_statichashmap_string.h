#ifndef GTEST_STATICHASHMAP_STRING_H
#define GTEST_STATICHASHMAP_STRING_H

#include "nctl/StaticHashMap.h"
#include "nctl/StaticHashMapIterator.h"
#include "nctl/String.h"
#include "gtest/gtest.h"

namespace {

const unsigned int Capacity = 32;
const unsigned int Size = 6;
const char *Keys[Size] = { "A", "a", "B", "C", "AB", "BA" };
const char *Values[Size] = { "AAAA", "aaaa", "BBBB", "CCCC", "ABABABAB", "BABABABA" };

void initHashMap(nctl::StaticStringHashMap<nctl::String, Capacity> &strHashmap)
{
	for (unsigned int i = 0; i < Size; i++)
		strHashmap[Keys[i]] = Values[i];
}

void printHashMap(nctl::StaticStringHashMap<nctl::String, Capacity> &strHashmap)
{
	unsigned int n = 0;

	for (nctl::StaticStringHashMap<nctl::String, Capacity>::ConstIterator i = strHashmap.begin(); i != strHashmap.end(); ++i)
		printf("[%u] hash: %u, key: %s, value: %s\n", n++, i.hash(), i.key().data(), i.value().data());
	printf("\n");
}

unsigned int calcSize(const nctl::StaticStringHashMap<nctl::String, Capacity> &strHashmap)
{
	unsigned int length = 0;

	for (typename nctl::StaticStringHashMap<nctl::String, Capacity>::ConstIterator i = strHashmap.begin(); i != strHashmap.end(); ++i)
		length++;

	return length;
}

void assertHashMapsAreEqual(const nctl::StaticStringHashMap<nctl::String, Capacity> &strHashmap1, const nctl::StaticStringHashMap<nctl::String, Capacity> &strHashmap2)
{
	nctl::StaticStringHashMap<nctl::String, Capacity>::ConstIterator strHashmap1It = strHashmap1.begin();
	nctl::StaticStringHashMap<nctl::String, Capacity>::ConstIterator strHashmap2It = strHashmap2.begin();
	while (strHashmap1It != strHashmap1.end())
	{
		ASSERT_EQ(strHashmap1It.key(), strHashmap2It.key());
		ASSERT_EQ(*strHashmap1It, *strHashmap2It);

		strHashmap1It++;
		strHashmap2It++;
	}
}

}

#endif
