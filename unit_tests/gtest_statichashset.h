#ifndef GTEST_STATICHASHSET_H
#define GTEST_STATICHASHSET_H

#include <nctl/algorithms.h>
#include <nctl/StaticHashSet.h>
#include <nctl/StaticHashSetIterator.h>
#include "gtest/gtest.h"

namespace {

const unsigned int Capacity = 32;
const unsigned int Size = 10;
using HashSetTestType = nctl::StaticHashSet<int, Capacity, nctl::FixedHashFunc<int>>;

template <class HashFunc>
void initHashSet(nctl::StaticHashSet<int, Capacity, HashFunc> &hashset)
{
	for (unsigned int i = 0; i < Size; i++)
		hashset.insert(i);
}

template <class HashFunc>
void printHashSet(const nctl::StaticHashSet<int, Capacity, HashFunc> &hashset)
{
	unsigned int n = 0;

	for (typename nctl::StaticHashSet<int, Capacity, HashFunc>::ConstIterator i = hashset.begin(); i != hashset.end(); ++i)
		printf("[%u] hash: %u, key: %d\n", n++, i.hash(), i.key());
	printf("\n");
}

template <class HashFunc>
unsigned int calcSize(const nctl::StaticHashSet<int, Capacity, HashFunc> &hashset)
{
	unsigned int length = 0;

	for (typename nctl::StaticHashSet<int, Capacity, HashFunc>::ConstIterator i = hashset.begin(); i != hashset.end(); ++i)
		length++;

	return length;
}

template <class HashFunc>
void assertHashSetsAreEqual(const nctl::StaticHashSet<int, Capacity, HashFunc> &hashset1, const nctl::StaticHashSet<int, Capacity, HashFunc> &hashset2)
{
	typename nctl::StaticHashSet<int, Capacity, HashFunc>::ConstIterator hashset1It = hashset1.begin();
	typename nctl::StaticHashSet<int, Capacity, HashFunc>::ConstIterator hashset2It = hashset2.begin();
	while (hashset1It != hashset1.end())
	{
		ASSERT_EQ(hashset1It.key(), hashset2It.key());

		hashset1It++;
		hashset2It++;
	}
}

}

#endif
