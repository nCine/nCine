#ifndef GTEST_HASHMAP_STRING_H
#define GTEST_HASHMAP_STRING_H

#include "HashMap.h"
#include "HashMapIterator.h"
#include "ncString.h"
#include "gtest/gtest.h"

namespace nc = ncine;

namespace {

const unsigned int Capacity = 32;
const unsigned int Size = 6;
const char *Keys[Size] = { "A", "a", "B", "C", "AB", "BA" };
const char *Values[Size] = { "AAAA", "aaaa", "BBBB", "CCCC", "ABABABAB", "BABABABA" };

void initHashMap(nc::StringHashMap<nc::String> &strHashmap)
{
	for (unsigned int i = 0; i < Size; i++)
		strHashmap[Keys[i]] = Values[i];
}

void printHashMap(nc::StringHashMap<nc::String> &strHashmap)
{
	unsigned int n = 0;

	for (nc::StringHashMap<nc::String>::ConstIterator i = strHashmap.begin(); i != strHashmap.end(); ++i)
		printf("[%u] hash: %lu, key: %s, value: %s\n", n++, i.hash(), i.key().data(), i.value().data());
	printf("\n");
}

void assertHashMapsAreEqual(const nc::StringHashMap<nc::String> &strHashmap1, const nc::StringHashMap<nc::String> &strHashmap2)
{
	nc::StringHashMap<nc::String>::ConstIterator strHashmap1It = strHashmap1.begin();
	nc::StringHashMap<nc::String>::ConstIterator strHashmap2It = strHashmap2.begin();
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
