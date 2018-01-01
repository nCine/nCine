#include "gtest_string.h"

namespace {

class StringOperationTest : public ::testing::Test
{
  public:
	StringOperationTest() : string_(Capacity) { }

  protected:
	void SetUp() { string_ = "String1"; }

	nc::String string_;
};

TEST_F(StringOperationTest, AppendString)
{
	nc::String newString = "Append";
	printf("Creating a new string: "); printString(newString);

	string_.append(newString);
	printf("Appending a new string to the first one: "); printString(string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), strnlen("String1Append", Capacity));
	ASSERT_STREQ(string_.data(), "String1Append");
}

TEST_F(StringOperationTest, Clear)
{
	string_.clear();
	printf("Clearing the string: "); printString(string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), 0u);
}

TEST_F(StringOperationTest, ImplicitCStringConstructor)
{
	nc::String newString = "String2";
	printf("Creating a new string from a C-style string assignment: "); printString(newString);

	// Termination character is taken into account for capacity
	ASSERT_EQ(newString.length() + 1, newString.capacity());
	ASSERT_EQ(newString.capacity(), strnlen("String2", Capacity) + 1);
	ASSERT_EQ(newString.length(), strnlen("String2", Capacity));
	ASSERT_STREQ(newString.data(), "String2");
}

TEST_F(StringOperationTest, CopyConstructor)
{
	nc::String newString(string_);
	printf("Creating a new string as a copy of the first one: "); printString(newString);

	ASSERT_EQ(newString.capacity(), string_.capacity());
	ASSERT_EQ(newString.length(), string_.length());
	ASSERT_STREQ(newString.data(), string_.data());
}

TEST_F(StringOperationTest, AssignmentOperator)
{
	nc::String newString = string_;
	printf("Creating a new string with the assignment operator: "); printString(newString);

	ASSERT_EQ(newString.capacity(), string_.capacity());
	ASSERT_EQ(newString.length(), string_.length());
	ASSERT_STREQ(newString.data(), string_.data());
}

TEST_F(StringOperationTest, ConstructByConcatenation)
{
	nc::String secondString = "String2";
	printf("Creating a second string: "); printString(secondString);

	nc::String thirdString = secondString + string_;
	printf("Creating a third string by concatenating the second and the first one: "); printString(thirdString);

	// Termination character is taken into account for capacity
	ASSERT_EQ(thirdString.length() + 1, thirdString.capacity());
	ASSERT_EQ(thirdString.capacity(), strnlen("String2String1", Capacity) + 1);
	ASSERT_EQ(thirdString.length(), strnlen("String2String1", Capacity));
	ASSERT_STREQ(thirdString.data(), "String2String1");
}

TEST_F(StringOperationTest, ConstructByConcatenationWithCStringAsFirst)
{
	const char *secondString = "String2";
	printf("Creating a second string: %s", secondString);

	// using `friend String operator+(const char *cString, const String &string)`
	nc::String thirdString = secondString + string_;
	printf("Creating a third string by concatenating the second and the first one: "); printString(thirdString);

	// Termination character is taken into account for capacity
	ASSERT_EQ(thirdString.length() + 1, thirdString.capacity());
	ASSERT_EQ(thirdString.capacity(), strnlen("String2String1", Capacity) + 1);
	ASSERT_EQ(thirdString.length(), strnlen("String2String1", Capacity));
	ASSERT_STREQ(thirdString.data(), "String2String1");
}

TEST_F(StringOperationTest, ConstructByConcatenationWithCStringAsSecond)
{
	const char *secondString = "String2";
	printf("Creating a second string: %s", secondString);

	// using `String String::operator+(const char *cString) const`
	nc::String thirdString = string_ + secondString;
	printf("Creating a third string by concatenating the first and the second one: "); printString(thirdString);

	// Termination character is taken into account for capacity
	ASSERT_EQ(thirdString.length() + 1, thirdString.capacity());
	ASSERT_EQ(thirdString.capacity(), strnlen("String1String2", Capacity) + 1);
	ASSERT_EQ(thirdString.length(), strnlen("String1String2", Capacity));
	ASSERT_STREQ(thirdString.data(), "String1String2");
}

