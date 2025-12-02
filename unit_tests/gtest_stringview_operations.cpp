#include "gtest_stringview.h"

namespace {

const unsigned int String1ExtraCapacity = 16;
char string1ExtraArray[String1ExtraCapacity] = "String1";

const char *SrcStringLiteral = "0123456";
const unsigned int SrcStringLength = 7;
char srcStringArray[SrcStringLength + 1] = "0123456";

const char *DestStringLiteral = "abcdefghil";
const unsigned int DestStringLength = 10;
char destStringArray[DestStringLength + 1] = "abcdefghil";

class StringViewOperationsTest : public ::testing::Test
{
  public:
	StringViewOperationsTest()
	    : stringView_(string1Array, String1Length + 1),
	      stringViewExtra_(string1ExtraArray, String1ExtraCapacity) {}

  protected:
	void SetUp() override
	{
		nctl::strncpy(string1Array, String1Literal, String1Length + 1);
		nctl::strncpy(string1ExtraArray, String1Literal, String1ExtraCapacity);
		nctl::strncpy(srcStringArray, SrcStringLiteral, SrcStringLength + 1);
		nctl::strncpy(destStringArray, DestStringLiteral, DestStringLength + 1);

		stringView_.set(string1Array, String1Length + 1);
		stringViewExtra_.set(string1ExtraArray, String1ExtraCapacity);
	}

