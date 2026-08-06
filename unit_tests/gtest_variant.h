#ifndef GTEST_VARIANT_H
#define GTEST_VARIANT_H

#include <nctl/Variant.h>
#include "gtest/gtest.h"

namespace nc = ncine;

namespace {

constexpr int DefaultValue = 5;
constexpr float DefaultFloatValue = 3.14f;

using IntFloatVariant = nctl::Variant<int, float>;

void printVariant(const IntFloatVariant &variant)
{
	if (variant.holds<int>())
		printf("(holds int: %d)\n", variant.get<int>());
	else if (variant.holds<float>())
		printf("(holds float: %f)\n", variant.get<float>());
}

}

#endif
