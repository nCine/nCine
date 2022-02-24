#include "gtest_staticstring.h"

namespace {

class StaticStringIteratorTest : public ::testing::Test
{
  protected:
	void SetUp() override { string_ = "String1String2"; }

	nctl::StaticString<Capacity> string_;
};

TEST_F(StaticStringIteratorTest, ForLoopIteration)
{
	unsigned int n = 0;

	printf("Iterating through string characters with for loop:");
	for (nctl::StaticString<Capacity>::ConstIterator i = string_.begin(); i != string_.end(); ++i)
	{
		printf(" %c", *i);
		ASSERT_EQ(*i, string_[n++]);
	}
	printf("\n");
}

TEST_F(StaticStringIteratorTest, ForRangeIteration)
{
	unsigned int n = 0;

	printf("Iterating through string characters with range-based for:");
	for (char i : string_)
	{
		printf(" %c", i);
		ASSERT_EQ(i, string_[n++]);
	}
	printf("\n");
}

TEST_F(StaticStringIteratorTest, ForLoopEmptyIteration)
{
	nctl::StaticString<Capacity> newString;

	printf("Iterating over an empty string with for loop:\n");
	for (nctl::StaticString<Capacity>::ConstIterator i = newString.begin(); i != newString.end(); ++i)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(StaticStringIteratorTest, ForRangeEmptyIteration)
{
	nctl::StaticString<Capacity> newString;

	printf("Iterating over an empty string with range-based for:\n");
	for (char i : newString)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(StaticStringIteratorTest, WhileLoopIteration)
{
	unsigned int n = 0;

	printf("Iterating through string characters with while loop:");
	nctl::StaticString<Capacity>::ConstIterator i = string_.begin();
	while (i != string_.end())
	{
		printf(" %c", *i);
		ASSERT_EQ(*i, string_[n]);
		++i;
		++n;
	}
	printf("\n");
}

TEST_F(StaticStringIteratorTest, WhileLoopEmptyIteration)
{
	nctl::StaticString<Capacity> newString;

	printf("Iterating over an empty string with while loop:\n");
	nctl::StaticString<Capacity>::ConstIterator i = newString.begin();
	while (i != newString.end())
	{
		ASSERT_TRUE(false); // should never reach this point
		++i;
	}
	printf("\n");
}

TEST_F(StaticStringIteratorTest, AddIndexToIterator)
{
	nctl::StaticString<Capacity>::ConstIterator it = string_.begin();

	printf("Accessing characters with an iterator and a positive index\n");
	for (int i = 0; i < static_cast<int>(string_.length()); i++)
	{
		printf(" %c", *(it + i));
		ASSERT_EQ(*(it + i), string_[i]);
	}
	printf("\n");
}

TEST_F(StaticStringIteratorTest, AddIndexToIteratorInPlace)
{
	printf("Accessing characters with an iterator and a positive index\n");
	for (int i = 0; i < static_cast<int>(string_.length()); i++)
	{
		nctl::StaticString<Capacity>::ConstIterator it = string_.begin();

		it += i;
		printf(" %c", *it);
		ASSERT_EQ(*it, string_[i]);
	}
	printf("\n");
}

TEST_F(StaticStringIteratorTest, SubtractIndexToIterator)
{
	nctl::StaticString<Capacity>::ConstIterator it = string_.end();

	printf("Accessing characters with an iterator and a negative index\n");
	for (int i = 1; i <= static_cast<int>(string_.length()); i++)
	{
		printf(" %c", *(it - i));
		ASSERT_EQ(*(it - i), string_[string_.length() - i]);
	}
	printf("\n");
}

TEST_F(StaticStringIteratorTest, SubtractIndexToIteratorInPlace)
{
	printf("Accessing characters with an iterator and a negative index\n");
	for (unsigned int i = 1; i <= static_cast<int>(string_.length()); i++)
	{
		nctl::StaticString<Capacity>::ConstIterator it = string_.end();

		it -= i;
		printf(" %c", *it);
		ASSERT_EQ(*it, string_[string_.length() - i]);
	}
	printf("\n");
}

TEST_F(StaticStringIteratorTest, SubtractIterators)
{
	const int diff = string_.end() - string_.begin();
	printf("Difference between end and begin iterators: %d\n", diff);
	ASSERT_EQ(diff, string_.length());
}

TEST_F(StaticStringIteratorTest, SubscriptOperator)
{
	nctl::StaticString<Capacity>::ConstIterator it = string_.begin();

	printf("Accessing characters with the iterator subscript operator\n");
	for (int i = 0; i < static_cast<int>(string_.length()); i++)
	{
		printf(" %c", it[i]);
		ASSERT_EQ(it[i], string_[i]);
	}
	printf("\n");
}

}