	nctl::StringView stringView_;
	nctl::StringView stringViewExtra_;
};

TEST_F(StringViewOperationsTest, AppendString)
{
	char appendStringArray[] = "Append";
	nctl::StringView newStringView(appendStringArray);
	printStringView("Creating a new string view: ", newStringView);

	stringViewExtra_.append(newStringView);
	printStringView("Appending a new string view to the first one: ", stringViewExtra_);

	ASSERT_EQ(stringViewExtra_.capacity(), String1ExtraCapacity);
	ASSERT_EQ(stringViewExtra_.length(), strnlen("String1Append", 14));
	ASSERT_STREQ(stringViewExtra_.data(), "String1Append");
}

TEST_F(StringViewOperationsTest, AppendCString)
{
	const char *newString = "Append";
	printf("Creating a new C string: %s\n", newString);

	stringViewExtra_.append(newString);
	printStringView("Appending a C string to the string view: ", stringViewExtra_);

	ASSERT_EQ(stringViewExtra_.capacity(), String1ExtraCapacity);
	ASSERT_EQ(stringViewExtra_.length(), strnlen("String1Append", 14));
	ASSERT_STREQ(stringViewExtra_.data(), "String1Append");
}

TEST_F(StringViewOperationsTest, Clear)
{
	stringView_.clear();
	printStringView("Clearing the string view: ", stringView_);

	ASSERT_EQ(stringView_.capacity(), String1Length + 1);
	ASSERT_EQ(stringView_.length(), 0u);
}

TEST_F(StringViewOperationsTest, RemovePrefix)
{
	stringView_.removePrefix(1);
	printStringView("Shrinking the string view from the start: ", stringView_);

	ASSERT_EQ(stringView_.capacity(), String1Length);
	ASSERT_EQ(stringView_.length(), String1Length - 1);
	ASSERT_STREQ(stringView_.data(), "tring1");
}

TEST_F(StringViewOperationsTest, RemovePrefixAll)
{
	stringView_.removePrefix(stringView_.length());
	printStringView("Shrinking the whole string view from the start: ", stringView_);

	ASSERT_EQ(stringView_.capacity(), 1);
	ASSERT_EQ(stringView_.length(), 0);
	ASSERT_STREQ(stringView_.data(), "");
}

TEST_F(StringViewOperationsTest, RemovePrefixBeyondEnd)
{
	stringView_.removePrefix(stringView_.length() + 1);
	printStringView("Shrinking the whole string view from the start: ", stringView_);

	ASSERT_EQ(stringView_.capacity(), String1Length + 1);
	ASSERT_EQ(stringView_.length(), String1Length);
	ASSERT_STREQ(stringView_.data(), String1Literal);
}

TEST_F(StringViewOperationsTest, RemoveSuffix)
{
	stringView_.removeSuffix(1);
	printStringView("Shrinking the string view from the end: ", stringView_);

	ASSERT_EQ(stringView_.capacity(), String1Length);
	ASSERT_EQ(stringView_.length(), String1Length - 1);
	ASSERT_STREQ(stringView_.data(), "String");
}

TEST_F(StringViewOperationsTest, RemoveSuffixAll)
{
	stringView_.removeSuffix(stringView_.length());
	printStringView("Shrinking the whole string view from the end: ", stringView_);

	ASSERT_EQ(stringView_.capacity(), 1);
	ASSERT_EQ(stringView_.length(), 0);
	ASSERT_STREQ(stringView_.data(), "");
}

TEST_F(StringViewOperationsTest, RemoveSuffixBeyondStart)
{
	stringView_.removeSuffix(stringView_.length() + 1);
	printStringView("Shrinking the whole string view from the end: ", stringView_);

	ASSERT_EQ(stringView_.capacity(), String1Length + 1);
	ASSERT_EQ(stringView_.length(), String1Length);
	ASSERT_STREQ(stringView_.data(), String1Literal);
}

TEST_F(StringViewOperationsTest, CopyConstruction)
{
	nctl::StringView newStringView(stringView_);
	printStringView("Creating a new string view as a copy of the first one: ", newStringView);

	ASSERT_EQ(newStringView.capacity(), stringView_.capacity());
	ASSERT_EQ(newStringView.length(), stringView_.length());
	ASSERT_STREQ(newStringView.data(), stringView_.data());

	printf("Appending a very long string to the first string view\n");
	stringView_.append(VeryLongCStringLiteral);
	ASSERT_EQ(newStringView.capacity(), stringView_.capacity());
	ASSERT_EQ(newStringView.length(), stringView_.length());
	ASSERT_STREQ(newStringView.data(), stringView_.data());
}

TEST_F(StringViewOperationsTest, MoveConstruction)
{
	nctl::StringView newStringView(nctl::move(stringView_));
	printStringView("Creating a new string view moving from the first one: ", newStringView);

	ASSERT_EQ(newStringView.capacity(), String1Length + 1);
	ASSERT_EQ(newStringView.length(), String1Length);
	ASSERT_STREQ(newStringView.data(), String1Literal);
	ASSERT_EQ(stringView_.capacity(), 1);
	ASSERT_EQ(stringView_.length(), 0);
	ASSERT_STREQ(stringView_.data(), "");

	printf("Appending a very long string to the string view\n");
	newStringView.append(VeryLongCStringLiteral);

	ASSERT_EQ(newStringView.capacity(), String1Length + 1);
	ASSERT_EQ(newStringView.length(), String1Length);
}

TEST_F(StringViewOperationsTest, AssignmentOperator)
{
	nctl::StringView newStringView;
	newStringView = stringView_;
	printStringView("Filling a new string view with the assignment operator: ", newStringView);

	ASSERT_EQ(newStringView.capacity(), stringView_.capacity());
	ASSERT_EQ(newStringView.length(), stringView_.length());
	ASSERT_STREQ(newStringView.data(), stringView_.data());
}

TEST_F(StringViewOperationsTest, MoveAssignmentOperator)
{
	nctl::StringView newStringView;
	newStringView = nctl::move(stringView_);
	printStringView("Creating a new string view with the move assignment operator: ", newStringView);

	ASSERT_EQ(newStringView.capacity(), String1Length + 1);
	ASSERT_EQ(newStringView.length(), String1Length);
	ASSERT_STREQ(newStringView.data(), String1Literal);
	ASSERT_EQ(stringView_.capacity(), 1);
	ASSERT_EQ(stringView_.length(), 0);
	ASSERT_STREQ(stringView_.data(), "");
}

TEST_F(StringViewOperationsTest, SelfAssignment)
{
	stringView_ = stringView_;
	printStringView("Assigning the string view to itself with the assignment operator: ", stringView_);

	ASSERT_EQ(stringView_.capacity(), String1Length + 1);
	ASSERT_EQ(stringView_.length(), String1Length);
	ASSERT_STREQ(stringView_.data(), String1Literal);
}

TEST_F(StringViewOperationsTest, AssignLongCStringExtend)
{
	char longString[] = "String2...String2";
	nctl::StringView newStringView = longString;
	printStringView("Creating a new string view with the assignment operator from a C-style string: ", newStringView);

	ASSERT_EQ(newStringView.capacity(), strnlen("String2...String2", 17) + 1);
	ASSERT_EQ(newStringView.length(), strnlen("String2...String2", 17));
	ASSERT_STREQ(newStringView.data(), "String2...String2");
}

TEST_F(StringViewOperationsTest, Format)
{
	stringView_.format("String%d", 2);
	printStringView("Resetting the string view to a formatted one: ", stringView_);

	ASSERT_EQ(stringView_.capacity(), String1Length + 1);
	ASSERT_EQ(stringView_.length(), strnlen("String2", 8));
	ASSERT_STREQ(stringView_.data(), "String2");
}

TEST_F(StringViewOperationsTest, FormatAppend)
{
	stringViewExtra_.formatAppend("String%d", 2);
	printStringView("Appending a formatted string to the string view: ", stringViewExtra_);

	ASSERT_EQ(stringViewExtra_.capacity(), String1ExtraCapacity);
	ASSERT_EQ(stringViewExtra_.length(), strnlen("String1String2", 15));
	ASSERT_STREQ(stringViewExtra_.data(), "String1String2");
}

TEST_F(StringViewOperationsTest, SetLengthShrink)
{
	const unsigned int length = stringView_.length();
	stringView_.data()[length - 1] = '\0';
	const unsigned int newLength = stringView_.setLength(length - 1);
	printStringView("Removing a single character by modifying the string view length: ", stringView_);

	ASSERT_EQ(stringView_.capacity(), String1Length + 1);
	ASSERT_EQ(stringView_.length(), length - 1);
	ASSERT_EQ(newLength, length - 1);
	ASSERT_STREQ(stringView_.data(), "String");
}

TEST_F(StringViewOperationsTest, SetLengthExpand)
{
	const unsigned int length = stringViewExtra_.length();
	stringViewExtra_.data()[length + 0] = '2';
	stringViewExtra_.data()[length + 1] = '3';
	stringViewExtra_.data()[length + 2] = '\0';
	const unsigned int newLength = stringViewExtra_.setLength(length + 2);
	printStringView("Appending a couple characters by accessing the data array: ", stringViewExtra_);

	ASSERT_EQ(stringViewExtra_.capacity(), String1ExtraCapacity);
	ASSERT_EQ(stringViewExtra_.length(), length + 2);
	ASSERT_EQ(newLength, length + 2);
	ASSERT_STREQ(stringViewExtra_.data(), "String123");
}

TEST_F(StringViewOperationsTest, SetLengthBeyondCapacity)
{
	const unsigned int newLength = stringView_.setLength(String1Length + 1 + 1);
	printStringView("Trying to set a length beyond string view capacity: ", stringView_);

	ASSERT_EQ(stringView_.capacity(), String1Length + 1);
	ASSERT_EQ(stringView_.length(), String1Length);
	ASSERT_EQ(newLength, String1Length);
	ASSERT_STREQ(stringView_.data(), String1Literal);
}

TEST_F(StringViewOperationsTest, FindFirstCharacter)
{
	stringViewExtra_ += String2Literal;
	printStringView("Appending a string to the string view: ", stringViewExtra_);

	const char character = 'S';
	const int position = stringViewExtra_.findFirstChar(character);
	printf("First position of the character '%c' in the string view: %d\n", character, position);

	ASSERT_GE(position, 0);
	ASSERT_EQ(stringViewExtra_[static_cast<unsigned int>(position)], character);
	ASSERT_EQ(findFirstCharacter(stringViewExtra_.data(), stringViewExtra_.length(), character), position);
}

TEST_F(StringViewOperationsTest, FindFirstNonExistentCharacter)
{
	const char character = 'z';
	const int position = stringView_.findFirstChar(character);
	printf("First position of the character '%c' in the string view: %d\n", character, position);

	ASSERT_EQ(position, -1);
}

TEST_F(StringViewOperationsTest, FindLastCharacter)
{
	stringViewExtra_ += String2Literal;
	printStringView("Appending a string to the string view: ", stringViewExtra_);

	const char character = 'S';
	const int position = stringViewExtra_.findLastChar(character);
	printf("Last position of the character '%c' in the string view: %d\n", character, position);

	ASSERT_GE(position, 0);
	ASSERT_EQ(stringViewExtra_[static_cast<unsigned int>(position)], character);
	ASSERT_EQ(findLastCharacter(stringViewExtra_.data(), stringViewExtra_.length(), character), position);
}

TEST_F(StringViewOperationsTest, FindLastNonExistentCharacter)
{
	const char character = 'z';
	const int position = stringViewExtra_.findLastChar(character);
	printf("Last position of the character '%c' in the string view: %d\n", character, position);

	ASSERT_EQ(position, -1);
}

TEST_F(StringViewOperationsTest, FindFirstCharacterAfterIndex)
{
	stringViewExtra_ += String2Literal;
	printStringView("Appending a string to the string view: ", stringViewExtra_);

	const char character = 't';
	const unsigned int index = 1;
	const int position = stringViewExtra_.findFirstCharAfterIndex(character, index);
	printf("Position of the character '%c' after position %u in the string view: %d\n", character, index, position);

	ASSERT_GE(position, 0);
	ASSERT_EQ(stringViewExtra_[static_cast<unsigned int>(position)], character);
	ASSERT_EQ(findFirstCharacterAfterIndex(stringViewExtra_.data(), stringViewExtra_.length(), character, index), position);
}

TEST_F(StringViewOperationsTest, FindFirstNonExistentCharacterAfterIndex)
{
	const char character = 'z';
	const unsigned int index = 1;
	const int position = stringViewExtra_.findFirstCharAfterIndex(character, index);
	printf("Position of the character '%c' after position %u in the string view: %d\n", character, index, position);

	ASSERT_EQ(position, -1);
}

TEST_F(StringViewOperationsTest, FindFirstCharacterAfterIndexBeyondLength)
{
	const char character = 'S';
	const unsigned int index = stringViewExtra_.length();
	const int position = stringViewExtra_.findFirstCharAfterIndex(character, index);
	printf("Position of the character '%c' after position %u in the string view: %d\n", character, index, position);

	ASSERT_EQ(position, -1);
}

TEST_F(StringViewOperationsTest, FindString)
{
	const unsigned int oldLength = stringViewExtra_.length();

	nctl::StringView newStringView(string2Array);
	printStringView("Creating a new string view: ", newStringView);
	stringViewExtra_ += newStringView;
	printStringView("Appending the new string view to the first one: ", stringViewExtra_);

	const int position = stringViewExtra_.find(newStringView);
	printf("Position of sub-string \"%s\" inside string view \"%s\": %d\n", newStringView.data(), stringViewExtra_.data(), position);
	ASSERT_EQ(position, static_cast<int>(oldLength));
}

TEST_F(StringViewOperationsTest, FindNonExistentString)
{
	nctl::StringView newStringView(string2Array);
	printStringView("Creating a new string view: ", newStringView);

	const int position = stringViewExtra_.find(newStringView);
	printf("Position of sub-string \"%s\" inside string view \"%s\": %d\n", newStringView.data(), stringViewExtra_.data(), position);
	ASSERT_EQ(position, -1);
}

TEST_F(StringViewOperationsTest, FindCString)
{
	const unsigned int oldLength = stringViewExtra_.length();

	const char *newString = String2Literal;
	printf("Creating a new string: %s\n", newString);
	stringViewExtra_ += newString;
	printStringView("Appending the new string to the string view: ", stringViewExtra_);

	const int position = stringViewExtra_.find(newString);
	printf("Position of sub-string \"%s\" inside string view \"%s\": %d\n", newString, stringViewExtra_.data(), position);
	ASSERT_EQ(position, static_cast<int>(oldLength));
}

TEST_F(StringViewOperationsTest, FindNonExistentCString)
{
	const char *newString = String2Literal;
	printf("Creating a new string: %s\n", newString);

	const int position = stringViewExtra_.find(newString);
	printf("Position of sub-string \"%s\" inside string view \"%s\": %d\n", newString, stringViewExtra_.data(), position);
	ASSERT_EQ(position, -1);
}

TEST_F(StringViewOperationsTest, CopyCharacters)
{
	nctl::StringView srcStringView = srcStringArray;
	printStringView("Creating a new source string view: ", srcStringView);
	nctl::StringView destStringView = destStringArray;
	printStringView("Creating a new destination string view: ", destStringView);

	const unsigned int srcChar = 4;
	const unsigned int numChar = 3;
	const unsigned int destChar = 2;
	const unsigned int numCopied = destStringView.replace(srcStringView, srcChar, numChar, destChar);
	printf("Copying %u characters from position %u of the source string view to position %u of the destination one: ", numChar, srcChar, destChar);
	printStringView(destStringView);

	ASSERT_NE(srcStringView.length(), destStringView.length()); // no truncation
	ASSERT_EQ(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcStringView.data(), destStringView.data(), srcChar, numCopied, destChar));
}

