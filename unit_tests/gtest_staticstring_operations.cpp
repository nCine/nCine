#include "gtest_staticstring.h"

namespace {

class StaticStringOperationsTest : public ::testing::Test
{
  protected:
	void SetUp() override { string_ = String1Literal; }

	nctl::StaticString<Capacity> string_;
};

TEST_F(StaticStringOperationsTest, AppendString)
{
	nctl::StaticString<Capacity> newString = "Append";
	printString("Creating a new string: ", newString);

	string_.append(newString);
	printString("Appending a new string to the first one: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), strnlen("String1Append", 14));
	ASSERT_STREQ(string_.data(), "String1Append");
}

TEST_F(StaticStringOperationsTest, AppendStringTruncate)
{
	nctl::StaticString<VeryLongCStringLength + 1> newString = VeryLongCStringLiteral;
	printString("Creating a new string: ", newString);

	string_.append(newString);
	printString("Appending a new string to the first one: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), strnlen("String1This_is_", Capacity));
	ASSERT_STREQ(string_.data(), "String1This_is_");
}

TEST_F(StaticStringOperationsTest, AppendCString)
{
	const char *newString = "Append";
	printf("Creating a new C string: %s\n", newString);

	string_.append(newString);
	printString("Appending a C string to the first one: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), strnlen("String1Append", 14));
	ASSERT_STREQ(string_.data(), "String1Append");
}

TEST_F(StaticStringOperationsTest, AppendCStringTruncate)
{
	printf("Using a very long C string: %s\n", VeryLongCStringLiteral);

	string_.append(VeryLongCStringLiteral);
	printString("Appending a C string to the first one: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), strnlen("String1This_is_", Capacity));
	ASSERT_STREQ(string_.data(), "String1This_is_");
}

TEST_F(StaticStringOperationsTest, Clear)
{
	string_.clear();
	printString("Clearing the string: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), 0u);
}

TEST_F(StaticStringOperationsTest, ImplicitCStringConstructorShort)
{
	nctl::StaticString<Capacity> newString = String2Literal;
	printString("Creating a new string from a C-style string assignment: ", newString);

	// Termination character is taken into account for capacity
	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), String2Length);
	ASSERT_STREQ(newString.data(), String2Literal);
}

TEST_F(StaticStringOperationsTest, ImplicitCStringConstructorLong)
{
	nctl::StaticString<Capacity> newString = "String2...String2";
	printString("Creating a new string from a C-style string assignment: ", newString);

	// Termination character is taken into account for capacity
	ASSERT_EQ(newString.length() + 1, newString.capacity());
	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_STREQ(newString.data(), "String2...Strin");
}

TEST_F(StaticStringOperationsTest, CopyConstruction)
{
	nctl::StaticString<Capacity> newString(string_);
	printString("Creating a new string as a copy of the first one: ", newString);

	ASSERT_EQ(newString.capacity(), string_.capacity());
	ASSERT_EQ(newString.length(), string_.length());
	ASSERT_STREQ(newString.data(), string_.data());

	printf("Appending a very long string to the first and the second string\n");
	string_.append(VeryLongCStringLiteral);
	newString.append(VeryLongCStringLiteral);
	ASSERT_EQ(newString.capacity(), string_.capacity());
	ASSERT_EQ(newString.length(), string_.length());
	ASSERT_STREQ(newString.data(), string_.data());
}

TEST_F(StaticStringOperationsTest, CopyConstructionDifferentCapacity)
{
	nctl::StaticString<Capacity * 2> newString(string_);
	printString("Creating a new string as a copy of the first one: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity * 2);
	ASSERT_EQ(newString.length(), string_.length());
	ASSERT_STREQ(newString.data(), string_.data());

	printf("Appending a very long string to the second string\n");
	newString.append(VeryLongCStringLiteral);
	ASSERT_EQ(newString.capacity(), Capacity * 2);
	ASSERT_EQ(newString.length(), Capacity * 2 - 1);
	ASSERT_STREQ(newString.data(), "String1This_is_a_very_long_stri");
}

TEST_F(StaticStringOperationsTest, AssignmentOperator)
{
	nctl::StaticString<Capacity> newString;
	newString = string_;
	printString("Filling a new string with the assignment operator: ", newString);

	ASSERT_EQ(newString.capacity(), string_.capacity());
	ASSERT_EQ(newString.length(), string_.length());
	ASSERT_STREQ(newString.data(), string_.data());
}

TEST_F(StaticStringOperationsTest, AssignmentOperatorTruncate)
{
	nctl::StaticString<Capacity> longString(VeryLongCStringLiteral);
	string_ = longString;
	printString("Filling a string with the assignment operator: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), string_.capacity() - 1);
	ASSERT_STREQ(string_.data(), "This_is_a_very_");
}

TEST_F(StaticStringOperationsTest, AssignmentOperatorShorter)
{
	nctl::StaticString<Capacity> newString(String1Literal);
	string_ = "LongString1";
	string_ = newString;
	printString("Shrinking a string with the assignment operator: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), newString.length());
	ASSERT_STREQ(string_.data(), String1Literal);
}

TEST_F(StaticStringOperationsTest, AssignmentOperatorDifferentCapacity)
{
	nctl::StaticString<Capacity * 2> newString;
	newString = string_;
	printString("Filling a new string with the assignment operator: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity * 2);
	ASSERT_EQ(newString.length(), string_.length());
	ASSERT_STREQ(newString.data(), string_.data());
}

TEST_F(StaticStringOperationsTest, AssignmentOperatorTruncateDifferentCapacity)
{
	nctl::StaticString<VeryLongCStringLength + 1> longString(VeryLongCStringLiteral);
	string_ = longString;
	printString("Filling a string with the assignment operator: ", string_);
	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), string_.capacity() - 1);
	ASSERT_STREQ(string_.data(), "This_is_a_very_");
}

TEST_F(StaticStringOperationsTest, AssignmentOperatorShorterDifferentCapacity)
{
	nctl::StaticString<Capacity * 2> newString(String1Literal);
	string_ = "LongString1";
	string_ = newString;
	printString("Shrinking a string with the assignment operator: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), newString.length());
	ASSERT_STREQ(string_.data(), String1Literal);
}

TEST_F(StaticStringOperationsTest, SelfAssignment)
{
	string_ = string_;
	printString("Assigning the string to itself with the assignment operator: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), String1Length);
	ASSERT_STREQ(string_.data(), String1Literal);
}

TEST_F(StaticStringOperationsTest, AssignLongCStringTruncate)
{
	nctl::StaticString<Capacity> newString;
	// The C-style string is longer than default string capacity
	newString = "String2...String2";
	printString("Creating a new string with the assignment operator from a C-style string: ", newString);

	// Termination character is taken into account for capacity
	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length() + 1, Capacity);
	ASSERT_STREQ(newString.data(), "String2...Strin");
}

TEST_F(StaticStringOperationsTest, ConstructByConcatenation)
{
	// The second string is big enough to hold the result from `operator+()` but with a different capacity
	nctl::StaticString<Capacity * 3> secondString = "String2...String2";
	printString("Creating a second string: ", secondString);

	nctl::StaticString<Capacity * 2> thirdString = secondString + string_;
	printString("Creating a third string by concatenating the second and the first one: ", thirdString);

	// Termination character is taken into account for capacity
	ASSERT_EQ(thirdString.capacity(), Capacity * 2);
	ASSERT_EQ(thirdString.length(), strnlen("String2...String2String1", 25));
	ASSERT_STREQ(thirdString.data(), "String2...String2String1");
}

TEST_F(StaticStringOperationsTest, ConstructByConcatenationTruncate)
{
	nctl::StaticString<Capacity> secondString = "String2...String2";
	printString("Creating a second string: ", secondString);

	nctl::StaticString<Capacity> thirdString = secondString + string_;
	printString("Creating a third string by concatenating the second and the first one: ", thirdString);

	// Termination character is taken into account for capacity
	ASSERT_EQ(thirdString.length() + 1, thirdString.capacity());
	ASSERT_EQ(thirdString.capacity(), Capacity);
	ASSERT_EQ(thirdString.length(), strnlen("String2...Strin", Capacity));
	ASSERT_STREQ(thirdString.data(), "String2...Strin");
}

TEST_F(StaticStringOperationsTest, ConstructByConcatenationWithCStringAsFirst)
{
	// The second string is big enough to hold the result from `operator+()` but with a different capacity
	nctl::StaticString<Capacity * 3> firstString = "String1";
	const char *secondString = "String2...String2";
	printf("Creating a second string: %s\n", secondString);

	// using `friend StaticString operator+(const char *cString, const StaticString &string)`
	nctl::StaticString<Capacity * 2> thirdString = secondString + firstString;
	printString("Creating a third string by concatenating the second and the first one: ", thirdString);

	// Termination character is taken into account for capacity
	ASSERT_EQ(thirdString.capacity(), Capacity * 2);
	ASSERT_EQ(thirdString.length(), strnlen("String2...String2String1", 25));
	ASSERT_STREQ(thirdString.data(), "String2...String2String1");
}

TEST_F(StaticStringOperationsTest, ConstructByConcatenationWithCStringAsFirstTruncate)
{
	const char *secondString = "String2...String2";
	printf("Creating a second string: %s\n", secondString);

	// using `friend StaticString operator+(const char *cString, const StaticString &string)`
	nctl::StaticString<Capacity> thirdString = secondString + string_;
	printString("Creating a third string by concatenating the second and the first one: ", thirdString);

	// Termination character is taken into account for capacity
	ASSERT_EQ(thirdString.length() + 1, thirdString.capacity());
	ASSERT_EQ(thirdString.capacity(), Capacity);
	ASSERT_EQ(thirdString.length(), strnlen("String2...Strin", Capacity));
	ASSERT_STREQ(thirdString.data(), "String2...Strin");
}

TEST_F(StaticStringOperationsTest, ConstructByConcatenationWithCStringAsSecond)
{
	// The second string is big enough to hold the result from `operator+()` but with a different capacity
	nctl::StaticString<Capacity * 3> firstString = "String1";
	const char *secondString = "String2...String2";
	printf("Creating a second string: %s\n", secondString);

	// using `StaticString StaticString::operator+(const char *cString) const`
	nctl::StaticString<Capacity * 2> thirdString = firstString + secondString;
	printString("Creating a third string by concatenating the first and the second one: ", thirdString);

	// Termination character is taken into account for capacity
	ASSERT_EQ(thirdString.capacity(), Capacity * 2);
	ASSERT_EQ(thirdString.length(), strnlen("String1String2...String2", 25));
	ASSERT_STREQ(thirdString.data(), "String1String2...String2");
}

TEST_F(StaticStringOperationsTest, ConstructByConcatenationWithCStringAsSecondTruncate)
{
	const char *secondString = "String2...String2";
	printf("Creating a second string: %s\n", secondString);

	// using `StaticString StaticString::operator+(const char *cString) const`
	nctl::StaticString<Capacity> thirdString = string_ + secondString;
	printString("Creating a third string by concatenating the first and the second one: ", thirdString);

	// Termination character is taken into account for capacity
	ASSERT_EQ(thirdString.length() + 1, thirdString.capacity());
	ASSERT_EQ(thirdString.capacity(), Capacity);
	ASSERT_STREQ(thirdString.data(), "String1String2.");
}

TEST_F(StaticStringOperationsTest, Format)
{
	string_.format("String%d", 2);
	printString("Resetting the string to a formatted one: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), strnlen("String2", 8));
	ASSERT_STREQ(string_.data(), "String2");
}

TEST_F(StaticStringOperationsTest, FormatAndTruncate)
{
	string_.format("VeryLongStringFormatting%d", 2);
	printString("Resetting the string to a long formatted one with truncation: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), strnlen("VeryLongStringF", Capacity));
	ASSERT_STREQ(string_.data(), "VeryLongStringF");
}

TEST_F(StaticStringOperationsTest, FormatAppend)
{
	string_.formatAppend("String%d", 2);
	printString("Appending a formatted string to the first one: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), strnlen("String1String2", 15));
	ASSERT_STREQ(string_.data(), "String1String2");
}

TEST_F(StaticStringOperationsTest, FormatAppendAndTruncate)
{
	string_.formatAppend("VeryLongStringFormatting%d", 2);
	printString("Appending a long formatted string to the first one with truncation: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), strnlen("String1VeryLong", 15));
	ASSERT_STREQ(string_.data(), "String1VeryLong");
}

TEST_F(StaticStringOperationsTest, SetLengthShrink)
{
	const unsigned int length = string_.length();
	string_.data()[length - 1] = '\0';
	const unsigned int newLength = string_.setLength(length - 1);
	printString("Removing a single character by modifying the string length: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), length - 1);
	ASSERT_EQ(newLength, length - 1);
	ASSERT_STREQ(string_.data(), "String");
}

TEST_F(StaticStringOperationsTest, SetLengthExpand)
{
	const unsigned int length = string_.length();
	string_.data()[length + 0] = '2';
	string_.data()[length + 1] = '3';
	string_.data()[length + 2] = '\0';
	const unsigned int newLength = string_.setLength(length + 2);
	printString("Appending a couple characters by accessing the data array: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), length + 2);
	ASSERT_EQ(newLength, length + 2);
	ASSERT_STREQ(string_.data(), "String123");
}

TEST_F(StaticStringOperationsTest, SetLengthBeyondCapacity)
{
	const unsigned int newLength = string_.setLength(Capacity + 1);
	printString("Trying to set a length beyond string capacity: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), Capacity - 1);
	ASSERT_EQ(newLength, Capacity - 1);
	ASSERT_STREQ(string_.data(), String1Literal);
}

TEST_F(StaticStringOperationsTest, FindFirstCharacter)
{
	string_ += "String2";
	printString("Appending a string to the first one: ", string_);

	const char character = 'S';
	const int position = string_.findFirstChar(character);
	printf("First position of the character '%c' in the string: %d\n", character, position);

	ASSERT_GE(position, 0);
	ASSERT_EQ(string_[static_cast<unsigned int>(position)], character);
	ASSERT_EQ(findFirstCharacter(string_.data(), string_.length(), character), position);
}

TEST_F(StaticStringOperationsTest, FindFirstNonExistentCharacter)
{
	const char character = 'z';
	const int position = string_.findFirstChar(character);
	printf("First position of the character '%c' in the string: %d\n", character, position);

	ASSERT_EQ(position, -1);
}

TEST_F(StaticStringOperationsTest, FindLastCharacter)
{
	string_ += "String2";
	printString("Appending a string to the first one: ", string_);

	const char character = 'S';
	const int position = string_.findLastChar(character);
	printf("Last position of the character '%c' in the string: %d\n", character, position);

	ASSERT_GE(position, 0);
	ASSERT_EQ(string_[static_cast<unsigned int>(position)], character);
	ASSERT_EQ(findLastCharacter(string_.data(), string_.length(), character), position);
}

TEST_F(StaticStringOperationsTest, FindLastNonExistentCharacter)
{
	const char character = 'z';
	const int position = string_.findLastChar(character);
	printf("Last position of the character '%c' in the string: %d\n", character, position);

	ASSERT_EQ(position, -1);
}

TEST_F(StaticStringOperationsTest, FindFirstCharacterAfterIndex)
{
	string_ += "String2";
	printString("Appending a string to the first one: ", string_);

	const char character = 't';
	const unsigned int index = 1;
	const int position = string_.findFirstCharAfterIndex(character, index);
	printf("Position of the character '%c' after position %u in the string: %d\n", character, index, position);

	ASSERT_GE(position, 0);
	ASSERT_EQ(string_[static_cast<unsigned int>(position)], character);
	ASSERT_EQ(findFirstCharacterAfterIndex(string_.data(), string_.length(), character, index), position);
}

TEST_F(StaticStringOperationsTest, FindFirstNonExistentCharacterAfterIndex)
{
	const char character = 'z';
	const unsigned int index = 1;
	const int position = string_.findFirstCharAfterIndex(character, index);
	printf("Position of the character '%c' after position %u in the string: %d\n", character, index, position);

	ASSERT_EQ(position, -1);
}

TEST_F(StaticStringOperationsTest, FindFirstCharacterAfterIndexBeyondLength)
{
	const char character = 'S';
	const unsigned int index = string_.length();
	const int position = string_.findFirstCharAfterIndex(character, index);
	printf("Position of the character '%c' after position %u in the string: %d\n", character, index, position);

	ASSERT_EQ(position, -1);
}

TEST_F(StaticStringOperationsTest, FindString)
{
	const unsigned int oldLength = string_.length();

	nctl::StaticString<Capacity> newString = String2Literal;
	printString("Creating a new string: ", newString);
	string_ += newString;
	printString("Appending the new string to the first one: ", string_);

	const int position = string_.find(newString);
	printf("Position of sub-string \"%s\" inside string \"%s\": %d\n", newString.data(), string_.data(), position);
	ASSERT_EQ(position, static_cast<int>(oldLength));
}

TEST_F(StaticStringOperationsTest, FindStringDifferentCapacity)
{
	const unsigned int oldLength = string_.length();

	nctl::StaticString<Capacity * 2> newString = String2Literal;
	printString("Creating a new string: ", newString);
	string_ += newString;
	printString("Appending the new string to the first one: ", string_);

	const int position = string_.find(newString);
	printf("Position of sub-string \"%s\" inside string \"%s\": %d\n", newString.data(), string_.data(), position);
	ASSERT_EQ(position, static_cast<int>(oldLength));
}

TEST_F(StaticStringOperationsTest, FindNonExistentString)
{
	nctl::StaticString<Capacity> newString = String2Literal;
	printString("Creating a new string: ", newString);

	const int position = string_.find(newString);
	printf("Position of sub-string \"%s\" inside string \"%s\": %d\n", newString.data(), string_.data(), position);
	ASSERT_EQ(position, -1);
}

TEST_F(StaticStringOperationsTest, FindNonExistentStringDifferentCapacity)
{
	nctl::StaticString<Capacity * 2> newString = String2Literal;
	printString("Creating a new string: ", newString);

	const int position = string_.find(newString);
	printf("Position of sub-string \"%s\" inside string \"%s\": %d\n", newString.data(), string_.data(), position);
	ASSERT_EQ(position, -1);
}

TEST_F(StaticStringOperationsTest, FindCString)
{
	const unsigned int oldLength = string_.length();

	const char *newString = String2Literal;
	printf("Creating a new string: %s\n", newString);
	string_ += newString;
	printString("Appending the new string to the first one: ", string_);

	const int position = string_.find(newString);
	printf("Position of sub-string \"%s\" inside string \"%s\": %d\n", newString, string_.data(), position);
	ASSERT_EQ(position, static_cast<int>(oldLength));
}

TEST_F(StaticStringOperationsTest, FindNonExistentCString)
{
	const char *newString = String2Literal;
	printf("Creating a new string: %s\n", newString);

	const int position = string_.find(newString);
	printf("Position of sub-string \"%s\" inside string \"%s\": %d\n", newString, string_.data(), position);
	ASSERT_EQ(position, -1);
}

TEST_F(StaticStringOperationsTest, CopyCharacters)
{
	nctl::StaticString<Capacity> srcString = "0123456";
	printString("Creating a new source string: ", srcString);
	nctl::StaticString<Capacity> destString = "abcdefghil";
	printString("Creating a new destination string: ", destString);

	const unsigned int srcChar = 4;
	const unsigned int numChar = 3;
	const unsigned int destChar = 2;
	const unsigned int numCopied = destString.replace(srcString, srcChar, numChar, destChar);
	printf("Copying %u characters from position %u of the source string to position %u of the destination one: ", numChar, srcChar, destChar);
	printString(destString);

	ASSERT_NE(srcString.length(), destString.length()); // no truncation
	ASSERT_EQ(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcString.data(), destString.data(), srcChar, numCopied, destChar));
}

TEST_F(StaticStringOperationsTest, CopyCharactersDifferentCapacity)
{
	nctl::StaticString<Capacity * 2> srcString = "0123456";
	printString("Creating a new source string: ", srcString);
	nctl::StaticString<Capacity> destString = "abcdefghil";
	printString("Creating a new destination string: ", destString);

	const unsigned int srcChar = 4;
	const unsigned int numChar = 3;
	const unsigned int destChar = 2;
	const unsigned int numCopied = destString.replace(srcString, srcChar, numChar, destChar);
	printf("Copying %u characters from position %u of the source string to position %u of the destination one: ", numChar, srcChar, destChar);
	printString(destString);

	ASSERT_NE(srcString.length(), destString.length()); // no truncation
	ASSERT_EQ(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcString.data(), destString.data(), srcChar, numCopied, destChar));
}

TEST_F(StaticStringOperationsTest, CopyCharactersToBeginning)
{
	nctl::StaticString<Capacity> srcString = "0123456";
	printString("Creating a new source string: ", srcString);
	nctl::StaticString<Capacity> destString = "abcdefghil";
	printString("Creating a new destination string: ", destString);

	const unsigned int srcChar = 4;
	const unsigned int numChar = 3;
	const unsigned int numCopied = destString.assign(srcString, srcChar, numChar);
	printf("Copying %u characters from position %u of the source string to position 0 of the destination one: ", numChar, srcChar);
	printString(destString);

	ASSERT_EQ(destString.length(), numCopied); // truncation
	ASSERT_EQ(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcString.data(), destString.data(), srcChar, numCopied, 0));
}

TEST_F(StaticStringOperationsTest, CopyAllCharactersToBeginning)
{
	nctl::StaticString<Capacity> srcString = "0123456";
	printString("Creating a new source string: ", srcString);
	nctl::StaticString<Capacity> destString = "abcdefghil";
	printString("Creating a new destination string: ", destString);

	const unsigned int numCopied = destString.assign(srcString);
	printf("Copying %u characters from position 0 of the source string to position 0 of the destination one: ", srcString.length());
	printString(destString);

	ASSERT_EQ(srcString.length(), destString.length()); // truncation
	ASSERT_EQ(numCopied, srcString.length());
	ASSERT_TRUE(charactersInStringsAreEqual(srcString.data(), destString.data(), 0, srcString.length(), 0));
}

TEST_F(StaticStringOperationsTest, CopyCharactersFromBeyondEnd)
{
	nctl::StaticString<Capacity> srcString = "0123456";
	printString("Creating a new source string: ", srcString);
	nctl::StaticString<Capacity> destString = "abcdefghil";
	printString("Creating a new destination string: ", destString);

	const unsigned int srcChar = srcString.length() + 1; // beyond the end of source
	const unsigned int numChar = 2;
	const unsigned int destChar = 0;
	const unsigned int numCopied = destString.replace(srcString, srcChar, numChar, destChar);
	printString("Trying to copy a character from beyond the end of the source string into the destination one: ", destString);

	ASSERT_NE(srcString.length(), destString.length()); // no truncation
	ASSERT_EQ(numCopied, 0u);
	ASSERT_NE(numCopied, numChar);
}

TEST_F(StaticStringOperationsTest, CopyCharactersToBeyondEndFixed)
{
	nctl::StaticString<Capacity> srcString = "0123456";
	printString("Creating a new source string: ", srcString);
	nctl::StaticString<Capacity> destString("abcdefghilmn");
	printString("Creating a new destination string: ", destString);

	const unsigned int numAvailable = destString.capacity() - destString.length() - 1;

	const unsigned int srcChar = 0;
	const unsigned int numChar = 6; // more than available in destination
	ASSERT_GT(numChar, numAvailable);
	const unsigned int destChar = destString.length() + 1; // beyond the end of destination
	const unsigned int numCopied = destString.replace(srcString, srcChar, numChar, destChar);
	printString("Trying to copy a character from the source string to beyond the end of the destination one: ", destString);

	ASSERT_EQ(destString.capacity(), Capacity);
	ASSERT_NE(srcString.length(), destString.length()); // no truncation
	ASSERT_EQ(numCopied, numAvailable);
	ASSERT_NE(numCopied, numChar);
}

TEST_F(StaticStringOperationsTest, CopyMoreCharactersThanSourceLength)
{
	nctl::StaticString<Capacity> srcString = "012";
	printString("Creating a new source string: ", srcString);
	nctl::StaticString<Capacity> destString = "abcdefg";
	printString("Creating a new destination string: ", destString);

	const unsigned int srcChar = 0;
	const unsigned int numChar = srcString.length() + 1; // more than available in source
	const unsigned int destChar = 0;
	const unsigned int numCopied = destString.replace(srcString, srcChar, numChar, destChar);
	printString("Trying to copy from the source string to the destination one more than source length: ", destString);

	ASSERT_NE(srcString.length(), destString.length()); // no truncation
	ASSERT_EQ(numCopied, srcString.length());
	ASSERT_NE(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcString.data(), destString.data(), srcChar, numCopied, destChar));
}

TEST_F(StaticStringOperationsTest, CopyMoreCharactersThanDestinationLength)
{
	nctl::StaticString<Capacity> srcString = "0123456";
	printString("Creating a new source string: ", srcString);
	nctl::StaticString<Capacity> destString = "abc";
	printString("Creating a new destination string: ", destString);

	const unsigned int srcChar = 0;
	const unsigned int numChar = destString.capacity() + 1; // more than available in destination
	const unsigned int destChar = 0;
	const unsigned int numCopied = destString.replace(srcString, srcChar, numChar, destChar);
	printString("Trying to copy from the source string to the destination one more than destination length: ", destString);

	ASSERT_EQ(srcString.length(), destString.length()); // extension
	ASSERT_EQ(numCopied, destString.length());
	ASSERT_NE(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcString.data(), destString.data(), srcChar, numCopied, destChar));
}

TEST_F(StaticStringOperationsTest, CopyCharactersFromCString)
{
	const unsigned int stringLength = 3;
	char srcString[stringLength + 1] = "abc";
	nctl::StaticString<Capacity> destString = "0123456";
	printString("Creating a new destination string: ", destString);

	const unsigned int srcChar = 0;
	const unsigned int numChar = stringLength;
	const unsigned int numCopied = destString.assign(srcString, srcChar, numChar);
	printf("Copying %u characters from an array of %u characters to the string: \"%s\"\n", numChar, stringLength, destString.data());

	ASSERT_EQ(stringLength, destString.length()); // truncation
	ASSERT_EQ(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcString, destString.data(), 0, numCopied, 0));
}

TEST_F(StaticStringOperationsTest, CopyCharactersFromCStringBeginning)
{
	const unsigned int stringLength = 3;
	char srcString[stringLength + 1] = "abc";
	nctl::StaticString<Capacity> destString = "0123456";
	printString("Creating a new destination string: ", destString);

	const unsigned int numChar = stringLength;
	const unsigned int numCopied = destString.assign(srcString, numChar);
	printf("Copying %u characters from an array of %u characters to the beginning of the string: \"%s\"\n", numChar, stringLength, destString.data());

	ASSERT_EQ(destString.length(), numCopied); // truncation
	ASSERT_EQ(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcString, destString.data(), 0, numCopied, 0));
}

TEST_F(StaticStringOperationsTest, CopyCharactersToCString)
{
	nctl::StaticString<Capacity> srcString = "abc";
	printString("Creating a new source string: ", srcString);
	const unsigned int stringLength = 7;
	char destString[stringLength + 1] = "0123456";

	const unsigned int srcChar = 0;
	const unsigned int numChar = srcString.length();
	const unsigned int numCopied = srcString.copy(destString, srcChar, numChar);
	printf("Copying %u characters from the string into an array of %u characters: \"%s\"\n", numChar, stringLength, destString);

	ASSERT_EQ(srcString.length(), strnlen(destString, 8)); // truncation
	ASSERT_EQ(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcString.data(), destString, srcChar, numCopied, 0));
}

TEST_F(StaticStringOperationsTest, CopyAllCharactersToCString)
{
	nctl::StaticString<Capacity> srcString = "abc";
	printString("Creating a new source string: ", srcString);
	const unsigned int stringLength = 8;
	char destString[stringLength] = "0123456";

	const unsigned int numChar = srcString.length();
	const unsigned int numCopied = srcString.copy(destString);
	printf("Copying %u characters from the beginning of the string into an array of %u characters: \"%s\"\n", numChar, stringLength, destString);

	ASSERT_EQ(srcString.length(), strnlen(destString, 8)); // truncation
	ASSERT_EQ(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcString.data(), destString, 0, numCopied, 0));
}

}
