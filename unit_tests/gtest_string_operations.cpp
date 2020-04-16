#include "gtest_string.h"

namespace {

class StringOperationTest : public ::testing::Test
{
  public:
	StringOperationTest()
	    : string_(Capacity) {}

  protected:
	void SetUp() override { string_ = "String1"; }

	nctl::String string_;
};

TEST_F(StringOperationTest, AppendString)
{
	nctl::String newString = "Append";
	printString("Creating a new string: ", newString);

	string_.append(newString);
	printString("Appending a new string to the first one: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), strnlen("String1Append", Capacity));
	ASSERT_STREQ(string_.data(), "String1Append");
}

TEST_F(StringOperationTest, AppendCString)
{
	const char *newString = "Append";
	printf("Creating a new C string: %s\n", newString);

	string_.append(newString);
	printString("Appending a C string to the first one: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), strnlen("String1Append", Capacity));
	ASSERT_STREQ(string_.data(), "String1Append");
}

TEST_F(StringOperationTest, Clear)
{
	string_.clear();
	printString("Clearing the string: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), 0u);
}

TEST_F(StringOperationTest, ImplicitCStringConstructorShort)
{
	nctl::String newString = "String2";
	printString("Creating a new string from a C-style string assignment: ", newString);

	// Termination character is taken into account for capacity
	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), strnlen("String2", Capacity));
	ASSERT_STREQ(newString.data(), "String2");
}

TEST_F(StringOperationTest, ImplicitCStringConstructorLong)
{
	// The string is longer than `SmallBufferSize`
	nctl::String newString = "String2...String2";
	printString("Creating a new string from a C-style string assignment: ", newString);

	// Termination character is taken into account for capacity
	ASSERT_EQ(newString.length() + 1, newString.capacity());
	ASSERT_EQ(newString.capacity(), strnlen("String2...String2", 18) + 1);
	ASSERT_EQ(newString.length(), strnlen("String2...String2", 18));
	ASSERT_STREQ(newString.data(), "String2...String2");
}

TEST_F(StringOperationTest, CopyConstruction)
{
	nctl::String newString(string_);
	printString("Creating a new string as a copy of the first one: ", newString);

	ASSERT_EQ(newString.capacity(), string_.capacity());
	ASSERT_EQ(newString.length(), string_.length());
	ASSERT_STREQ(newString.data(), string_.data());
}

TEST_F(StringOperationTest, MoveConstruction)
{
	nctl::String newString(nctl::move(string_));
	printString("Creating a new string moving from the first one: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), strnlen("String1", Capacity));
	ASSERT_STREQ(newString.data(), "String1");
	ASSERT_EQ(string_.capacity(), 0);
	ASSERT_EQ(string_.length(), 0);
}

TEST_F(StringOperationTest, AssignmentOperator)
{
	nctl::String newString(Capacity);
	newString = string_;
	printString("Creating a new string with the assignment operator: ", newString);

	ASSERT_EQ(newString.capacity(), string_.capacity());
	ASSERT_EQ(newString.length(), string_.length());
	ASSERT_STREQ(newString.data(), string_.data());
}

TEST_F(StringOperationTest, MoveAssignmentOperator)
{
	nctl::String newString(Capacity);
	newString = nctl::move(string_);
	printString("Creating a new string with the move assignment operator: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), strnlen("String1", Capacity));
	ASSERT_STREQ(newString.data(), "String1");
	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), 0);
}

TEST_F(StringOperationTest, SelfAssignment)
{
	string_ = string_;
	printString("Assigning the string to itself with the assignment operator: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), strnlen("String1", Capacity));
	ASSERT_STREQ(string_.data(), "String1");
}

TEST_F(StringOperationTest, AssignLongCStringAndTruncate)
{
	nctl::String newString;
	// The C-style string is longer than string capacity
	newString = "String2...String2";
	printString("Creating a new string with the assignment operator from a C-style string: ", newString);

	// Termination character is taken into account for capacity
	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length() + 1, Capacity);
	ASSERT_STREQ(newString.data(), "String2...Strin");
}

TEST_F(StringOperationTest, ConstructByConcatenation)
{
	// The string is longer than `SmallBufferSize`
	nctl::String secondString = "String2...String2";
	printString("Creating a second string: ", secondString);

	nctl::String thirdString = secondString + string_;
	printString("Creating a third string by concatenating the second and the first one: ", thirdString);

	// Termination character is taken into account for capacity
	ASSERT_EQ(thirdString.length() + 1, thirdString.capacity());
	ASSERT_EQ(thirdString.capacity(), strnlen("String2...String2String1", 25) + 1);
	ASSERT_EQ(thirdString.length(), strnlen("String2...String2String1", 25));
	ASSERT_STREQ(thirdString.data(), "String2...String2String1");
}

TEST_F(StringOperationTest, ConstructByConcatenationWithCStringAsFirst)
{
	// The string is longer than `SmallBufferSize`
	const char *secondString = "String2...String2";
	printf("Creating a second string: %s\n", secondString);

	// using `friend String operator+(const char *cString, const String &string)`
	nctl::String thirdString = secondString + string_;
	printString("Creating a third string by concatenating the second and the first one: ", thirdString);

	// Termination character is taken into account for capacity
	ASSERT_EQ(thirdString.length() + 1, thirdString.capacity());
	ASSERT_EQ(thirdString.capacity(), strnlen("String2...String2String1", 25) + 1);
	ASSERT_EQ(thirdString.length(), strnlen("String2...String2String1", 25));
	ASSERT_STREQ(thirdString.data(), "String2...String2String1");
}

TEST_F(StringOperationTest, ConstructByConcatenationWithCStringAsSecond)
{
	// The string is longer than `SmallBufferSize`
	const char *secondString = "String2...String2";
	printf("Creating a second string: %s\n", secondString);

	// using `String String::operator+(const char *cString) const`
	nctl::String thirdString = string_ + secondString;
	printString("Creating a third string by concatenating the first and the second one: ", thirdString);

	// Termination character is taken into account for capacity
	ASSERT_EQ(thirdString.length() + 1, thirdString.capacity());
	ASSERT_EQ(thirdString.capacity(), strnlen("String1String2...String2", 25) + 1);
	ASSERT_EQ(thirdString.length(), strnlen("String1String2...String2", 25));
	ASSERT_STREQ(thirdString.data(), "String1String2...String2");
}

TEST_F(StringOperationTest, Format)
{
	string_.format("String%d", 2);
	printString("Resetting the string to a formatted one: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), strnlen("String2", Capacity));
	ASSERT_STREQ(string_.data(), "String2");
}

TEST_F(StringOperationTest, FormatAndTruncate)
{
	string_.format("VeryLongStringFormatting%d", 2);
	printString("Resetting the string to a long formatted one with truncation: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), strnlen("VeryLongStringF", Capacity));
	ASSERT_STREQ(string_.data(), "VeryLongStringF");
}

TEST_F(StringOperationTest, FormatAppend)
{
	string_.formatAppend("String%d", 2);
	printString("Appending a formatted string to the first one: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), strnlen("String1String2", Capacity));
	ASSERT_STREQ(string_.data(), "String1String2");
}

TEST_F(StringOperationTest, FormatAppendAndTruncate)
{
	string_.formatAppend("VeryLongStringFormatting%d", 2);
	printString("Appending a long formatted string to the first one with truncation: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), strnlen("String1VeryLong", Capacity));
	ASSERT_STREQ(string_.data(), "String1VeryLong");
}

TEST_F(StringOperationTest, SetLengthShrink)
{
	const unsigned int length = string_.length();
	string_.data()[length - 1] = '\0';
	const unsigned int newLength = string_.setLength(length - 1);
	printString("Appending a single character by accessing the data array: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), length - 1);
	ASSERT_EQ(newLength, length - 1);
	ASSERT_STREQ(string_.data(), "String");
}

TEST_F(StringOperationTest, SetLengthExpand)
{
	const unsigned int length = string_.length();
	string_.data()[length + 0] = '2';
	string_.data()[length + 1] = '3';
	string_.data()[length + 2] = '\0';
	const unsigned int newLength = string_.setLength(length + 2);
	printString("Appending a single character by accessing the data array: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), length + 2);
	ASSERT_EQ(newLength, length + 2);
	ASSERT_STREQ(string_.data(), "String123");
}

TEST_F(StringOperationTest, SetLengthBeyondCapacity)
{
	const unsigned int newLength = string_.setLength(Capacity + 1);
	printString("Appending a single character by accessing the data array: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), Capacity - 1);
	ASSERT_EQ(newLength, Capacity - 1);
	ASSERT_STREQ(string_.data(), "String1");
}

TEST_F(StringOperationTest, FindFirstCharacter)
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

TEST_F(StringOperationTest, FindFirstNonExistentCharacter)
{
	const char character = 'z';
	const int position = string_.findFirstChar(character);
	printf("First position of the character '%c' in the string: %d\n", character, position);

	ASSERT_EQ(position, -1);
}

TEST_F(StringOperationTest, FindLastCharacter)
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

TEST_F(StringOperationTest, FindLastNonExistentCharacter)
{
	const char character = 'z';
	const int position = string_.findLastChar(character);
	printf("Last position of the character '%c' in the string: %d\n", character, position);

	ASSERT_EQ(position, -1);
}

TEST_F(StringOperationTest, FindFirstCharacterAfterIndex)
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

TEST_F(StringOperationTest, FindFirstNonExistentCharacterAfterIndex)
{
	const char character = 'z';
	const unsigned int index = 1;
	const int position = string_.findFirstCharAfterIndex(character, index);
	printf("Position of the character '%c' after position %u in the string: %d\n", character, index, position);

	ASSERT_EQ(position, -1);
}

TEST_F(StringOperationTest, FindFirstCharacterAfterIndexBeyondLength)
{
	const char character = 'S';
	const unsigned int index = string_.length();
	const int position = string_.findFirstCharAfterIndex(character, index);
	printf("Position of the character '%c' after position %u in the string: %d\n", character, index, position);

	ASSERT_EQ(position, -1);
}

TEST_F(StringOperationTest, FindString)
{
	const unsigned int oldLength = string_.length();

	nctl::String newString = "String2";
	printString("Creating a new string: ", newString);
	string_ += newString;
	printString("Appending the new string to the first one: ", string_);

	const int position = string_.find(newString);
	printf("Position of sub-string \"%s\" inside string \"%s\": %d\n", newString.data(), string_.data(), position);
	ASSERT_EQ(position, static_cast<int>(oldLength));
}

TEST_F(StringOperationTest, FindNonExistentString)
{
	nctl::String newString = "String2";
	printString("Creating a new string: ", newString);

	const int position = string_.find(newString);
	printf("Position of sub-string \"%s\" inside string \"%s\": %d\n", newString.data(), string_.data(), position);
	ASSERT_EQ(position, -1);
}

TEST_F(StringOperationTest, FindCString)
{
	const unsigned int oldLength = string_.length();

	const char *newString = "String2";
	printf("Creating a new string: %s\n", newString);
	string_ += newString;
	printString("Appending the new string to the first one: ", string_);

	const int position = string_.find(newString);
	printf("Position of sub-string \"%s\" inside string \"%s\": %d\n", newString, string_.data(), position);
	ASSERT_EQ(position, static_cast<int>(oldLength));
}

TEST_F(StringOperationTest, FindNonExistentCString)
{
	const char *newString = "String2";
	printf("Creating a new string: %s\n", newString);

	const int position = string_.find(newString);
	printf("Position of sub-string \"%s\" inside string \"%s\": %d\n", newString, string_.data(), position);
	ASSERT_EQ(position, -1);
}

TEST_F(StringOperationTest, CopyCharacters)
{
	nctl::String srcString = "0123456";
	printString("Creating a new source string: ", srcString);
	nctl::String destString = "abcdefg";
	printString("Creating a new destination string: ", destString);

	const unsigned int srcChar = 4;
	const unsigned int numChar = 3;
	const unsigned int destChar = 2;
	const unsigned int numCopied = destString.assign(srcString, srcChar, numChar, destChar);
	printf("Copying %u characters from position %u of the source string to position %u of the destination one: ", numChar, srcChar, destChar);
	printString(destString);

	ASSERT_EQ(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcString.data(), destString.data(), srcChar, numCopied, destChar));
}

TEST_F(StringOperationTest, CopyCharactersToBeginning)
{
	nctl::String srcString = "0123456";
	printString("Creating a new source string: ", srcString);
	nctl::String destString = "abcdefg";
	printString("Creating a new destination string: ", destString);

	const unsigned int srcChar = 4;
	const unsigned int numChar = 3;
	const unsigned int numCopied = destString.assign(srcString, srcChar, numChar);
	printf("Copying %u characters from position %u of the source string to position 0 of the destination one: ", numChar, srcChar);
	printString(destString);

	ASSERT_EQ(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcString.data(), destString.data(), srcChar, numCopied, 0));
}

TEST_F(StringOperationTest, CopyAllCharactersToBeginning)
{
	nctl::String srcString = "0123456";
	printString("Creating a new source string: ", srcString);
	nctl::String destString = "abcdefg";
	printString("Creating a new destination string: ", destString);

	const unsigned int numCopied = destString.assign(srcString);
	printf("Copying %u characters from position 0 of the source string to position 0 of the destination one: ", srcString.length());
	printString(destString);

	ASSERT_EQ(numCopied, srcString.length());
	ASSERT_TRUE(charactersInStringsAreEqual(srcString.data(), destString.data(), 0, srcString.length(), 0));
}

TEST_F(StringOperationTest, CopyCharactersFromBeyondEnd)
{
	nctl::String srcString = "0123456";
	printString("Creating a new source string: ", srcString);
	nctl::String destString = "abcdefg";
	printString("Creating a new destination string: ", destString);

	const unsigned int srcChar = srcString.length() + 1; // beyond the end of source
	const unsigned int numChar = 2;
	const unsigned int destChar = 0;
	const unsigned int numCopied = destString.assign(srcString, srcChar, numChar, destChar);
	printString("Trying to copy a character from beyond the end of the source string into the destination one: ", destString);

	ASSERT_EQ(numCopied, 0u);
	ASSERT_NE(numCopied, numChar);
}

TEST_F(StringOperationTest, CopyCharactersToBeyondEnd)
{
	nctl::String srcString = "0123456";
	printString("Creating a new source string: ", srcString);
	nctl::String destString = "abcdefghilmn";
	printString("Creating a new destination string: ", destString);

	const unsigned int numAvailable = destString.capacity() - destString.length() - 1;

	const unsigned int srcChar = 0;
	const unsigned int numChar = 6; // more than available in destination
	const unsigned int destChar = destString.length() + 1; // beyond the end of destination
	const unsigned int numCopied = destString.assign(srcString, srcChar, numChar, destChar);
	printString("Trying to copy a character from the source string to beyond the end of the destination one: ", destString);

	ASSERT_EQ(numCopied, numAvailable);
	ASSERT_NE(numCopied, numChar);
}

TEST_F(StringOperationTest, CopyMoreCharactersThanSourceLength)
{
	nctl::String srcString = "012";
	printString("Creating a new source string: ", srcString);
	nctl::String destString = "abcdefg";
	printString("Creating a new destination string: ", destString);

	const unsigned int srcChar = 0;
	const unsigned int numChar = srcString.length() + 1; // more than available in source
	const unsigned int destChar = 0;
	const unsigned int numCopied = destString.assign(srcString, srcChar, numChar, destChar);
	printString("Trying to copy from the source string to the destination one more than source length: ", destString);

	ASSERT_EQ(numCopied, srcString.length());
	ASSERT_NE(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcString.data(), destString.data(), srcChar, numCopied, destChar));
}

TEST_F(StringOperationTest, CopyMoreCharactersThanDestinationLength)
{
	nctl::String srcString = "0123456";
	printString("Creating a new source string: ", srcString);
	nctl::String destString = "abc";
	printString("Creating a new destination string: ", destString);

	const unsigned int srcChar = 0;
	const unsigned int numChar = destString.capacity() + 1; // more than available in destination
	const unsigned int destChar = 0;
	const unsigned int numCopied = destString.assign(srcString, srcChar, numChar, destChar);
	printString("Trying to copy from the source string to the destination one more than destination length: ", destString);

	ASSERT_EQ(numCopied, destString.length());
	ASSERT_NE(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcString.data(), destString.data(), srcChar, numCopied, destChar));
}

TEST_F(StringOperationTest, CopyCharactersFromCString)
{
	const unsigned int stringLength = 4;
	char srcString[stringLength] = "abc";
	nctl::String destString = "0123456";
	printString("Creating a new destination string: ", destString);

	const unsigned int destChar = 0;
	const unsigned int numChar = stringLength - 1;
	const unsigned int numCopied = destString.assign(srcString, numChar, destChar);
	printf("Copying %u characters from an array of %u characters to the string: \"%s\"\n", numChar, stringLength, destString.data());

	ASSERT_EQ(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcString, destString.data(), 0, numCopied, 0));
}

TEST_F(StringOperationTest, CopyCharactersFromCStringBeginning)
{
	const unsigned int stringLength = 4;
	char srcString[stringLength] = "abc";
	nctl::String destString = "0123456";
	printString("Creating a new destination string: ", destString);

	const unsigned int numChar = stringLength - 1;
	const unsigned int numCopied = destString.assign(srcString, numChar);
	printf("Copying %u characters from an array of %u characters to the beginning of the string: \"%s\"\n", numChar, stringLength, destString.data());

	ASSERT_EQ(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcString, destString.data(), 0, numCopied, 0));
}

TEST_F(StringOperationTest, CopyCharactersToCString)
{
	nctl::String srcString = "abc";
	printString("Creating a new source string: ", srcString);
	const unsigned int stringLength = 8;
	char destString[stringLength] = "0123456";

	const unsigned int srcChar = 0;
	const unsigned int numChar = srcString.length();
	const unsigned int numCopied = srcString.copy(destString, srcChar, numChar);
	printf("Copying %u characters from the string into an array of %u characters: \"%s\"\n", numChar, stringLength, destString);

	ASSERT_EQ(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcString.data(), destString, srcChar, numCopied, 0));
}

TEST_F(StringOperationTest, CopyAllCharactersToCString)
{
	nctl::String srcString = "abc";
	printString("Creating a new source string: ", srcString);
	const unsigned int stringLength = 8;
	char destString[stringLength] = "0123456";

	const unsigned int numChar = srcString.length();
	const unsigned int numCopied = srcString.copy(destString);
	printf("Copying %u characters from the beginning of the string into an array of %u characters: \"%s\"\n", numChar, stringLength, destString);

	ASSERT_EQ(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcString.data(), destString, 0, numCopied, 0));
}

}
