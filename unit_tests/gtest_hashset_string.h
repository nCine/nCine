#ifndef GTEST_HASHSET_STRING_H
#define GTEST_HASHSET_STRING_H

#include <nctl/HashSet.h>
#include <nctl/HashSetIterator.h>
#include <nctl/String.h>
#include "gtest/gtest.h"

namespace {

const unsigned int Capacity = 32;
const unsigned int Size = 6;
using HashSetStrType = nctl::HashSet<nctl::String>;
const char *Keys[Size] = { "A", "a", "B", "C", "AB", "BA" };

void initHashSet(HashSetStrType &strHashset)
{
	for (unsigned int i = 0; i < Size; i++)
		strHashset.insert(Keys[i]);
}

void printHashSet(const HashSetStrType &strHashset)
{
	unsigned int n = 0;

	for (HashSetStrType::ConstIterator i = strHashset.begin(); i != strHashset.end(); ++i)
		printf("[%u] hash: %u, key: %s\n", n++, i.hash(), i.key().data());
	printf("\n");
}

unsigned int calcSize(const HashSetStrType &strHashset)
{
	unsigned int length = 0;

	for (HashSetStrType::ConstIterator i = strHashset.begin(); i != strHashset.end(); ++i)
		length++;

	return length;
}

void assertHashSetsAreEqual(const HashSetStrType &strHashset1, const HashSetStrType &strHashset2)
{
	HashSetStrType::ConstIterator strHashset1It = strHashset1.begin();
	HashSetStrType::ConstIterator strHashset2It = strHashset2.begin();
	while (strHashset1It != strHashset1.end())
	{
		ASSERT_EQ(strHashset1It.key(), strHashset2It.key());

		strHashset1It++;
		strHashset2It++;
	}
}

}

#endif
