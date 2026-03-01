#include "gtest_list.h"

namespace {

class ListIteratorTest : public ::testing::Test
{
  public:
	ListIteratorTest() {}

  protected:
	void SetUp() override { initList(list_); }

	nctl::List<int> list_;
};

TEST_F(ListIteratorTest, BeginIteratorInvariant)
{
	nctl::List<int>::ConstIterator it = list_.begin();
	nctl::List<int>::ConstIterator copy = it;
	++it;
	--it;

	printf("Increment and then decrement from a begin iterator: %d\n", it == copy);
	ASSERT_EQ(it, copy);
}

TEST_F(ListIteratorTest, EndIteratorInvariants)
{
	nctl::List<int>::ConstIterator it = list_.end();
	nctl::List<int>::ConstIterator copy = it;
	--it;
	++it;

	printf("Decrement and then increment from an end iterator: %d\n", it == copy);
	ASSERT_EQ(it, copy);
}

TEST_F(ListIteratorTest, ReverseIteratorInvariants)
{
	printf("Reverse begin iterator should be the same as the end iterator: %d\n", list_.rBegin().base() == list_.end());
	ASSERT_EQ(list_.rBegin().base(), list_.end());
	printf("Reverse end iterator should be the same as the begin iterator: %d\n", list_.rEnd().base() == list_.begin());
	ASSERT_EQ(list_.rEnd().base(), list_.begin());

	nctl::List<int>::ConstReverseIterator r = list_.rBegin();
	for (unsigned int i = 0; i < list_.size(); i++)
		++r;

	printf("Reverse iterator should have reached the end: %d\n", r == list_.rEnd());
	ASSERT_EQ(r, list_.rEnd());
	printf("Reverse iterator should have be the same as the begin iterator: %d\n", r.base() == list_.begin());
	ASSERT_EQ(r.base(), list_.begin());
}

TEST_F(ListIteratorTest, ReverseIteratorInvariantsEmpty)
{
	nctl::List<int> newList;
	printf("Reverse begin iterator should be the same as the end iterator: %d\n", newList.rBegin().base() == newList.end());
	ASSERT_EQ(newList.rBegin().base(), newList.end());
	printf("Reverse end iterator should be the same as the begin iterator: %d\n", newList.rEnd().base() == newList.begin());
	ASSERT_EQ(newList.rEnd().base(), newList.begin());
}

TEST_F(ListIteratorTest, ForLoopIteration)
{
	int n = FirstElement;

	printf("Iterating through nodes with for loop:");
	for (nctl::List<int>::ConstIterator i = list_.begin(); i != list_.end(); ++i)
	{
		printf(" %d", *i);
		ASSERT_EQ(*i, n++);
	}
	printf("\n");
}

TEST_F(ListIteratorTest, ForRangeIteration)
{
	int n = FirstElement;

	printf("Iterating through nodes with range-based for:");
	for (int i : list_)
	{
		printf(" %d", i);
		ASSERT_EQ(i, n++);
	}
	printf("\n");
}

TEST_F(ListIteratorTest, ForLoopEmptyIteration)
{
	nctl::List<int> newList;

	printf("Iterating over an empty list with for loop:\n");
	for (nctl::List<int>::ConstIterator i = newList.begin(); i != newList.end(); ++i)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(ListIteratorTest, ForRangeEmptyIteration)
{
	nctl::List<int> newList;

	printf("Iterating over an empty list with range-based for:\n");
	for (int i : newList)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(ListIteratorTest, ReverseForLoopIteration)
{
	int n = LastElement;

	printf("Reverse iterating through nodes with for loop:");
	for (nctl::List<int>::ConstReverseIterator r = list_.rBegin(); r != list_.rEnd(); ++r)
	{
		printf(" %d", *r);
		ASSERT_EQ(*r, n--);
	}
	printf("\n");
}

TEST_F(ListIteratorTest, ReverseForRangeIteration)
{
	int n = LastElement;

	printf("Reverse iterating through nodes with range-based for:");
	for (int r : reverse(list_))
	{
		printf(" %d", r);
		ASSERT_EQ(r, n--);
	}
	printf("\n");
}

TEST_F(ListIteratorTest, ReverseForLoopEmptyIteration)
{
	nctl::List<int> newList;

	printf("Reverse iterating over an empty list with for loop:\n");
	for (nctl::List<int>::ConstReverseIterator r = newList.rBegin(); r != newList.rEnd(); ++r)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(ListIteratorTest, ReverseForRangeEmptyIteration)
{
	nctl::List<int> newList;

	printf("Reverse iterating over an empty list with range-based for:\n");
	for (int r : reverse(newList))
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(ListIteratorTest, WhileLoopIteration)
{
	int n = FirstElement;

	printf("Iterating through nodes with while loop:");
	nctl::List<int>::ConstIterator i = list_.begin();
	while (i != list_.end())
	{
		printf(" %d", *i);
		ASSERT_EQ(*i, n);
		++i;
		++n;
	}
	printf("\n");
}

TEST_F(ListIteratorTest, WhileLoopEmptyIteration)
{
	nctl::List<int> newList;

	printf("Iterating over an empty list with while loop:\n");
	nctl::List<int>::ConstIterator i = newList.begin();
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

	printf("Reverse iterating through nodes with while loop:");
	nctl::List<int>::ConstReverseIterator r = list_.rBegin();
	while (r != list_.rEnd())
	{
		printf(" %d", *r);
		ASSERT_EQ(*r, n);
		++r;
		--n;
	}
	printf("\n");
}

TEST_F(ListIteratorTest, ReverseWhileLoopEmptyIteration)
{
	nctl::List<int> newList;

	printf("Reverse iterating over an empty list with while loop:\n");
	nctl::List<int>::ConstReverseIterator r = newList.rBegin();
	while (r != newList.rEnd())
	{
		ASSERT_TRUE(false); // should never reach this point
		++r;
	}
	printf("\n");
}

}
