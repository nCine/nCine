#ifndef GTEST_ALLOCATORS_H
#define GTEST_ALLOCATORS_H

#include <nctl/MallocAllocator.h>
#include <nctl/LinearAllocator.h>
#include <nctl/StackAllocator.h>
#include <nctl/PoolAllocator.h>
#include <nctl/FreeListAllocator.h>
#include <nctl/ProxyAllocator.h>
#include "gtest/gtest.h"

namespace {

const unsigned int NumElements = 16;
const unsigned int Capacity = 64;

struct ElementType
{
	unsigned int a;
	unsigned int b;
	float c;
	float d;

	ElementType()
	    : a(0), b(0), c(0.0f), d(0.0f) {}
	ElementType(unsigned int aa, unsigned int bb, float cc, float dd)
	    : a(aa), b(bb), c(cc), d(dd) {}
	bool operator==(const ElementType &element) const
	{
		return (a == element.a && b == element.b &&
		        c == element.c && d == element.d);
	}
};

class ElementTypeStructHashFunc
{
  public:
	unsigned int operator()(const ElementType &key) const { return key.a; }
};

const size_t ElementSize = sizeof(ElementType);
const size_t BufferSize = Capacity * ElementSize;

void fillElements(ElementType *ptr, unsigned int size)
{
	for (unsigned int i = 0; i < size; i++)
	{
		ptr[i].a = i;
		ptr[i].b = NumElements - i - 1;
		ptr[i].c = static_cast<float>(i);
		ptr[i].d = static_cast<float>(NumElements - i - 1);
	}
}

}

#endif
