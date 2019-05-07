#ifndef GTEST_HASHSETLIST_H
#define GTEST_HASHSETLIST_H

#include <nctl/algorithms.h>
#include <nctl/HashSetList.h>
#include <nctl/HashSetListIterator.h>
#include "gtest/gtest.h"

namespace {

const unsigned int Capacity = 32;
const unsigned int Size = 10;
using HashSetTestType = nctl::HashSetList<int, nctl::FixedHashFunc<int>>;

template <class HashFunc>
void initHashSet(nctl::HashSetList<int, HashFunc> &hashset)
{
	for (unsigned int i = 0; i < Size; i++)
		hashset.insert(i);
}

template <class HashFunc>
void printHashSet(const nctl::HashSetList<int, HashFunc> &hashset)
{
	unsigned int n = 0;

	for (typename nctl::HashSetList<int, HashFunc>::ConstIterator i = hashset.begin(); i != hashset.end(); ++i)
		printf("[%u] hash: %u, key: %d\n", n++, i.hash(), i.key());
	printf("\n");
}

template <class HashFunc>
unsigned int calcSize(const nctl::HashSetList<int, HashFunc> &hashset)
{
	unsigned int length = 0;

	for (typename nctl::HashSetList<int, HashFunc>::ConstIterator i = hashset.begin(); i != hashset.end(); ++i)
		length++;

	return length;
}

template <class HashFunc>
void assertHashSetsAreEqual(const nctl::HashSetList<int, HashFunc> &hashset1, const nctl::HashSetList<int, HashFunc> &hashset2)
{
	typename nctl::HashSetList<int, HashFunc>::ConstIterator hashset1It = hashset1.begin();
	typename nctl::HashSetList<int, HashFunc>::ConstIterator hashset2It = hashset2.begin();
	while (hashset1It != hashset1.end())
	{
		ASSERT_EQ(hashset1It.key(), hashset2It.key());

		hashset1It++;
		hashset2It++;
	}
}

}

#endif
