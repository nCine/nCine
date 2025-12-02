#include "gtest_stringview.h"

namespace {

const unsigned int String1ExtraCapacity = 16;
char string1ExtraArray[String1ExtraCapacity] = "String1";

class StringViewEmptyTest : public ::testing::Test
{
  public:
	StringViewEmptyTest()
	    : emptyStringView_() {}

  protected:
	void SetUp() override
	{
		nctl::strncpy(string1Array, String1Literal, String1Length + 1);
		nctl::strncpy(string1ExtraArray, String1Literal, String1ExtraCapacity);

		emptyStringView_.set(nullptr);
	}

	nctl::StringView emptyStringView_;
};

TEST_F(StringViewEmptyTest, AppendString)
{
	char appendStringArray[] = "Append";

	emptyStringView_.append(appendStringArray);
	printStringView("Trying to append a string to an empty string view: ", emptyStringView_);

	ASSERT_EQ(emptyStringView_.capacity(), 1);
	ASSERT_EQ(emptyStringView_.length(), 0);
	ASSERT_STREQ(emptyStringView_.data(), "");
}

TEST_F(StringViewEmptyTest, AppendCString)
{
	const char *newString = "Append";
	printf("Creating a new C string: %s\n", newString);

	emptyStringView_.append(newString);
	printStringView("Trying to append a C string to an empty string view: ", emptyStringView_);

	ASSERT_EQ(emptyStringView_.capacity(), 1);
	ASSERT_EQ(emptyStringView_.length(), 0);
	ASSERT_STREQ(emptyStringView_.data(), "");
}

TEST_F(StringViewEmptyTest, Clear)
{
	emptyStringView_.clear();
	printStringView("Clearing the empty string view: ", emptyStringView_);

	ASSERT_EQ(emptyStringView_.capacity(), 1);
	ASSERT_EQ(emptyStringView_.length(), 0);
	ASSERT_STREQ(emptyStringView_.data(), "");
}

TEST_F(StringViewEmptyTest, RemovePrefix)
{
	emptyStringView_.removePrefix(1);
	printStringView("Shrinking the empty string view from the start: ", emptyStringView_);

	ASSERT_EQ(emptyStringView_.capacity(), 1);
	ASSERT_EQ(emptyStringView_.length(), 0);
	ASSERT_STREQ(emptyStringView_.data(), "");
}

TEST_F(StringViewEmptyTest, RemoveSuffix)
{
	emptyStringView_.removeSuffix(1);
	printStringView("Shrinking the empty string view from the end: ", emptyStringView_);

	ASSERT_EQ(emptyStringView_.capacity(), 1);
	ASSERT_EQ(emptyStringView_.length(), 0);
	ASSERT_STREQ(emptyStringView_.data(), "");
}

TEST_F(StringViewEmptyTest, CopyConstruction)
{
	nctl::StringView newStringView(emptyStringView_);
	printStringView("Creating a new string view as a copy of the empty one: ", newStringView);

	ASSERT_EQ(newStringView.capacity(), emptyStringView_.capacity());
	ASSERT_EQ(newStringView.length(), emptyStringView_.length());
	ASSERT_EQ(newStringView.data(), emptyStringView_.data());
}

TEST_F(StringViewEmptyTest, MoveConstruction)
{
	nctl::StringView newStringView(nctl::move(emptyStringView_));
	printStringView("Creating a new string view moving from the empty one: ", newStringView);

	ASSERT_EQ(newStringView.capacity(), 1);
	ASSERT_EQ(newStringView.length(), 0);
	ASSERT_STREQ(newStringView.data(), "");
	ASSERT_EQ(emptyStringView_.capacity(), 1);
	ASSERT_EQ(emptyStringView_.length(), 0);
	ASSERT_STREQ(emptyStringView_.data(), "");
}

TEST_F(StringViewEmptyTest, AssignmentOperator)
{
	nctl::StringView newStringView;
	newStringView = emptyStringView_;
	printf("Filling a new string view with the assignment operator\n");

	ASSERT_EQ(newStringView.capacity(), emptyStringView_.capacity());
	ASSERT_EQ(newStringView.length(), emptyStringView_.length());
	ASSERT_EQ(newStringView.data(), emptyStringView_.data());
}

TEST_F(StringViewEmptyTest, MoveAssignmentOperator)
{
	nctl::StringView newStringView;
	newStringView = nctl::move(emptyStringView_);
	printStringView("Creating a new string view with the move assignment operator: ", newStringView);

	ASSERT_EQ(newStringView.capacity(), 1);
	ASSERT_EQ(newStringView.length(), 0);
	ASSERT_STREQ(newStringView.data(), "");
	ASSERT_EQ(emptyStringView_.capacity(), 1);
	ASSERT_EQ(emptyStringView_.length(), 0);
	ASSERT_STREQ(emptyStringView_.data(), "");
}

TEST_F(StringViewEmptyTest, SelfAssignment)
{
	emptyStringView_ = emptyStringView_;
	printStringView("Assigning the empty string view to itself with the assignment operator: ", emptyStringView_);

	ASSERT_EQ(emptyStringView_.capacity(), 1);
	ASSERT_EQ(emptyStringView_.length(), 0);
	ASSERT_STREQ(emptyStringView_.data(), "");
}

TEST_F(StringViewEmptyTest, AssignCString)
{
	char longString[] = "String2...String2";
	emptyStringView_ = longString;
	printStringView("Creating a new string view with the assignment operator from a C-style string: ", emptyStringView_);

	ASSERT_EQ(emptyStringView_.capacity(), 1);
	ASSERT_EQ(emptyStringView_.length(), 0);
	ASSERT_STREQ(emptyStringView_.data(), "");
}

TEST_F(StringViewEmptyTest, Format)
{
	emptyStringView_.format("String%d", 2);
	printStringView("Resetting the empty string view to a formatted one: ", emptyStringView_);

	ASSERT_EQ(emptyStringView_.capacity(), 1);
	ASSERT_EQ(emptyStringView_.length(), 0);
	ASSERT_STREQ(emptyStringView_.data(), "");
}

TEST_F(StringViewEmptyTest, FormatAppend)
{
	emptyStringView_.formatAppend("String%d", 2);
	printStringView("Appending a formatted string to the empty string view: ", emptyStringView_);

	ASSERT_EQ(emptyStringView_.capacity(), 1);
	ASSERT_EQ(emptyStringView_.length(), 0);
	ASSERT_STREQ(emptyStringView_.data(), "");
}

TEST_F(StringViewEmptyTest, SetLengthBeyondCapacity)
{
	const unsigned int newLength = emptyStringView_.setLength(1);
	printStringView("Trying to set a length beyond the empty string view capacity: ", emptyStringView_);

	ASSERT_EQ(emptyStringView_.capacity(), 1);
	ASSERT_EQ(emptyStringView_.length(), 0);
	ASSERT_EQ(newLength, 0);
	ASSERT_STREQ(emptyStringView_.data(), "");
}

TEST_F(StringViewEmptyTest, FindFirstCharacter)
{
	const char character = 'S';
	const int position = emptyStringView_.findFirstChar(character);
	printf("First position of the character '%c' in the empty string view: %d\n", character, position);

	ASSERT_EQ(position, -1);
}

TEST_F(StringViewEmptyTest, FindLastCharacter)
{
	const char character = 'S';
	const int position = emptyStringView_.findLastChar(character);
	printf("Last position of the character '%c' in the empty string view: %d\n", character, position);

	ASSERT_EQ(position, -1);
}

TEST_F(StringViewEmptyTest, FindString)
{
	nctl::StringView newStringView(string2Array);
	printStringView("Creating a new string view: ", newStringView);

	const int position = emptyStringView_.find(newStringView);
	printf("Position of sub-string \"%s\" inside the empty string view \"%s\": %d\n", newStringView.data(), emptyStringView_.data(), position);
	ASSERT_EQ(position, -1);
}

TEST_F(StringViewEmptyTest, FindCString)
{
	const char *newString = String2Literal;
	printf("Creating a new string: %s\n", newString);

	const int position = emptyStringView_.find(newString);
	printf("Position of sub-string \"%s\" inside the empty string view \"%s\": %d\n", newString, emptyStringView_.data(), position);
	ASSERT_EQ(position, -1);
}

TEST_F(StringViewEmptyTest, CopyCharacters)
{
	char srcStringArray[] = "0123456";
	nctl::StringView srcStringView = srcStringArray;
	printStringView("Creating a new source string view: ", srcStringView);

	const unsigned int srcChar = 4;
	const unsigned int numChar = 3;
	const unsigned int destChar = 2;
	const unsigned int numCopied = emptyStringView_.replace(srcStringView, srcChar, numChar, destChar);
	printf("Copying %u characters from position %u of the source string view to position %u of the destination one: ", numChar, srcChar, destChar);
	printStringView(emptyStringView_);

	ASSERT_EQ(emptyStringView_.length(), 0);
	ASSERT_EQ(numCopied, 0);
}

TEST_F(StringViewEmptyTest, CopyCharactersFromCString)
{
	const unsigned int stringLength = 3;
	char srcString[stringLength + 1] = "abc";

	const unsigned int srcChar = 0;
	const unsigned int numChar = stringLength;
	const unsigned int numCopied = emptyStringView_.assign(srcString, srcChar, numChar);
	printf("Copying %u characters from an array of %u characters to the empty string view: \"%s\"\n", numChar, stringLength, emptyStringView_.data());

	ASSERT_EQ(emptyStringView_.length(), 0);
	ASSERT_EQ(numCopied, 0);
}

}
