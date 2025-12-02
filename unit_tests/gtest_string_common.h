#ifndef GTEST_STRING_COMMON_H
#define GTEST_STRING_COMMON_H

namespace {

const char *String1Literal = "String1";
const unsigned int String1Length = 7;
char string1Array[String1Length + 1] = "String1";

const char *String2Literal = "String2";
const unsigned int String2Length = 7;
char string2Array[String2Length + 1] = "String2";

const char *VeryLongCStringLiteral = "This_is_a_very_long_string_which_goes_beyond_the_default_capacity_of_a_string";
const unsigned int VeryLongCStringLength = 77;

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
