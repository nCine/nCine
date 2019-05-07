#ifndef GTEST_HASHSET_H
#define GTEST_HASHSET_H

#include <nctl/algorithms.h>
#include <nctl/HashSet.h>
#include <nctl/HashSetIterator.h>
#include "gtest/gtest.h"

namespace {

const unsigned int Capacity = 32;
const unsigned int Size = 10;
using HashSetTestType = nctl::HashSet<int, nctl::FixedHashFunc<int>>;

template <class HashFunc>
void initHashSet(nctl::HashSet<int, HashFunc> &hashset)
{
	for (unsigned int i = 0; i < Size; i++)
		hashset.insert(i);
}

template <class HashFunc>
void printHashSet(const nctl::HashSet<int, HashFunc> &hashset)
{
	unsigned int n = 0;

	for (typename nctl::HashSet<int, HashFunc>::ConstIterator i = hashset.begin(); i != hashset.end(); ++i)
		printf("[%u] hash: %u, key: %d\n", n++, i.hash(), i.key());
	printf("\n");
}

template <class HashFunc>
unsigned int calcSize(const nctl::HashSet<int, HashFunc> &hashset)
{
	unsigned int length = 0;

	for (typename nctl::HashSet<int, HashFunc>::ConstIterator i = hashset.begin(); i != hashset.end(); ++i)
		length++;

	return length;
}

template <class HashFunc>
void assertHashSetsAreEqual(const nctl::HashSet<int, HashFunc> &hashset1, const nctl::HashSet<int, HashFunc> &hashset2)
{
	typename nctl::HashSet<int, HashFunc>::ConstIterator hashset1It = hashset1.begin();
	typename nctl::HashSet<int, HashFunc>::ConstIterator hashset2It = hashset2.begin();
	while (hashset1It != hashset1.end())
	{
		ASSERT_EQ(hashset1It.key(), hashset2It.key());

		hashset1It++;
		hashset2It++;
	}
}

}

#endif
