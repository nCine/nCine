#ifndef GTEST_HASHMAPLIST_H
#define GTEST_HASHMAPLIST_H

#include <nctl/algorithms.h>
#include <nctl/HashMapList.h>
#include <nctl/HashMapListIterator.h>
#include "gtest/gtest.h"

namespace {

const unsigned int Capacity = 32;
const unsigned int Size = 10;
const int KeyValueDifference = 10;
using HashMapTestType = nctl::HashMapList<int, int, nctl::FixedHashFunc<int>>;

template <class HashFunc>
void initHashMap(nctl::HashMapList<int, int, HashFunc> &hashmap)
{
	for (unsigned int i = 0; i < Size; i++)
		hashmap[i] = i + KeyValueDifference;
}

template <class HashFunc>
void printHashMap(const nctl::HashMapList<int, int, HashFunc> &hashmap)
{
	unsigned int n = 0;

	for (typename nctl::HashMapList<int, int, HashFunc>::ConstIterator i = hashmap.begin(); i != hashmap.end(); ++i)
		printf("[%u] hash: %u, key: %d, value: %d\n", n++, i.hash(), i.key(), i.value());
	printf("\n");
}

template <class HashFunc>
unsigned int calcSize(const nctl::HashMapList<int, int, HashFunc> &hashmap)
{
	unsigned int length = 0;

	for (typename nctl::HashMapList<int, int, HashFunc>::ConstIterator i = hashmap.begin(); i != hashmap.end(); ++i)
		length++;

	return length;
}

template <class HashFunc>
void assertHashMapsAreEqual(const nctl::HashMapList<int, int, HashFunc> &hashmap1, const nctl::HashMapList<int, int, HashFunc> &hashmap2)
{
	typename nctl::HashMapList<int, int, HashFunc>::ConstIterator hashmap1It = hashmap1.begin();
	typename nctl::HashMapList<int, int, HashFunc>::ConstIterator hashmap2It = hashmap2.begin();
	while (hashmap1It != hashmap1.end())
	{
		ASSERT_EQ(hashmap1It.key(), hashmap2It.key());
		ASSERT_EQ(*hashmap1It, *hashmap2It);

		hashmap1It++;
		hashmap2It++;
	}
}

}

#endif
