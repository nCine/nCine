#ifndef GTEST_ATOMIC_H
#define GTEST_ATOMIC_H

#include <nctl/Atomic.h>
#include "gtest/gtest.h"

namespace {

const unsigned int NumThreads = 100;
const unsigned NumIterations = 1000;
const unsigned int ArrayLength = 8;

template <typename T>
void printValue(T value) {}

template <>
void printValue<int32_t>(int32_t value)
{
	printf("%d\n", value);
}

template <>
void printValue<uint32_t>(uint32_t value)
{
	printf("%u\n", value);
}

template <>
void printValue<int64_t>(int64_t value)
{
	printf("%ld\n", value);
}

template <>
void printValue<uint64_t>(uint64_t value)
{
	printf("%lu\n", value);
}

template <typename T>
void printHexValue(T value) {}

template <>
void printHexValue<int32_t>(int32_t value)
{
	printf("%x\n", value);
}

template <>
void printHexValue<uint32_t>(uint32_t value)
{
	printf("%x\n", value);
}

template <>
void printHexValue<int64_t>(int64_t value)
{
	printf("%lx\n", value);
}

template <>
void printHexValue<uint64_t>(uint64_t value)
{
	printf("%lx\n", value);
}

}

#endif