TEST_F(StringOperationTest, Format)
{
	string_.format("String%d", 2);
	printf("Resetting the string to a formatted one: "); printString(string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), strnlen("String2", Capacity));
	ASSERT_STREQ(string_.data(), "String2");
}

TEST_F(StringOperationTest, FormatAppend)
{
	string_.formatAppend("String%d", 2);
	printf("Appending a formatted string to the first one: "); printString(string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), strnlen("String1String2", Capacity));
	ASSERT_STREQ(string_.data(), "String1String2");
}

TEST_F(StringOperationTest, FindFirstCharacter)
{
	string_ += "String2";
	printf("Appending a string to the first one: "); printString(string_);

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
	printf("Appending a string to the first one: "); printString(string_);

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
	printf("Appending a string to the first one: "); printString(string_);

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

	nc::String newString = "String2";
	printf("Creating a new string: "); printString(newString);
	string_ += newString;
	printf("Appending the new string to the first one: "); printString(string_);

	const int position = string_.find(newString);
	printf("Position of sub-string \"%s\" inside string \"%s\": %d\n", newString.data(), string_.data(), position);
	ASSERT_EQ(position, static_cast<int>(oldLength));
}

TEST_F(StringOperationTest, FindNonExistentString)
{
	nc::String newString = "String2";
	printf("Creating a new string: "); printString(newString);

	const int position = string_.find(newString);
	printf("Position of sub-string \"%s\" inside string \"%s\": %d\n", newString.data(), string_.data(), position);
	ASSERT_EQ(position, -1);
}

TEST_F(StringOperationTest, FindCString)
{
	const unsigned int oldLength = string_.length();

	const char *newString = "String2";
	printf("Creating a new string: %s", newString);
	string_ += newString;
	printf("Appending the new string to the first one: "); printString(string_);

	const int position = string_.find(newString);
	printf("Position of sub-string \"%s\" inside string \"%s\": %d\n", newString, string_.data(), position);
	ASSERT_EQ(position, static_cast<int>(oldLength));
}

TEST_F(StringOperationTest, FindNonExistentCString)
{
	const char *newString = "String2";
	printf("Creating a new string: %s", newString);

	const int position = string_.find(newString);
	printf("Position of sub-string \"%s\" inside string \"%s\": %d\n", newString, string_.data(), position);
	ASSERT_EQ(position, -1);
}

TEST_F(StringOperationTest, CopyCharacters)
{
	nc::String srcString = "0123456";
	printf("Creating a new source string: "); printString(srcString);
	nc::String destString = "abcdefg";
	printf("Creating a new destination string: "); printString(destString);

	const unsigned int srcChar = 4;
	const unsigned int numChar = 3;
	const unsigned int destChar = 2;
	const unsigned int numCopied = srcString.copy(destString, srcChar, numChar, destChar);
	printf("Copying %u characters from position %u of the source string to position %u of the destination one: ", numChar, srcChar, destChar); printString(destString);

	ASSERT_EQ(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcString.data(), destString.data(), srcChar, numCopied, destChar));
}

TEST_F(StringOperationTest, CopyCharactersToBeginning)
{
	nc::String srcString = "0123456";
	printf("Creating a new source string: "); printString(srcString);
	nc::String destString = "abcdefg";
	printf("Creating a new destination string: "); printString(destString);

	const unsigned int srcChar = 4;
	const unsigned int numChar = 3;
	const unsigned int numCopied = srcString.copy(destString, srcChar, numChar);
	printf("Copying %u characters from position %u of the source string to position 0 of the destination one: ", numChar, srcChar); printString(destString);

	ASSERT_EQ(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcString.data(), destString.data(), srcChar, numCopied, 0));
}

