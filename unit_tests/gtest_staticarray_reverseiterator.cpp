#include "gtest_staticarray.h"

namespace {

class StaticArrayReverseIteratorTest : public ::testing::Test
{
  protected:
	void SetUp() override { initArray(array_); }

	nctl::StaticArray<int, Capacity> array_;
};

TEST_F(StaticArrayReverseIteratorTest, ReverseIteratorInvariants)
{
	printf("Reverse begin iterator should be the same as the end iterator: %d\n", array_.rBegin().base() == array_.end());
	ASSERT_EQ(array_.rBegin().base(), array_.end());
	printf("Reverse end iterator should be the same as the begin iterator: %d\n", array_.rEnd().base() == array_.begin());
	ASSERT_EQ(array_.rEnd().base(), array_.begin());

	nctl::StaticArray<int, Capacity>::ConstReverseIterator r = array_.rBegin();
	for (unsigned int i = 0; i < array_.size(); i++)
		++r;

	printf("Reverse iterator should have reached the end: %d\n", r == array_.rEnd());
	ASSERT_EQ(r, array_.rEnd());
	printf("Reverse iterator should have be the same as the begin iterator: %d\n", r.base() == array_.begin());
	ASSERT_EQ(r.base(), array_.begin());
}

TEST_F(StaticArrayReverseIteratorTest, ReverseIteratorInvariantsEmpty)
{
	nctl::StaticArray<int, Capacity> newArray;
	printf("Reverse begin iterator should be the same as the end iterator: %d\n", newArray.rBegin().base() == newArray.end());
	ASSERT_EQ(newArray.rBegin().base(), newArray.end());
	printf("Reverse end iterator should be the same as the begin iterator: %d\n", newArray.rEnd().base() == newArray.begin());
	ASSERT_EQ(newArray.rEnd().base(), newArray.begin());
}

TEST_F(StaticArrayReverseIteratorTest, ForLoopIteration)
{
	int n = Capacity - 1;

	printf("Reverse iterating through elements with for loop:");
	for (nctl::StaticArray<int, Capacity>::ConstReverseIterator r = array_.rBegin(); r != array_.rEnd(); ++r)
	{
		printf(" %d", *r);
		ASSERT_EQ(*r, n--);
	}
	printf("\n");
}

TEST_F(StaticArrayReverseIteratorTest, ForRangeIteration)
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

TEST_F(StaticArrayReverseIteratorTest, ForLoopEmptyIteration)
{
	nctl::StaticArray<int, Capacity> newArray;

	printf("Reverse iterating over an empty array with for loop:\n");
	for (nctl::StaticArray<int, Capacity>::ConstReverseIterator r = newArray.rBegin(); r != newArray.rEnd(); ++r)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(StaticArrayReverseIteratorTest, ForRangeEmptyIteration)
{
	nctl::StaticArray<int, Capacity> newArray;

	printf("Reverse iterating over an empty array with range-based for:\n");
	for (int r : reverse(newArray))
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(StaticArrayReverseIteratorTest, WhileLoopIteration)
{
	int n = Capacity - 1;

	printf("Reverse iterating through elements with while loop:");
	nctl::StaticArray<int, Capacity>::ConstReverseIterator r = array_.rBegin();
	while (r != array_.rEnd())
	{
		printf(" %d", *r);
		ASSERT_EQ(*r, n);
		++r;
		--n;
	}
	printf("\n");
}

TEST_F(StaticArrayReverseIteratorTest, WhileLoopEmptyIteration)
{
	nctl::StaticArray<int, Capacity> newArray;

	printf("Reverse iterating over an empty array with while loop:\n");
	nctl::StaticArray<int, Capacity>::ConstReverseIterator r = newArray.rBegin();
	while (r != newArray.rEnd())
	{
		ASSERT_TRUE(false); // should never reach this point
		++r;
	}
	printf("\n");
}

TEST_F(StaticArrayReverseIteratorTest, AddIndexToIterator)
{
	nctl::StaticArray<int, Capacity>::ConstReverseIterator it = array_.rBegin();

	printf("Accessing elements with a reverse iterator and a positive index\n");
	for (int i = 0; i < static_cast<int>(array_.size()); i++)
	{
		printf(" [%d]=%d", i, *(it + i));
		ASSERT_EQ(*(it + i), Capacity - 1 - i);
	}
	printf("\n");
}

TEST_F(StaticArrayReverseIteratorTest, AddIndexToIteratorInPlace)
{
	printf("Accessing elements with a reverse iterator and a positive index\n");
	for (int i = 0; i < static_cast<int>(array_.size()); i++)
	{
		nctl::StaticArray<int, Capacity>::ConstReverseIterator it = array_.rBegin();

		it += i;
		printf(" [%d]=%d", i, *it);
		ASSERT_EQ(*it, Capacity - 1 - i);
	}
	printf("\n");
}

TEST_F(StaticArrayReverseIteratorTest, SubtractIndexToIterator)
{
	nctl::StaticArray<int, Capacity>::ConstReverseIterator it = array_.rEnd();

	printf("Accessing elements with a reverse iterator and a negative index\n");
	for (int i = 1; i <= static_cast<int>(array_.size()); i++)
	{
		printf(" [%d]=%d", i, *(it - i));
		ASSERT_EQ(*(it - i), i - 1);
	}
	printf("\n");
}

TEST_F(StaticArrayReverseIteratorTest, SubtractIndexToIteratorInPlace)
{
	printf("Accessing elements with a reverse iterator and a negative index\n");
	for (int i = 1; i <= static_cast<int>(array_.size()); i++)
	{
		nctl::StaticArray<int, Capacity>::ConstReverseIterator it = array_.rEnd();

		it -= i;
		printf(" [%d]=%d", i, *it);
		ASSERT_EQ(*it, i - 1);
	}
	printf("\n");
}

TEST_F(StaticArrayReverseIteratorTest, SubtractIterators)
{
	const int diff = array_.rEnd() - array_.rBegin();
	printf("Difference between end and begin reverse iterators: %d\n", diff);
	ASSERT_EQ(diff, array_.size());
}

TEST_F(StaticArrayReverseIteratorTest, SubscriptOperator)
{
	nctl::StaticArray<int, Capacity>::ConstReverseIterator it = array_.rBegin();

	printf("Accessing elements with the reverse iterator subscript operator\n");
	for (int i = 0; i < static_cast<int>(array_.size()); i++)
	{
		printf(" [%d]=%d", i, it[i]);
		ASSERT_EQ(it[i], Capacity - 1 - i);
	}
	printf("\n");
}

}
