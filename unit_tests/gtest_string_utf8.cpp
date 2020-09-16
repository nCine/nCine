#include "gtest_string.h"

namespace {

class StringUTF8Test : public ::testing::Test
{
  public:
	StringUTF8Test()
	    : string_(Capacity) {}

  protected:
	nctl::String string_;
};

TEST_F(StringUTF8Test, Utf8CodePoints)
{
	const unsigned int codepoints[3] = { 0x3a9, 0x204b, 0x1d11e };
	const unsigned int codeUnits[3] = { 0xcea9, 0xe2818b, 0xf09d849e };
	string_ = "Ω⁋𝄞";
	printString("The UTF-8 encoded string: ", string_);

	unsigned int decodeCount = 0;
	for (unsigned int i = 0; i < string_.length();) // increments handled by UTF-8 decoding
	{
		unsigned int codePoint = nctl::String::InvalidUnicode;
		unsigned int utf8Encoded = nctl::String::InvalidUtf8;
		i += string_.utf8ToCodePoint(i, codePoint, &utf8Encoded);
		printf("Character %u at string position %u has code point 0x%x encoded as 0x%x\n", decodeCount, i, codePoint, utf8Encoded);
		ASSERT_EQ(codePoint, codepoints[decodeCount]);
		ASSERT_EQ(utf8Encoded, codeUnits[decodeCount]);
		decodeCount += (codePoint != nctl::String::InvalidUnicode) ? 1 : 0;
	}

	ASSERT_EQ(decodeCount, 3);
}

TEST_F(StringUTF8Test, Utf8CodePointsWithASCIIString)
{
	const unsigned int codepoints[3] = { 0x61, 0x62, 0x63 };
	const unsigned int codeUnits[3] = { 0x61, 0x62, 0x63 };
	string_ = "abc";
	printString("The ASCII string: ", string_);

	unsigned int decodeCount = 0;
	for (unsigned int i = 0; i < string_.length();) // increments handled by UTF-8 decoding
	{
		unsigned int codePoint = nctl::String::InvalidUnicode;
		unsigned int utf8Encoded = nctl::String::InvalidUtf8;
		i += string_.utf8ToCodePoint(i, codePoint, &utf8Encoded);
		printf("Character %u at string position %u has code point 0x%x encoded as 0x%x\n", decodeCount, i, codePoint, utf8Encoded);
		ASSERT_EQ(codePoint, codepoints[decodeCount]);
		ASSERT_EQ(utf8Encoded, codeUnits[decodeCount]);
		decodeCount += (codePoint != nctl::String::InvalidUnicode) ? 1 : 0;
	}

	ASSERT_EQ(decodeCount, 3);
}

TEST_F(StringUTF8Test, Utf8InvalidSequenceTwo)
{
	string_.setLength(3);
	string_[0] = 0xc3;
	string_[1] = 0x28;
	string_[2] = '\0';
	string_.setLength(2);
	printString("Invalid sequence of two code units: ", string_);

	bool firstCharacter = true;
	unsigned int decodeCount = 0;
	for (unsigned int i = 0; i < string_.length();) // increments handled by UTF-8 decoding
	{
		unsigned int codePoint = nctl::String::InvalidUnicode;
		unsigned int utf8Encoded = nctl::String::InvalidUtf8;
		i += string_.utf8ToCodePoint(i, codePoint, &utf8Encoded);
		printf("Character %u at string position %u has code point 0x%x encoded as 0x%x\n", decodeCount, i, codePoint, utf8Encoded);
		if (firstCharacter)
		{
			ASSERT_TRUE(codePoint == nctl::String::InvalidUnicode);
			ASSERT_TRUE(utf8Encoded == nctl::String::InvalidUtf8);
			ASSERT_EQ(i, 1);
			firstCharacter = false;
		}
		decodeCount += (codePoint != nctl::String::InvalidUnicode) ? 1 : 0;
	}

	ASSERT_EQ(decodeCount, 1);
}

TEST_F(StringUTF8Test, Utf8InvalidSequenceIdentifierTwo)
{
	string_.setLength(3);
	string_[0] = 0xa0;
	string_[1] = 0xa1;
	string_[2] = '\0';
	string_.setLength(2);
	printString("Invalid sequence identifier of two code units: ", string_);

	unsigned int decodeCount = 0;
	for (unsigned int i = 0; i < string_.length();) // increments handled by UTF-8 decoding
	{
		unsigned int codePoint = nctl::String::InvalidUnicode;
		unsigned int utf8Encoded = nctl::String::InvalidUtf8;
		i += string_.utf8ToCodePoint(i, codePoint, &utf8Encoded);
		printf("Character %u at string position %u has code point 0x%x encoded as 0x%x\n", decodeCount, i, codePoint, utf8Encoded);
		ASSERT_TRUE(codePoint == nctl::String::InvalidUnicode);
		ASSERT_TRUE(utf8Encoded == nctl::String::InvalidUtf8);
		decodeCount += (codePoint != nctl::String::InvalidUnicode) ? 1 : 0;
	}

	ASSERT_EQ(decodeCount, 0);
}

TEST_F(StringUTF8Test, Utf8InvalidSequenceMissingTwo)
{
	string_.setLength(2);
	string_[0] = 0xce;
	string_[1] = '\0';
	string_.setLength(1);
	printString("Sequence of two code units missing second one: ", string_);

	unsigned int decodeCount = 0;
	for (unsigned int i = 0; i < string_.length();) // increments handled by UTF-8 decoding
	{
		unsigned int codePoint = nctl::String::InvalidUnicode;
		unsigned int utf8Encoded = nctl::String::InvalidUtf8;
		i += string_.utf8ToCodePoint(i, codePoint, &utf8Encoded);
		printf("Character %u at string position %u has code point 0x%x encoded as 0x%x\n", decodeCount, i, codePoint, utf8Encoded);
		ASSERT_TRUE(codePoint == nctl::String::InvalidUnicode);
		ASSERT_TRUE(utf8Encoded == nctl::String::InvalidUtf8);
		ASSERT_EQ(i, 1);
		decodeCount += (codePoint != nctl::String::InvalidUnicode) ? 1 : 0;
	}

	ASSERT_EQ(decodeCount, 0);
}

TEST_F(StringUTF8Test, Utf8InvalidSequenceThreeSecond)
{
	string_.setLength(4);
	string_[0] = 0xe2;
	string_[1] = 0x28;
	string_[2] = 0xa1;
	string_[3] = '\0';
	string_.setLength(3);
	printString("Invalid sequence identifier of three code units (in second unit): ", string_);

	bool firstCharacter = true;
	unsigned int decodeCount = 0;
	for (unsigned int i = 0; i < string_.length();) // increments handled by UTF-8 decoding
	{
		unsigned int codePoint = nctl::String::InvalidUnicode;
		unsigned int utf8Encoded = nctl::String::InvalidUtf8;
		i += string_.utf8ToCodePoint(i, codePoint, &utf8Encoded);
		printf("Character %u at string position %u has code point 0x%x encoded as 0x%x\n", decodeCount, i, codePoint, utf8Encoded);
		if (firstCharacter)
		{
			ASSERT_TRUE(codePoint == nctl::String::InvalidUnicode);
			ASSERT_TRUE(utf8Encoded == nctl::String::InvalidUtf8);
			ASSERT_EQ(i, 1);
			firstCharacter = false;
		}
		decodeCount += (codePoint != nctl::String::InvalidUnicode) ? 1 : 0;
	}

	ASSERT_EQ(decodeCount, 1);
}

TEST_F(StringUTF8Test, Utf8InvalidSequenceThreeThird)
{
	string_.setLength(4);
	string_[0] = 0xe2;
	string_[1] = 0x82;
	string_[2] = 0x28;
	string_[3] = '\0';
	string_.setLength(3);
	printString("Invalid sequence identifier of three code units (in third unit): ", string_);

	bool firstCharacter = true;
	unsigned int decodeCount = 0;
	for (unsigned int i = 0; i < string_.length();) // increments handled by UTF-8 decoding
	{
		unsigned int codePoint = nctl::String::InvalidUnicode;
		unsigned int utf8Encoded = nctl::String::InvalidUtf8;
		i += string_.utf8ToCodePoint(i, codePoint, &utf8Encoded);
		printf("Character %u at string position %u has code point 0x%x encoded as 0x%x\n", decodeCount, i, codePoint, utf8Encoded);
		if (firstCharacter)
		{
			ASSERT_TRUE(codePoint == nctl::String::InvalidUnicode);
			ASSERT_TRUE(utf8Encoded == nctl::String::InvalidUtf8);
			ASSERT_EQ(i, 2);
			firstCharacter = false;
		}
		decodeCount += (codePoint != nctl::String::InvalidUnicode) ? 1 : 0;
	}

	ASSERT_EQ(decodeCount, 1);
}

TEST_F(StringUTF8Test, Utf8InvalidSequenceMissingThree)
{
	string_.setLength(3);
	string_[0] = 0xe2;
	string_[1] = 0x81;
	string_[2] = '\0';
	string_.setLength(2);
	printString("Sequence of three code units missing third one: ", string_);

	unsigned int decodeCount = 0;
	for (unsigned int i = 0; i < string_.length();) // increments handled by UTF-8 decoding
	{
		unsigned int codePoint = nctl::String::InvalidUnicode;
		unsigned int utf8Encoded = nctl::String::InvalidUtf8;
		i += string_.utf8ToCodePoint(i, codePoint, &utf8Encoded);
		printf("Character %u at string position %u has code point 0x%x encoded as 0x%x\n", decodeCount, i, codePoint, utf8Encoded);
		ASSERT_TRUE(codePoint == nctl::String::InvalidUnicode);
		ASSERT_TRUE(utf8Encoded == nctl::String::InvalidUtf8);
		ASSERT_EQ(i, 2);
		decodeCount += (codePoint != nctl::String::InvalidUnicode) ? 1 : 0;
	}

	ASSERT_EQ(decodeCount, 0);
}

TEST_F(StringUTF8Test, Utf8InvalidSequenceFourSecond)
{
	string_.setLength(5);
	string_[0] = 0xf0;
	string_[1] = 0x28;
	string_[2] = 0x8c;
	string_[3] = 0xbc;
	string_[4] = '\0';
	string_.setLength(4);
	printString("Invalid sequence identifier of four code units (in second unit): ", string_);

	bool firstCharacter = true;
	unsigned int decodeCount = 0;
	for (unsigned int i = 0; i < string_.length();) // increments handled by UTF-8 decoding
	{
		unsigned int codePoint = nctl::String::InvalidUnicode;
		unsigned int utf8Encoded = nctl::String::InvalidUtf8;
		i += string_.utf8ToCodePoint(i, codePoint, &utf8Encoded);
		printf("Character %u at string position %u has code point 0x%x encoded as 0x%x\n", decodeCount, i, codePoint, utf8Encoded);
		if (firstCharacter)
		{
			ASSERT_TRUE(codePoint == nctl::String::InvalidUnicode);
			ASSERT_TRUE(utf8Encoded == nctl::String::InvalidUtf8);
			ASSERT_EQ(i, 1);
			firstCharacter = false;
		}
		decodeCount += (codePoint != nctl::String::InvalidUnicode) ? 1 : 0;
	}

	ASSERT_EQ(decodeCount, 1);
}

TEST_F(StringUTF8Test, Utf8InvalidSequenceFourThird)
{
	string_.setLength(5);
	string_[0] = 0xf0;
	string_[1] = 0x90;
	string_[2] = 0x28;
	string_[3] = 0xbc;
	string_[4] = '\0';
	string_.setLength(4);
	printString("Invalid sequence identifier of four code units (in third unit): ", string_);

	bool firstCharacter = true;
	unsigned int decodeCount = 0;
	for (unsigned int i = 0; i < string_.length();) // increments handled by UTF-8 decoding
	{
		unsigned int codePoint = nctl::String::InvalidUnicode;
		unsigned int utf8Encoded = nctl::String::InvalidUtf8;
		i += string_.utf8ToCodePoint(i, codePoint, &utf8Encoded);
		printf("Character %u at string position %u has code point 0x%x encoded as 0x%x\n", decodeCount, i, codePoint, utf8Encoded);
		if (firstCharacter)
		{
			ASSERT_TRUE(codePoint == nctl::String::InvalidUnicode);
			ASSERT_TRUE(utf8Encoded == nctl::String::InvalidUtf8);
			ASSERT_EQ(i, 2);
			firstCharacter = false;
		}
		decodeCount += (codePoint != nctl::String::InvalidUnicode) ? 1 : 0;
	}

	ASSERT_EQ(decodeCount, 1);
}

TEST_F(StringUTF8Test, Utf8InvalidSequenceFourFourth)
{
	string_.setLength(5);
	string_[0] = 0xf0;
	string_[1] = 0x90;
	string_[2] = 0x8c;
	string_[3] = 0x28;
	string_[4] = '\0';
	string_.setLength(4);
	printString("Invalid sequence identifier of four code units (in fourth unit): ", string_);

	bool firstCharacter = true;
	unsigned int decodeCount = 0;
	for (unsigned int i = 0; i < string_.length();) // increments handled by UTF-8 decoding
	{
		unsigned int codePoint = nctl::String::InvalidUnicode;
		unsigned int utf8Encoded = nctl::String::InvalidUtf8;
		i += string_.utf8ToCodePoint(i, codePoint, &utf8Encoded);
		printf("Character %u at string position %u has code point 0x%x encoded as 0x%x\n", decodeCount, i, codePoint, utf8Encoded);
		if (firstCharacter)
		{
			ASSERT_TRUE(codePoint == nctl::String::InvalidUnicode);
			ASSERT_TRUE(utf8Encoded == nctl::String::InvalidUtf8);
			ASSERT_EQ(i, 3);
			firstCharacter = false;
		}
		decodeCount += (codePoint != nctl::String::InvalidUnicode) ? 1 : 0;
	}

	ASSERT_EQ(decodeCount, 1);
}

TEST_F(StringUTF8Test, Utf8InvalidSequenceMissingFour)
{
	string_.setLength(4);
	string_[0] = 0xf0;
	string_[1] = 0x9d;
	string_[2] = 0x84;
	string_[3] = '\0';
	string_.setLength(3);
	printString("Sequence of four code units missing fourth one: ", string_);

	unsigned int decodeCount = 0;
	for (unsigned int i = 0; i < string_.length();) // increments handled by UTF-8 decoding
	{
		unsigned int codePoint = nctl::String::InvalidUnicode;
		unsigned int utf8Encoded = nctl::String::InvalidUtf8;
		i += string_.utf8ToCodePoint(i, codePoint, &utf8Encoded);
		printf("Character %u at string position %u has code point 0x%x encoded as 0x%x\n", decodeCount, i, codePoint, utf8Encoded);
		ASSERT_TRUE(codePoint == nctl::String::InvalidUnicode);
		ASSERT_TRUE(utf8Encoded == nctl::String::InvalidUtf8);
		ASSERT_EQ(i, 3);
		decodeCount += (codePoint != nctl::String::InvalidUnicode) ? 1 : 0;
	}

	ASSERT_EQ(decodeCount, 0);
}

TEST_F(StringUTF8Test, Utf8BeyondStringEnd)
{
	string_.setLength(1);
	string_[0] = '\0';
	printString("Empty string: ", string_);

	unsigned int i = 5;
	unsigned int codePoint = nctl::String::InvalidUnicode;
	unsigned int utf8Encoded = nctl::String::InvalidUtf8;
	i += string_.utf8ToCodePoint(i, codePoint, &utf8Encoded);
	ASSERT_TRUE(codePoint == nctl::String::InvalidUnicode);
	ASSERT_TRUE(utf8Encoded == nctl::String::InvalidUtf8);
	ASSERT_EQ(i, 5);
}

}
