#ifndef GTEST_STATICSTRING_H
#define GTEST_STATICSTRING_H

#include <nctl/StaticString.h>
#include "gtest_string_common.h"
#include "gtest/gtest.h"

namespace {

const unsigned int Capacity = 16;

template <unsigned int C>
void printString(const nctl::StaticString<C> &string)
{
	printf("\"%s\" (capacity %u, length %u)\n", string.data(), string.capacity(), string.length());
}

template <unsigned int C>
void printString(const char *message, const nctl::StaticString<C> &string)
{
	printf("%s", message);
	printString(string);
}

}

#endif