TEST_F(StringViewOperationsTest, CopyCharactersToBeginning)
{
	nctl::StringView srcStringView = srcStringArray;
	printStringView("Creating a new source string view: ", srcStringView);
	nctl::StringView destStringView = destStringArray;
	printStringView("Creating a new destination string view: ", destStringView);

	const unsigned int srcChar = 4;
	const unsigned int numChar = 3;
	const unsigned int numCopied = destStringView.assign(srcStringView, srcChar, numChar);
	printf("Copying %u characters from position %u of the source string view to position 0 of the destination one: ", numChar, srcChar);
	printStringView(destStringView);

	ASSERT_EQ(destStringView.length(), numCopied); // truncation
	ASSERT_EQ(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcStringView.data(), destStringView.data(), srcChar, numCopied, 0));
}

TEST_F(StringViewOperationsTest, CopyAllCharactersToBeginning)
{
	nctl::StringView srcStringView = srcStringArray;
	printStringView("Creating a new source string view: ", srcStringView);
	nctl::StringView destStringView = destStringArray;
	printStringView("Creating a new destination string view: ", destStringView);

	const unsigned int numCopied = destStringView.assign(srcStringView);
	printf("Copying %u characters from position 0 of the source string view to position 0 of the destination one: ", srcStringView.length());
	printStringView(destStringView);

	ASSERT_EQ(srcStringView.length(), destStringView.length()); // truncation
	ASSERT_EQ(numCopied, srcStringView.length());
	ASSERT_TRUE(charactersInStringsAreEqual(srcStringView.data(), destStringView.data(), 0, srcStringView.length(), 0));
}

