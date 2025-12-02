#include "gtest_string.h"

namespace {

class StringReverseIteratorTest : public ::testing::Test
{
  public:
	StringReverseIteratorTest()
	    : string_(Capacity) {}

  protected:
	void SetUp() override { string_ = "String1String2"; }

	nctl::String string_;
};

TEST_F(StringReverseIteratorTest, ForLoopIteration)
{
	unsigned int n = string_.length() - 1;

	printf("Reverse iterating through string characters with for loop:");
	for (nctl::String::ConstReverseIterator r = string_.rBegin(); r != string_.rEnd(); ++r)
	{
		printf(" %c", *r);
		ASSERT_EQ(*r, string_[n--]);
	}
	printf("\n");
}

TEST_F(StringReverseIteratorTest, ForRangeIteration)
{
	unsigned int n = string_.length() - 1;

	printf("Reverse iterating through string characters with range-based for:");
	for (char r : reverse(string_))
	{
		printf(" %c", r);
		ASSERT_EQ(r, string_[n--]);
	}
	printf("\n");
}

TEST_F(StringReverseIteratorTest, ForLoopEmptyIteration)
{
	nctl::String newString(Capacity);

	printf("Reverse iterating over an empty string with for loop:\n");
	for (nctl::String::ConstReverseIterator r = newString.rBegin(); r != newString.rEnd(); ++r)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(StringReverseIteratorTest, ForRangeEmptyIteration)
{
	nctl::String newString(Capacity);

	printf("Reverse iterating over an empty string with range-based for:\n");
	for (char r : reverse(newString))
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(StringReverseIteratorTest, WhileLoopIteration)
{
	unsigned int n = string_.length() - 1;

	printf("Reverse iterating through string characters with while loop:");
	nctl::String::ConstReverseIterator r = string_.rBegin();
	while (r != string_.rEnd())
	{
		printf(" %c", *r);
		ASSERT_EQ(*r, string_[n]);
		++r;
		--n;
	}
	printf("\n");
}

TEST_F(StringReverseIteratorTest, WhileLoopEmptyIteration)
{
	nctl::String newString(Capacity);

	printf("Reverse iterating over an empty string with while loop:\n");
	nctl::String::ConstReverseIterator r = newString.rBegin();
	while (r != newString.rEnd())
	{
		ASSERT_TRUE(false); // should never reach this point
		++r;
	}
	printf("\n");
}

TEST_F(StringReverseIteratorTest, AddIndexToIterator)
{
	nctl::String::ConstReverseIterator it = string_.rBegin();

	printf("Accessing characters with a reverse iterator and a positive index\n");
	for (int i = 0; i < static_cast<int>(string_.length()); i++)
	{
		printf(" %c", *(it + i));
		ASSERT_EQ(*(it + i), string_[string_.length() - 1 - i]);
	}
	printf("\n");
}

TEST_F(StringReverseIteratorTest, AddIndexToIteratorInPlace)
{
	printf("Accessing characters with a reverse iterator and a positive index\n");
	for (int i = 0; i < static_cast<int>(string_.length()); i++)
	{
		nctl::String::ConstReverseIterator it = string_.rBegin();

		it += i;
		printf(" %c", *it);
		ASSERT_EQ(*it, string_[string_.length() - 1 - i]);
	}
	printf("\n");
}

TEST_F(StringReverseIteratorTest, SubtractIndexToIterator)
{
	nctl::String::ConstReverseIterator it = string_.rEnd();

	printf("Accessing characters with a reverse iterator and a negative index\n");
	for (int i = 1; i <= static_cast<int>(string_.length()); i++)
	{
		printf(" %c", *(it - i));
		ASSERT_EQ(*(it - i), string_[i - 1]);
	}
	printf("\n");
}

TEST_F(StringReverseIteratorTest, SubtractIndexToIteratorInPlace)
{
	printf("Accessing characters with a reverse iterator and a negative index\n");
	for (unsigned int i = 1; i <= string_.length(); i++)
	{
		nctl::String::ConstReverseIterator it = string_.rEnd();

		it -= i;
		printf(" %c", *it);
		ASSERT_EQ(*it, string_[i - 1]);
	}
	printf("\n");
}

TEST_F(StringReverseIteratorTest, SubtractIterators)
{
	const int diff = string_.rEnd() - string_.rBegin();
	printf("Difference between end and begin reverse iterators: %d\n", diff);
	ASSERT_EQ(diff, string_.length());
}

TEST_F(StringReverseIteratorTest, SubscriptOperator)
{
	nctl::String::ConstReverseIterator it = string_.rBegin();

	printf("Accessing characters with the reverse iterator subscript operator\n");
	for (int i = 0; i < static_cast<int>(string_.length()); i++)
	{
		printf(" %c", it[i]);
		ASSERT_EQ(it[i], string_[string_.length() - 1 - i]);
	}
	printf("\n");
}

}
