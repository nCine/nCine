﻿#include "gtest_string.h"

namespace {

class StringOperationsTest : public ::testing::Test
{
  public:
	StringOperationsTest()
	    : string_(Capacity), fixedString_(Capacity, nctl::StringMode::FIXED_CAPACITY) {}

  protected:
	void SetUp() override
	{
		string_ = "String1";
		fixedString_ = "String1";
	}

	nctl::String string_;
	nctl::String fixedString_;
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
	nctl::String newString = veryLongCString;
	printString("Creating a new string: ", newString);

	string_.append(newString);
	printString("Appending a new string to the first one: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity * 8);
	ASSERT_EQ(string_.length(), strnlen("String1", 8) + strnlen(veryLongCString, 78));
	ASSERT_STREQ(string_.data(), "String1This_is_a_very_long_string_which_goes_beyond_the_default_capacity_of_a_string");
}

TEST_F(StringOperationsTest, AppendStringTruncate)
{
	nctl::String newString = veryLongCString;
	printString("Creating a new string: ", newString);

	fixedString_.append(newString);
	printString("Appending a new string to the first one: ", fixedString_);

	ASSERT_EQ(fixedString_.capacity(), Capacity);
	ASSERT_EQ(fixedString_.length(), strnlen("String1This_is_", Capacity));
	ASSERT_STREQ(fixedString_.data(), "String1This_is_");
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
	printf("Using a very long C string: %s\n", veryLongCString);

	string_.append(veryLongCString);
	printString("Appending a C string to the first one: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity * 8);
	ASSERT_EQ(string_.length(), strnlen("String1", 8) + strnlen(veryLongCString, 78));
	ASSERT_STREQ(string_.data(), "String1This_is_a_very_long_string_which_goes_beyond_the_default_capacity_of_a_string");
}

TEST_F(StringOperationsTest, AppendCStringTruncate)
{
	printf("Using a very long C string: %s\n", veryLongCString);

	fixedString_.append(veryLongCString);
	printString("Appending a C string to the first one: ", fixedString_);

	ASSERT_EQ(fixedString_.capacity(), Capacity);
	ASSERT_EQ(fixedString_.length(), strnlen("String1This_is_", Capacity));
	ASSERT_STREQ(fixedString_.data(), "String1This_is_");
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
	nctl::String newString = "String2";
	printString("Creating a new string from a C-style string assignment: ", newString);

	// Termination character is taken into account for capacity
	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), strnlen("String2", 8));
	ASSERT_STREQ(newString.data(), "String2");
}

TEST_F(StringOperationsTest, ImplicitCStringConstructorLong)
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

TEST_F(StringOperationsTest, CopyConstruction)
{
	nctl::String newString(string_);
	printString("Creating a new string as a copy of the first one: ", newString);

	ASSERT_EQ(newString.capacity(), string_.capacity());
	ASSERT_EQ(newString.length(), string_.length());
	ASSERT_STREQ(newString.data(), string_.data());

	printf("Appending a very long string to the first and the second string\n");
	string_.append(veryLongCString);
	newString.append(veryLongCString);
	ASSERT_EQ(newString.capacity(), string_.capacity());
	ASSERT_EQ(newString.length(), string_.length());
	ASSERT_STREQ(newString.data(), string_.data());
}

TEST_F(StringOperationsTest, CopyConstructionFixed)
{
	nctl::String newString(fixedString_);
	printString("Creating a new string as a copy of the first one (fixed): ", newString);

	ASSERT_EQ(newString.capacity(), fixedString_.capacity());
	ASSERT_EQ(newString.length(), fixedString_.length());
	ASSERT_STREQ(newString.data(), fixedString_.data());

	printf("Appending a very long string to the first and the second string\n");
	fixedString_.append(veryLongCString);
	newString.append(veryLongCString);
	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.capacity(), fixedString_.capacity());
	ASSERT_EQ(newString.length(), fixedString_.length());
	ASSERT_STREQ(newString.data(), fixedString_.data());
}