TEST_F(StringViewOperationsTest, CopyCharactersFromBeyondEnd)
{
	nctl::StringView srcStringView = srcStringArray;
	printStringView("Creating a new source string view: ", srcStringView);
	nctl::StringView destStringView = destStringArray;
	printStringView("Creating a new destination string view: ", destStringView);

	const unsigned int srcChar = srcStringView.length() + 1; // beyond the end of source
	const unsigned int numChar = 2;
	const unsigned int destChar = 0;
	const unsigned int numCopied = destStringView.replace(srcStringView, srcChar, numChar, destChar);
	printStringView("Trying to copy a character from beyond the end of the source string view into the destination one: ", destStringView);

	ASSERT_NE(srcStringView.length(), destStringView.length()); // no truncation
	ASSERT_EQ(numCopied, 0u);
	ASSERT_NE(numCopied, numChar);
}

TEST_F(StringViewOperationsTest, CopyCharactersToBeyondEnd)
{
	nctl::StringView srcStringView = srcStringArray;
	printStringView("Creating a new source string view: ", srcStringView);
	char destStringArrayLocal[] = "abcdefghilmn";
	nctl::StringView destStringView(destStringArrayLocal, DestStringLength + 1, DestStringLength - 3); // leaving some space
	const unsigned int destStringViewCapacity = destStringView.capacity();
	printStringView("Creating a new destination string view: ", destStringView);

	const unsigned int numAvailable = destStringView.capacity() - destStringView.length() - 1;

	const unsigned int srcChar = 0;
	const unsigned int numChar = 6; // more than available in destination
	ASSERT_GT(numChar, numAvailable);
	const unsigned int destChar = destStringView.length() + 1; // beyond the end of destination
	const unsigned int numCopied = destStringView.replace(srcStringView, srcChar, numChar, destChar);
	printStringView("Trying to copy a character from the source string view to beyond the end of the destination one: ", destStringView);

	ASSERT_EQ(destStringView.capacity(), destStringViewCapacity);
	ASSERT_NE(srcStringView.length(), destStringView.length()); // truncation
	ASSERT_EQ(numCopied, numAvailable);
	ASSERT_LT(numCopied, numChar);
}

