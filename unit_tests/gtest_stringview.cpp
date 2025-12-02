#include "gtest_stringview.h"

namespace {

char veryLongCStringArray[] = "This_is_a_very_long_string_which_goes_beyond_the_default_capacity_of_a_string";

class StringViewTest : public ::testing::Test
{
  public:
	StringViewTest()
	    : stringView_(string1Array) {}

  protected:
	void SetUp() override
	{
		nctl::strncpy(string1Array, String1Literal, String1Length + 1);
		nctl::strncpy(string2Array, String2Literal, String2Length + 1);
		nctl::strncpy(veryLongCStringArray, VeryLongCStringLiteral, VeryLongCStringLength + 1);
		stringView_.set(string1Array, String1Length + 1);
	}

	nctl::StringView stringView_;
};

TEST_F(StringViewTest, ConstructEmptyStringView)
{
	nctl::StringView newStringView;
	printStringView("Creating an empty string view: ", newStringView);

	ASSERT_EQ(newStringView.capacity(), 1);
	ASSERT_EQ(newStringView.length(), 0);
	ASSERT_STREQ(newStringView.data(), "");
	ASSERT_TRUE(newStringView.isEmpty());
}

TEST_F(StringViewTest, ConstructStringViewFromNullWithCapacity)
{
	nctl::StringView newStringView(nullptr, 16);
	printStringView("Creating an empty string view: ", newStringView);

	ASSERT_EQ(newStringView.capacity(), 1);
	ASSERT_EQ(newStringView.length(), 0);
	ASSERT_STREQ(newStringView.data(), "");
	ASSERT_TRUE(newStringView.isEmpty());
}

TEST_F(StringViewTest, ConstructStringViewFromNullWithLength)
{
	nctl::StringView newStringView(nullptr, 16, 15);
	printStringView("Creating an empty string view: ", newStringView);

	ASSERT_EQ(newStringView.capacity(), 1);
	ASSERT_EQ(newStringView.length(), 0);
	ASSERT_STREQ(newStringView.data(), "");
	ASSERT_TRUE(newStringView.isEmpty());
}

TEST_F(StringViewTest, ConstructStringView)
{
	nctl::StringView newStringView(veryLongCStringArray);
	printStringView("Creating a string view: ", newStringView);

	ASSERT_EQ(newStringView.capacity(), VeryLongCStringLength + 1);
	ASSERT_EQ(newStringView.length(), VeryLongCStringLength);
	ASSERT_FALSE(newStringView.isEmpty());
}

TEST_F(StringViewTest, ConstructStringViewWithCapacity)
{
	nctl::StringView newStringView(veryLongCStringArray, VeryLongCStringLength + 1);
	printStringView("Creating a string view: ", newStringView);

	ASSERT_EQ(newStringView.capacity(), VeryLongCStringLength + 1);
	ASSERT_EQ(newStringView.length(), VeryLongCStringLength);
	ASSERT_FALSE(newStringView.isEmpty());
}

TEST_F(StringViewTest, ConstructStringViewWithCapacityAndLength)
{
	nctl::StringView newStringView(veryLongCStringArray, VeryLongCStringLength + 1, VeryLongCStringLength);
	printStringView("Creating a string view: ", newStringView);

	ASSERT_EQ(newStringView.capacity(), VeryLongCStringLength + 1);
	ASSERT_EQ(newStringView.length(), VeryLongCStringLength);
	ASSERT_FALSE(newStringView.isEmpty());
}

TEST_F(StringViewTest, SetAlone)
{
	nctl::StringView newStringView;
	newStringView.set(veryLongCStringArray);
	printStringView("Setting a new string view: ", newStringView);

	ASSERT_EQ(newStringView.capacity(), VeryLongCStringLength + 1);
	ASSERT_EQ(newStringView.length(), VeryLongCStringLength);
	ASSERT_FALSE(newStringView.isEmpty());
}

TEST_F(StringViewTest, SetWithCapacity)
{
	nctl::StringView newStringView;
	newStringView.set(veryLongCStringArray, VeryLongCStringLength + 1);
	printStringView("Setting a new string view: ", newStringView);

	ASSERT_EQ(newStringView.capacity(), VeryLongCStringLength + 1);
	ASSERT_EQ(newStringView.length(), VeryLongCStringLength);
	ASSERT_FALSE(newStringView.isEmpty());
}

TEST_F(StringViewTest, SetWithCapacityAndLength)
{
	nctl::StringView newStringView;
	newStringView.set(veryLongCStringArray, VeryLongCStringLength + 1, VeryLongCStringLength);
	printStringView("Setting a new string view: ", newStringView);

	ASSERT_EQ(newStringView.capacity(), VeryLongCStringLength + 1);
	ASSERT_EQ(newStringView.length(), VeryLongCStringLength);
	ASSERT_FALSE(newStringView.isEmpty());
}

TEST_F(StringViewTest, AssignCString)
{
	nctl::StringView newStringView(string1Array);
	newStringView = string2Array;
	printStringView("Assigning a C-style string to a string view: ", newStringView);

	ASSERT_EQ(newStringView.capacity(), String2Length + 1);
	ASSERT_EQ(newStringView.length(), String2Length);
	ASSERT_STREQ(newStringView.data(), "String2");
}

TEST_F(StringViewTest, EqualityOperator)
{
	nctl::StringView newStringView(string1Array);
	printf("The first string view and the new one are equal: %d\n", stringView_ == newStringView);

	ASSERT_TRUE(stringView_ == newStringView);
	ASSERT_FALSE(stringView_ != newStringView);
}

TEST_F(StringViewTest, InequalityOperator)
{
	nctl::StringView newStringView(string2Array);
	printf("The first string view and the new one are not equal: %d\n", stringView_ != newStringView);

	ASSERT_TRUE(stringView_ != newStringView);
	ASSERT_FALSE(stringView_ == newStringView);
}

TEST_F(StringViewTest, GreaterThanOperator)
{
	nctl::StringView newStringView(string2Array);
	printf("The second string view is lexicographically bigger than the first: %d\n", newStringView > stringView_);

	ASSERT_TRUE(newStringView > stringView_);
	ASSERT_FALSE(newStringView < stringView_);
}

TEST_F(StringViewTest, LessThanOperator)
{
	nctl::StringView newStringView(string2Array);
	printf("The first string view is lexicographically smaller than the second: %d\n", stringView_ < newStringView);

	ASSERT_TRUE(stringView_ < newStringView);
	ASSERT_FALSE(stringView_ > newStringView);
}

TEST_F(StringViewTest, GreaterThanOrEqualOperator)
{
	nctl::StringView newStringView(string2Array);
	printf("The second string view is lexicographically bigger than or equal to the first: %d\n", newStringView >= stringView_);

	ASSERT_TRUE(newStringView >= stringView_);
	ASSERT_FALSE(newStringView < stringView_);
}

TEST_F(StringViewTest, LessThanOrEqualOperator)
{
	nctl::StringView newStringView(string2Array);
	printf("The first string view is lexicographically smaller than or equal to the second: %d\n", stringView_ <= newStringView);

	ASSERT_TRUE(stringView_ <= newStringView);
	ASSERT_FALSE(stringView_ > newStringView);
}

TEST_F(StringViewTest, EqualityOperatorCString)
{
	printf("The first string view and the new one are equal: %d\n", stringView_ == String1Literal);

	ASSERT_TRUE(stringView_ == String1Literal);
	ASSERT_FALSE(stringView_ != String1Literal);
	ASSERT_TRUE(String1Literal == stringView_);
	ASSERT_FALSE(String1Literal != stringView_);
}

TEST_F(StringViewTest, InequalityOperatorCString)
{
	printf("The first string view and the new one are not equal: %d\n", stringView_ != String2Literal);

	ASSERT_TRUE(stringView_ != String2Literal);
	ASSERT_FALSE(stringView_ == String2Literal);
	ASSERT_TRUE(String2Literal != stringView_);
	ASSERT_FALSE(String2Literal == stringView_);
}

TEST_F(StringViewTest, GreaterThanOperatorCString)
{
	printf("The second string view is lexicographically bigger than the first: %d\n", String2Literal > stringView_);

	ASSERT_TRUE(String2Literal > stringView_);
	ASSERT_FALSE(String2Literal < stringView_);
	ASSERT_TRUE(stringView_ < String2Literal);
	ASSERT_FALSE(stringView_ > String2Literal);
}

TEST_F(StringViewTest, LessThanOperatorCString)
{
	printf("The first string view is lexicographically smaller than the second: %d\n", stringView_ < String2Literal);

	ASSERT_TRUE(stringView_ < String2Literal);
	ASSERT_FALSE(stringView_ > String2Literal);
	ASSERT_TRUE(String2Literal > stringView_);
	ASSERT_FALSE(String2Literal < stringView_);
}

TEST_F(StringViewTest, GreaterThanOrEqualOperatorCString)
{
	printf("The second string view is lexicographically bigger than or equal to the first: %d\n", String2Literal >= stringView_);

	ASSERT_TRUE(String2Literal >= stringView_);
	ASSERT_FALSE(String2Literal < stringView_);
	ASSERT_TRUE(stringView_ < String2Literal);
	ASSERT_FALSE(stringView_ >= String2Literal);
}

TEST_F(StringViewTest, LessThanOrEqualOperatorCString)
{
	printf("The first string view is lexicographically smaller than or equal to the second: %d\n", stringView_ <= String2Literal);

	ASSERT_TRUE(stringView_ <= String2Literal);
	ASSERT_FALSE(stringView_ > String2Literal);
	ASSERT_TRUE(String2Literal > stringView_);
	ASSERT_FALSE(String2Literal <= stringView_);
}

TEST_F(StringViewTest, AppendOperator)
{
	char string[18] = "String1";
	nctl::StringView newStringView(string, 14);
	newStringView += "Append";
	printStringView("Appending a C-style string to the string view: ", newStringView);

	ASSERT_EQ(newStringView.capacity(), 14);
	ASSERT_EQ(newStringView.length(), strnlen("String1Append", 14));
	ASSERT_STREQ(newStringView.data(), "String1Append");
}

TEST_F(StringViewTest, SubscriptAccessFirstCharacter)
{
	printf("First character of the string view: %c\n", stringView_[0]);
	ASSERT_EQ(stringView_[0], 'S');
}

TEST_F(StringViewTest, SubscriptAccessFirstConstCharacter)
{
	const nctl::StringView constStringView = stringView_;
	printf("First character of the constant string view: %c\n", constStringView[0]);
	ASSERT_EQ(constStringView[0], 'S');
}

TEST_F(StringViewTest, SubscriptAccessLastCharacter)
{
	printf("Last character of the string view: %c\n", stringView_[stringView_.length() - 1]);
	ASSERT_EQ(stringView_[stringView_.length() - 1], '1');
}

TEST_F(StringViewTest, SubscriptAccessLastConstCharacter)
{
	const nctl::StringView constStringView = stringView_;
	printf("Last character of the constant string view: %c\n", constStringView[constStringView.length() - 1]);
	ASSERT_EQ(constStringView[constStringView.length() - 1], '1');
}

TEST_F(StringViewTest, AccessAtFirstCharacter)
{
	printf("First character of the string view: %c\n", stringView_.at(0));
	ASSERT_EQ(stringView_.at(0), 'S');
}

TEST_F(StringViewTest, AccessAtFirstConstCharacter)
{
	const nctl::StringView constStringView = stringView_;
	printf("First character of the constant string view: %c\n", constStringView.at(0));
	ASSERT_EQ(constStringView.at(0), 'S');
}

TEST_F(StringViewTest, AccessAtLastCharacter)
{
	printf("Last character of the string view: %c\n", stringView_.at(stringView_.length() - 1));
	ASSERT_EQ(stringView_.at(stringView_.length() - 1), '1');
}

TEST_F(StringViewTest, AccessAtLastConstCharacter)
{
	const nctl::StringView constStringView = stringView_;
	printf("Last character of the constant string view: %c\n", constStringView.at(constStringView.length() - 1));
	ASSERT_EQ(constStringView.at(constStringView.length() - 1), '1');
}

#ifndef __EMSCRIPTEN__
	#ifdef NCINE_DEBUG
TEST(StringViewDeathTest, SubscriptAccessBeyondLastCharacter)
{
	char smallString[2] = "0";
	const nctl::StringView stringView(smallString);
	printf("Accessing a character beyond the length of the string view\n");
	ASSERT_DEATH(stringView[5], "");
}

TEST(StringViewDeathTest, SubscriptAccessBeyondLastConstCharacter)
{
	char smallString[2] = "0";
	const nctl::StringView stringView(smallString);
	printf("Accessing a character beyond the length of the string view\n");
	ASSERT_DEATH(stringView[5], "");
}
	#endif

TEST(StringViewDeathTest, AccessAtBeyondLastCharacter)
{
	char smallString[2] = "0";
	const nctl::StringView stringView(smallString);
	printf("Accessing a character beyond the length of the string view\n");
	ASSERT_DEATH(stringView.at(5), "");
}

TEST(StringViewDeathTest, AccessAtBeyondLastConstCharacter)
{
	char smallString[2] = "0";
	const nctl::StringView stringView(smallString);
	printf("Accessing a character beyond the length of the string view\n");
	ASSERT_DEATH(stringView.at(5), "");
}
#endif

}
