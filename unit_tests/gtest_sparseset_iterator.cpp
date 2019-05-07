#include "gtest_sparseset.h"

namespace {

class SparseSetIteratorTest : public ::testing::Test
{
  public:
	SparseSetIteratorTest()
	    : sparseset_(Capacity, MaxValue) {}

  protected:
	void SetUp() override { initSparseSet(sparseset_); }

	SparseSetTestType sparseset_;
};

TEST_F(SparseSetIteratorTest, ForLoopIteration)
{
	int n = 0;

	printf("Iterating through elements with for loop:\n");
	for (SparseSetTestType::ConstIterator i = sparseset_.begin(); i != sparseset_.end(); ++i)
	{
		printf(" [%d] value: %d\n", n, *i);
		ASSERT_EQ(*i, n);
		n++;
	}
	printf("\n");
}

TEST_F(SparseSetIteratorTest, ForLoopEmptyIteration)
{
	SparseSetTestType newSparseset(Capacity, MaxValue);

	printf("Iterating over an empty sparseset with for loop:\n");
	for (SparseSetTestType::ConstIterator i = newSparseset.begin(); i != newSparseset.end(); ++i)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(SparseSetIteratorTest, ReverseForLoopIteration)
{
	int n = Size - 1;

	printf("Reverse iterating through elements with for loop:\n");
	for (SparseSetTestType::ConstReverseIterator r = sparseset_.rBegin(); r != sparseset_.rEnd(); ++r)
	{
		printf(" [%d] value: %d\n", n, *r.base());
		ASSERT_EQ(*r.base(), n);
		n--;
	}
	printf("\n");
}

TEST_F(SparseSetIteratorTest, ReverseForLoopEmptyIteration)
{
	SparseSetTestType newSparseset(Capacity, MaxValue);

	printf("Reverse iterating over an empty sparseset with for loop:\n");
	for (SparseSetTestType::ConstReverseIterator r = newSparseset.rBegin(); r != newSparseset.rEnd(); ++r)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(SparseSetIteratorTest, WhileLoopIteration)
{
	int n = 0;

	printf("Iterating through elements with while loop:\n");
	SparseSetTestType::ConstIterator i = sparseset_.begin();
	while (i != sparseset_.end())
	{
		printf(" [%d] value: %d\n", n, *i);
		ASSERT_EQ(*i, n);
		++i;
		++n;
	}
	printf("\n");
}

TEST_F(SparseSetIteratorTest, WhileLoopEmptyIteration)
{
	SparseSetTestType newSparseset(Capacity, MaxValue);

	printf("Iterating over an empty sparseset with while loop:\n");
	SparseSetTestType::ConstIterator i = newSparseset.begin();
	while (i != newSparseset.end())
	{
		ASSERT_TRUE(false); // should never reach this point
		++i;
	}
	printf("\n");
}

TEST_F(SparseSetIteratorTest, ReverseWhileLoopIteration)
{
	int n = Size - 1;

	printf("Reverse iterating through elements with while loop:\n");
	SparseSetTestType::ConstReverseIterator r = sparseset_.rBegin();
	while (r != sparseset_.rEnd())
	{
		printf(" [%d] value: %d\n", n, *r.base());
		ASSERT_EQ(*r.base(), n);
		++r;
		--n;
	}
	printf("\n");
}

TEST_F(SparseSetIteratorTest, ReverseWhileLoopEmptyIteration)
{
	SparseSetTestType newSparseset(Capacity, MaxValue);

	printf("Reverse iterating over an empty sparseset with while loop:\n");
	SparseSetTestType::ConstReverseIterator r = newSparseset.rBegin();
	while (r != newSparseset.rEnd())
	{
		ASSERT_TRUE(false); // should never reach this point
		++r;
	}
	printf("\n");
}

}
