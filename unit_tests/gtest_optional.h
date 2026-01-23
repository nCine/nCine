#ifndef GTEST_OPTIONAL_H
#define GTEST_OPTIONAL_H

#include <nctl/Optional.h>
#include "gtest/gtest.h"

namespace nc = ncine;

namespace {

	constexpr uint32_t DefaultValue = 5;

	void printOptional(const nctl::Optional<uint32_t> &opt)
	{
		if (opt.hasValue())
			printf("(value is %u)\n", opt.value());
		else
			printf("(no value)\n");
	}

}

#endif
