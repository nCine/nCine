#include "gtest_array.h"

namespace {

class ArrayIteratorTest : public ::testing::Test
{
  public:
	ArrayIteratorTest() : array_(Capacity) { }

  protected:
	void SetUp() { initArray(array_); }

	nc::Array<int> array_;
};

TEST_F(ArrayIteratorTest, ForLoopIteration)
{
	int n = FirstElement;

	printf("Iterating through elements with for loop:");
	for (nc::Array<int>::ConstIterator i = array_.begin(); i != array_.end(); ++i)
	{
		printf(" %d", *i);
		ASSERT_EQ(*i, n++);
	}
	printf("\n");
}

TEST_F(ArrayIteratorTest, ForLoopEmptyIteration)
{
	nc::Array<int> newArray;

	printf("Iterating over an empty array with for loop:\n");
	for (nc::Array<int>::ConstIterator i = newArray.begin(); i != newArray.end(); ++i)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(ArrayIteratorTest, ReverseForLoopIteration)
{
	int n = Capacity - 1;

	printf("Iterating through elements with for loop (reverse):");
	for (nc::Array<int>::ConstIterator r = array_.rBegin(); r != array_.rEnd(); --r)
	{
		printf(" %d", *r);
		ASSERT_EQ(*r, n--);
	}
	printf("\n");
}

TEST_F(ArrayIteratorTest, ReverseForLoopEmptyIteration)
{
	nc::Array<int> newArray;

	printf("Iterating over an empty array with for loop (reverse):\n");
	for (nc::Array<int>::ConstIterator r = newArray.rBegin(); r != newArray.rEnd(); --r)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(ArrayIteratorTest, WhileLoopIteration)
{
	int n = FirstElement;

	printf("Iterating through elements with while loop:");
	nc::Array<int>::ConstIterator i = array_.begin();
	while (i != array_.end())
	{
		printf(" %d", *i);
		ASSERT_EQ(*i, n);
		++i; ++n;
	}
	printf("\n");
}

TEST_F(ArrayIteratorTest, WhileLoopEmptyIteration)
{
	nc::Array<int> newArray;

	printf("Iterating over an empty array with while loop:\n");
	nc::Array<int>::ConstIterator i = newArray.begin();
	while (i != newArray.end())
	{
		ASSERT_TRUE(false); // should never reach this point
		++i;
	}
	printf("\n");
}

TEST_F(ArrayIteratorTest, ReverseWhileLoopIteration)
{
	int n = Capacity - 1;

	printf("Iterating through elements with while loop (reverse):");
	nc::Array<int>::ConstIterator r = array_.rBegin();
	while (r != array_.rEnd())
	{
		printf(" %d", *r);
		ASSERT_EQ(*r, n);
		--r; --n;
	}
	printf("\n");
}

TEST_F(ArrayIteratorTest, ReverseWhileLoopEmptyIteration)
{
	nc::Array<int> newArray;

	printf("Iterating over an empty array with while loop (reverse):\n");
	nc::Array<int>::ConstIterator r = newArray.rBegin();
	while (r != newArray.rEnd())
	{
		ASSERT_TRUE(false); // should never reach this point
		--r;
	}
	printf("\n");
}

TEST_F(ArrayIteratorTest, AddIndexToIterator)
{
	nc::Array<int>::ConstIterator it = array_.begin();

	printf("Accessing elements with an iterator and a positive index\n");
	for (int i = 0; i < static_cast<int>(array_.size()); i++)
	{
		printf(" [%d]=%d", i, *(it + i));
		ASSERT_EQ(*(it + i), i);
	}
	printf("\n");
}

TEST_F(ArrayIteratorTest, AddIndexToIteratorInPlace)
{
	printf("Accessing elements with an iterator and a positive index\n");
	for (int i = 0; i < static_cast<int>(array_.size()); i++)
	{
		nc::Array<int>::ConstIterator it = array_.begin();

		it += i;
		printf(" [%d]=%d", i, *it);
		ASSERT_EQ(*it, i);
	}
	printf("\n");
}

TEST_F(ArrayIteratorTest, SubtractIndexToIterator)
{
	nc::Array<int>::ConstIterator it = array_.end();

	printf("Accessing elements with an iterator and a negative index\n");
	for (int i = 1; i <= static_cast<int>(array_.size()); i++)
	{
		printf(" [%d]=%d", i, *(it - i));
		ASSERT_EQ(*(it - i), Capacity - i);
	}
	printf("\n");
}

TEST_F(ArrayIteratorTest, SubtractIndexToIteratorInPlace)
{
	printf("Accessing elements with an iterator and a negative index\n");
	for (int i = 1; i <= static_cast<int>(array_.size()); i++)
	{
		nc::Array<int>::ConstIterator it = array_.end();

		it -= i;
		printf(" [%d]=%d", i, *it);
		ASSERT_EQ(*it, Capacity - i);
	}
	printf("\n");
}

TEST_F(ArrayIteratorTest, SubtractIterators)
{
	const int diff = array_.end() - array_.begin();
	printf("Difference between end and begin iterators: %d\n", diff);
	ASSERT_EQ(diff, array_.size());
}

TEST_F(ArrayIteratorTest, SubscriptOperator)
{
	nc::Array<int>::ConstIterator it = array_.begin();

	printf("Accessing elements with the iterator subscript operator\n");
	for (int i = 0; i < static_cast<int>(array_.size()); i++)
	{
		printf(" [%d]=%d", i, it[i]);
		ASSERT_EQ(it[i], i);
	}
	printf("\n");
}

}
