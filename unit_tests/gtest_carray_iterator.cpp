#include "gtest_carray.h"

namespace {

class CArrayIteratorTest : public ::testing::Test
{
  protected:
	void SetUp() override { initArray(array_); }

	int array_[Capacity];
};

TEST_F(CArrayIteratorTest, BeginIteratorInvariant)
{
	nctl::ArrayIterator<int, true> it = nctl::cBegin(array_);
	nctl::ArrayIterator<int, true> copy = it;
	++it;
	--it;

	printf("Increment and then decrement from a begin iterator: %d\n", it == copy);
	ASSERT_EQ(it, copy);
}

TEST_F(CArrayIteratorTest, EndIteratorInvariants)
{
	nctl::ArrayIterator<int, true> it = nctl::cEnd(array_);
	nctl::ArrayIterator<int, true> copy = it;
	--it;
	++it;

	printf("Decrement and then increment from an end iterator: %d\n", it == copy);
	ASSERT_EQ(it, copy);
}

TEST_F(CArrayIteratorTest, ReverseIteratorInvariants)
{
	printf("Reverse begin iterator should be the same as the end iterator: %d\n", nctl::crBegin(array_).base() == nctl::cEnd(array_));
	ASSERT_EQ(nctl::crBegin(array_).base(), nctl::cEnd(array_));
	printf("Reverse end iterator should be the same as the begin iterator: %d\n", nctl::crEnd(array_).base() == nctl::cBegin(array_));
	ASSERT_EQ(nctl::crEnd(array_).base(), nctl::cBegin(array_));

	nctl::ReverseIterator<nctl::ArrayIterator<int, true>> r = nctl::crBegin(array_);
	for (unsigned int i = 0; i < Capacity; i++)
		++r;

	printf("Reverse iterator should have reached the end: %d\n", r == nctl::crEnd(array_));
	ASSERT_EQ(r, nctl::crEnd(array_));
	printf("Reverse iterator should have be the same as the begin iterator: %d\n", r.base() == nctl::cBegin(array_));
	ASSERT_EQ(r.base(), nctl::cBegin(array_));
}

TEST_F(CArrayIteratorTest, ReverseIteratorInvariantsEmpty)
{
	int newArray[1];

	printf("Reverse begin iterator should be the same as the end iterator: %d\n", nctl::crBegin(newArray).base() == nctl::cEnd(newArray));
	ASSERT_EQ(nctl::crBegin(newArray).base(), nctl::cEnd(newArray));
	printf("Reverse end iterator should be the same as the begin iterator: %d\n", nctl::crEnd(newArray).base() == nctl::cBegin(newArray));
	ASSERT_EQ(nctl::crEnd(newArray).base(), nctl::cBegin(newArray));
}

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
	for (nctl::ReverseIterator<nctl::ArrayIterator<int, true>> r = nctl::crBegin(array_); r != nctl::crEnd(array_); ++r)
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
		++i;
		++n;
	}
	printf("\n");
}

TEST_F(CArrayIteratorTest, ReverseWhileLoopIteration)
{
	int n = Capacity - 1;

	printf("Reverse iterating through elements with while loop:");
	nctl::ReverseIterator<nctl::ArrayIterator<int, true>> r = nctl::crBegin(array_);
	while (r != nctl::crEnd(array_))
	{
		printf(" %d", *r);
		ASSERT_EQ(*r, n);
		++r;
		--n;
	}
	printf("\n");
}

}
