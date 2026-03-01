#include "gtest_hashsetlist.h"

namespace {

class HashSetListIteratorTest : public ::testing::Test
{
  public:
	HashSetListIteratorTest()
	    : hashset_(Capacity) {}

  protected:
	void SetUp() override { initHashSet(hashset_); }

	HashSetTestType hashset_;
};

TEST_F(HashSetListIteratorTest, BeginIteratorInvariant)
{
	HashSetTestType::ConstIterator it = hashset_.begin();
	HashSetTestType::ConstIterator copy = it;
	++it;
	--it;

	printf("Increment and then decrement from a begin iterator: %d\n", it == copy);
	ASSERT_EQ(it, copy);
}

TEST_F(HashSetListIteratorTest, EndIteratorInvariants)
{
	HashSetTestType::ConstIterator it = hashset_.end();
	HashSetTestType::ConstIterator copy = it;
	--it;
	++it;

	printf("Decrement and then increment from an end iterator: %d\n", it == copy);
	ASSERT_EQ(it, copy);
}

TEST_F(HashSetListIteratorTest, ReverseIteratorInvariants)
{
	printf("Reverse begin iterator should be the same as the end iterator: %d\n", hashset_.rBegin().base() == hashset_.end());
	ASSERT_EQ(hashset_.rBegin().base(), hashset_.end());
	printf("Reverse end iterator should be the same as the begin iterator: %d\n", hashset_.rEnd().base() == hashset_.begin());
	ASSERT_EQ(hashset_.rEnd().base(), hashset_.begin());

	HashSetTestType::ConstReverseIterator r = hashset_.rBegin();
	for (unsigned int i = 0; i < hashset_.size(); i++)
		++r;

	printf("Reverse iterator should have reached the end: %d\n", r == hashset_.rEnd());
	ASSERT_EQ(r, hashset_.rEnd());
	printf("Reverse iterator should have be the same as the begin iterator: %d\n", r.base() == hashset_.begin());
	ASSERT_EQ(r.base(), hashset_.begin());
}

TEST_F(HashSetListIteratorTest, ReverseIteratorInvariantsEmpty)
{
	HashSetTestType newHashset(Capacity);
	printf("Reverse begin iterator should be the same as the end iterator: %d\n", newHashset.rBegin().base() == newHashset.end());
	ASSERT_EQ(newHashset.rBegin().base(), newHashset.end());
	printf("Reverse end iterator should be the same as the begin iterator: %d\n", newHashset.rEnd().base() == newHashset.begin());
	ASSERT_EQ(newHashset.rEnd().base(), newHashset.begin());
}

TEST_F(HashSetListIteratorTest, ForLoopIteration)
{
	int n = 0;

	printf("Iterating through elements with for loop:\n");
	for (HashSetTestType::ConstIterator i = hashset_.begin(); i != hashset_.end(); ++i)
	{
		printf(" [%d] hash: %u, key: %d\n", n, i.hash(), i.key());
		ASSERT_EQ(i.key(), n);
		n++;
	}
	printf("\n");
}

TEST_F(HashSetListIteratorTest, ForLoopEmptyIteration)
{
	HashSetTestType newHashset(Capacity);

	printf("Iterating over an empty hashset with for loop:\n");
	for (HashSetTestType::ConstIterator i = newHashset.begin(); i != newHashset.end(); ++i)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(HashSetListIteratorTest, ReverseForLoopIteration)
{
	int n = Size - 1;

	printf("Reverse iterating through elements with for loop:\n");
	for (HashSetTestType::ConstReverseIterator r = hashset_.rBegin(); r != hashset_.rEnd(); ++r)
	{
		HashSetTestType::ConstIterator it = r.base();
		--it;

		printf(" [%d] hash: %u, key: %d\n", n, it.hash(), it.key());
		ASSERT_EQ(it.key(), n);
		n--;
	}
	printf("\n");
}

TEST_F(HashSetListIteratorTest, ReverseForLoopEmptyIteration)
{
	HashSetTestType newHashset(Capacity);

	printf("Reverse iterating over an empty hashset with for loop:\n");
	for (HashSetTestType::ConstReverseIterator r = newHashset.rBegin(); r != newHashset.rEnd(); ++r)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(HashSetListIteratorTest, WhileLoopIteration)
{
	int n = 0;

	printf("Iterating through elements with while loop:\n");
	HashSetTestType::ConstIterator i = hashset_.begin();
	while (i != hashset_.end())
	{
		printf(" [%d] hash: %u, key: %d\n", n, i.hash(), i.key());
		ASSERT_EQ(i.key(), n);
		++i;
		++n;
	}
	printf("\n");
}

TEST_F(HashSetListIteratorTest, WhileLoopEmptyIteration)
{
	HashSetTestType newHashset(Capacity);

	printf("Iterating over an empty hashset with while loop:\n");
	HashSetTestType::ConstIterator i = newHashset.begin();
	while (i != newHashset.end())
	{
		ASSERT_TRUE(false); // should never reach this point
		++i;
	}
	printf("\n");
}

TEST_F(HashSetListIteratorTest, ReverseWhileLoopIteration)
{
	int n = Size - 1;

	printf("Reverse iterating through elements with while loop:\n");
	HashSetTestType::ConstReverseIterator r = hashset_.rBegin();
	while (r != hashset_.rEnd())
	{
		HashSetTestType::ConstIterator it = r.base();
		--it;

		printf(" [%d] hash: %u, key: %d\n", n, it.hash(), it.key());
		ASSERT_EQ(it.key(), n);
		++r;
		--n;
	}
	printf("\n");
}

TEST_F(HashSetListIteratorTest, ReverseWhileLoopEmptyIteration)
{
	HashSetTestType newHashset(Capacity);

	printf("Reverse iterating over an empty hashset with while loop:\n");
	HashSetTestType::ConstReverseIterator r = newHashset.rBegin();
	while (r != newHashset.rEnd())
	{
		ASSERT_TRUE(false); // should never reach this point
		++r;
	}
	printf("\n");
}

}
