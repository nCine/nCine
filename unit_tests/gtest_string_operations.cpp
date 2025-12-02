#include "gtest_string.h"

namespace {

const char *LongStringLiteral = "String2...String2...String2";
const unsigned int LongStringLength = 27;
static_assert(LongStringLength > nctl::String::SmallBufferSize, "The string should be longer than the small buffer size");

class StringOperationsTest : public ::testing::Test
{
  public:
	StringOperationsTest()
	    : string_(Capacity) {}

  protected:
	void SetUp() override { string_ = String1Literal; }

	nctl::String string_;
};

TEST_F(StringOperationsTest, AppendString)
{
	nctl::String newString = "Append";
	printString("Creating a new string: ", newString);

	string_.append(newString);
	printString("Appending a new string to the first one: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), strnlen("String1Append", 14));
	ASSERT_STREQ(string_.data(), "String1Append");
}

TEST_F(StringOperationsTest, AppendStringExtend)
{
	nctl::String newString = VeryLongCStringLiteral;
	printString("Creating a new string: ", newString);

	string_.append(newString);
	printString("Appending a new string to the first one: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity * 4);
	ASSERT_EQ(string_.length(), String1Length + VeryLongCStringLength);
	ASSERT_STREQ(string_.data(), "String1This_is_a_very_long_string_which_goes_beyond_the_default_capacity_of_a_string");
}

TEST_F(StringOperationsTest, AppendCString)
{
	const char *newString = "Append";
	printf("Creating a new C string: %s\n", newString);

	string_.append(newString);
	printString("Appending a C string to the first one: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), strnlen("String1Append", 14));
	ASSERT_STREQ(string_.data(), "String1Append");
}

TEST_F(StringOperationsTest, AppendCStringExtend)
{
	printf("Using a very long C string: %s\n", VeryLongCStringLiteral);

	string_.append(VeryLongCStringLiteral);
	printString("Appending a C string to the first one: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity * 4);
	ASSERT_EQ(string_.length(), String1Length + VeryLongCStringLength);
	ASSERT_STREQ(string_.data(), "String1This_is_a_very_long_string_which_goes_beyond_the_default_capacity_of_a_string");
}

TEST_F(StringOperationsTest, Clear)
{
	string_.clear();
	printString("Clearing the string: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), 0u);
}

TEST_F(StringOperationsTest, ImplicitCStringConstructorShort)
{
	nctl::String newString = String2Literal;
	printString("Creating a new string from a C-style string assignment: ", newString);

	// Termination character is taken into account for capacity
	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), String2Length);
	ASSERT_STREQ(newString.data(), String2Literal);
}

TEST_F(StringOperationsTest, ImplicitCStringConstructorLong)
{
	// The string is longer than `SmallBufferSize`
	nctl::String newString = LongStringLiteral;
	ASSERT_GT(newString.length(), nctl::String::SmallBufferSize);
	printString("Creating a new string from a C-style string assignment: ", newString);

	// Termination character is taken into account for capacity
	ASSERT_EQ(newString.length() + 1, newString.capacity());
	ASSERT_EQ(newString.capacity(), LongStringLength + 1);
	ASSERT_EQ(newString.length(), LongStringLength);
	ASSERT_STREQ(newString.data(), LongStringLiteral);
}

TEST_F(StringOperationsTest, CopyConstruction)
{
	nctl::String newString(string_);
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

TEST_F(StringOperationsTest, MoveConstruction)
{
	nctl::String newString(nctl::move(string_));
	printString("Creating a new string moving from the first one: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), String1Length);
	ASSERT_STREQ(newString.data(), String1Literal);
	ASSERT_EQ(string_.capacity(), 0);
	ASSERT_EQ(string_.length(), 0);

	printf("Appending a very long string to the second string\n");
	newString.append(VeryLongCStringLiteral);

	ASSERT_EQ(newString.capacity(), Capacity * 4);
	ASSERT_GT(newString.length(), String1Length);
}

TEST_F(StringOperationsTest, AssignmentOperator)
{
	nctl::String newString(Capacity);
	newString = string_;
	printString("Filling a new string with the assignment operator: ", newString);

	ASSERT_EQ(newString.capacity(), string_.capacity());
	ASSERT_EQ(newString.length(), string_.length());
	ASSERT_STREQ(newString.data(), string_.data());
}

TEST_F(StringOperationsTest, AssignmentOperatorExtend)
{
	nctl::String longString(VeryLongCStringLiteral);
	string_ = longString;
	printString("Extending a string with the assignment operator: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity * 4);
	ASSERT_EQ(string_.length(), longString.length());
	ASSERT_STREQ(string_.data(), longString.data());
}

TEST_F(StringOperationsTest, AssignmentOperatorShorter)
{
	nctl::String newString(String1Literal);
	string_ = "LongString1";
	string_ = newString;
	printString("Shrinking a string with the assignment operator: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), newString.length());
	ASSERT_STREQ(string_.data(), String1Literal);
}

TEST_F(StringOperationsTest, MoveAssignmentOperator)
{
	nctl::String newString(Capacity);
	newString = nctl::move(string_);
	printString("Creating a new string with the move assignment operator: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), String1Length);
	ASSERT_STREQ(newString.data(), String1Literal);
	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), 0);
}

TEST_F(StringOperationsTest, MoveAssignmentOperatorExtend)
{
	nctl::String longString(VeryLongCStringLiteral);
	nctl::String newString(Capacity);
	newString = nctl::move(longString);
	printString("Extending a string with the move assignment operator: ", newString);

	ASSERT_GT(newString.capacity(), Capacity);
	ASSERT_GT(newString.length(), Capacity);
	ASSERT_EQ(longString.capacity(), Capacity);
	ASSERT_EQ(longString.length(), 0);
}

TEST_F(StringOperationsTest, MoveAssignmentOperatorShorter)
{
	nctl::String shortString(String1Literal);
	nctl::String newString("LongString1");
	newString = nctl::move(shortString);
	printString("Shrinking a string with the move assignment operator: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), String1Length);
	ASSERT_STREQ(newString.data(), String1Literal);
}

TEST_F(StringOperationsTest, SelfAssignment)
{
	string_ = string_;
	printString("Assigning the string to itself with the assignment operator: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), String1Length);
	ASSERT_STREQ(string_.data(), String1Literal);
}

TEST_F(StringOperationsTest, AssignLongCStringExtend)
{
	nctl::String newString;
	// The C-style string is longer than `SmallBufferSize`
	newString = LongStringLiteral;
	ASSERT_GT(newString.length(), nctl::String::SmallBufferSize);
	printString("Creating a new string with the assignment operator from a C-style string: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity * 2);
	ASSERT_EQ(newString.length(), LongStringLength);
	ASSERT_STREQ(newString.data(), LongStringLiteral);
}

TEST_F(StringOperationsTest, ConstructByConcatenation)
{
	// The string is longer than `SmallBufferSize`
	nctl::String secondString = LongStringLiteral;
	ASSERT_GT(secondString.length(), nctl::String::SmallBufferSize);
	printString("Creating a second string: ", secondString);

	nctl::String thirdString = secondString + string_;
	printString("Creating a third string by concatenating the second and the first one: ", thirdString);

	// Termination character is taken into account for capacity
	ASSERT_EQ(thirdString.length() + 1, thirdString.capacity());
	ASSERT_EQ(thirdString.capacity(), LongStringLength + String1Length + 1);
	ASSERT_EQ(thirdString.length(), LongStringLength + String1Length);
	ASSERT_STREQ(thirdString.data(), "String2...String2...String2String1");
}

TEST_F(StringOperationsTest, ConstructByConcatenationWithCStringAsFirst)
{
	// The string is longer than `SmallBufferSize`
	const char *secondString = LongStringLiteral;
	ASSERT_GT(strnlen(secondString, LongStringLength + 1), nctl::String::SmallBufferSize);
	printf("Creating a second string: %s\n", secondString);

	// using `friend String operator+(const char *cString, const String &string)`
	nctl::String thirdString = secondString + string_;
	printString("Creating a third string by concatenating the second and the first one: ", thirdString);

	// Termination character is taken into account for capacity
	ASSERT_EQ(thirdString.length() + 1, thirdString.capacity());
	ASSERT_EQ(thirdString.capacity(), LongStringLength + String1Length + 1);
	ASSERT_EQ(thirdString.length(), LongStringLength + String1Length);
	ASSERT_STREQ(thirdString.data(), "String2...String2...String2String1");
}

TEST_F(StringOperationsTest, ConstructByConcatenationWithCStringAsSecond)
{
	// The string is longer than `SmallBufferSize`
	const char *secondString = LongStringLiteral;
	ASSERT_GT(strnlen(secondString, LongStringLength + 1), nctl::String::SmallBufferSize);
	printf("Creating a second string: %s\n", secondString);

	// using `String String::operator+(const char *cString) const`
	nctl::String thirdString = string_ + secondString;
	printString("Creating a third string by concatenating the first and the second one: ", thirdString);

	// Termination character is taken into account for capacity
	ASSERT_EQ(thirdString.length() + 1, thirdString.capacity());
	ASSERT_EQ(thirdString.capacity(), LongStringLength + String1Length + 1);
	ASSERT_EQ(thirdString.length(), LongStringLength + String1Length);
	ASSERT_STREQ(thirdString.data(), "String1String2...String2...String2");
}

TEST_F(StringOperationsTest, Format)
{
	string_.format("String%d", 2);
	printString("Resetting the string to a formatted one: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), strnlen("String2", 8));
	ASSERT_STREQ(string_.data(), "String2");
}

TEST_F(StringOperationsTest, FormatAndExtend)
{
	string_.format("VeryLongStringFormatting%d", 2);
	printString("Resetting the string to a long formatted one with truncation: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity * 2);
	ASSERT_EQ(string_.length(), strnlen("VeryLongStringFormatting2", 26));
	ASSERT_STREQ(string_.data(), "VeryLongStringFormatting2");
}

TEST_F(StringOperationsTest, FormatAppend)
{
	string_.formatAppend("String%d", 2);
	printString("Appending a formatted string to the first one: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), strnlen("String1String2", 15));
	ASSERT_STREQ(string_.data(), "String1String2");
}

TEST_F(StringOperationsTest, FormatAppendAndExtend)
{
	string_.formatAppend("VeryLongStringFormatting%d", 2);
	printString("Appending a long formatted string to the first one with truncation: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity * 2);
	ASSERT_EQ(string_.length(), strnlen("String1VeryLongStringFormatting2", 33));
	ASSERT_STREQ(string_.data(), "String1VeryLongStringFormatting2");
}

TEST_F(StringOperationsTest, SetLengthShrink)
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

TEST_F(StringOperationsTest, SetLengthExpand)
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

TEST_F(StringOperationsTest, SetLengthBeyondCapacity)
{
	const unsigned int newLength = string_.setLength(Capacity + 1);
	printString("Trying to set a length beyond string capacity: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), Capacity - 1);
	ASSERT_EQ(newLength, Capacity - 1);
	ASSERT_STREQ(string_.data(), String1Literal);
}

TEST_F(StringOperationsTest, SetCapacityExtendFromDynamicToDynamic)
{
	nctl::String newString(Capacity * 2);
	newString = String1Literal;
	const unsigned int length = newString.length();
	newString.setCapacity(Capacity * 4);
	printString("Extending string capacity when already on a dynamic buffer: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity * 4);
	ASSERT_EQ(newString.length(), length);
	ASSERT_STREQ(newString.data(), String1Literal);
}

TEST_F(StringOperationsTest, SetCapacityExtendFromLocalToDynamic)
{
	const unsigned int length = string_.length();
	string_.setCapacity(Capacity * 2);
	printString("Extending string capacity to go from the local buffer to the dynamic one: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity * 2);
	ASSERT_EQ(string_.length(), length);
	ASSERT_STREQ(string_.data(), String1Literal);
}

TEST_F(StringOperationsTest, SetCapacityShrinkFromDynamicToDynamic)
{
	nctl::String newString(Capacity * 4);
	newString = String1Literal;
	const unsigned int length = newString.length();
	newString.setCapacity(Capacity * 2);
	printString("Shrinking string capacity when already on a dynamic buffer: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity * 2);
	ASSERT_EQ(newString.length(), length);
	ASSERT_STREQ(newString.data(), String1Literal);
}

TEST_F(StringOperationsTest, SetCapacityShrinkFromDynamicToLocal)
{
	nctl::String newString(Capacity * 2);
	newString = String1Literal;
	const unsigned int length = newString.length();
	newString.setCapacity(Capacity);
	printString("Shrinking string capacity to go from a dynamic buffer to a local one: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), length);
	ASSERT_STREQ(newString.data(), String1Literal);
}

TEST_F(StringOperationsTest, SetCapacityShrinkFromLocalToLocal)
{
	const unsigned int length = string_.length();
	string_.setCapacity(Capacity / 2);
	printString("Shrinking string capacity when already on a local buffer: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), length);
	ASSERT_STREQ(string_.data(), String1Literal);
}

TEST_F(StringOperationsTest, ShrinkToFitFromDynamicToDynamic)
{
	nctl::String newString(Capacity * 2);
	newString = "String1String1String1String1";
	const unsigned int length = newString.length();
	newString.shrinkToFit();
	printString("Shrinking string to fit its content: ", newString);

	ASSERT_GT(newString.capacity(), nctl::String::SmallBufferSize);
	ASSERT_EQ(newString.capacity(), length + 1);
	ASSERT_EQ(newString.length(), length);
	ASSERT_STREQ(newString.data(), "String1String1String1String1");
}

TEST_F(StringOperationsTest, ShrinkToFitFromDynamicToLocal)
{
	nctl::String newString(Capacity * 2);
	newString = String1Literal;
	const unsigned int length = newString.length();
	newString.shrinkToFit();
	printString("Shrinking string to fit its content: ", newString);

	ASSERT_LE(newString.capacity(), nctl::String::SmallBufferSize);
	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), length);
	ASSERT_STREQ(newString.data(), String1Literal);
}

TEST_F(StringOperationsTest, ShrinkToFitFromLocalToLocal)
{
	const unsigned int length = string_.length();
	string_.shrinkToFit();
	printString("Shrinking string to fit its content: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), length);
	ASSERT_STREQ(string_.data(), String1Literal);
}

TEST_F(StringOperationsTest, FindFirstCharacter)
{
	string_ += String2Literal;
	printString("Appending a string to the first one: ", string_);

	const char character = 'S';
	const int position = string_.findFirstChar(character);
	printf("First position of the character '%c' in the string: %d\n", character, position);

	ASSERT_GE(position, 0);
	ASSERT_EQ(string_[static_cast<unsigned int>(position)], character);
	ASSERT_EQ(findFirstCharacter(string_.data(), string_.length(), character), position);
}

TEST_F(StringOperationsTest, FindFirstNonExistentCharacter)
{
	const char character = 'z';
	const int position = string_.findFirstChar(character);
	printf("First position of the character '%c' in the string: %d\n", character, position);

	ASSERT_EQ(position, -1);
}

TEST_F(StringOperationsTest, FindLastCharacter)
{
	string_ += String2Literal;
	printString("Appending a string to the first one: ", string_);

	const char character = 'S';
	const int position = string_.findLastChar(character);
	printf("Last position of the character '%c' in the string: %d\n", character, position);

	ASSERT_GE(position, 0);
	ASSERT_EQ(string_[static_cast<unsigned int>(position)], character);
	ASSERT_EQ(findLastCharacter(string_.data(), string_.length(), character), position);
}

TEST_F(StringOperationsTest, FindLastNonExistentCharacter)
{
	const char character = 'z';
	const int position = string_.findLastChar(character);
	printf("Last position of the character '%c' in the string: %d\n", character, position);

	ASSERT_EQ(position, -1);
}

TEST_F(StringOperationsTest, FindFirstCharacterAfterIndex)
{
	string_ += String2Literal;
	printString("Appending a string to the first one: ", string_);

	const char character = 't';
	const unsigned int index = 1;
	const int position = string_.findFirstCharAfterIndex(character, index);
	printf("Position of the character '%c' after position %u in the string: %d\n", character, index, position);

	ASSERT_GE(position, 0);
	ASSERT_EQ(string_[static_cast<unsigned int>(position)], character);
	ASSERT_EQ(findFirstCharacterAfterIndex(string_.data(), string_.length(), character, index), position);
}

TEST_F(StringOperationsTest, FindFirstNonExistentCharacterAfterIndex)
{
	const char character = 'z';
	const unsigned int index = 1;
	const int position = string_.findFirstCharAfterIndex(character, index);
	printf("Position of the character '%c' after position %u in the string: %d\n", character, index, position);

	ASSERT_EQ(position, -1);
}

TEST_F(StringOperationsTest, FindFirstCharacterAfterIndexBeyondLength)
{
	const char character = 'S';
	const unsigned int index = string_.length();
	const int position = string_.findFirstCharAfterIndex(character, index);
	printf("Position of the character '%c' after position %u in the string: %d\n", character, index, position);

	ASSERT_EQ(position, -1);
}

TEST_F(StringOperationsTest, FindString)
{
	const unsigned int oldLength = string_.length();

	nctl::String newString = String2Literal;
	printString("Creating a new string: ", newString);
	string_ += newString;
	printString("Appending the new string to the first one: ", string_);

	const int position = string_.find(newString);
	printf("Position of sub-string \"%s\" inside string \"%s\": %d\n", newString.data(), string_.data(), position);
	ASSERT_EQ(position, static_cast<int>(oldLength));
}

TEST_F(StringOperationsTest, FindNonExistentString)
{
	nctl::String newString = String2Literal;
	printString("Creating a new string: ", newString);

	const int position = string_.find(newString);
	printf("Position of sub-string \"%s\" inside string \"%s\": %d\n", newString.data(), string_.data(), position);
	ASSERT_EQ(position, -1);
}

TEST_F(StringOperationsTest, FindCString)
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

TEST_F(StringOperationsTest, FindNonExistentCString)
{
	const char *newString = String2Literal;
	printf("Creating a new string: %s\n", newString);

	const int position = string_.find(newString);
	printf("Position of sub-string \"%s\" inside string \"%s\": %d\n", newString, string_.data(), position);
	ASSERT_EQ(position, -1);
}

TEST_F(StringOperationsTest, CopyCharacters)
{
	nctl::String srcString = "0123456";
	printString("Creating a new source string: ", srcString);
	nctl::String destString = "abcdefghil";
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

TEST_F(StringOperationsTest, CopyCharactersToBeginning)
{
	nctl::String srcString = "0123456";
	printString("Creating a new source string: ", srcString);
	nctl::String destString = "abcdefghil";
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

TEST_F(StringOperationsTest, CopyAllCharactersToBeginning)
{
	nctl::String srcString = "0123456";
	printString("Creating a new source string: ", srcString);
	nctl::String destString = "abcdefghil";
	printString("Creating a new destination string: ", destString);

	const unsigned int numCopied = destString.assign(srcString);
	printf("Copying %u characters from position 0 of the source string to position 0 of the destination one: ", srcString.length());
	printString(destString);

	ASSERT_EQ(srcString.length(), destString.length()); // truncation
	ASSERT_EQ(numCopied, srcString.length());
	ASSERT_TRUE(charactersInStringsAreEqual(srcString.data(), destString.data(), 0, srcString.length(), 0));
}

TEST_F(StringOperationsTest, CopyCharactersFromBeyondEnd)
{
	nctl::String srcString = "0123456";
	printString("Creating a new source string: ", srcString);
	nctl::String destString = "abcdefghil";
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

TEST_F(StringOperationsTest, CopyCharactersToBeyondEndExtend)
{
	nctl::String srcString = "01234567890123456";
	printString("Creating a new source string: ", srcString);
	nctl::String destString = "abcdefghilmn";
	printString("Creating a new destination string: ", destString);

	const unsigned int numAvailable = destString.capacity() - destString.length() - 1;

	const unsigned int srcChar = 0;
	const unsigned int numChar = 17; // more than available in destination
	ASSERT_GT(numChar, numAvailable);
	const unsigned int destChar = destString.length() + 1; // beyond the end of destination
	const unsigned int numCopied = destString.replace(srcString, srcChar, numChar, destChar);
	printString("Trying to copy a character from the source string to beyond the end of the destination one: ", destString);

	ASSERT_EQ(destString.capacity(), Capacity * 2);
	ASSERT_NE(srcString.length(), destString.length()); // no truncation
	ASSERT_GT(numCopied, numAvailable);
	ASSERT_EQ(numCopied, numChar);
}

TEST_F(StringOperationsTest, CopyMoreCharactersThanSourceLength)
{
	nctl::String srcString = "012";
	printString("Creating a new source string: ", srcString);
	nctl::String destString = "abcdefg";
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

TEST_F(StringOperationsTest, CopyMoreCharactersThanDestinationLength)
{
	nctl::String srcString = "0123456";
	printString("Creating a new source string: ", srcString);
	nctl::String destString = "abc";
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

TEST_F(StringOperationsTest, CopyCharactersFromCString)
{
	const unsigned int stringLength = 3;
	char srcString[stringLength + 1] = "abc";
	nctl::String destString = "0123456";
	printString("Creating a new destination string: ", destString);

	const unsigned int srcChar = 0;
	const unsigned int numChar = stringLength;
	const unsigned int numCopied = destString.assign(srcString, srcChar, numChar);
	printf("Copying %u characters from an array of %u characters to the string: \"%s\"\n", numChar, stringLength, destString.data());

	ASSERT_EQ(stringLength, destString.length()); // truncation
	ASSERT_EQ(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcString, destString.data(), 0, numCopied, 0));
}

TEST_F(StringOperationsTest, CopyCharactersFromCStringBeginning)
{
	const unsigned int stringLength = 3;
	char srcString[stringLength + 1] = "abc";
	nctl::String destString = "0123456";
	printString("Creating a new destination string: ", destString);

	const unsigned int numChar = stringLength;
	const unsigned int numCopied = destString.assign(srcString, numChar);
	printf("Copying %u characters from an array of %u characters to the beginning of the string: \"%s\"\n", numChar, stringLength, destString.data());

	ASSERT_EQ(destString.length(), numCopied); // truncation
	ASSERT_EQ(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcString, destString.data(), 0, numCopied, 0));
}

TEST_F(StringOperationsTest, CopyCharactersToCString)
{
	nctl::String srcString = "abc";
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

TEST_F(StringOperationsTest, CopyAllCharactersToCString)
{
	nctl::String srcString = "abc";
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