TEST_F(StringViewOperationsTest, CopyMoreCharactersThanSourceLength)
{
	char srcStringArrayLocal[] = "012";
	nctl::StringView srcStringView = srcStringArrayLocal;
	printStringView("Creating a new source string view: ", srcStringView);
	nctl::StringView destStringView = destStringArray;
	printStringView("Creating a new destination string view: ", destStringView);

	const unsigned int srcChar = 0;
	const unsigned int numChar = srcStringView.length() + 1; // more than available in source
	const unsigned int destChar = 0;
	const unsigned int numCopied = destStringView.replace(srcStringView, srcChar, numChar, destChar);
	printStringView("Trying to copy from the source string view to the destination one more than source length: ", destStringView);

	ASSERT_NE(srcStringView.length(), destStringView.length()); // no truncation
	ASSERT_EQ(numCopied, srcStringView.length());
	ASSERT_NE(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcStringView.data(), destStringView.data(), srcChar, numCopied, destChar));
}

TEST_F(StringViewOperationsTest, CopyMoreCharactersThanDestinationLength)
{
	nctl::StringView srcStringView = srcStringArray;
	printStringView("Creating a new source string view: ", srcStringView);
	char destStringArrayLocal[] = "abc";
	nctl::StringView destStringView = destStringArrayLocal;
	printStringView("Creating a new destination string view: ", destStringView);
	const unsigned int oldLength = destStringView.length();

	const unsigned int srcChar = 0;
	const unsigned int numChar = destStringView.capacity() + 1; // more than available in destination
	const unsigned int destChar = 0;
	const unsigned int numCopied = destStringView.replace(srcStringView, srcChar, numChar, destChar);
	printStringView("Trying to copy from the source string view to the destination one more than destination length: ", destStringView);

	ASSERT_EQ(destStringView.length(), oldLength); // truncation
	ASSERT_EQ(numCopied, destStringView.length());
	ASSERT_NE(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcStringView.data(), destStringView.data(), srcChar, numCopied, destChar));
}

