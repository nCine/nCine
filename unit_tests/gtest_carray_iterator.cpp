#include "gtest_carray.h"

namespace {

class CArrayIteratorTest : public ::testing::Test
{
  protected:
	void SetUp() override { initArray(array_); }

	int array_[Capacity];
};

TEST_F(CArrayIteratorTest, ForLoopIteration)
{
	int n = FirstElement;

	printf("Iterating through elements with for loop:");
	for (nctl::ArrayIterator<int, true> i = nctl::cBegin(array_); i != nctl::cEnd(array_); ++i)
	{
		printf(" %d", *i);
		ASSERT_EQ(*i, n++);
	}
	printf("\n");
}

TEST_F(CArrayIteratorTest, ForRangeIteration)
{
	int n = FirstElement;

	printf("Iterating through elements with range-based for:");
	for (int i : array_)
	{
		printf(" %d", i);
		ASSERT_EQ(i, n++);
	}
	printf("\n");
}

TEST_F(CArrayIteratorTest, ReverseForLoopIteration)
{
	int n = Capacity - 1;

	printf("Reverse iterating through elements with for loop:");
	for (nctl::ReverseIterator<nctl::ArrayIterator<int, true> > r = nctl::crBegin(array_); r != nctl::crEnd(array_); ++r)
	{
		printf(" %d", *r);
		ASSERT_EQ(*r, n--);
	}
	printf("\n");
}

TEST_F(CArrayIteratorTest, WhileLoopIteration)
{
	int n = FirstElement;

	printf("Iterating through elements with while loop:");
	nctl::ArrayIterator<int, true> i = nctl::cBegin(array_);
	while (i != nctl::cEnd(array_))
	{
		printf(" %d", *i);
		ASSERT_EQ(*i, n);
		++i; ++n;
	}
	printf("\n");
}

TEST_F(CArrayIteratorTest, ReverseWhileLoopIteration)
{
	int n = Capacity - 1;

	printf("Reverse iterating through elements with while loop:");
	nctl::ReverseIterator<nctl::ArrayIterator<int, true> > r = nctl::crBegin(array_);
	while (r != nctl::crEnd(array_))
	{
		printf(" %d", *r);
		ASSERT_EQ(*r, n);
		++r; --n;
	}
	printf("\n");
}

}
