#ifndef GTEST_SPAN_H
#define GTEST_SPAN_H

#include <nctl/Span.h>
#include "gtest/gtest.h"

namespace nc = ncine;

namespace {

const unsigned int Capacity = 10;
const int FirstElement = 0;
using SpanType = nctl::Span<const int>;

void printSpan(const SpanType &span)
{
	printf("Size %u: ", span.size());
	for (unsigned int i = 0; i < span.size(); i++)
		printf("[%u]=%d ", i, span[i]);
	printf("\n");
}

}

#endif
