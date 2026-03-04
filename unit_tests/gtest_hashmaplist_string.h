#ifndef GTEST_HASHMAPLIST_STRING_H
#define GTEST_HASHMAPLIST_STRING_H

#include <nctl/HashMapList.h>
#include <nctl/HashMapListIterator.h>
#include <nctl/String.h>
#include "gtest/gtest.h"

namespace {

const unsigned int Capacity = 32;
const unsigned int Size = 6;
using HashMapStrType = nctl::HashMapList<nctl::String, nctl::String>;
using PairStrType = nctl::Pair<nctl::String, nctl::String>;
const char *Keys[Size] = { "A", "a", "B", "C", "AB", "BA" };
const char *Values[Size] = { "AAAA", "aaaa", "BBBB", "CCCC", "ABABABAB", "BABABABA" };

void initHashMap(HashMapStrType &strHashmap)
{
	for (unsigned int i = 0; i < Size; i++)
		strHashmap[Keys[i]] = Values[i];
}

void printHashMap(const HashMapStrType &strHashmap)
{
	unsigned int n = 0;

	for (HashMapStrType::ConstIterator i = strHashmap.begin(); i != strHashmap.end(); ++i)
		printf("[%u] hash: %u, key: %s, value: %s\n", n++, i.hash(), i.key().data(), i.value().data());
	printf("\n");
}

unsigned int calcSize(const HashMapStrType &strHashmap)
{
	unsigned int length = 0;

	for (HashMapStrType::ConstIterator i = strHashmap.begin(); i != strHashmap.end(); ++i)
		length++;

	return length;
}

void assertHashMapsAreEqual(const HashMapStrType &strHashmap1, const HashMapStrType &strHashmap2)
{
	HashMapStrType::ConstIterator strHashmap1It = strHashmap1.begin();
	HashMapStrType::ConstIterator strHashmap2It = strHashmap2.begin();
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
