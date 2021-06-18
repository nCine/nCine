#include "gtest_string.h"

namespace {

class StringTest : public ::testing::Test
{
  public:
	StringTest()
	    : string_(Capacity) {}

  protected:
	void SetUp() override { string_ = "String1"; }

	nctl::String string_;
};

TEST_F(StringTest, EmptyString)
{
	nctl::String newString(Capacity);
	printString("Creating an empty string: ", newString);

	// Using the local buffer
	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), 0);
}

TEST_F(StringTest, EmptyStringShort)
{
	nctl::String newString(Capacity - 4);
	printString("Creating an empty string: ", newString);

	// Capacity can't be smaller than `SmallBufferSize`
	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), 0);
}

TEST_F(StringTest, EmptyStringLong)
{
	nctl::String newString(Capacity + 4);
	printString("Creating an empty string: ", newString);

	// Using the dynamic buffer
	ASSERT_EQ(newString.capacity(), Capacity + 4);
	ASSERT_EQ(newString.length(), 0);
}

TEST_F(StringTest, AssignCString)
{
	nctl::String newString(Capacity);
	newString = "String1";
	printString("Assigning a C-style string to an empty one: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), strnlen("String1", Capacity));
	ASSERT_STREQ(newString.data(), "String1");
}

TEST_F(StringTest, AssignCStringExtend)
{
	nctl::String newString(Capacity);
	newString = veryLongCString;
	printString("Assigning a C-style string to an empty one: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity * 8);
	ASSERT_EQ(newString.length(), strnlen(veryLongCString, Capacity * 8));
	ASSERT_STREQ(newString.data(), veryLongCString);
}

TEST_F(StringTest, AssignCStringTruncate)
{
	nctl::String newString(Capacity, nctl::StringMode::FIXED_CAPACITY);
	newString = veryLongCString;
	printString("Assigning a C-style string to an empty one: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), strnlen("This_is_a_very_", Capacity));
	ASSERT_STREQ(newString.data(), "This_is_a_very_");
}

TEST_F(StringTest, AssignCStringShorter)
{
	nctl::String newString("LongString1");
	newString = "String1";
	printString("Assigning a shorter C-style string to a longer one: ", newString);

	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), strnlen("String1", Capacity));
	ASSERT_STREQ(newString.data(), "String1");
}

TEST_F(StringTest, EqualityOperatorCString)
{
	const char *newString = "String1";
	printf("The first string and the new one are equal: %d\n", string_ == newString);

	ASSERT_TRUE(string_ == newString);
	ASSERT_FALSE(string_ != newString);
}

TEST_F(StringTest, EqualityOperator)
{
	nctl::String newString(string_);
	printf("The first string and the new one are equal: %d\n", string_ == newString);

	ASSERT_TRUE(string_ == newString);
	ASSERT_FALSE(string_ != newString);
}

TEST_F(StringTest, InequalityOperator)
{
	nctl::String newString = "String2";
	printf("The first string and the new one are not equal: %d\n", string_ != newString);

	ASSERT_TRUE(string_ != newString);
	ASSERT_FALSE(string_ == newString);
}

TEST_F(StringTest, GreaterThanOperator)
{
	nctl::String newString = "String2";
	printf("The second string is lexicographically bigger than the first: %d\n", newString > string_);

	ASSERT_TRUE(newString > string_);
}

TEST_F(StringTest, AppendOperator)
{
	string_ += "Append";
	printString("Appending a C-style string to the first one: ", string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), strnlen("String1Append", Capacity));
	ASSERT_STREQ(string_.data(), "String1Append");
}

TEST_F(StringTest, SubscriptAccessFirstCharacter)
{
	printf("First character of the string: %c\n", string_[0]);
	ASSERT_EQ(string_[0], 'S');
}

TEST_F(StringTest, SubscriptAccessFirstConstCharacter)
{
	const nctl::String constSting = string_;
	printf("First character of the constant string: %c\n", constSting[0]);
	ASSERT_EQ(constSting[0], 'S');
}

TEST_F(StringTest, SubscriptAccessLastCharacter)
{
	printf("Last character of the string: %c\n", string_[string_.length() - 1]);
	ASSERT_EQ(string_[string_.length() - 1], '1');
}

TEST_F(StringTest, SubscriptAccessLastConstCharacter)
{
	const nctl::String constSting = string_;
	printf("Last character of the constant string: %c\n", constSting[constSting.length() - 1]);
	ASSERT_EQ(constSting[constSting.length() - 1], '1');
}

TEST_F(StringTest, AccessAtFirstCharacter)
{
	printf("First character of the string: %c\n", string_.at(0));
	ASSERT_EQ(string_.at(0), 'S');
}

TEST_F(StringTest, AccessAtFirstConstCharacter)
{
	const nctl::String constSting = string_;
	printf("First character of the constant string: %c\n", constSting.at(0));
	ASSERT_EQ(constSting.at(0), 'S');
}

TEST_F(StringTest, AccessAtLastCharacter)
{
	printf("Last character of the string: %c\n", string_.at(string_.length() - 1));
	ASSERT_EQ(string_.at(string_.length() - 1), '1');
}

TEST_F(StringTest, AccessAtLastConstCharacter)
{
	const nctl::String constSting = string_;
	printf("Last character of the constant string: %c\n", constSting.at(constSting.length() - 1));
	ASSERT_EQ(constSting.at(constSting.length() - 1), '1');
}

#ifndef __EMSCRIPTEN__
	#ifdef NCINE_DEBUG
TEST(StringDeathTest, SubscriptAccessBeyondLastCharacter)
{
	nctl::String string("0");
	printf("Accessing a character beyond the length of the string\n");
	ASSERT_DEATH(string[5], "");
}

TEST(StringDeathTest, SubscriptAccessBeyondLastConstCharacter)
{
	const nctl::String string("0");
	printf("Accessing a character beyond the length of the string\n");
	ASSERT_DEATH(string[5], "");
}
	#endif

TEST(StringDeathTest, AccessAtBeyondLastCharacter)
{
	nctl::String string("0");
	printf("Accessing a character beyond the length of the string\n");
	ASSERT_DEATH(string.at(5), "");
}

TEST(StringDeathTest, AccessAtBeyondLastConstCharacter)
{
	const nctl::String string("0");
	printf("Accessing a character beyond the length of the string\n");
	ASSERT_DEATH(string.at(5), "");
}
#endif

}