TEST_F(StringViewOperationsTest, CopyCharactersFromCString)
{
	const unsigned int stringLength = 3;
	char srcString[stringLength + 1] = "abc";
	nctl::StringView destStringView = srcStringArray;
	printStringView("Creating a new destination string view: ", destStringView);

	const unsigned int srcChar = 0;
	const unsigned int numChar = stringLength;
	const unsigned int numCopied = destStringView.assign(srcString, srcChar, numChar);
	printf("Copying %u characters from an array of %u characters to the string view: \"%s\"\n", numChar, stringLength, destStringView.data());

	ASSERT_EQ(stringLength, destStringView.length()); // truncation
	ASSERT_EQ(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcString, destStringView.data(), 0, numCopied, 0));
}

TEST_F(StringViewOperationsTest, CopyCharactersFromCStringBeginning)
{
	const unsigned int stringLength = 3;
	char srcString[stringLength + 1] = "abc";
	nctl::StringView destStringView = srcStringArray;
	printStringView("Creating a new destination string view: ", destStringView);

	const unsigned int numChar = stringLength;
	const unsigned int numCopied = destStringView.assign(srcString, numChar);
	printf("Copying %u characters from an array of %u characters to the beginning of the string view: \"%s\"\n", numChar, stringLength, destStringView.data());

	ASSERT_EQ(destStringView.length(), numCopied); // truncation
	ASSERT_EQ(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcString, destStringView.data(), 0, numCopied, 0));
}

TEST_F(StringViewOperationsTest, CopyCharactersToCString)
{
	char srcStringArrayLocal[] = "abc";
	nctl::StringView srcStringView = srcStringArrayLocal;
	printStringView("Creating a new source string view: ", srcStringView);
	const unsigned int stringLength = 7;
	char destString[stringLength + 1] = "0123456";

	const unsigned int srcChar = 0;
	const unsigned int numChar = srcStringView.length();
	const unsigned int numCopied = srcStringView.copy(destString, srcChar, numChar);
	printf("Copying %u characters from the string view into an array of %u characters: \"%s\"\n", numChar, stringLength, destString);

	ASSERT_EQ(srcStringView.length(), strnlen(destString, 8)); // truncation
	ASSERT_EQ(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcStringView.data(), destString, srcChar, numCopied, 0));
}

TEST_F(StringViewOperationsTest, CopyAllCharactersToCString)
{
	char srcStringArrayLocal[] = "abc";
	nctl::StringView srcStringView = srcStringArrayLocal;
	printStringView("Creating a new source string view: ", srcStringView);
	const unsigned int stringLength = 8;
	char destString[stringLength] = "0123456";

	const unsigned int numChar = srcStringView.length();
	const unsigned int numCopied = srcStringView.copy(destString);
	printf("Copying %u characters from the beginning of the string view into an array of %u characters: \"%s\"\n", numChar, stringLength, destString);

	ASSERT_EQ(srcStringView.length(), strnlen(destString, 8)); // truncation
	ASSERT_EQ(numCopied, numChar);
	ASSERT_TRUE(charactersInStringsAreEqual(srcStringView.data(), destString, 0, numCopied, 0));
}

}
