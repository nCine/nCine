#ifndef GTEST_PAIR_H
#define GTEST_PAIR_H

#include <nctl/Pair.h>
#include "gtest/gtest.h"

namespace nc = ncine;

namespace {

const int FirstElement = 0;
const int SecondElement = 1;
using PairType = nctl::Pair<int, int>;

void printPair(const PairType &pair)
{
	printf("Pair: <%d, %d>\n", pair.first, pair.second);
}

}

#endif
