#ifndef GTEST_BITSET_H
#define GTEST_BITSET_H

#include <nctl/BitSet.h>
#include <ncine/Random.h>
#include "gtest/gtest.h"

namespace nc = ncine;

namespace {

const uint32_t AllOne = 0xFFFFFFFF;
const uint32_t FifthBit = 16U;

template <class T>
void printBitSet(const nctl::BitSet<T> &bitset)
{
	for (int i = bitset.size() - 1; i >= 0; i--)
		printf("%s", bitset.test(i) ? "1" : "0");
	printf(" (%d bit) \n", bitset.size());
}

}

#endif
