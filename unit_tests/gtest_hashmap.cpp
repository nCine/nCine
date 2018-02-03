#include "gtest_hashmap.h"

namespace {

class HashMapTest : public ::testing::Test
{
  public:
	HashMapTest() : hashmap_(Capacity) { }

  protected:
	void SetUp() override { initHashMap(hashmap_); }

	nctl::HashMap<int, int, nctl::FixedHashFunc<int> > hashmap_;
};

TEST_F(HashMapTest, BucketAmount)
{
	const unsigned int numBuckets = hashmap_.bucketAmount();
	printf("Number of buckets: %u\n", numBuckets);

	ASSERT_EQ(numBuckets, Capacity);
}

TEST_F(HashMapTest, BucketSize)
{
	const int key = 0;
	const unsigned int bucketSize = hashmap_.bucketSize(key);
	printf("Bucket size for key %d: %d\n", key, bucketSize);

	ASSERT_EQ(bucketSize, Size);
}

TEST_F(HashMapTest, RetrieveElements)
{
	printf("Retrieving the elements\n");
	for (unsigned int i = 0; i < Size; i++)
	{
		printf("key: %u, value: %d\n", i, hashmap_[i]);
		ASSERT_EQ(hashmap_[i], KeyValueDifference + i);
	}

	ASSERT_EQ(calcSize(hashmap_), Size);
}

TEST_F(HashMapTest, RemoveElements)
{
	printf("Removing a couple elements\n");
	hashmap_.remove(5);
	hashmap_.remove(7);
	printHashMap(hashmap_);

	int value = 0;
	ASSERT_FALSE(hashmap_.contains(5, value));
	ASSERT_FALSE(hashmap_.contains(7, value));
	ASSERT_EQ(calcSize(hashmap_), Size - 2);
}

TEST_F(HashMapTest, CopyConstruction)
{
	printf("Creating a new hashmap with copy construction\n");
	nctl::HashMap<int, int, nctl::FixedHashFunc<int> > newHashmap(hashmap_);
	printHashMap(newHashmap);

	assertHashMapsAreEqual(hashmap_, newHashmap);
	ASSERT_EQ(calcSize(hashmap_), Size);
	ASSERT_EQ(calcSize(newHashmap), Size);
}

TEST_F(HashMapTest, MoveConstruction)
{
	printf("Creating a new hashmap with move construction\n");
	nctl::HashMap<int, int, nctl::FixedHashFunc<int> > newHashmap = nctl::move(hashmap_);
	printHashMap(newHashmap);

	ASSERT_EQ(hashmap_.bucketAmount(), 0);
	ASSERT_EQ(newHashmap.bucketAmount(), Capacity);
	ASSERT_EQ(calcSize(newHashmap), Size);
}

TEST_F(HashMapTest, AssignmentOperator)
{
	printf("Creating a new hashmap with the assignment operator\n");
	nctl::HashMap<int, int, nctl::FixedHashFunc<int> > newHashmap = hashmap_;
	printHashMap(newHashmap);

	assertHashMapsAreEqual(hashmap_, newHashmap);
	ASSERT_EQ(calcSize(hashmap_), Size);
	ASSERT_EQ(calcSize(newHashmap), Size);
}

TEST_F(HashMapTest, MoveAssignmentOperator)
{
	printf("Creating a new hashmap with the move assignment operator\n");
	nctl::HashMap<int, int, nctl::FixedHashFunc<int> > newHashmap(Capacity);
	newHashmap = nctl::move(hashmap_);
	printHashMap(newHashmap);

	ASSERT_EQ(hashmap_.bucketAmount(), 0);
	ASSERT_EQ(newHashmap.bucketAmount(), Capacity);
	ASSERT_EQ(calcSize(newHashmap), Size);
}

TEST_F(HashMapTest, Contains)
{
	const int key = 1;
	int value = 0;
	const bool found = hashmap_.contains(key, value);
	printf("Key %d is in the hashmap: %d - Value: %d\n", key, found, value);

	ASSERT_TRUE(found);
	ASSERT_EQ(value, key + KeyValueDifference);
}

TEST_F(HashMapTest, DoesNotContain)
{
	const int key = 10;
	int value = 0;
	const bool found = hashmap_.contains(key, value);
	printf("Key %d is in the hashmap: %d - Value: %d\n", key, found, value);

	ASSERT_FALSE(found);
}

}