TEST_F(StringOperationsTest, MoveConstruction)
{
	nctl::String newString(nctl::move(string_));
	printString("Creating a new string moving from the first one: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), strnlen("String1", 8));
	ASSERT_STREQ(newString.data(), "String1");
	ASSERT_EQ(string_.capacity(), 0);
	ASSERT_EQ(string_.length(), 0);

	printf("Appending a very long string to the second string\n");
	newString.append(veryLongCString);

	ASSERT_EQ(newString.capacity(), Capacity * 8);
	ASSERT_GT(newString.length(), strnlen("String1", 8));
}

TEST_F(StringOperationsTest, MoveConstructionFixed)
{
	nctl::String newString(nctl::move(fixedString_));
	printString("Creating a new string moving from the first one (fixed): ", newString);

	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), strnlen("String1", 8));
	ASSERT_STREQ(newString.data(), "String1");
	ASSERT_EQ(fixedString_.capacity(), 0);
	ASSERT_EQ(fixedString_.length(), 0);

	printf("Appending a very long string to the second string\n");
	newString.append(veryLongCString);

	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), Capacity - 1);
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
	nctl::String longString(veryLongCString);
	string_ = longString;
	printString("Extending a string with the assignment operator: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity * 8);
	ASSERT_EQ(string_.length(), longString.length());
	ASSERT_STREQ(string_.data(), longString.data());
}

TEST_F(StringOperationsTest, AssignmentOperatorTruncate)
{
	nctl::String longString(veryLongCString);
	fixedString_ = longString;
	printString("Filling a fixed string with the assignment operator: ", fixedString_);

	// The assignment operator does not copy the `fixedCapacity` value
	ASSERT_EQ(fixedString_.capacity(), Capacity);
	ASSERT_EQ(fixedString_.length(), fixedString_.capacity() - 1);
	ASSERT_STREQ(fixedString_.data(), "This_is_a_very_");
}

TEST_F(StringOperationsTest, AssignmentOperatorShorter)
{
	nctl::String newString("String1");
	string_ = "LongString1";
	string_ = newString;
	printString("Shrinking a string with the assignment operator: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), newString.length());
	ASSERT_STREQ(string_.data(), "String1");
}

TEST_F(StringOperationsTest, MoveAssignmentOperator)
{
	nctl::String newString(Capacity);
	newString = nctl::move(string_);
	printString("Creating a new string with the move assignment operator: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), strnlen("String1", 8));
	ASSERT_STREQ(newString.data(), "String1");
	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), 0);
}

TEST_F(StringOperationsTest, MoveAssignmentOperatorExtend)
{
	nctl::String longString(veryLongCString);
	nctl::String newString(Capacity);
	newString = nctl::move(longString);
	printString("Extending a string with the move assignment operator: ", newString);

	ASSERT_GT(newString.capacity(), Capacity);
	ASSERT_GT(newString.length(), Capacity);
	ASSERT_EQ(longString.capacity(), Capacity);
	ASSERT_EQ(longString.length(), 0);
}

TEST_F(StringOperationsTest, MoveAssignmentOperatorTruncate)
{
	nctl::String longString(veryLongCString);
	nctl::String newString(Capacity, nctl::StringMode::FIXED_CAPACITY);
	newString = nctl::move(longString);
	printString("Filling a fixed string with the move assignment operator: ", newString);

	// The move assignment operator also moves the `fixedCapacity` value
	ASSERT_GT(newString.capacity(), Capacity);
	ASSERT_GT(newString.length(), Capacity);
	ASSERT_EQ(longString.capacity(), Capacity);
	ASSERT_EQ(longString.length(), 0);
}

