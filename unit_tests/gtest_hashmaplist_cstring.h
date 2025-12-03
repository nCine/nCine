#ifndef GTEST_HASHMAPLIST_CSTRING_H
#define GTEST_HASHMAPLIST_CSTRING_H

#include <nctl/HashMapList.h>
#include <nctl/HashMapListIterator.h>
#include "gtest/gtest.h"

namespace {

const unsigned int Capacity = 32;
const unsigned int Size = 6;
const char *Keys[Size] = { "A", "a", "B", "C", "AB", "BA" };
const unsigned int KeyCapacity = 3;
const char *Values[Size] = { "AAAA", "aaaa", "BBBB", "CCCC", "ABABABAB", "BABABABA" };
const unsigned int ValueCapacity = 9;
/// A new set of C-style string keys, same in content but different in memory address
char KeysCopy[Size][KeyCapacity];

void initHashMap(nctl::HashMapList<const char *, const char *> &cstrHashmap)
{
	for (unsigned int i = 0; i < Size; i++)
	{
		cstrHashmap[Keys[i]] = Values[i];
		strncpy(KeysCopy[i], Keys[i], KeyCapacity);
	}
}

void printHashMap(nctl::HashMapList<const char *, const char *> &cstrHashmap)
{
	unsigned int n = 0;

	for (nctl::HashMapList<const char *, const char *>::ConstIterator i = cstrHashmap.begin(); i != cstrHashmap.end(); ++i)
		printf("[%u] hash: %u, key: %s, value: %s\n", n++, i.hash(), i.key(), i.value());
	printf("\n");
}

unsigned int calcSize(const nctl::HashMapList<const char *, const char *> &cstrHashmap)
{
	unsigned int length = 0;

	for (typename nctl::HashMapList<const char *, const char *>::ConstIterator i = cstrHashmap.begin(); i != cstrHashmap.end(); ++i)
		length++;

	return length;
}

}

#endif
