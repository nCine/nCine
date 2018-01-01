#include "gtest_list.h"

namespace {

class ListIteratorTest : public ::testing::Test
{
  public:
	ListIteratorTest() { }

  protected:
	void SetUp() { initList(list_); }

	nc::List<int> list_;
};

TEST_F(ListIteratorTest, ForLoopIteration)
{
	int n = FirstElement;

	printf("Iterating through nodes with for loop:");
	for (nc::List<int>::ConstIterator i = list_.begin(); i != list_.end(); ++i)
	{
		printf(" %d", *i);
		ASSERT_EQ(*i, n++);
	}
	printf("\n");
}

TEST_F(ListIteratorTest, ForLoopEmptyIteration)
{
	nc::List<int> newList;

	printf("Iterating over an empty list with for loop:\n");
	for (nc::List<int>::ConstIterator i = newList.begin(); i != newList.end(); ++i)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(ListIteratorTest, ReverseForLoopIteration)
{
	int n = LastElement;

	printf("Iterating through nodes with for loop (reverse):");
	for (nc::List<int>::ConstIterator r = list_.rBegin(); r != list_.rEnd(); --r)
	{
		printf(" %d", *r);
		ASSERT_EQ(*r, n--);
	}
	printf("\n");
}

TEST_F(ListIteratorTest, ReverseForLoopEmptyIteration)
{
	nc::List<int> newList;

	printf("Iterating over an empty list with for loop (reverse):\n");
	for (nc::List<int>::ConstIterator r = newList.rBegin(); r != newList.rEnd(); --r)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(ListIteratorTest, WhileLoopIteration)
{
	int n = FirstElement;

	printf("Iterating through nodes with while loop:");
	nc::List<int>::ConstIterator i = list_.begin();
	while (i != list_.end())
	{
		printf(" %d", *i);
		ASSERT_EQ(*i, n);
		++i; ++n;
	}
	printf("\n");
}

TEST_F(ListIteratorTest, WhileLoopEmptyIteration)
{
	nc::List<int> newList;

	printf("Iterating over an empty list with while loop:\n");
	nc::List<int>::ConstIterator i = newList.begin();
	while (i != newList.end())
	{
		ASSERT_TRUE(false); // should never reach this point
		++i;
	}
	printf("\n");
}

TEST_F(ListIteratorTest, ReverseWhileLoopIteration)
{
	int n = LastElement;

	printf("Iterating through nodes with while loop (reverse):");
	nc::List<int>::ConstIterator r = list_.rBegin();
	while (r != list_.rEnd())
	{
		printf(" %d", *r);
		ASSERT_EQ(*r, n);
		--r; --n;
	}
	printf("\n");
}

TEST_F(ListIteratorTest, ReverseWhileLoopEmptyIteration)
{
	nc::List<int> newList;

	printf("Iterating over an empty list with while loop (reverse):\n");
	nc::List<int>::ConstIterator r = newList.rBegin();
	while (r != newList.rEnd())
	{
		ASSERT_TRUE(false); // should never reach this point
		--r;
	}
	printf("\n");
}

}