TEST_F(StringOperationTest, CopyAllCharactersToBeginning)
{
	nc::String srcString = "0123456";
	printf("Creating a new source string: "); printString(srcString);
	nc::String destString = "abcdefg";
	printf("Creating a new destination string: "); printString(destString);

	const unsigned int numCopied = srcString.copy(destString);
	printf("Copying %u characters from position 0 of the source string to position 0 of the destination one: ", srcString.length()); printString(destString);

	ASSERT_EQ(numCopied, srcString.length());
	ASSERT_TRUE(charactersInStringsAreEqual(srcString.data(), destString.data(), 0, srcString.length(), 0));
}

TEST_F(StringOperationTest, CopyCharactersFromBeyondEnd)
{
	nc::String srcString = "0123456";
	printf("Creating a new source string: "); printString(srcString);
	nc::String destString = "abcdefg";
	printf("Creating a new destination string: "); printString(destString);

	const unsigned int srcChar = srcString.length() + 1; // beyond the end of source
	const unsigned int numChar = 2;
	const unsigned int destChar = 0;
	const unsigned int numCopied = srcString.copy(destString, srcChar, numChar, destChar);
	printf("Trying to copy a character from beyond the end of the source string into the destination one: "); printString(destString);

	ASSERT_EQ(numCopied, 0u);
	ASSERT_NE(numCopied, numChar);
}

TEST_F(StringOperationTest, CopyCharactersToBeyondEnd)
{
	nc::String srcString = "0123456";
	printf("Creating a new source string: "); printString(srcString);
	nc::String destString = "abcdefg";
	printf("Creating a new destination string: "); printString(destString);

	const unsigned int numAvailable = destString.capacity() - destString.length();

	const unsigned int srcChar = 0;
	const unsigned int numChar = 2; // more than available in destination
	const unsigned int destChar = destString.length() + 1; // beyond the end of destination
	const unsigned int numCopied = srcString.copy(destString, srcChar, numChar, destChar);
	printf("Trying to copy a character from the source string to beyond the end of the destination one: "); printString(destString);

	ASSERT_EQ(numCopied, numAvailable);
	ASSERT_NE(numCopied, numChar);
}

TEST_F(StringOperationTest, CopyMoreCharactersThanSourceLength)
{
	nc::String srcString = "012";
	printf("Creating a new source string: "); printString(srcString);
	nc::String destString = "abcdefg";
	printf("Creating a new destination string: "); printString(destString);

	const unsigned int srcChar = 0;
	const unsigned int numChar = srcString.length() + 1; // more than available in source
	const unsigned int destChar = 0;
	const unsigned int numCopied = srcString.copy(destString, srcChar, numChar, destChar);
	printf("Trying to copy from the source string to the destination one more than source length: "); printString(destString);

	ASSERT_EQ(numCopied, srcString.length());
	ASSERT_NE(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcString.data(), destString.data(), srcChar, numCopied, destChar));
}

TEST_F(StringOperationTest, CopyMoreCharactersThanDestinationLength)
{
	nc::String srcString = "0123456";
	printf("Creating a new source string: "); printString(srcString);
	nc::String destString = "abc";
	printf("Creating a new destination string: "); printString(destString);

	const unsigned int srcChar = 0;
	const unsigned int numChar = destString.capacity() + 1; // more than available in destination
	const unsigned int destChar = 0;
	const unsigned int numCopied = srcString.copy(destString, srcChar, numChar, destChar);
	printf("Trying to copy from the source string to the destination one more than destination length: "); printString(destString);

	ASSERT_EQ(numCopied, destString.length());
	ASSERT_NE(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcString.data(), destString.data(), srcChar, numCopied, destChar));
}

TEST_F(StringOperationTest, CopyCharactersToCString)
{
	nc::String srcString = "0123456";
	printf("Creating a new source string: "); printString(srcString);
	const unsigned int stringLength = 4;
	char destString[stringLength] = "abc";

	const unsigned int srcChar = 0;
	const unsigned int numChar = stringLength;
	const unsigned int numCopied = srcString.copy(destString, srcChar, numChar);
	printf("Copying %u characters from the string into an array of %u characters: \"%s\"\n", numChar, stringLength, destString);

	ASSERT_EQ(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcString.data(), destString, 0, numCopied, 0));
}

}
