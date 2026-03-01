#include "gtest_staticstring.h"

namespace {

class StaticStringReverseIteratorTest : public ::testing::Test
{
  protected:
	void SetUp() override { string_ = "String1String2"; }

	nctl::StaticString<Capacity> string_;
};

TEST_F(StaticStringReverseIteratorTest, ReverseIteratorInvariants)
{
	printf("Reverse begin iterator should be the same as the end iterator: %d\n", string_.rBegin().base() == string_.end());
	ASSERT_EQ(string_.rBegin().base(), string_.end());
	printf("Reverse end iterator should be the same as the begin iterator: %d\n", string_.rEnd().base() == string_.begin());
	ASSERT_EQ(string_.rEnd().base(), string_.begin());

	nctl::StaticString<Capacity>::ConstReverseIterator r = string_.rBegin();
	for (unsigned int i = 0; i < string_.length(); i++)
		++r;

	printf("Reverse iterator should have reached the end: %d\n", r == string_.rEnd());
	ASSERT_EQ(r, string_.rEnd());
	printf("Reverse iterator should have be the same as the begin iterator: %d\n", r.base() == string_.begin());
	ASSERT_EQ(r.base(), string_.begin());
}

TEST_F(StaticStringReverseIteratorTest, ReverseIteratorInvariantsEmpty)
{
	nctl::StaticString<Capacity> newString;
	printf("Reverse begin iterator should be the same as the end iterator: %d\n", newString.rBegin().base() == newString.end());
	ASSERT_EQ(newString.rBegin().base(), newString.end());
	printf("Reverse end iterator should be the same as the begin iterator: %d\n", newString.rEnd().base() == newString.begin());
	ASSERT_EQ(newString.rEnd().base(), newString.begin());
}

TEST_F(StaticStringReverseIteratorTest, ForLoopIteration)
{
	unsigned int n = string_.length() - 1;

	printf("Reverse iterating through string characters with for loop:");
	for (nctl::StaticString<Capacity>::ConstReverseIterator r = string_.rBegin(); r != string_.rEnd(); ++r)
	{
		printf(" %c", *r);
		ASSERT_EQ(*r, string_[n--]);
	}
	printf("\n");
}

TEST_F(StaticStringReverseIteratorTest, ForRangeIteration)
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

TEST_F(StaticStringReverseIteratorTest, ForLoopEmptyIteration)
{
	nctl::StaticString<Capacity> newString;

	printf("Reverse iterating over an empty string with for loop:\n");
	for (nctl::StaticString<Capacity>::ConstReverseIterator r = newString.rBegin(); r != newString.rEnd(); ++r)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(StaticStringReverseIteratorTest, ForRangeEmptyIteration)
{
	nctl::StaticString<Capacity> newString;

	printf("Reverse iterating over an empty string with range-based for:\n");
	for (char r : reverse(newString))
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(StaticStringReverseIteratorTest, WhileLoopIteration)
{
	unsigned int n = string_.length() - 1;

	printf("Reverse iterating through string characters with while loop:");
	nctl::StaticString<Capacity>::ConstReverseIterator r = string_.rBegin();
	while (r != string_.rEnd())
	{
		printf(" %c", *r);
		ASSERT_EQ(*r, string_[n]);
		++r;
		--n;
	}
	printf("\n");
}

TEST_F(StaticStringReverseIteratorTest, WhileLoopEmptyIteration)
{
	nctl::StaticString<Capacity> newString;

	printf("Reverse iterating over an empty string with while loop:\n");
	nctl::StaticString<Capacity>::ConstReverseIterator r = newString.rBegin();
	while (r != newString.rEnd())
	{
		ASSERT_TRUE(false); // should never reach this point
		++r;
	}
	printf("\n");
}

TEST_F(StaticStringReverseIteratorTest, AddIndexToIterator)
{
	nctl::StaticString<Capacity>::ConstReverseIterator it = string_.rBegin();

	printf("Accessing characters with a reverse iterator and a positive index\n");
	for (int i = 0; i < static_cast<int>(string_.length()); i++)
	{
		printf(" %c", *(it + i));
		ASSERT_EQ(*(it + i), string_[string_.length() - 1 - i]);
	}
	printf("\n");
}

TEST_F(StaticStringReverseIteratorTest, AddIndexToIteratorInPlace)
{
	printf("Accessing characters with a reverse iterator and a positive index\n");
	for (int i = 0; i < static_cast<int>(string_.length()); i++)
	{
		nctl::StaticString<Capacity>::ConstReverseIterator it = string_.rBegin();

		it += i;
		printf(" %c", *it);
		ASSERT_EQ(*it, string_[string_.length() - 1 - i]);
	}
	printf("\n");
}

TEST_F(StaticStringReverseIteratorTest, SubtractIndexToIterator)
{
	nctl::StaticString<Capacity>::ConstReverseIterator it = string_.rEnd();

	printf("Accessing characters with a reverse iterator and a negative index\n");
	for (int i = 1; i <= static_cast<int>(string_.length()); i++)
	{
		printf(" %c", *(it - i));
		ASSERT_EQ(*(it - i), string_[i - 1]);
	}
	printf("\n");
}

TEST_F(StaticStringReverseIteratorTest, SubtractIndexToIteratorInPlace)
{
	printf("Accessing characters with a reverse iterator and a negative index\n");
	for (unsigned int i = 1; i <= string_.length(); i++)
	{
		nctl::StaticString<Capacity>::ConstReverseIterator it = string_.rEnd();

		it -= i;
		printf(" %c", *it);
		ASSERT_EQ(*it, string_[i - 1]);
	}
	printf("\n");
}

TEST_F(StaticStringReverseIteratorTest, SubtractIterators)
{
	const int diff = string_.rEnd() - string_.rBegin();
	printf("Difference between end and begin reverse iterators: %d\n", diff);
	ASSERT_EQ(diff, string_.length());
}

TEST_F(StaticStringReverseIteratorTest, SubscriptOperator)
{
	nctl::StaticString<Capacity>::ConstReverseIterator it = string_.rBegin();

	printf("Accessing characters with the reverse iterator subscript operator\n");
	for (int i = 0; i < static_cast<int>(string_.length()); i++)
	{
		printf(" %c", it[i]);
		ASSERT_EQ(it[i], string_[string_.length() - 1 - i]);
	}
	printf("\n");
}

}
