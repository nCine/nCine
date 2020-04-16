#include "gtest_hashmaplist.h"

namespace {

class HashMapListTest : public ::testing::Test
{
  public:
	HashMapListTest()
	    : hashmap_(Capacity) {}

  protected:
	void SetUp() override { initHashMap(hashmap_); }

	HashMapTestType hashmap_;
};

#ifndef __EMSCRIPTEN__
TEST(HashMapListDeathTest, ZeroCapacity)
{
	printf("Creating an hashmap of zero capacity\n");
	ASSERT_DEATH(HashMapTestType newHashmap(0), "");
}
#endif

TEST_F(HashMapListTest, Size)
{
	const unsigned int size = hashmap_.size();
	printf("Size: %u\n", size);

	ASSERT_EQ(size, Size);
	ASSERT_EQ(calcSize(hashmap_), Size);
}

TEST_F(HashMapListTest, LoadFactor)
{
	const float loadFactor = hashmap_.loadFactor();
	printf("Size: %u, Capacity: %u, Load Factor: %f\n", Size, Capacity, loadFactor);

	ASSERT_FLOAT_EQ(loadFactor, Size / static_cast<float>(Capacity));
}

TEST_F(HashMapListTest, BucketAmount)
{
	const unsigned int numBuckets = hashmap_.bucketAmount();
	printf("Number of buckets: %u\n", numBuckets);

	ASSERT_EQ(numBuckets, Capacity);
}

TEST_F(HashMapListTest, BucketSize)
{
	const int key = 0;
	const unsigned int bucketSize = hashmap_.bucketSize(key);
	printf("Bucket size for key %d: %u\n", key, bucketSize);

	ASSERT_EQ(bucketSize, Size);
}

TEST_F(HashMapListTest, BucketIndex)
{
	for (unsigned int i = 0; i < Size; i++)
	{
		const unsigned int bucketIndex = hashmap_.bucket(i);
		printf("Bucket index for key %u and hash %u: %u\n", i, hashmap_.hash(i), bucketIndex);

		ASSERT_EQ(bucketIndex, hashmap_.hash(i) % Capacity);
	}
}

TEST_F(HashMapListTest, Clear)
{
	ASSERT_FALSE(hashmap_.isEmpty());
	hashmap_.clear();
	printHashMap(hashmap_);
	ASSERT_TRUE(hashmap_.isEmpty());
	ASSERT_EQ(hashmap_.size(), 0u);
	ASSERT_EQ(hashmap_.bucketAmount(), Capacity);
}

TEST_F(HashMapListTest, RetrieveElements)
{
	printf("Retrieving the elements\n");
	for (unsigned int i = 0; i < Size; i++)
	{
		printf("key: %u, value: %d\n", i, hashmap_[i]);
		ASSERT_EQ(hashmap_[i], i + KeyValueDifference);
	}

	ASSERT_EQ(hashmap_.size(), Size);
	ASSERT_EQ(calcSize(hashmap_), Size);
}

TEST_F(HashMapListTest, InsertElements)
{
	printf("Inserting elements\n");
	for (unsigned int i = Size; i < Size * 2; i++)
		hashmap_.insert(i, i + KeyValueDifference);

	for (unsigned int i = 0; i < Size * 2; i++)
		ASSERT_EQ(hashmap_[i], i + KeyValueDifference);

	ASSERT_EQ(hashmap_.size(), Size * 2);
	ASSERT_EQ(calcSize(hashmap_), Size * 2);
}

TEST_F(HashMapListTest, InsertConstElements)
{
	printf("Inserting const elements\n");
	for (unsigned int i = Size; i < Size * 2; i++)
	{
		const int value = i + KeyValueDifference;
		hashmap_.insert(i, value);
	}

	for (unsigned int i = 0; i < Size * 2; i++)
		ASSERT_EQ(hashmap_[i], i + KeyValueDifference);

	ASSERT_EQ(hashmap_.size(), Size * 2);
	ASSERT_EQ(calcSize(hashmap_), Size * 2);
}

TEST_F(HashMapListTest, FailInsertElements)
{
	printf("Trying to insert elements already in the hashmap\n");
	for (unsigned int i = 0; i < Size * 2; i++)
		hashmap_.insert(i, i + 2 * KeyValueDifference);

	for (unsigned int i = 0; i < Size; i++)
		ASSERT_EQ(hashmap_[i], i + KeyValueDifference);
	for (unsigned int i = Size; i < Size * 2; i++)
		ASSERT_EQ(hashmap_[i], i + 2 * KeyValueDifference);

	ASSERT_EQ(hashmap_.size(), Size * 2);
	ASSERT_EQ(calcSize(hashmap_), Size * 2);
}

TEST_F(HashMapListTest, FailInsertConstElements)
{
	printf("Trying to insert elements already in the hashmap\n");
	for (unsigned int i = 0; i < Size * 2; i++)
	{
		const int value = i + 2 * KeyValueDifference;
		hashmap_.insert(i, value);
	}

	for (unsigned int i = 0; i < Size; i++)
		ASSERT_EQ(hashmap_[i], i + KeyValueDifference);
	for (unsigned int i = Size; i < Size * 2; i++)
		ASSERT_EQ(hashmap_[i], i + 2 * KeyValueDifference);

	ASSERT_EQ(hashmap_.size(), Size * 2);
	ASSERT_EQ(calcSize(hashmap_), Size * 2);
}

TEST_F(HashMapListTest, EmplaceElements)
{
	printf("Emplacing elements\n");
	for (unsigned int i = Size; i < Size * 2; i++)
		hashmap_.emplace(i, i + KeyValueDifference);

	for (unsigned int i = 0; i < Size * 2; i++)
		ASSERT_EQ(hashmap_[i], i + KeyValueDifference);

	ASSERT_EQ(hashmap_.size(), Size * 2);
	ASSERT_EQ(calcSize(hashmap_), Size * 2);
}

TEST_F(HashMapListTest, FailEmplaceElements)
{
	printf("Trying to emplace elements already in the hashmap\n");
	for (unsigned int i = 0; i < Size * 2; i++)
		hashmap_.emplace(i, i + 2 * KeyValueDifference);

	for (unsigned int i = 0; i < Size; i++)
		ASSERT_EQ(hashmap_[i], i + KeyValueDifference);
	for (unsigned int i = Size; i < Size * 2; i++)
		ASSERT_EQ(hashmap_[i], i + 2 * KeyValueDifference);

	ASSERT_EQ(hashmap_.size(), Size * 2);
	ASSERT_EQ(calcSize(hashmap_), Size * 2);
}

TEST_F(HashMapListTest, RemoveElements)
{
	printf("Original size: %u\n", calcSize(hashmap_));
	printf("Removing a couple elements\n");
	printf("New size: %u\n", calcSize(hashmap_));
	hashmap_.remove(5);
	hashmap_.remove(7);
	printHashMap(hashmap_);

	int value = 0;
	ASSERT_FALSE(hashmap_.contains(5, value));
	ASSERT_FALSE(hashmap_.contains(7, value));
	ASSERT_EQ(hashmap_.size(), Size - 2);
	ASSERT_EQ(calcSize(hashmap_), Size - 2);
}

TEST_F(HashMapListTest, RehashExtend)
{
	const float loadFactor = hashmap_.loadFactor();
	printf("Original size: %u, bucket amount: %u, load factor: %f\n", hashmap_.size(), hashmap_.bucketAmount(), hashmap_.loadFactor());
	printHashMap(hashmap_);
	ASSERT_EQ(hashmap_.bucketAmount(), Capacity);

	printf("Doubling capacity by rehashing\n");
	hashmap_.rehash(hashmap_.bucketAmount() * 2);
	printf("New size: %u, bucket amount: %u, load factor: %f\n", hashmap_.size(), hashmap_.bucketAmount(), hashmap_.loadFactor());
	printHashMap(hashmap_);

	ASSERT_EQ(hashmap_.bucketAmount(), Capacity * 2);
	ASSERT_EQ(hashmap_.size(), Size);
	ASSERT_EQ(calcSize(hashmap_), Size);
	ASSERT_FLOAT_EQ(hashmap_.loadFactor(), loadFactor * 0.5f);

	for (unsigned int i = 0; i < Size; i++)
		ASSERT_EQ(hashmap_[i], i + KeyValueDifference);
}

TEST_F(HashMapListTest, RehashShrink)
{
	printf("Original size: %u, bucket amount: %u, load factor: %f\n", hashmap_.size(), hashmap_.bucketAmount(), hashmap_.loadFactor());
	printHashMap(hashmap_);
	ASSERT_EQ(hashmap_.bucketAmount(), Capacity);

	printf("Set capacity to current size by rehashing\n");
	hashmap_.rehash(hashmap_.size());
	printf("New size: %u, bucket amount: %u, load factor: %f\n", hashmap_.size(), hashmap_.bucketAmount(), hashmap_.loadFactor());
	printHashMap(hashmap_);

	ASSERT_EQ(hashmap_.bucketAmount(), Size);
	ASSERT_EQ(hashmap_.size(), Size);
	ASSERT_EQ(calcSize(hashmap_), Size);
	ASSERT_FLOAT_EQ(hashmap_.loadFactor(), 1.0f);

	for (unsigned int i = 0; i < Size; i++)
		ASSERT_EQ(hashmap_[i], i + KeyValueDifference);
}

TEST_F(HashMapListTest, CopyConstruction)
{
	printf("Creating a new hashmap with copy construction\n");
	HashMapTestType newHashmap(hashmap_);
	printHashMap(newHashmap);

	assertHashMapsAreEqual(hashmap_, newHashmap);
	ASSERT_EQ(hashmap_.size(), Size);
	ASSERT_EQ(calcSize(hashmap_), Size);
	ASSERT_EQ(newHashmap.size(), Size);
	ASSERT_EQ(calcSize(newHashmap), Size);
}

TEST_F(HashMapListTest, MoveConstruction)
{
	printf("Creating a new hashmap with move construction\n");
	HashMapTestType newHashmap = nctl::move(hashmap_);
	printHashMap(newHashmap);

	ASSERT_EQ(hashmap_.bucketAmount(), 0);
	ASSERT_EQ(newHashmap.bucketAmount(), Capacity);
	ASSERT_EQ(newHashmap.size(), Size);
	ASSERT_EQ(calcSize(newHashmap), Size);
}

TEST_F(HashMapListTest, AssignmentOperator)
{
	printf("Creating a new hashmap with the assignment operator\n");
	HashMapTestType newHashmap(Capacity);
	newHashmap = hashmap_;
	printHashMap(newHashmap);

	assertHashMapsAreEqual(hashmap_, newHashmap);
	ASSERT_EQ(hashmap_.size(), Size);
	ASSERT_EQ(calcSize(hashmap_), Size);
	ASSERT_EQ(newHashmap.size(), Size);
	ASSERT_EQ(calcSize(newHashmap), Size);
}

TEST_F(HashMapListTest, MoveAssignmentOperator)
{
	printf("Creating a new hashmap with the move assignment operator\n");
	HashMapTestType newHashmap(Capacity);
	newHashmap = nctl::move(hashmap_);
	printHashMap(newHashmap);

	ASSERT_EQ(hashmap_.bucketAmount(), 0);
	ASSERT_EQ(hashmap_.size(), 0);
	ASSERT_EQ(newHashmap.bucketAmount(), Capacity);
	ASSERT_EQ(newHashmap.size(), Size);
	ASSERT_EQ(calcSize(newHashmap), Size);
}

TEST_F(HashMapListTest, SelfAssignment)
{
	printf("Assigning the hashmap to itself with the assignment operator\n");
	hashmap_ = hashmap_;
	printHashMap(hashmap_);

	ASSERT_EQ(hashmap_.size(), Size);
	ASSERT_EQ(calcSize(hashmap_), Size);
}

TEST_F(HashMapListTest, Contains)
{
	const int key = 1;
	int value = 0;
	const bool found = hashmap_.contains(key, value);
	printf("Key %d is in the hashmap: %d - Value: %d\n", key, found, value);

	ASSERT_TRUE(found);
	ASSERT_EQ(value, key + KeyValueDifference);
}

TEST_F(HashMapListTest, DoesNotContain)
{
	const int key = 10;
	int value = 0;
	const bool found = hashmap_.contains(key, value);
	printf("Key %d is in the hashmap: %d - Value: %d\n", key, found, value);

	ASSERT_FALSE(found);
}

TEST_F(HashMapListTest, Find)
{
	const int key = 1;
	const int *value = hashmap_.find(key);
	printf("Key %d is in the hashmap: %d - Value: %d\n", key, value != nullptr, *value);

	ASSERT_TRUE(value != nullptr);
	ASSERT_EQ(*value, key + KeyValueDifference);
}

TEST_F(HashMapListTest, ConstFind)
{
	const HashMapTestType &constHashmap = hashmap_;
	const int key = 1;
	const int *value = constHashmap.find(key);
	printf("Key %d is in the hashmap: %d - Value: %d\n", key, value != nullptr, *value);

	ASSERT_TRUE(value != nullptr);
	ASSERT_EQ(*value, key + KeyValueDifference);
}

TEST_F(HashMapListTest, CannotFind)
{
	const int key = 10;
	const int *value = hashmap_.find(key);
	printf("Key %d is in the hashmap: %d\n", key, value != nullptr);

	ASSERT_FALSE(value != nullptr);
}

}
