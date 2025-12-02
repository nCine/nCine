#include "gtest_stringview.h"

namespace {

char stringArray[15] = "String1String2";

class StringViewIteratorTest : public ::testing::Test
{
  public:
	StringViewIteratorTest()
	    : stringView_(stringArray) {}

  protected:
	nctl::StringView stringView_;
};

TEST_F(StringViewIteratorTest, ForLoopIteration)
{
	unsigned int n = 0;

	printf("Iterating through string view characters with for loop:");
	for (nctl::StringView::ConstIterator i = stringView_.begin(); i != stringView_.end(); ++i)
	{
		printf(" %c", *i);
		ASSERT_EQ(*i, stringView_[n++]);
	}
	printf("\n");
}

TEST_F(StringViewIteratorTest, ForRangeIteration)
{
	unsigned int n = 0;

	printf("Iterating through string view characters with range-based for:");
	for (char i : stringView_)
	{
		printf(" %c", i);
		ASSERT_EQ(i, stringView_[n++]);
	}
	printf("\n");
}

TEST_F(StringViewIteratorTest, ForLoopEmptyIteration)
{
	nctl::StringView newStringView;

	printf("Iterating over an empty string view with for loop:\n");
	for (nctl::StringView::ConstIterator i = newStringView.begin(); i != newStringView.end(); ++i)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(StringViewIteratorTest, ForRangeEmptyIteration)
{
	nctl::StringView newStringView;

	printf("Iterating over an empty string view with range-based for:\n");
	for (char i : newStringView)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(StringViewIteratorTest, WhileLoopIteration)
{
	unsigned int n = 0;

	printf("Iterating through string view characters with while loop:");
	nctl::StringView::ConstIterator i = stringView_.begin();
	while (i != stringView_.end())
	{
		printf(" %c", *i);
		ASSERT_EQ(*i, stringView_[n]);
		++i;
		++n;
	}
	printf("\n");
}

TEST_F(StringViewIteratorTest, WhileLoopEmptyIteration)
{
	nctl::StringView newStringView;

	printf("Iterating over an empty string view with while loop:\n");
	nctl::StringView::ConstIterator i = newStringView.begin();
	while (i != newStringView.end())
	{
		ASSERT_TRUE(false); // should never reach this point
		++i;
	}
	printf("\n");
}

TEST_F(StringViewIteratorTest, AddIndexToIterator)
{
	nctl::StringView::ConstIterator it = stringView_.begin();

	printf("Accessing characters with an iterator and a positive index\n");
	for (int i = 0; i < static_cast<int>(stringView_.length()); i++)
	{
		printf(" %c", *(it + i));
		ASSERT_EQ(*(it + i), stringView_[i]);
	}
	printf("\n");
}

TEST_F(StringViewIteratorTest, AddIndexToIteratorInPlace)
{
	printf("Accessing characters with an iterator and a positive index\n");
	for (int i = 0; i < static_cast<int>(stringView_.length()); i++)
	{
		nctl::StringView::ConstIterator it = stringView_.begin();

		it += i;
		printf(" %c", *it);
		ASSERT_EQ(*it, stringView_[i]);
	}
	printf("\n");
}

TEST_F(StringViewIteratorTest, SubtractIndexToIterator)
{
	nctl::StringView::ConstIterator it = stringView_.end();

	printf("Accessing characters with an iterator and a negative index\n");
	for (int i = 1; i <= static_cast<int>(stringView_.length()); i++)
	{
		printf(" %c", *(it - i));
		ASSERT_EQ(*(it - i), stringView_[stringView_.length() - i]);
	}
	printf("\n");
}

TEST_F(StringViewIteratorTest, SubtractIndexToIteratorInPlace)
{
	printf("Accessing characters with an iterator and a negative index\n");
	for (unsigned int i = 1; i <= stringView_.length(); i++)
	{
		nctl::StringView::ConstIterator it = stringView_.end();

		it -= i;
		printf(" %c", *it);
		ASSERT_EQ(*it, stringView_[stringView_.length() - i]);
	}
	printf("\n");
}

TEST_F(StringViewIteratorTest, SubtractIterators)
{
	const int diff = stringView_.end() - stringView_.begin();
	printf("Difference between end and begin iterators: %d\n", diff);
	ASSERT_EQ(diff, stringView_.length());
}

TEST_F(StringViewIteratorTest, SubscriptOperator)
{
	nctl::StringView::ConstIterator it = stringView_.begin();

	printf("Accessing characters with the iterator subscript operator\n");
	for (int i = 0; i < static_cast<int>(stringView_.length()); i++)
	{
		printf(" %c", it[i]);
		ASSERT_EQ(it[i], stringView_[i]);
	}
	printf("\n");
}

}
