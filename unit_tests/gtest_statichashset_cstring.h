#ifndef GTEST_STATICHASHSET_CSTRING_H
#define GTEST_STATICHASHSET_CSTRING_H

#include <nctl/StaticHashSet.h>
#include <nctl/StaticHashSetIterator.h>
#include "gtest/gtest.h"

namespace {

const unsigned int Capacity = 32;
const unsigned int Size = 6;
using HashSetCStrType = nctl::StaticHashSet<const char *, Capacity>;
const char *Keys[Size] = { "A", "a", "B", "C", "AB", "BA" };
const unsigned int KeyCapacity = 3;
/// A new set of C-style string keys, same in content but different in memory address
char KeysCopy[Size][KeyCapacity];

void initHashSet(HashSetCStrType &cstrHashset)
{
	for (unsigned int i = 0; i < Size; i++)
	{
		cstrHashset.insert(Keys[i]);
		strncpy(KeysCopy[i], Keys[i], KeyCapacity);
	}
}

void printHashSet(const HashSetCStrType &cstrHashset)
{
	unsigned int n = 0;

	for (HashSetCStrType::ConstIterator i = cstrHashset.begin(); i != cstrHashset.end(); ++i)
		printf("[%u] hash: %u, key: %s\n", n++, i.hash(), i.key());
	printf("\n");
}

unsigned int calcSize(const HashSetCStrType &cstrHashset)
{
	unsigned int length = 0;

	for (HashSetCStrType::ConstIterator i = cstrHashset.begin(); i != cstrHashset.end(); ++i)
		length++;

	return length;
}

}

#endif
