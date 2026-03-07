#include "gtest_span.h"

namespace {

class SpanIteratorTest : public ::testing::Test
{
  public:
	SpanIteratorTest()
	    : span_(array_, Capacity) {}

  protected:
	const int array_[Capacity] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	SpanType span_;
};

TEST_F(SpanIteratorTest, BeginIteratorInvariant)
{
	SpanType::ConstIterator it = span_.begin();
	SpanType::ConstIterator copy = it;
	++it;
	--it;

	printf("Increment and then decrement from a begin iterator: %d\n", it == copy);
	ASSERT_EQ(it, copy);
}

TEST_F(SpanIteratorTest, EndIteratorInvariants)
{
	SpanType::ConstIterator it = span_.end();
	SpanType::ConstIterator copy = it;
	--it;
	++it;

	printf("Decrement and then increment from an end iterator: %d\n", it == copy);
	ASSERT_EQ(it, copy);
}

TEST_F(SpanIteratorTest, ForLoopIteration)
{
	int n = FirstElement;

	printf("Iterating through elements with for loop:");
	for (SpanType::ConstIterator i = span_.begin(); i != span_.end(); ++i)
	{
		printf(" %d", *i);
		ASSERT_EQ(*i, n++);
	}
	printf("\n");
}

TEST_F(SpanIteratorTest, ForRangeIteration)
{
	int n = FirstElement;

	printf("Iterating through elements with range-based for:");
	for (int i : span_)
	{
		printf(" %d", i);
		ASSERT_EQ(i, n++);
	}
	printf("\n");
}

TEST_F(SpanIteratorTest, ForLoopEmptyIteration)
{
	SpanType newSpan;

	printf("Iterating over an empty array with for loop:\n");
	for (SpanType::ConstIterator i = newSpan.begin(); i != newSpan.end(); ++i)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(SpanIteratorTest, ForRangeEmptyIteration)
{
	SpanType newSpan;

	printf("Iterating over an empty array with range-based for:\n");
	for (int i : newSpan)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(SpanIteratorTest, WhileLoopIteration)
{
	int n = FirstElement;

	printf("Iterating through elements with while loop:");
	SpanType::ConstIterator i = span_.begin();
	while (i != span_.end())
	{
		printf(" %d", *i);
		ASSERT_EQ(*i, n);
		++i;
		++n;
	}
	printf("\n");
}

TEST_F(SpanIteratorTest, WhileLoopEmptyIteration)
{
	SpanType newSpan;

	printf("Iterating over an empty array with while loop:\n");
	SpanType::ConstIterator i = newSpan.begin();
	while (i != newSpan.end())
	{
		ASSERT_TRUE(false); // should never reach this point
		++i;
	}
	printf("\n");
}

TEST_F(SpanIteratorTest, AddIndexToIterator)
{
	SpanType::ConstIterator it = span_.begin();

	printf("Accessing elements with an iterator and a positive index\n");
	for (int i = 0; i < static_cast<int>(span_.size()); i++)
	{
		printf(" [%d]=%d", i, *(it + i));
		ASSERT_EQ(*(it + i), i);
	}
	printf("\n");
}

TEST_F(SpanIteratorTest, AddIndexToIteratorInPlace)
{
	printf("Accessing elements with an iterator and a positive index\n");
	for (int i = 0; i < static_cast<int>(span_.size()); i++)
	{
		SpanType::ConstIterator it = span_.begin();

		it += i;
		printf(" [%d]=%d", i, *it);
		ASSERT_EQ(*it, i);
	}
	printf("\n");
}

TEST_F(SpanIteratorTest, SubtractIndexToIterator)
{
	SpanType::ConstIterator it = span_.end();

	printf("Accessing elements with an iterator and a negative index\n");
	for (int i = 1; i <= static_cast<int>(span_.size()); i++)
	{
		printf(" [%d]=%d", i, *(it - i));
		ASSERT_EQ(*(it - i), Capacity - i);
	}
	printf("\n");
}

TEST_F(SpanIteratorTest, SubtractIndexToIteratorInPlace)
{
	printf("Accessing elements with an iterator and a negative index\n");
	for (int i = 1; i <= static_cast<int>(span_.size()); i++)
	{
		SpanType::ConstIterator it = span_.end();

		it -= i;
		printf(" [%d]=%d", i, *it);
		ASSERT_EQ(*it, Capacity - i);
	}
	printf("\n");
}

TEST_F(SpanIteratorTest, SubtractIterators)
{
	const int diff = span_.end() - span_.begin();
	printf("Difference between end and begin iterators: %d\n", diff);
	ASSERT_EQ(diff, span_.size());
}

TEST_F(SpanIteratorTest, SubscriptOperator)
{
	SpanType::ConstIterator it = span_.begin();

	printf("Accessing elements with the iterator subscript operator\n");
	for (int i = 0; i < static_cast<int>(span_.size()); i++)
	{
		printf(" [%d]=%d", i, it[i]);
		ASSERT_EQ(it[i], i);
	}
	printf("\n");
}

}
