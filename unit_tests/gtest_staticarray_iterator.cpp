#include "gtest_staticarray.h"

namespace {

class StaticArrayIteratorTest : public ::testing::Test
{
  protected:
	void SetUp() override { initArray(array_); }

	nctl::StaticArray<int, Capacity> array_;
};

TEST_F(StaticArrayIteratorTest, ForLoopIteration)
{
	int n = FirstElement;

	printf("Iterating through elements with for loop:");
	for (nctl::StaticArray<int, Capacity>::ConstIterator i = array_.begin(); i != array_.end(); ++i)
	{
		printf(" %d", *i);
		ASSERT_EQ(*i, n++);
	}
	printf("\n");
}

TEST_F(StaticArrayIteratorTest, ForLoopEmptyIteration)
{
	nctl::StaticArray<int, Capacity> newArray;

	printf("Iterating over an empty array with for loop:\n");
	for (nctl::StaticArray<int, Capacity>::ConstIterator i = newArray.begin(); i != newArray.end(); ++i)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(StaticArrayIteratorTest, WhileLoopIteration)
{
	int n = FirstElement;

	printf("Iterating through elements with while loop:");
	nctl::StaticArray<int, Capacity>::ConstIterator i = array_.begin();
	while (i != array_.end())
	{
		printf(" %d", *i);
		ASSERT_EQ(*i, n);
		++i; ++n;
	}
	printf("\n");
}

TEST_F(StaticArrayIteratorTest, WhileLoopEmptyIteration)
{
	nctl::StaticArray<int, Capacity> newArray;

	printf("Iterating over an empty array with while loop:\n");
	nctl::StaticArray<int, Capacity>::ConstIterator i = newArray.begin();
	while (i != newArray.end())
	{
		ASSERT_TRUE(false); // should never reach this point
		++i;
	}
	printf("\n");
}

TEST_F(StaticArrayIteratorTest, AddIndexToIterator)
{
	nctl::StaticArray<int, Capacity>::ConstIterator it = array_.begin();

	printf("Accessing elements with an iterator and a positive index\n");
	for (int i = 0; i < static_cast<int>(array_.size()); i++)
	{
		printf(" [%d]=%d", i, *(it + i));
		ASSERT_EQ(*(it + i), i);
	}
	printf("\n");
}

TEST_F(StaticArrayIteratorTest, AddIndexToIteratorInPlace)
{
	printf("Accessing elements with an iterator and a positive index\n");
	for (int i = 0; i < static_cast<int>(array_.size()); i++)
	{
		nctl::StaticArray<int, Capacity>::ConstIterator it = array_.begin();

		it += i;
		printf(" [%d]=%d", i, *it);
		ASSERT_EQ(*it, i);
	}
	printf("\n");
}

TEST_F(StaticArrayIteratorTest, SubtractIndexToIterator)
{
	nctl::StaticArray<int, Capacity>::ConstIterator it = array_.end();

	printf("Accessing elements with an iterator and a negative index\n");
	for (int i = 1; i <= static_cast<int>(array_.size()); i++)
	{
		printf(" [%d]=%d", i, *(it - i));
		ASSERT_EQ(*(it - i), Capacity - i);
	}
	printf("\n");
}

TEST_F(StaticArrayIteratorTest, SubtractIndexToIteratorInPlace)
{
	printf("Accessing elements with an iterator and a negative index\n");
	for (int i = 1; i <= static_cast<int>(array_.size()); i++)
	{
		nctl::StaticArray<int, Capacity>::ConstIterator it = array_.end();

		it -= i;
		printf(" [%d]=%d", i, *it);
		ASSERT_EQ(*it, Capacity - i);
	}
	printf("\n");
}

TEST_F(StaticArrayIteratorTest, SubtractIterators)
{
	const int diff = array_.end() - array_.begin();
	printf("Difference between end and begin iterators: %d\n", diff);
	ASSERT_EQ(diff, array_.size());
}

TEST_F(StaticArrayIteratorTest, SubscriptOperator)
{
	nctl::StaticArray<int, Capacity>::ConstIterator it = array_.begin();

	printf("Accessing elements with the iterator subscript operator\n");
	for (int i = 0; i < static_cast<int>(array_.size()); i++)
	{
		printf(" [%d]=%d", i, it[i]);
		ASSERT_EQ(it[i], i);
	}
	printf("\n");
}

}
