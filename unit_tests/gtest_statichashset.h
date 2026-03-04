#ifndef GTEST_STATICHASHSET_H
#define GTEST_STATICHASHSET_H

#include <nctl/algorithms.h>
#include <nctl/StaticHashSet.h>
#include <nctl/StaticHashSetIterator.h>
#include "gtest/gtest.h"

namespace {

const unsigned int Capacity = 32;
const unsigned int Size = 10;
template <class HashFunc> using HashSetType = nctl::StaticHashSet<int, Capacity, HashFunc>;
using HashSetTestType = HashSetType<nctl::FixedHashFunc<int>>;

template <class HashFunc>
void initHashSet(HashSetType<HashFunc> &hashset)
{
	for (unsigned int i = 0; i < Size; i++)
		hashset.insert(i);
}

template <class HashFunc>
void printHashSet(const HashSetType<HashFunc> &hashset)
{
	unsigned int n = 0;

	for (typename HashSetType<HashFunc>::ConstIterator i = hashset.begin(); i != hashset.end(); ++i)
		printf("[%u] hash: %u, key: %d\n", n++, i.hash(), i.key());
	printf("\n");
}

template <class HashFunc>
unsigned int calcSize(const HashSetType<HashFunc> &hashset)
{
	unsigned int length = 0;

	for (typename HashSetType<HashFunc>::ConstIterator i = hashset.begin(); i != hashset.end(); ++i)
		length++;

	return length;
}

template <class HashFunc>
void assertHashSetsAreEqual(const HashSetType<HashFunc> &hashset1, const HashSetType<HashFunc> &hashset2)
{
	typename HashSetType<HashFunc>::ConstIterator hashset1It = hashset1.begin();
	typename HashSetType<HashFunc>::ConstIterator hashset2It = hashset2.begin();
	while (hashset1It != hashset1.end())
	{
		ASSERT_EQ(hashset1It.key(), hashset2It.key());

		hashset1It++;
		hashset2It++;
	}
}

}

#endif
