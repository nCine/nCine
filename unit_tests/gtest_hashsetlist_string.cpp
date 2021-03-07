#include "gtest_hashsetlist_string.h"

namespace {

class HashSetListStringTest : public ::testing::Test
{
  public:
	HashSetListStringTest()
	    : strHashset_(Capacity) {}

  protected:
	void SetUp() override { initHashSet(strHashset_); }

	nctl::HashSetList<nctl::String> strHashset_;
};

TEST_F(HashSetListStringTest, BucketAmount)
{
	const unsigned int numBuckets = strHashset_.bucketAmount();
	printf("Number of buckets: %u\n", numBuckets);

	ASSERT_EQ(numBuckets, Capacity);
}

TEST_F(HashSetListStringTest, BucketSize)
{
	const unsigned int bucketSize = strHashset_.bucketSize(Keys[0]);
	printf("Bucket size for key %s: %u\n", Keys[0], bucketSize);

	ASSERT_EQ(bucketSize, 3u);
}

TEST_F(HashSetListStringTest, InsertElements)
{
	printf("Inserting elements\n");
	nctl::String newKey(32);
	for (unsigned int i = Size; i < Size * 2; i++)
	{
		newKey.format("%s_2", Keys[i % Size]);
		strHashset_.insert(newKey);
	}

	for (unsigned int i = 0; i < Size; i++)
		ASSERT_TRUE(strHashset_.contains(Keys[i]));
	for (unsigned int i = Size; i < Size * 2; i++)
	{
		newKey.format("%s_2", Keys[i % Size]);
		ASSERT_TRUE(strHashset_.contains(newKey));
	}

	ASSERT_EQ(strHashset_.size(), Size * 2);
	ASSERT_EQ(calcSize(strHashset_), Size * 2);
}

TEST_F(HashSetListStringTest, RemoveElements)
{
	printf("Removing a couple elements\n");
	strHashset_.remove(Keys[0]);
	strHashset_.remove(Keys[3]);
	printHashSet(strHashset_);

	ASSERT_FALSE(strHashset_.contains(Keys[0]));
	ASSERT_FALSE(strHashset_.contains(Keys[3]));
}

TEST_F(HashSetListStringTest, CopyConstruction)
{
	printf("Creating a new hashset with copy construction\n");
	nctl::HashSetList<nctl::String> newStrHashset(strHashset_);
	printHashSet(newStrHashset);

	assertHashSetsAreEqual(strHashset_, newStrHashset);
}

TEST_F(HashSetListStringTest, MoveConstruction)
{
	printf("Creating a new hashset with move construction\n");
	nctl::HashSetList<nctl::String> newStrHashset = nctl::move(strHashset_);
	printHashSet(newStrHashset);

	ASSERT_EQ(strHashset_.bucketAmount(), 0);
	ASSERT_EQ(newStrHashset.bucketAmount(), Capacity);
	ASSERT_EQ(calcSize(newStrHashset), Size);
}

TEST_F(HashSetListStringTest, AssignmentOperator)
{
	printf("Creating a new hashset with the assignment operator\n");
	nctl::HashSetList<nctl::String> newStrHashset(Capacity);
	newStrHashset = strHashset_;
	printHashSet(newStrHashset);

	assertHashSetsAreEqual(strHashset_, newStrHashset);
}

TEST_F(HashSetListStringTest, MoveAssignmentOperator)
{
	printf("Creating a new hashset with the move assignment operator\n");
	nctl::HashSetList<nctl::String> newStrHashset(Capacity);
	newStrHashset = nctl::move(strHashset_);
	printHashSet(newStrHashset);

	ASSERT_EQ(strHashset_.bucketAmount(), 0);
	ASSERT_EQ(newStrHashset.bucketAmount(), Capacity);
	ASSERT_EQ(calcSize(newStrHashset), Size);
}

TEST_F(HashSetListStringTest, Contains)
{
	const bool found = strHashset_.contains(Keys[0]);
	printf("Key %s is in the hashset: %d\n", Keys[0], found);

	ASSERT_TRUE(found);
}

TEST_F(HashSetListStringTest, DoesNotContain)
{
	const char *key = "Z";
	const bool found = strHashset_.contains(key);
	printf("Key %s is in the hashset: %d\n", key, found);

	ASSERT_FALSE(found);
}

TEST_F(HashSetListStringTest, Find)
{
	const nctl::String *value = strHashset_.find(Keys[0]);
	printf("Key %s is in the hashset: %d\n", Keys[0], value != nullptr);

	ASSERT_TRUE(value != nullptr);
}

TEST_F(HashSetListStringTest, CannotFind)
{
	const char *key = "Z";
	const nctl::String *value = strHashset_.find(key);
	printf("Key %s is in the hashset: %d\n", key, value != nullptr);

	ASSERT_FALSE(value != nullptr);
}

}
