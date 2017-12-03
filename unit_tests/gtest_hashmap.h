#ifndef GTEST_HASHMAP_H
#define GTEST_HASHMAP_H

#include "nctl/HashMap.h"
#include "nctl/HashMapIterator.h"
#include "gtest/gtest.h"

namespace {

const unsigned int Capacity = 32;
const unsigned int Size = 10;
const int KeyValueDifference = 10;

template <class HashFunc>
void initHashMap(nctl::HashMap<int, int, HashFunc> &hashmap)
{
	for (unsigned int i = 0; i < Size; i++)
		hashmap[i] = i + KeyValueDifference;
}

template <class HashFunc>
void printHashMap(const nctl::HashMap<int, int, HashFunc> &hashmap)
{
	unsigned int n = 0;

	for (typename nctl::HashMap<int, int, HashFunc>::ConstIterator i = hashmap.begin(); i != hashmap.end(); ++i)
		printf("[%u] hash: %lu, key: %d, value: %d\n", n++, i.hash(), i.key(), i.value());
	printf("\n");
}

template <class HashFunc>
void assertHashMapsAreEqual(const nctl::HashMap<int, int, HashFunc> &hashmap1, const nctl::HashMap<int, int, HashFunc> &hashmap2)
{
	typename nctl::HashMap<int, int, HashFunc>::ConstIterator hashmap1It = hashmap1.begin();
	typename nctl::HashMap<int, int, HashFunc>::ConstIterator hashmap2It = hashmap2.begin();
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
