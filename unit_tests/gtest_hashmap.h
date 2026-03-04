#ifndef GTEST_HASHMAP_H
#define GTEST_HASHMAP_H

#include <nctl/algorithms.h>
#include <nctl/HashMap.h>
#include <nctl/HashMapIterator.h>
#include "gtest/gtest.h"

namespace {

const unsigned int Capacity = 32;
const unsigned int Size = 10;
const int KeyValueDifference = 10;
template <class HashFunc> using HashMapType = nctl::HashMap<int, int, HashFunc>;
using HashMapTestType = HashMapType<nctl::FixedHashFunc<int>>;
using PairType = nctl::Pair<int, int>;

template <class HashFunc>
void initHashMap(HashMapType<HashFunc> &hashmap)
{
	for (unsigned int i = 0; i < Size; i++)
		hashmap[i] = i + KeyValueDifference;
}

template <class HashFunc>
void printHashMap(const HashMapType<HashFunc> &hashmap)
{
	unsigned int n = 0;

	for (typename HashMapType<HashFunc>::ConstIterator i = hashmap.begin(); i != hashmap.end(); ++i)
		printf("[%u] hash: %u, key: %d, value: %d\n", n++, i.hash(), i.key(), i.value());
	printf("\n");
}

template <class HashFunc>
unsigned int calcSize(const HashMapType<HashFunc> &hashmap)
{
	unsigned int length = 0;

	for (typename HashMapType<HashFunc>::ConstIterator i = hashmap.begin(); i != hashmap.end(); ++i)
		length++;

	return length;
}

template <class HashFunc>
void assertHashMapsAreEqual(const HashMapType<HashFunc> &hashmap1, const HashMapType<HashFunc> &hashmap2)
{
	typename HashMapType<HashFunc>::ConstIterator hashmap1It = hashmap1.begin();
	typename HashMapType<HashFunc>::ConstIterator hashmap2It = hashmap2.begin();
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
