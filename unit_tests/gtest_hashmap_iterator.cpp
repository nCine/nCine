#include "gtest_hashmap.h"

namespace {

class HashMapIteratorTest : public ::testing::Test
{
  public:
	HashMapIteratorTest() : hashmap_(Capacity) { }

  protected:
	void SetUp() override { initHashMap(hashmap_); }

	nctl::HashMap<int, int, nctl::FixedHashFunc<int> > hashmap_;
};

TEST_F(HashMapIteratorTest, ForLoopIteration)
{
	int n = 0;

	printf("Iterating through elements with for loop:\n");
	for (nctl::HashMap<int, int, nctl::FixedHashFunc<int> >::ConstIterator i = hashmap_.begin(); i != hashmap_.end(); ++i)
	{
		printf(" [%d] hash: %lu, key: %d, value: %d\n", n, i.hash(), i.key(), i.value());
		ASSERT_EQ(i.key(), n);
		ASSERT_EQ(*i, KeyValueDifference + n);
		n++;
	}
	printf("\n");
}

TEST_F(HashMapIteratorTest, ForLoopEmptyIteration)
{
	nctl::HashMap<int, int, nctl::FixedHashFunc<int> > newHashmap(Capacity);

	printf("Iterating over an empty hashmap with for loop:\n");
	for (nctl::HashMap<int, int, nctl::FixedHashFunc<int> >::ConstIterator i = newHashmap.begin(); i != newHashmap.end(); ++i)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(HashMapIteratorTest, ReverseForLoopIteration)
{
	int n = Size - 1;

	printf("Reverse iterating through elements with for loop:\n");
	for (nctl::HashMap<int, int, nctl::FixedHashFunc<int> >::ConstReverseIterator r = hashmap_.rBegin(); r != hashmap_.rEnd(); ++r)
	{
		printf(" [%d] hash: %lu, key: %d, value: %d\n", n, r.base().hash(), r.base().key(), r.base().value());
		ASSERT_EQ(r.base().key(), n);
		ASSERT_EQ(*r, KeyValueDifference + n);
		n--;
	}
	printf("\n");
}

TEST_F(HashMapIteratorTest, ReverseForLoopEmptyIteration)
{
	nctl::HashMap<int, int, nctl::FixedHashFunc<int> > newHashmap(Capacity);

	printf("Reverse iterating over an empty hashmap with for loop:\n");
	for (nctl::HashMap<int, int, nctl::FixedHashFunc<int> >::ConstReverseIterator r = newHashmap.rBegin(); r != newHashmap.rEnd(); ++r)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(HashMapIteratorTest, WhileLoopIteration)
{
	int n = 0;

	printf("Iterating through elements with while loop:\n");
	nctl::HashMap<int, int, nctl::FixedHashFunc<int> >::ConstIterator i = hashmap_.begin();
	while (i != hashmap_.end())
	{
		printf(" [%d] hash: %lu, key: %d, value: %d\n", n, i.hash(), i.key(), i.value());
		ASSERT_EQ(i.key(), n);
		ASSERT_EQ(*i, KeyValueDifference + n);
		++i; ++n;
	}
	printf("\n");
}

TEST_F(HashMapIteratorTest, WhileLoopEmptyIteration)
{
	nctl::HashMap<int, int, nctl::FixedHashFunc<int> > newHashmap(Capacity);

	printf("Iterating over an empty hashmap with while loop:\n");
	nctl::HashMap<int, int, nctl::FixedHashFunc<int> >::ConstIterator i = newHashmap.begin();
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
	nctl::HashMap<int, int, nctl::FixedHashFunc<int> >::ConstReverseIterator r = hashmap_.rBegin();
	while (r != hashmap_.rEnd())
	{
		printf(" [%d] hash: %lu, key: %d, value: %d\n", n, r.base().hash(), r.base().key(), r.base().value());
		ASSERT_EQ(r.base().key(), n);
		ASSERT_EQ(*r, KeyValueDifference + n);
		++r; --n;
	}
	printf("\n");
}

TEST_F(HashMapIteratorTest, ReverseWhileLoopEmptyIteration)
{
	nctl::HashMap<int, int, nctl::FixedHashFunc<int> > newHashmap(Capacity);

	printf("Reverse iterating over an empty hashmap with while loop:\n");
	nctl::HashMap<int, int, nctl::FixedHashFunc<int> >::ConstReverseIterator r = newHashmap.rBegin();
	while (r != newHashmap.rEnd())
	{
		ASSERT_TRUE(false); // should never reach this point
		++r;
	}
	printf("\n");
}

}
