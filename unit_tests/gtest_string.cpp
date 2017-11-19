#include "gtest_string.h"

namespace {

class StringTest : public ::testing::Test
{
  public:
	StringTest() : string_(Capacity) { }

  protected:
	void SetUp() override { string_ = "String1"; }

	nc::String string_;
};

TEST_F(StringTest, EmptyString)
{
	nc::String newString(Capacity);
	printf("Creating an empty string: "); printString(newString);

	ASSERT_EQ(newString.capacity(), Capacity);
	ASSERT_EQ(newString.length(), 0);
}

TEST_F(StringTest, AssignCString)
{
	nc::String newString(Capacity);
	newString = "String1";
	printf("Assigning a C-style string to an empty one: "); printString(newString);

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
	nc::String newString(string_);
	printf("The first string and the new one are equal: %d\n", string_ == newString);

	ASSERT_TRUE(string_ == newString);
	ASSERT_FALSE(string_ != newString);
}

TEST_F(StringTest, InequalityOperator)
{
	nc::String newString = "String2";
	printf("The first string and the new one are not equal: %d\n", string_ != newString);

	ASSERT_TRUE(string_ != newString);
	ASSERT_FALSE(string_ == newString);
}

TEST_F(StringTest, GreaterThanOperator)
{
	nc::String newString = "String2";
	printf("The second string is lexicographically bigger than the first: %d\n", newString > string_);

	ASSERT_TRUE(newString > string_);
}

TEST_F(StringTest, AppendOperator)
{
	string_ += "Append";
	printf("Appending a C-style string to the first one: "); printString(string_);

	ASSERT_EQ(string_.capacity(), Capacity);
	ASSERT_EQ(string_.length(), strnlen("String1Append", Capacity));
	ASSERT_STREQ(string_.data(), "String1Append");
}

TEST_F(StringTest, AccessFirstCharacter)
{
	printf("First character of the string: %c\n", string_[0]);
	ASSERT_EQ(string_[0], 'S');
}

TEST_F(StringTest, AccessLastCharacter)
{
	printf("Last character of the string: %c\n", string_[string_.length() - 1]);
	ASSERT_EQ(string_[string_.length() - 1], '1');
}

TEST_F(StringTest, AccessBeyondLastCharacter)
{
	printf("Accessing a character beyond the length of the third string: %c\n", string_[string_.length()]);
	ASSERT_EQ(string_[string_.length()], '\0');
}

}
