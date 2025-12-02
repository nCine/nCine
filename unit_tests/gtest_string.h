#ifndef GTEST_STRING_H
#define GTEST_STRING_H

#include <nctl/String.h>
#include "gtest_string_common.h"
#include "gtest/gtest.h"

namespace {

// Capacity is within the size of the small local buffer
const unsigned int Capacity = nctl::String::SmallBufferSize;

void printString(const nctl::String &string)
{
	printf("\"%s\" (capacity %u, length %u)\n", string.data(), string.capacity(), string.length());
}

void printString(const char *message, const nctl::String &string)
{
	printf("%s", message);
	printString(string);
}

}

#endif
