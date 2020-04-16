#include "gtest_hashmaplist_string.h"

namespace {

class HashMapListStringTest : public ::testing::Test
{
  public:
	HashMapListStringTest()
	    : strHashmap_(Capacity) {}

  protected:
	void SetUp() override { initHashMap(strHashmap_); }

	nctl::StringHashMapList<nctl::String> strHashmap_;
};

TEST_F(HashMapListStringTest, BucketAmount)
{
	const unsigned int numBuckets = strHashmap_.bucketAmount();
	printf("Number of buckets: %u\n", numBuckets);

	ASSERT_EQ(numBuckets, Capacity);
}

TEST_F(HashMapListStringTest, BucketSize)
{
	const unsigned int bucketSize = strHashmap_.bucketSize(Keys[0]);
	printf("Bucket size for key %s: %u\n", Keys[0], bucketSize);

	ASSERT_EQ(bucketSize, 3u);
}

TEST_F(HashMapListStringTest, RetrieveElements)
{
	printf("Retrieving the elements\n");
	for (unsigned int i = 0; i < Size; i++)
	{
		const nctl::String value = strHashmap_[Keys[i]];
		printf("key: %s, value: %s\n", Keys[i], value.data());
		ASSERT_STREQ(value.data(), Values[i]);
	}
}

TEST_F(HashMapListStringTest, InsertElements)
{
	printf("Inserting elements\n");
	nctl::String newKey(32);
	nctl::String newValue(32);
	for (unsigned int i = Size; i < Size * 2; i++)
	{
		newKey.format("%s_2", Keys[i % Size]);
		newValue.format("%s_2", Values[i % Size]);
		strHashmap_.insert(newKey, newValue);
	}

	for (unsigned int i = 0; i < Size; i++)
		ASSERT_STREQ(strHashmap_[Keys[i]].data(), Values[i]);
	for (unsigned int i = Size; i < Size * 2; i++)
	{
		newKey.format("%s_2", Keys[i % Size]);
		newValue.format("%s_2", Values[i % Size]);
		ASSERT_STREQ(strHashmap_[newKey].data(), newValue.data());
	}

	ASSERT_EQ(strHashmap_.size(), Size * 2);
	ASSERT_EQ(calcSize(strHashmap_), Size * 2);
}

TEST_F(HashMapListStringTest, EmplaceElements)
{
	printf("Emplacing elements\n");
	nctl::String newKey(32);
	nctl::String newValue(32);
	for (unsigned int i = Size; i < Size * 2; i++)
	{
		newKey.format("%s_2", Keys[i % Size]);
		newValue.format("%s_2", Values[i % Size]);
		strHashmap_.emplace(newKey, newValue);
	}

	for (unsigned int i = 0; i < Size; i++)
		ASSERT_STREQ(strHashmap_[Keys[i]].data(), Values[i]);
	for (unsigned int i = Size; i < Size * 2; i++)
	{
		newKey.format("%s_2", Keys[i % Size]);
		newValue.format("%s_2", Values[i % Size]);
		ASSERT_STREQ(strHashmap_[newKey].data(), newValue.data());
	}

	ASSERT_EQ(strHashmap_.size(), Size * 2);
	ASSERT_EQ(calcSize(strHashmap_), Size * 2);
}

TEST_F(HashMapListStringTest, RemoveElements)
{
	printf("Removing a couple elements\n");
	strHashmap_.remove(Keys[0]);
	strHashmap_.remove(Keys[3]);
	printHashMap(strHashmap_);

	nctl::String value;
	ASSERT_FALSE(strHashmap_.contains(Keys[0], value));
	ASSERT_FALSE(strHashmap_.contains(Keys[3], value));
}

TEST_F(HashMapListStringTest, CopyConstruction)
{
	printf("Creating a new hashmap with copy construction\n");
	nctl::StringHashMapList<nctl::String> newStrHashmap(strHashmap_);
	printHashMap(newStrHashmap);

	assertHashMapsAreEqual(strHashmap_, newStrHashmap);
}

TEST_F(HashMapListStringTest, MoveConstruction)
{
	printf("Creating a new hashmap with move construction\n");
	nctl::StringHashMapList<nctl::String> newStrHashmap = nctl::move(strHashmap_);
	printHashMap(newStrHashmap);

	ASSERT_EQ(strHashmap_.bucketAmount(), 0);
	ASSERT_EQ(newStrHashmap.bucketAmount(), Capacity);
	ASSERT_EQ(calcSize(newStrHashmap), Size);
}

TEST_F(HashMapListStringTest, AssignmentOperator)
{
	printf("Creating a new hashmap with the assignment operator\n");
	nctl::StringHashMapList<nctl::String> newStrHashmap(Capacity);
	newStrHashmap = strHashmap_;
	printHashMap(newStrHashmap);

	assertHashMapsAreEqual(strHashmap_, newStrHashmap);
}

TEST_F(HashMapListStringTest, MoveAssignmentOperator)
{
	printf("Creating a new hashmap with the move assignment operator\n");
	nctl::StringHashMapList<nctl::String> newStrHashmap(Capacity);
	newStrHashmap = nctl::move(strHashmap_);
	printHashMap(newStrHashmap);

	ASSERT_EQ(strHashmap_.bucketAmount(), 0);
	ASSERT_EQ(strHashmap_.size(), 0);
	ASSERT_EQ(newStrHashmap.bucketAmount(), Capacity);
	ASSERT_EQ(newStrHashmap.size(), Size);
	ASSERT_EQ(calcSize(newStrHashmap), Size);
}

TEST_F(HashMapListStringTest, Contains)
{
	nctl::String value;
	const bool found = strHashmap_.contains(Keys[0], value);
	printf("Key %s is in the hashmap: %d - Value: %s\n", Keys[0], found, value.data());

	ASSERT_TRUE(found);
	ASSERT_STREQ(value.data(), Values[0]);
}

TEST_F(HashMapListStringTest, DoesNotContain)
{
	const char *key = "Z";
	nctl::String value;
	const bool found = strHashmap_.contains(key, value);
	printf("Key %s is in the hashmap: %d - Value: %s\n", key, found, value.data());

	ASSERT_FALSE(found);
}

}
