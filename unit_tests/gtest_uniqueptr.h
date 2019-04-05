#ifndef GTEST_UNIQUEPTR_H
#define GTEST_UNIQUEPTR_H

#include <nctl/UniquePtr.h>
#include <nctl/SharedPtr.h>
#include "gtest/gtest.h"

namespace {

const int Value = 5;
const unsigned int Size = 10;
const int FirstElement = 0;

void printPointer(const int *ptr)
{
	printf("address: %p, value: %d\n", static_cast<const int *>(ptr), ptr ? *ptr : 0);
}

void printPointer(const nctl::UniquePtr<int> &ptr)
{
	printf("address: %p, value: %d\n", static_cast<const int *>(ptr.get()), ptr ? *ptr : 0);
}

void printPointer(const nctl::UniquePtr<int[]> &ptr)
{
	printf("address: %p, first value: %d\n", static_cast<const int *>(ptr.get()), ptr ? ptr[0] : 0);
}

void printPointer(const nctl::SharedPtr<int> &ptr)
{
	printf("address: %p, counter: %d, value: %d\n", static_cast<const int *>(ptr.get()), ptr.useCount(), ptr ? *ptr : 0);
}

void initPtrArray(int *ptr, unsigned int size)
{
	int value = FirstElement;

	for (unsigned int i = 0; i < size; i++)
		ptr[i] = value++;
}

class Base
{
  public:
	virtual bool isBase() const { return true; }
	virtual const char* name() const { return "base"; }
};

class Derived : public Base
{
  public:
	bool isBase() const override { return false; }
	const char* name() const override { return "derived"; }
};

}

#endif
