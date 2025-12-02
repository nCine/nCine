#ifndef GTEST_STRINGVIEW_H
#define GTEST_STRINGVIEW_H

#include <nctl/StringView.h>
#include <nctl/CString.h>
#include "gtest_string_common.h"
#include "gtest/gtest.h"

namespace {

void printStringView(const nctl::StringView &stringView)
{
	printf("\"%s\" (capacity %u, length %u)\n", stringView.data(), stringView.capacity(), stringView.length());
}

void printStringView(const char *message, const nctl::StringView &stringView)
{
	printf("%s", message);
	printStringView(stringView);
}

}

#endif
