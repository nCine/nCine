#include "gtest_hashmap.h"

namespace {

class HashMapIteratorTest : public ::testing::Test
{
  public:
	HashMapIteratorTest()
	    : hashmap_(Capacity) {}

  protected:
	void SetUp() override { initHashMap(hashmap_); }

	HashMapTestType hashmap_;
};

TEST_F(HashMapIteratorTest, BeginIteratorInvariant)
{
	HashMapTestType::ConstIterator it = hashmap_.begin();
	HashMapTestType::ConstIterator copy = it;
	++it;
	--it;

	printf("Increment and then decrement from a begin iterator: %d\n", it == copy);
	ASSERT_EQ(it, copy);
}

TEST_F(HashMapIteratorTest, EndIteratorInvariants)
{
	HashMapTestType::ConstIterator it = hashmap_.end();
	HashMapTestType::ConstIterator copy = it;
	--it;
	++it;

	printf("Decrement and then increment from an end iterator: %d\n", it == copy);
	ASSERT_EQ(it, copy);
}

TEST_F(HashMapIteratorTest, ReverseIteratorInvariants)
{
	printf("Reverse begin iterator should be the same as the end iterator: %d\n", hashmap_.rBegin().base() == hashmap_.end());
	ASSERT_EQ(hashmap_.rBegin().base(), hashmap_.end());
	printf("Reverse end iterator should be the same as the begin iterator: %d\n", hashmap_.rEnd().base() == hashmap_.begin());
	ASSERT_EQ(hashmap_.rEnd().base(), hashmap_.begin());

	HashMapTestType::ConstReverseIterator r = hashmap_.rBegin();
	for (unsigned int i = 0; i < hashmap_.size(); i++)
		++r;

	printf("Reverse iterator should have reached the end: %d\n", r == hashmap_.rEnd());
	ASSERT_EQ(r, hashmap_.rEnd());
	printf("Reverse iterator should have be the same as the begin iterator: %d\n", r.base() == hashmap_.begin());
	ASSERT_EQ(r.base(), hashmap_.begin());
}

TEST_F(HashMapIteratorTest, ReverseIteratorInvariantsEmpty)
{
	HashMapTestType newHashmap(Capacity);
	printf("Reverse begin iterator should be the same as the end iterator: %d\n", newHashmap.rBegin().base() == newHashmap.end());
	ASSERT_EQ(newHashmap.rBegin().base(), newHashmap.end());
	printf("Reverse end iterator should be the same as the begin iterator: %d\n", newHashmap.rEnd().base() == newHashmap.begin());
	ASSERT_EQ(newHashmap.rEnd().base(), newHashmap.begin());
}

TEST_F(HashMapIteratorTest, ForLoopIteration)
{
	int n = 0;

	printf("Iterating through elements with for loop:\n");
	for (HashMapTestType::ConstIterator i = hashmap_.begin(); i != hashmap_.end(); ++i)
	{
		printf(" [%d] hash: %u, key: %d, value: %d\n", n, i.hash(), i.key(), i.value());
		ASSERT_EQ(i.key(), n);
		ASSERT_EQ(*i, KeyValueDifference + n);
		n++;
	}
	printf("\n");
}

TEST_F(HashMapIteratorTest, ForLoopEmptyIteration)
{
	HashMapTestType newHashmap(Capacity);

	printf("Iterating over an empty hashmap with for loop:\n");
	for (HashMapTestType::ConstIterator i = newHashmap.begin(); i != newHashmap.end(); ++i)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(HashMapIteratorTest, ReverseForLoopIteration)
{
	int n = Size - 1;

	printf("Reverse iterating through elements with for loop:\n");
	for (HashMapTestType::ConstReverseIterator r = hashmap_.rBegin(); r != hashmap_.rEnd(); ++r)
	{
		HashMapTestType::ConstIterator it = r.base();
		--it;

		printf(" [%d] hash: %u, key: %d, value: %d\n", n, it.hash(), it.key(), it.value());
		ASSERT_EQ(it.key(), n);
		ASSERT_EQ(*r, KeyValueDifference + n);
		n--;
	}
	printf("\n");
}

TEST_F(HashMapIteratorTest, ReverseForLoopEmptyIteration)
{
	HashMapTestType newHashmap(Capacity);

	printf("Reverse iterating over an empty hashmap with for loop:\n");
	for (HashMapTestType::ConstReverseIterator r = newHashmap.rBegin(); r != newHashmap.rEnd(); ++r)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(HashMapIteratorTest, WhileLoopIteration)
{
	int n = 0;

	printf("Iterating through elements with while loop:\n");
	HashMapTestType::ConstIterator i = hashmap_.begin();
	while (i != hashmap_.end())
	{
		printf(" [%d] hash: %u, key: %d, value: %d\n", n, i.hash(), i.key(), i.value());
		ASSERT_EQ(i.key(), n);
		ASSERT_EQ(*i, KeyValueDifference + n);
		++i;
		++n;
	}
	printf("\n");
}

TEST_F(HashMapIteratorTest, WhileLoopEmptyIteration)
{
	HashMapTestType newHashmap(Capacity);

	printf("Iterating over an empty hashmap with while loop:\n");
	HashMapTestType::ConstIterator i = newHashmap.begin();
	while (i != newHashmap.end())
	{
		ASSERT_TRUE(false); // should never reach this point
		++i;
	}
	printf("\n");
}

TEST_F(HashMapIteratorTest, ReverseWhileLoopIteration)
{
	int n = Size - 1;

	printf("Reverse iterating through elements with while loop:\n");
	HashMapTestType::ConstReverseIterator r = hashmap_.rBegin();
	while (r != hashmap_.rEnd())
	{
		HashMapTestType::ConstIterator it = r.base();
		--it;

		printf(" [%d] hash: %u, key: %d, value: %d\n", n, it.hash(), it.key(), it.value());
		ASSERT_EQ(it.key(), n);
		ASSERT_EQ(*r, KeyValueDifference + n);
		++r;
		--n;
	}
	printf("\n");
}

TEST_F(HashMapIteratorTest, ReverseWhileLoopEmptyIteration)
{
	HashMapTestType newHashmap(Capacity);

	printf("Reverse iterating over an empty hashmap with while loop:\n");
	HashMapTestType::ConstReverseIterator r = newHashmap.rBegin();
	while (r != newHashmap.rEnd())
	{
		ASSERT_TRUE(false); // should never reach this point
		++r;
	}
	printf("\n");
}

}
