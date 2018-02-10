#ifndef GTEST_STRING_H
#define GTEST_STRING_H

#include "nctl/String.h"
#include "gtest/gtest.h"

namespace {

// Capacity is within the size of the small local buffer
const unsigned int Capacity = 16;

void printString(const nctl::String &string)
{
	printf("\"%s\" (capacity %u, length %u)\n", string.data(), string.capacity(), string.length());
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