TEST_F(StringOperationsTest, MoveAssignmentOperatorShorter)
{
	nctl::String shortString("String1");
	nctl::String newString("LongString1");
	newString = nctl::move(shortString);
	printString("Shrinking a string with the move assignment operator: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), strnlen("String1", 8));
	ASSERT_STREQ(newString.data(), "String1");
}

TEST_F(StringOperationsTest, SelfAssignment)
{
	string_ = string_;
	printString("Assigning the string to itself with the assignment operator: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), strnlen("String1", 8));
	ASSERT_STREQ(string_.data(), "String1");
}

TEST_F(StringOperationsTest, AssignLongCStringExtend)
{
	nctl::String newString;
	// The C-style string is longer than default string capacity
	newString = "String2...String2";
	printString("Creating a new string with the assignment operator from a C-style string: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity * 2);
	ASSERT_EQ(newString.length(), strnlen("String2...String2", 17));
	ASSERT_STREQ(newString.data(), "String2...String2");
}

TEST_F(StringOperationsTest, AssignLongCStringTruncate)
{
	nctl::String newString(nctl::StringMode::FIXED_CAPACITY);
	// The C-style string is longer than default string capacity
	newString = "String2...String2";
	printString("Creating a new string with the assignment operator from a C-style string: ", newString);

	// Termination character is taken into account for capacity
	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length() + 1, Capacity);
	ASSERT_STREQ(newString.data(), "String2...Strin");
}

TEST_F(StringOperationsTest, ConstructByConcatenation)
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

TEST_F(StringOperationsTest, ConstructByConcatenationWithCStringAsFirst)
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

TEST_F(StringOperationsTest, ConstructByConcatenationWithCStringAsSecond)
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

TEST_F(StringOperationsTest, FormatAndTruncate)
{
	fixedString_.format("VeryLongStringFormatting%d", 2);
	printString("Resetting the string to a long formatted one with truncation: ", fixedString_);

	ASSERT_EQ(fixedString_.capacity(), Capacity);
	ASSERT_EQ(fixedString_.length(), strnlen("VeryLongStringF", Capacity));
	ASSERT_STREQ(fixedString_.data(), "VeryLongStringF");
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

	ASSERT_EQ(string_.capacity(), Capacity * 4);
	ASSERT_EQ(string_.length(), strnlen("String1VeryLongStringFormatting2", 33));
	ASSERT_STREQ(string_.data(), "String1VeryLongStringFormatting2");
}

TEST_F(StringOperationsTest, FormatAppendAndTruncate)
{
	fixedString_.formatAppend("VeryLongStringFormatting%d", 2);
	printString("Appending a long formatted string to the first one with truncation: ", fixedString_);

	ASSERT_EQ(fixedString_.capacity(), Capacity);
	ASSERT_EQ(fixedString_.length(), strnlen("String1VeryLong", 15));
	ASSERT_STREQ(fixedString_.data(), "String1VeryLong");
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
	ASSERT_STREQ(string_.data(), "String1");
}

TEST_F(StringOperationsTest, SetCapacityExtendFromDynamicToDynamic)
{
	nctl::String newString(Capacity * 2);
	newString = "String1";
	const unsigned int length = newString.length();
	newString.setCapacity(Capacity * 4);
	printString("Extending string capacity when already on a dynamic buffer: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity * 4);
	ASSERT_EQ(newString.length(), length);
	ASSERT_STREQ(newString.data(), "String1");
}

TEST_F(StringOperationsTest, SetCapacityExtendFromLocalToDynamic)
{
	const unsigned int length = string_.length();
	string_.setCapacity(Capacity * 2);
	printString("Extending string capacity to go from the local buffer to the dynamic one: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity * 2);
	ASSERT_EQ(string_.length(), length);
	ASSERT_STREQ(string_.data(), "String1");
}

TEST_F(StringOperationsTest, SetCapacityShrinkFromDynamicToDynamic)
{
	nctl::String newString(Capacity * 4);
	newString = "String1";
	const unsigned int length = newString.length();
	newString.setCapacity(Capacity * 2);
	printString("Shrinking string capacity when already on a dynamic buffer: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity * 2);
	ASSERT_EQ(newString.length(), length);
	ASSERT_STREQ(newString.data(), "String1");
}

TEST_F(StringOperationsTest, SetCapacityShrinkFromDynamicToLocal)
{
	nctl::String newString(Capacity * 2);
	newString = "String1";
	const unsigned int length = newString.length();
	newString.setCapacity(Capacity);
	printString("Shrinking string capacity to go from a dynamic buffer to a local one: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), length);
	ASSERT_STREQ(newString.data(), "String1");
}

TEST_F(StringOperationsTest, SetCapacityShrinkFromLocalToLocal)
{
	const unsigned int length = string_.length();
	string_.setCapacity(Capacity / 2);
	printString("Shrinking string capacity when already on a local buffer: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), length);
	ASSERT_STREQ(string_.data(), "String1");
}

TEST_F(StringOperationsTest, SetCapacityOnFixed)
{
	const unsigned int length = fixedString_.length();
	fixedString_.setCapacity(Capacity * 2);
	printString("Trying to extend the capacity of a fixed string: ", fixedString_);

	ASSERT_EQ(fixedString_.capacity(), Capacity);
	ASSERT_EQ(fixedString_.length(), length);
	ASSERT_STREQ(fixedString_.data(), "String1");
}

TEST_F(StringOperationsTest, ShrinkToFitFromDynamicToDynamic)
{
	nctl::String newString(Capacity * 2);
	newString = "String1String1String1";
	const unsigned int length = newString.length();
	newString.shrinkToFit();
	printString("Shrinking string to fit its content: ", newString);

	ASSERT_EQ(newString.capacity(), length + 1);
	ASSERT_EQ(newString.length(), length);
	ASSERT_STREQ(newString.data(), "String1String1String1");
}

TEST_F(StringOperationsTest, ShrinkToFitFromDynamicToLocal)
{
	nctl::String newString(Capacity * 2);
	newString = "String1";
	const unsigned int length = newString.length();
	newString.shrinkToFit();
	printString("Shrinking string to fit its content: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), length);
	ASSERT_STREQ(newString.data(), "String1");
}

TEST_F(StringOperationsTest, ShrinkToFitFromLocalToLocal)
{
	const unsigned int length = string_.length();
	string_.shrinkToFit();
	printString("Shrinking string to fit its content: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), length);
	ASSERT_STREQ(string_.data(), "String1");
}

TEST_F(StringOperationsTest, FindFirstCharacter)
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

TEST_F(StringOperationsTest, FindFirstNonExistentCharacter)
{
	const char character = 'z';
	const int position = string_.findFirstChar(character);
	printf("First position of the character '%c' in the string: %d\n", character, position);

	ASSERT_EQ(position, -1);
}

TEST_F(StringOperationsTest, FindLastCharacter)
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

TEST_F(StringOperationsTest, FindLastNonExistentCharacter)
{
	const char character = 'z';
	const int position = string_.findLastChar(character);
	printf("Last position of the character '%c' in the string: %d\n", character, position);

	ASSERT_EQ(position, -1);
}

TEST_F(StringOperationsTest, FindFirstCharacterAfterIndex)
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

	nctl::String newString = "String2";
	printString("Creating a new string: ", newString);
	string_ += newString;
	printString("Appending the new string to the first one: ", string_);

	const int position = string_.find(newString);
	printf("Position of sub-string \"%s\" inside string \"%s\": %d\n", newString.data(), string_.data(), position);
	ASSERT_EQ(position, static_cast<int>(oldLength));
}

TEST_F(StringOperationsTest, FindNonExistentString)
{
	nctl::String newString = "String2";
	printString("Creating a new string: ", newString);

	const int position = string_.find(newString);
	printf("Position of sub-string \"%s\" inside string \"%s\": %d\n", newString.data(), string_.data(), position);
	ASSERT_EQ(position, -1);
}

TEST_F(StringOperationsTest, FindCString)
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

TEST_F(StringOperationsTest, FindNonExistentCString)
{
	const char *newString = "String2";
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
	nctl::String srcString = "0123456";
	printString("Creating a new source string: ", srcString);
	nctl::String destString = "abcdefghilmn";
	printString("Creating a new destination string: ", destString);

	const unsigned int numAvailable = destString.capacity() - destString.length() - 1;

	const unsigned int srcChar = 0;
	const unsigned int numChar = 6; // more than available in destination
	const unsigned int destChar = destString.length() + 1; // beyond the end of destination
	const unsigned int numCopied = destString.replace(srcString, srcChar, numChar, destChar);
	printString("Trying to copy a character from the source string to beyond the end of the destination one: ", destString);

	ASSERT_EQ(destString.capacity(), Capacity * 2);
	ASSERT_NE(srcString.length(), destString.length()); // no truncation
	ASSERT_GT(numCopied, numAvailable);
	ASSERT_EQ(numCopied, numChar);
}

TEST_F(StringOperationsTest, CopyCharactersToBeyondEndFixed)
{
	nctl::String srcString = "0123456";
	printString("Creating a new source string: ", srcString);
	nctl::String destString("abcdefghilmn", nctl::StringMode::FIXED_CAPACITY);
	printString("Creating a new destination string: ", destString);

	const unsigned int numAvailable = destString.capacity() - destString.length() - 1;

	const unsigned int srcChar = 0;
	const unsigned int numChar = 6; // more than available in destination
	const unsigned int destChar = destString.length() + 1; // beyond the end of destination
	const unsigned int numCopied = destString.replace(srcString, srcChar, numChar, destChar);
	printString("Trying to copy a character from the source string to beyond the end of the destination one: ", destString);

	ASSERT_EQ(destString.capacity(), Capacity);
	ASSERT_NE(srcString.length(), destString.length()); // no truncation
	ASSERT_EQ(numCopied, numAvailable);
	ASSERT_NE(numCopied, numChar);
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
