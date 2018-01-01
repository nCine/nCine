#include "gtest_hashmap.h"

namespace {

class HashMapIteratorTest : public ::testing::Test
{
  public:
	HashMapIteratorTest() : hashmap_(Capacity) { }

  protected:
	void SetUp() { initHashMap(hashmap_); }

	nc::HashMap<int, int, nc::FixedHashFunc<int> > hashmap_;
};

TEST_F(HashMapIteratorTest, ForLoopIteration)
{
	int n = 0;

	printf("Iterating through elements with for loop:\n");
	for (nc::HashMap<int, int, nc::FixedHashFunc<int> >::ConstIterator i = hashmap_.begin(); i != hashmap_.end(); ++i)
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
	nc::HashMap<int, int, nc::FixedHashFunc<int> > newHashmap(Capacity);

	printf("Iterating over an empty hashmap with for loop:\n");
	for (nc::HashMap<int, int, nc::FixedHashFunc<int> >::ConstIterator i = newHashmap.begin(); i != newHashmap.end(); ++i)
		ASSERT_TRUE(false); // should never reach this point
	printf("\n");
}

TEST_F(HashMapIteratorTest, WhileLoopIteration)
{
	int n = 0;

	printf("Iterating through elements with while loop:\n");
	nc::HashMap<int, int, nc::FixedHashFunc<int> >::ConstIterator i = hashmap_.begin();
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
	nc::HashMap<int, int, nc::FixedHashFunc<int> > newHashmap(Capacity);

	printf("Iterating over an empty hashmap with while loop:\n");
	nc::HashMap<int, int, nc::FixedHashFunc<int> >::ConstIterator i = newHashmap.begin();
	while (i != newHashmap.end())
	{
		ASSERT_TRUE(false); // should never reach this point
		++i;
	}
	printf("\n");
}

}
