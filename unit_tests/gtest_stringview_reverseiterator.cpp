#include "gtest_stringview.h"

namespace {

char stringArray[15] = "String1String2";

class StringViewReverseIteratorTest : public ::testing::Test
{
  public:
	StringViewReverseIteratorTest()
	    : stringView_(stringArray) {}

  protected:
	nctl::StringView stringView_;
};

TEST_F(StringViewReverseIteratorTest, ForLoopIteration)
{
	unsigned int n = stringView_.length() - 1;

	printf("Reverse iterating through string view characters with for loop:");
	for (nctl::StringView::ConstReverseIterator r = stringView_.rBegin(); r != stringView_.rEnd(); ++r)
	{
		printf(" %c", *r);
		ASSERT_EQ(*r, stringView_[n--]);
	}
	printf("\n");
}

TEST_F(StringViewReverseIteratorTest, ForRangeIteration)
{
	unsigned int n = stringView_.length() - 1;

	printf("Reverse iterating through string view characters with range-based for:");
	for (char r : reverse(stringView_))
	{
		printf(" %c", r);
		ASSERT_EQ(r, stringView_[n--]);
	}
	printf("\n");
}

TEST_F(StringViewReverseIteratorTest, ForLoopEmptyIteration)
{
	nctl::StringView newStringView;

	printf("Reverse iterating over an empty string view with for loop:\n");
	for (nctl::StringView::ConstReverseIterator r = newStringView.rBegin(); r != newStringView.rEnd(); ++r)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(StringViewReverseIteratorTest, ForRangeEmptyIteration)
{
	nctl::StringView newStringView;

	printf("Reverse iterating over an empty string view with range-based for:\n");
	for (char r : reverse(newStringView))
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(StringViewReverseIteratorTest, WhileLoopIteration)
{
	unsigned int n = stringView_.length() - 1;

	printf("Reverse iterating through string view characters with while loop:");
	nctl::StringView::ConstReverseIterator r = stringView_.rBegin();
	while (r != stringView_.rEnd())
	{
		printf(" %c", *r);
		ASSERT_EQ(*r, stringView_[n]);
		++r;
		--n;
	}
	printf("\n");
}

TEST_F(StringViewReverseIteratorTest, WhileLoopEmptyIteration)
{
	nctl::StringView newStringView;

	printf("Reverse iterating over an empty string view with while loop:\n");
	nctl::StringView::ConstReverseIterator r = newStringView.rBegin();
	while (r != newStringView.rEnd())
	{
		ASSERT_TRUE(false); // should never reach this point
		++r;
	}
	printf("\n");
}

TEST_F(StringViewReverseIteratorTest, AddIndexToIterator)
{
	nctl::StringView::ConstReverseIterator it = stringView_.rBegin();

	printf("Accessing characters with a reverse iterator and a positive index\n");
	for (int i = 0; i < static_cast<int>(stringView_.length()); i++)
	{
		printf(" %c", *(it + i));
		ASSERT_EQ(*(it + i), stringView_[stringView_.length() - 1 - i]);
	}
	printf("\n");
}

TEST_F(StringViewReverseIteratorTest, AddIndexToIteratorInPlace)
{
	printf("Accessing characters with a reverse iterator and a positive index\n");
	for (int i = 0; i < static_cast<int>(stringView_.length()); i++)
	{
		nctl::StringView::ConstReverseIterator it = stringView_.rBegin();

		it += i;
		printf(" %c", *it);
		ASSERT_EQ(*it, stringView_[stringView_.length() - 1 - i]);
	}
	printf("\n");
}

TEST_F(StringViewReverseIteratorTest, SubtractIndexToIterator)
{
	nctl::StringView::ConstReverseIterator it = stringView_.rEnd();

	printf("Accessing characters with a reverse iterator and a negative index\n");
	for (int i = 1; i <= static_cast<int>(stringView_.length()); i++)
	{
		printf(" %c", *(it - i));
		ASSERT_EQ(*(it - i), stringView_[i - 1]);
	}
	printf("\n");
}

TEST_F(StringViewReverseIteratorTest, SubtractIndexToIteratorInPlace)
{
	printf("Accessing characters with a reverse iterator and a negative index\n");
	for (unsigned int i = 1; i <= stringView_.length(); i++)
	{
		nctl::StringView::ConstReverseIterator it = stringView_.rEnd();

		it -= i;
		printf(" %c", *it);
		ASSERT_EQ(*it, stringView_[i - 1]);
	}
	printf("\n");
}

TEST_F(StringViewReverseIteratorTest, SubtractIterators)
{
	const int diff = stringView_.rEnd() - stringView_.rBegin();
	printf("Difference between end and begin reverse iterators: %d\n", diff);
	ASSERT_EQ(diff, stringView_.length());
}

TEST_F(StringViewReverseIteratorTest, SubscriptOperator)
{
	nctl::StringView::ConstReverseIterator it = stringView_.rBegin();

	printf("Accessing characters with the reverse iterator subscript operator\n");
	for (int i = 0; i < static_cast<int>(stringView_.length()); i++)
	{
		printf(" %c", it[i]);
		ASSERT_EQ(it[i], stringView_[stringView_.length() - 1 - i]);
	}
	printf("\n");
}

}
