#include "gtest_staticstring.h"

namespace {

class StaticStringTest : public ::testing::Test
{
  protected:
	void SetUp() override { string_ = String1Literal; }

	nctl::StaticString<Capacity> string_;
};

TEST_F(StaticStringTest, EmptyString)
{
	nctl::StaticString<Capacity> newString;
	printString("Creating an empty string: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), 0);
	ASSERT_TRUE(newString.isEmpty());
}

TEST_F(StaticStringTest, AssignCString)
{
	nctl::StaticString<Capacity> newString;
	newString = String1Literal;
	printString("Assigning a C-style string to an empty one: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), String1Length);
	ASSERT_STREQ(newString.data(), String1Literal);
}

TEST_F(StaticStringTest, AssignCStringTruncate)
{
	nctl::StaticString<Capacity> newString;
	newString = VeryLongCStringLiteral;
	printString("Assigning a C-style string to an empty one: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), strnlen("This_is_a_very_", Capacity));
	ASSERT_STREQ(newString.data(), "This_is_a_very_");
}

TEST_F(StaticStringTest, AssignCStringShorter)
{
	nctl::StaticString<Capacity> newString("LongString1");
	newString = String1Literal;
	printString("Assigning a shorter C-style string to a longer one: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), String1Length);
	ASSERT_STREQ(newString.data(), String1Literal);
}

TEST_F(StaticStringTest, EqualityOperator)
{
	nctl::StaticString<Capacity> newString(string_);
	printf("The first string and the new one are equal: %d\n", string_ == newString);

	ASSERT_TRUE(string_ == newString);
	ASSERT_FALSE(string_ != newString);
}

TEST_F(StaticStringTest, EqualityOperatorDifferentCapacity)
{
	nctl::StaticString<Capacity * 2> newString(string_);
	printf("The first string and the new one are equal: %d\n", string_ == newString);

	ASSERT_TRUE(string_ == newString);
	ASSERT_FALSE(string_ != newString);
}

TEST_F(StaticStringTest, InequalityOperator)
{
	nctl::StaticString<Capacity> newString = String2Literal;
	printf("The first string and the new one are not equal: %d\n", string_ != newString);

	ASSERT_TRUE(string_ != newString);
	ASSERT_FALSE(string_ == newString);
}

TEST_F(StaticStringTest, InequalityOperatorDifferentCapacity)
{
	nctl::StaticString<Capacity * 2> newString = String2Literal;
	printf("The first string and the new one are not equal: %d\n", string_ != newString);

	ASSERT_TRUE(string_ != newString);
	ASSERT_FALSE(string_ == newString);
}

TEST_F(StaticStringTest, GreaterThanOperator)
{
	nctl::StaticString<Capacity> newString = String2Literal;
	printf("The second string is lexicographically bigger than the first: %d\n", newString > string_);

	ASSERT_TRUE(newString > string_);
	ASSERT_FALSE(newString < string_);
}

TEST_F(StaticStringTest, GreaterThanOperatorDifferentCapacity)
{
	nctl::StaticString<Capacity * 2> newString = String2Literal;
	printf("The second string is lexicographically bigger than the first: %d\n", newString > string_);

	ASSERT_TRUE(newString > string_);
	ASSERT_FALSE(newString < string_);
}

TEST_F(StaticStringTest, LessThanOperator)
{
	nctl::StaticString<Capacity> newString = String2Literal;
	printf("The first string is lexicographically smaller than the second: %d\n", string_ < newString);

	ASSERT_TRUE(string_ < newString);
	ASSERT_FALSE(string_ > newString);
}

TEST_F(StaticStringTest, LessThanOperatorDifferentCapacity)
{
	nctl::StaticString<Capacity * 2> newString = String2Literal;
	printf("The first string is lexicographically smaller than the second: %d\n", string_ < newString);

	ASSERT_TRUE(string_ < newString);
	ASSERT_FALSE(string_ > newString);
}

TEST_F(StaticStringTest, GreaterThanOrEqualOperator)
{
	nctl::StaticString<Capacity> newString = String2Literal;
	printf("The second string is lexicographically bigger than or equal to the first: %d\n", newString >= string_);

	ASSERT_TRUE(newString >= string_);
	ASSERT_FALSE(newString < string_);
}

TEST_F(StaticStringTest, GreaterThanOrEqualOperatorDifferentCapacity)
{
	nctl::StaticString<Capacity * 2> newString = String2Literal;
	printf("The second string is lexicographically bigger than or equal to the first: %d\n", newString >= string_);

	ASSERT_TRUE(newString >= string_);
	ASSERT_FALSE(newString < string_);
}

TEST_F(StaticStringTest, LessThanOrEqualOperator)
{
	nctl::StaticString<Capacity> newString = String2Literal;
	printf("The first string is lexicographically smaller than or equal to the second: %d\n", string_ <= newString);

	ASSERT_TRUE(string_ <= newString);
	ASSERT_FALSE(string_ > newString);
}

TEST_F(StaticStringTest, LessThanOrEqualOperatorDifferentCapacity)
{
	nctl::StaticString<Capacity * 2> newString = String2Literal;
	printf("The first string is lexicographically smaller than or equal to the second: %d\n", string_ <= newString);

	ASSERT_TRUE(string_ <= newString);
	ASSERT_FALSE(string_ > newString);
}

TEST_F(StaticStringTest, EqualityOperatorCString)
{
	const char *newString = String1Literal;
	printf("The first string and the new one are equal: %d\n", string_ == newString);

	ASSERT_TRUE(string_ == newString);
	ASSERT_FALSE(string_ != newString);
	ASSERT_TRUE(newString == string_);
	ASSERT_FALSE(newString != string_);
}

TEST_F(StaticStringTest, InequalityOperatorCString)
{
	const char *newString = String2Literal;
	printf("The first string and the new one are not equal: %d\n", string_ != newString);

	ASSERT_TRUE(string_ != newString);
	ASSERT_FALSE(string_ == newString);
	ASSERT_TRUE(newString != string_);
	ASSERT_FALSE(newString == string_);
}

TEST_F(StaticStringTest, GreaterThanOperatorCString)
{
	const char *newString = String2Literal;
	printf("The second string is lexicographically bigger than the first: %d\n", newString > string_);

	ASSERT_TRUE(newString > string_);
	ASSERT_FALSE(newString < string_);
	ASSERT_TRUE(string_ < newString);
	ASSERT_FALSE(string_ > newString);
}

TEST_F(StaticStringTest, LessThanOperatorCString)
{
	const char *newString = String2Literal;
	printf("The first string is lexicographically smaller than the second: %d\n", string_ < newString);

	ASSERT_TRUE(string_ < newString);
	ASSERT_FALSE(string_ > newString);
	ASSERT_TRUE(newString > string_);
	ASSERT_FALSE(newString < string_);
}

TEST_F(StaticStringTest, GreaterThanOrEqualOperatorCString)
{
	const char *newString = String2Literal;
	printf("The second string is lexicographically bigger than or equal to the first: %d\n", newString >= string_);

	ASSERT_TRUE(newString >= string_);
	ASSERT_FALSE(newString < string_);
	ASSERT_TRUE(string_ < newString);
	ASSERT_FALSE(string_ >= newString);
}

TEST_F(StaticStringTest, LessThanOrEqualOperatorCString)
{
	const char *newString = String2Literal;
	printf("The first string is lexicographically smaller than or equal to the second: %d\n", string_ <= newString);

	ASSERT_TRUE(string_ <= newString);
	ASSERT_FALSE(string_ > newString);
	ASSERT_TRUE(newString > string_);
	ASSERT_FALSE(newString <= string_);
}

TEST_F(StaticStringTest, AppendOperator)
{
	string_ += "Append";
	printString("Appending a C-style string to the first one: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), strnlen("String1Append", Capacity));
	ASSERT_STREQ(string_.data(), "String1Append");
}

TEST_F(StaticStringTest, SubscriptAccessFirstCharacter)
{
	printf("First character of the string: %c\n", string_[0]);
	ASSERT_EQ(string_[0], 'S');
}

TEST_F(StaticStringTest, SubscriptAccessFirstConstCharacter)
{
	const nctl::StaticString<Capacity> constString = string_;
	printf("First character of the constant string: %c\n", constString[0]);
	ASSERT_EQ(constString[0], 'S');
}

TEST_F(StaticStringTest, SubscriptAccessLastCharacter)
{
	printf("Last character of the string: %c\n", string_[string_.length() - 1]);
	ASSERT_EQ(string_[string_.length() - 1], '1');
}

TEST_F(StaticStringTest, SubscriptAccessLastConstCharacter)
{
	const nctl::StaticString<Capacity> constString = string_;
	printf("Last character of the constant string: %c\n", constString[constString.length() - 1]);
	ASSERT_EQ(constString[constString.length() - 1], '1');
}

TEST_F(StaticStringTest, AccessAtFirstCharacter)
{
	printf("First character of the string: %c\n", string_.at(0));
	ASSERT_EQ(string_.at(0), 'S');
}

TEST_F(StaticStringTest, AccessAtFirstConstCharacter)
{
	const nctl::StaticString<Capacity> constString = string_;
	printf("First character of the constant string: %c\n", constString.at(0));
	ASSERT_EQ(constString.at(0), 'S');
}

TEST_F(StaticStringTest, AccessAtLastCharacter)
{
	printf("Last character of the string: %c\n", string_.at(string_.length() - 1));
	ASSERT_EQ(string_.at(string_.length() - 1), '1');
}

TEST_F(StaticStringTest, AccessAtLastConstCharacter)
{
	const nctl::StaticString<Capacity> constString = string_;
	printf("Last character of the constant string: %c\n", constString.at(constString.length() - 1));
	ASSERT_EQ(constString.at(constString.length() - 1), '1');
}

#ifndef __EMSCRIPTEN__
	#ifdef NCINE_DEBUG
TEST(StaticStringDeathTest, SubscriptAccessBeyondLastCharacter)
{
	nctl::StaticString<Capacity> string("0");
	printf("Accessing a character beyond the length of the string\n");
	ASSERT_DEATH(string[5], "");
}

TEST(StaticStringDeathTest, SubscriptAccessBeyondLastConstCharacter)
{
	const nctl::StaticString<Capacity> string("0");
	printf("Accessing a character beyond the length of the string\n");
	ASSERT_DEATH(string[5], "");
}
	#endif

TEST(StaticStringDeathTest, AccessAtBeyondLastCharacter)
{
	nctl::StaticString<Capacity> string("0");
	printf("Accessing a character beyond the length of the string\n");
	ASSERT_DEATH(string.at(5), "");
}

TEST(StaticStringDeathTest, AccessAtBeyondLastConstCharacter)
{
	const nctl::StaticString<Capacity> string("0");
	printf("Accessing a character beyond the length of the string\n");
	ASSERT_DEATH(string.at(5), "");
}
#endif

}
