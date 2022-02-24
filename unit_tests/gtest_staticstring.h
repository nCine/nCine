#ifndef GTEST_STATICSTRING_H
#define GTEST_STATICSTRING_H

#include <nctl/StaticString.h>
#include "gtest/gtest.h"

namespace {

const unsigned int Capacity = 16;
const char veryLongCString[] = "This_is_a_very_long_string_which_goes_beyond_the_default_capacity_of_a_string";
const unsigned int veryLongCStringLength = 77;

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

int findFirstCharacter(const char *string, unsigned int length, char character)
{
	int position = -1;

	for (unsigned int i = 0; i < length; i++)
	{
		if (string[i] == character)
		{
			position = i;
			break;
		}
	}

	return position;
}

int findLastCharacter(const char *string, unsigned int length, char character)
{
	int position = -1;

	for (unsigned int i = 0; i < length; i++)
	{
		if (string[i] == character)
			position = i;
	}

	return position;
}

int findFirstCharacterAfterIndex(const char *string, unsigned int length, char character, unsigned int index)
{
	int position = -1;

	for (unsigned int i = index + 1; i < length - index - 1; i++)
	{
		if (string[i] == character)
		{
			position = i;
			break;
		}
	}

	return position;
}

bool charactersInStringsAreEqual(const char *src, const char *dest, unsigned int srcChar, unsigned int numChar, unsigned int destChar)
{
	for (unsigned int i = 0; i < numChar; i++)
	{
		if (src[srcChar + i] != dest[destChar + i])
			return false;
	}

	return true;
}

}

#endif
