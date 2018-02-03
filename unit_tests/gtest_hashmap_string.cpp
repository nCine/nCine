#include "gtest_hashmap_string.h"

namespace {

class HashMapStringTest : public ::testing::Test
{
  public:
	HashMapStringTest() : strHashmap_(Capacity) { }

  protected:
	void SetUp() override { initHashMap(strHashmap_); }

	nctl::StringHashMap<nctl::String> strHashmap_;
};

TEST_F(HashMapStringTest, BucketAmount)
{
	const unsigned int numBuckets = strHashmap_.bucketAmount();
	printf("Number of buckets: %u\n", numBuckets);

	ASSERT_EQ(numBuckets, Capacity);
}

TEST_F(HashMapStringTest, BucketSize)
{
	const unsigned int bucketSize = strHashmap_.bucketSize(Keys[0]);
	printf("Bucket size for key %s: %d\n", Keys[0], bucketSize);

	ASSERT_EQ(bucketSize, 2u);
}

TEST_F(HashMapStringTest, RetrieveElements)
{
	printf("Retrieving the elements\n");
	for (unsigned int i = 0; i < Size; i++)
	{
		const nctl::String value = strHashmap_[Keys[i]];
		printf("key: %s, value: %s\n", Keys[i], value.data());
		ASSERT_STREQ(value.data(), Values[i]);
	}
}

TEST_F(HashMapStringTest, RemoveElements)
{
	printf("Removing a couple elements\n");
	strHashmap_.remove(Keys[0]);
	strHashmap_.remove(Keys[3]);
	printHashMap(strHashmap_);

	nctl::String value;
	ASSERT_FALSE(strHashmap_.contains(Keys[0], value));
	ASSERT_FALSE(strHashmap_.contains(Keys[3], value));
}

TEST_F(HashMapStringTest, CopyConstruction)
{
	printf("Creating a new hashmap with copy construction\n");
	nctl::StringHashMap<nctl::String> newStrHashmap(strHashmap_);
	printHashMap(newStrHashmap);

	assertHashMapsAreEqual(strHashmap_, newStrHashmap);
}

TEST_F(HashMapStringTest, MoveConstruction)
{
	printf("Creating a new hashmap with move construction\n");
	nctl::StringHashMap<nctl::String> newStrHashmap = nctl::move(strHashmap_);
	printHashMap(newStrHashmap);

	ASSERT_EQ(strHashmap_.bucketAmount(), 0);
	ASSERT_EQ(newStrHashmap.bucketAmount(), Capacity);
	ASSERT_EQ(calcSize(newStrHashmap), Size);
}

TEST_F(HashMapStringTest, AssignmentOperator)
{
	printf("Creating a new hashmap with the assignment operator\n");
	nctl::StringHashMap<nctl::String> newStrHashmap = strHashmap_;
	printHashMap(newStrHashmap);

	assertHashMapsAreEqual(strHashmap_, newStrHashmap);
}

TEST_F(HashMapStringTest, MoveAssignmentOperator)
{
	printf("Creating a new hashmap with the move assignment operator\n");
	nctl::StringHashMap<nctl::String> newStrHashmap(Capacity);
	newStrHashmap = nctl::move(strHashmap_);
	printHashMap(newStrHashmap);

	ASSERT_EQ(strHashmap_.bucketAmount(), 0);
	ASSERT_EQ(newStrHashmap.bucketAmount(), Capacity);
	ASSERT_EQ(calcSize(newStrHashmap), Size);
}

TEST_F(HashMapStringTest, Contains)
{
	nctl::String value;
	const bool found = strHashmap_.contains(Keys[0], value);
	printf("Key %s is in the hashmap: %d - Value: %s\n", Keys[0], found, value.data());

	ASSERT_TRUE(found);
	ASSERT_STREQ(value.data(), Values[0]);
}

TEST_F(HashMapStringTest, DoesNotContain)
{
	const char *key = "Z";
	nctl::String value;
	const bool found = strHashmap_.contains(key, value);
	printf("Key %s is in the hashmap: %d - Value: %s\n", key, found, value.data());

	ASSERT_FALSE(found);
}

}
