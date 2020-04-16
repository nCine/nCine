#include "gtest_hashsetlist.h"

namespace {

class HashSetListTest : public ::testing::Test
{
  public:
	HashSetListTest()
	    : hashset_(Capacity) {}

  protected:
	void SetUp() override { initHashSet(hashset_); }

	HashSetTestType hashset_;
};

#ifndef __EMSCRIPTEN__
TEST(HashSetListDeathTest, ZeroCapacity)
{
	printf("Creating an hashset of zero capacity\n");
	ASSERT_DEATH(HashSetTestType newHashset(0), "");
}
#endif

TEST_F(HashSetListTest, Size)
{
	const unsigned int size = hashset_.size();
	printf("Size: %u\n", size);

	ASSERT_EQ(size, Size);
	ASSERT_EQ(calcSize(hashset_), Size);
}

TEST_F(HashSetListTest, LoadFactor)
{
	const float loadFactor = hashset_.loadFactor();
	printf("Size: %u, Capacity: %u, Load Factor: %f\n", Size, Capacity, loadFactor);

	ASSERT_FLOAT_EQ(loadFactor, Size / static_cast<float>(Capacity));
}

TEST_F(HashSetListTest, BucketAmount)
{
	const unsigned int numBuckets = hashset_.bucketAmount();
	printf("Number of buckets: %u\n", numBuckets);

	ASSERT_EQ(numBuckets, Capacity);
}

TEST_F(HashSetListTest, BucketSize)
{
	const int key = 0;
	const unsigned int bucketSize = hashset_.bucketSize(key);
	printf("Bucket size for key %d: %u\n", key, bucketSize);

	ASSERT_EQ(bucketSize, Size);
}

TEST_F(HashSetListTest, BucketIndex)
{
	for (unsigned int i = 0; i < Size; i++)
	{
		const unsigned int bucketIndex = hashset_.bucket(i);
		printf("Bucket index for key %u and hash %u: %u\n", i, hashset_.hash(i), bucketIndex);

		ASSERT_EQ(bucketIndex, hashset_.hash(i) % Capacity);
	}
}

TEST_F(HashSetListTest, Clear)
{
	ASSERT_FALSE(hashset_.isEmpty());
	hashset_.clear();
	printHashSet(hashset_);
	ASSERT_TRUE(hashset_.isEmpty());
	ASSERT_EQ(hashset_.size(), 0u);
	ASSERT_EQ(hashset_.bucketAmount(), Capacity);
}

TEST_F(HashSetListTest, InsertElements)
{
	printf("Inserting elements\n");
	for (unsigned int i = Size; i < Size * 2; i++)
		hashset_.insert(i);

	for (unsigned int i = 0; i < Size * 2; i++)
		ASSERT_TRUE(hashset_.contains(i));

	ASSERT_EQ(hashset_.size(), Size * 2);
	ASSERT_EQ(calcSize(hashset_), Size * 2);
}

TEST_F(HashSetListTest, InsertConstElements)
{
	printf("Inserting const elements\n");
	for (unsigned int i = Size; i < Size * 2; i++)
	{
		const int key = i;
		hashset_.insert(key);
	}

	for (unsigned int i = 0; i < Size * 2; i++)
		ASSERT_TRUE(hashset_.contains(i));

	ASSERT_EQ(hashset_.size(), Size * 2);
	ASSERT_EQ(calcSize(hashset_), Size * 2);
}

TEST_F(HashSetListTest, FailInsertElements)
{
	printf("Trying to insert elements already in the hashset\n");
	for (unsigned int i = 0; i < Size * 2; i++)
		hashset_.insert(i);

	for (unsigned int i = 0; i < Size * 2; i++)
		ASSERT_TRUE(hashset_.contains(i));

	ASSERT_EQ(hashset_.size(), Size * 2);
	ASSERT_EQ(calcSize(hashset_), Size * 2);
}

TEST_F(HashSetListTest, FailInsertConstElements)
{
	printf("Trying to insert const elements already in the hashset\n");
	for (unsigned int i = 0; i < Size * 2; i++)
	{
		const int key = i;
		hashset_.insert(key);
	}

	for (unsigned int i = 0; i < Size * 2; i++)
		ASSERT_TRUE(hashset_.contains(i));

	ASSERT_EQ(hashset_.size(), Size * 2);
	ASSERT_EQ(calcSize(hashset_), Size * 2);
}

TEST_F(HashSetListTest, RemoveElements)
{
	printf("Original size: %u\n", calcSize(hashset_));
	printf("Removing a couple elements\n");
	printf("New size: %u\n", calcSize(hashset_));
	hashset_.remove(5);
	hashset_.remove(7);
	printHashSet(hashset_);

	ASSERT_FALSE(hashset_.contains(5));
	ASSERT_FALSE(hashset_.contains(7));
	ASSERT_EQ(hashset_.size(), Size - 2);
	ASSERT_EQ(calcSize(hashset_), Size - 2);
}

TEST_F(HashSetListTest, RehashExtend)
{
	const float loadFactor = hashset_.loadFactor();
	printf("Original size: %u, bucket amount: %u, load factor: %f\n", hashset_.size(), hashset_.bucketAmount(), hashset_.loadFactor());
	printHashSet(hashset_);
	ASSERT_EQ(hashset_.bucketAmount(), Capacity);

	printf("Doubling capacity by rehashing\n");
	hashset_.rehash(hashset_.bucketAmount() * 2);
	printf("New size: %u, bucket amount: %u, load factor: %f\n", hashset_.size(), hashset_.bucketAmount(), hashset_.loadFactor());
	printHashSet(hashset_);

	ASSERT_EQ(hashset_.bucketAmount(), Capacity * 2);
	ASSERT_EQ(hashset_.size(), Size);
	ASSERT_EQ(calcSize(hashset_), Size);
	ASSERT_FLOAT_EQ(hashset_.loadFactor(), loadFactor * 0.5f);

	for (unsigned int i = 0; i < Size; i++)
		ASSERT_TRUE(hashset_.contains(i));
}

TEST_F(HashSetListTest, RehashShrink)
{
	printf("Original size: %u, bucket amount: %u, load factor: %f\n", hashset_.size(), hashset_.bucketAmount(), hashset_.loadFactor());
	printHashSet(hashset_);
	ASSERT_EQ(hashset_.bucketAmount(), Capacity);

	printf("Set capacity to current size by rehashing\n");
	hashset_.rehash(hashset_.size());
	printf("New size: %u, bucket amount: %u, load factor: %f\n", hashset_.size(), hashset_.bucketAmount(), hashset_.loadFactor());
	printHashSet(hashset_);

	ASSERT_EQ(hashset_.bucketAmount(), Size);
	ASSERT_EQ(hashset_.size(), Size);
	ASSERT_EQ(calcSize(hashset_), Size);
	ASSERT_FLOAT_EQ(hashset_.loadFactor(), 1.0f);

	for (unsigned int i = 0; i < Size; i++)
		ASSERT_TRUE(hashset_.contains(i));
}

TEST_F(HashSetListTest, CopyConstruction)
{
	printf("Creating a new hashset with copy construction\n");
	HashSetTestType newHashset(hashset_);
	printHashSet(newHashset);

	assertHashSetsAreEqual(hashset_, newHashset);
	ASSERT_EQ(hashset_.size(), Size);
	ASSERT_EQ(calcSize(hashset_), Size);
	ASSERT_EQ(newHashset.size(), Size);
	ASSERT_EQ(calcSize(newHashset), Size);
}

TEST_F(HashSetListTest, MoveConstruction)
{
	printf("Creating a new hashset with move construction\n");
	HashSetTestType newHashset = nctl::move(hashset_);
	printHashSet(newHashset);

	ASSERT_EQ(hashset_.bucketAmount(), 0);
	ASSERT_EQ(newHashset.bucketAmount(), Capacity);
	ASSERT_EQ(newHashset.size(), Size);
	ASSERT_EQ(calcSize(newHashset), Size);
}

TEST_F(HashSetListTest, AssignmentOperator)
{
	printf("Creating a new hashset with the assignment operator\n");
	HashSetTestType newHashset(Capacity);
	newHashset = hashset_;
	printHashSet(newHashset);

	assertHashSetsAreEqual(hashset_, newHashset);
	ASSERT_EQ(hashset_.size(), Size);
	ASSERT_EQ(calcSize(hashset_), Size);
	ASSERT_EQ(newHashset.size(), Size);
	ASSERT_EQ(calcSize(newHashset), Size);
}

TEST_F(HashSetListTest, MoveAssignmentOperator)
{
	printf("Creating a new hashset with the move assignment operator\n");
	HashSetTestType newHashset(Capacity);
	newHashset = nctl::move(hashset_);
	printHashSet(newHashset);

	ASSERT_EQ(hashset_.bucketAmount(), 0);
	ASSERT_EQ(hashset_.size(), 0);
	ASSERT_EQ(newHashset.bucketAmount(), Capacity);
	ASSERT_EQ(newHashset.size(), Size);
	ASSERT_EQ(calcSize(newHashset), Size);
}

TEST_F(HashSetListTest, SelfAssignment)
{
	printf("Assigning the hashset to itself with the assignment operator\n");
	hashset_ = hashset_;
	printHashSet(hashset_);

	ASSERT_EQ(hashset_.size(), Size);
	ASSERT_EQ(calcSize(hashset_), Size);
}

TEST_F(HashSetListTest, Contains)
{
	const int key = 1;
	const bool found = hashset_.contains(key);
	printf("Key %d is in the hashset: %d\n", key, found);

	ASSERT_TRUE(found);
}

TEST_F(HashSetListTest, DoesNotContain)
{
	const int key = 10;
	const bool found = hashset_.contains(key);
	printf("Key %d is in the hashset: %d\n", key, found);

	ASSERT_FALSE(found);
}

TEST_F(HashSetListTest, Find)
{
	const int key = 1;
	const int *value = hashset_.find(key);
	printf("Key %d is in the hashset: %d - Value: %d\n", key, value != nullptr, *value);

	ASSERT_TRUE(value != nullptr);
	ASSERT_EQ(*value, key);
}

TEST_F(HashSetListTest, ConstFind)
{
	const HashSetTestType &constHashset = hashset_;
	const int key = 1;
	const int *value = constHashset.find(key);
	printf("Key %d is in the hashset: %d - Value: %d\n", key, value != nullptr, *value);

	ASSERT_TRUE(value != nullptr);
	ASSERT_EQ(*value, key);
}

TEST_F(HashSetListTest, CannotFind)
{
	const int key = 10;
	const int *value = hashset_.find(key);
	printf("Key %d is in the hashset: %d\n", key, value != nullptr);

	ASSERT_FALSE(value != nullptr);
}

}
