#include "gtest_array.h"

namespace {

class ArrayReverseIteratorTest : public ::testing::Test
{
  public:
	ArrayReverseIteratorTest() : array_(Capacity) { }

  protected:
	void SetUp() override { initArray(array_); }

	nctl::Array<int> array_;
};

TEST_F(ArrayReverseIteratorTest, ForLoopIteration)
{
	int n = Capacity - 1;

	printf("Reverse iterating through elements with for loop:");
	for (nctl::Array<int>::ConstReverseIterator r = array_.rBegin(); r != array_.rEnd(); ++r)
	{
		printf(" %d", *r);
		ASSERT_EQ(*r, n--);
	}
	printf("\n");
}

TEST_F(ArrayReverseIteratorTest, ForRangeIteration)
{
	int n = Capacity - 1;

	printf("Reverse iterating through elements with range-based for:");
	for (int r : reverse(array_))
	{
		printf(" %d", r);
		ASSERT_EQ(r, n--);
	}
	printf("\n");
}

TEST_F(ArrayReverseIteratorTest, ForLoopEmptyIteration)
{
	nctl::Array<int> newArray;

	printf("Reverse iterating over an empty array with for loop:\n");
	for (nctl::Array<int>::ConstReverseIterator r = newArray.rBegin(); r != newArray.rEnd(); ++r)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(ArrayReverseIteratorTest, ForRangeEmptyIteration)
{
	nctl::Array<int> newArray;

	printf("Reverse iterating over an empty array with range-based for:\n");
	for (int r : reverse(newArray))
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(ArrayReverseIteratorTest, WhileLoopIteration)
{
	int n = Capacity - 1;

	printf("Reverse iterating through elements with while loop:");
	nctl::Array<int>::ConstReverseIterator r = array_.rBegin();
	while (r != array_.rEnd())
	{
		printf(" %d", *r);
		ASSERT_EQ(*r, n);
		++r; --n;
	}
	printf("\n");
}

TEST_F(ArrayReverseIteratorTest, WhileLoopEmptyIteration)
{
	nctl::Array<int> newArray;

	printf("Reverse iterating over an empty array with while loop:\n");
	nctl::Array<int>::ConstReverseIterator r = newArray.rBegin();
	while (r != newArray.rEnd())
	{
		ASSERT_TRUE(false); // should never reach this point
		++r;
	}
	printf("\n");
}

TEST_F(ArrayReverseIteratorTest, AddIndexToIterator)
{
	nctl::Array<int>::ConstReverseIterator it = array_.rBegin();

	printf("Accessing elements with a reverse iterator and a positive index\n");
	for (int i = 0; i < static_cast<int>(array_.size()); i++)
	{
		printf(" [%d]=%d", i, *(it + i));
		ASSERT_EQ(*(it + i), Capacity - 1 - i);
	}
	printf("\n");
}

TEST_F(ArrayReverseIteratorTest, AddIndexToIteratorInPlace)
{
	printf("Accessing elements with a reverse iterator and a positive index\n");
	for (int i = 0; i < static_cast<int>(array_.size()); i++)
	{
		nctl::Array<int>::ConstReverseIterator it = array_.rBegin();

		it += i;
		printf(" [%d]=%d", i, *it);
		ASSERT_EQ(*it, Capacity - 1 - i);
	}
	printf("\n");
}

TEST_F(ArrayReverseIteratorTest, SubtractIndexToIterator)
{
	nctl::Array<int>::ConstReverseIterator it = array_.rEnd();

	printf("Accessing elements with a reverse iterator and a negative index\n");
	for (int i = 1; i <= static_cast<int>(array_.size()); i++)
	{
		printf(" [%d]=%d", i, *(it - i));
		ASSERT_EQ(*(it - i), i - 1);
	}
	printf("\n");
}

TEST_F(ArrayReverseIteratorTest, SubtractIndexToIteratorInPlace)
{
	printf("Accessing elements with a reverse iterator and a negative index\n");
	for (int i = 1; i <= static_cast<int>(array_.size()); i++)
	{
		nctl::Array<int>::ConstReverseIterator it = array_.rEnd();

		it -= i;
		printf(" [%d]=%d", i, *it);
		ASSERT_EQ(*it, i - 1);
	}
	printf("\n");
}

TEST_F(ArrayReverseIteratorTest, SubtractIterators)
{
	const int diff = array_.rEnd() - array_.rBegin();
	printf("Difference between end and begin reverse iterators: %d\n", diff);
	ASSERT_EQ(diff, array_.size());
}

TEST_F(ArrayReverseIteratorTest, SubscriptOperator)
{
	nctl::Array<int>::ConstReverseIterator it = array_.rBegin();

	printf("Accessing elements with the reverse iterator subscript operator\n");
	for (int i = 0; i < static_cast<int>(array_.size()); i++)
	{
		printf(" [%d]=%d", i, it[i]);
		ASSERT_EQ(it[i], Capacity - 1 - i);
	}
	printf("\n");
}

}
