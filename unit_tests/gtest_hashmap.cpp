#include "gtest_hashmap.h"

namespace {

class HashMapTest : public ::testing::Test
{
  public:
	HashMapTest()
	    : hashmap_(Capacity) {}

  protected:
	void SetUp() override { initHashMap(hashmap_); }

	HashMapTestType hashmap_;
};

#ifndef __EMSCRIPTEN__
TEST(HashMapDeathTest, ZeroCapacity)
{
	printf("Creating an hashmap of zero capacity\n");
	ASSERT_DEATH(HashMapTestType newHashmap(0), "");
}
#endif

TEST_F(HashMapTest, Capacity)
{
	const unsigned int capacity = hashmap_.capacity();
	printf("Capacity: %u\n", capacity);

	ASSERT_EQ(capacity, Capacity);
}

TEST_F(HashMapTest, Size)
{
	const unsigned int size = hashmap_.size();
	printf("Size: %u\n", size);

	ASSERT_EQ(size, Size);
	ASSERT_EQ(calcSize(hashmap_), Size);
}

TEST_F(HashMapTest, LoadFactor)
{
	const float loadFactor = hashmap_.loadFactor();
	printf("Size: %u, Capacity: %u, Load Factor: %f\n", Size, Capacity, loadFactor);

	ASSERT_FLOAT_EQ(loadFactor, Size / static_cast<float>(Capacity));
}

TEST_F(HashMapTest, Clear)
{
	ASSERT_FALSE(hashmap_.isEmpty());
	hashmap_.clear();
	printHashMap(hashmap_);
	ASSERT_TRUE(hashmap_.isEmpty());
	ASSERT_EQ(hashmap_.size(), 0u);
	ASSERT_EQ(hashmap_.capacity(), Capacity);
}

TEST_F(HashMapTest, RetrieveElements)
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

TEST_F(HashMapTest, InsertElements)
{
	printf("Inserting elements\n");
	for (unsigned int i = Size; i < Size * 2; i++)
		hashmap_.insert(i, i + KeyValueDifference);

	for (unsigned int i = 0; i < Size * 2; i++)
		ASSERT_EQ(hashmap_[i], i + KeyValueDifference);

	ASSERT_EQ(hashmap_.size(), Size * 2);
	ASSERT_EQ(calcSize(hashmap_), Size * 2);
}

TEST_F(HashMapTest, InsertConstElements)
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

TEST_F(HashMapTest, FailInsertElements)
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

TEST_F(HashMapTest, FailInsertConstElements)
{
	printf("Trying to insert const elements already in the hashmap\n");
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

TEST_F(HashMapTest, EmplaceElements)
{
	printf("Emplacing elements\n");
	for (unsigned int i = Size; i < Size * 2; i++)
		hashmap_.emplace(i, i + KeyValueDifference);

	for (unsigned int i = 0; i < Size * 2; i++)
		ASSERT_EQ(hashmap_[i], i + KeyValueDifference);

	ASSERT_EQ(hashmap_.size(), Size * 2);
	ASSERT_EQ(calcSize(hashmap_), Size * 2);
}

TEST_F(HashMapTest, FailEmplaceElements)
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

TEST_F(HashMapTest, RemoveElements)
{
	printf("Original size: %u\n", hashmap_.size());
	printf("Removing a couple elements\n");
	printf("New size: %u\n", hashmap_.size());
	hashmap_.remove(5);
	hashmap_.remove(7);
	printHashMap(hashmap_);

	int value = 0;
	ASSERT_FALSE(hashmap_.contains(5, value));
	ASSERT_FALSE(hashmap_.contains(7, value));
	ASSERT_EQ(hashmap_.size(), Size - 2);
	ASSERT_EQ(calcSize(hashmap_), Size - 2);
}

TEST_F(HashMapTest, RehashExtend)
{
	const float loadFactor = hashmap_.loadFactor();
	printf("Original size: %u, capacity: %u, load factor: %f\n", hashmap_.size(), hashmap_.capacity(), hashmap_.loadFactor());
	printHashMap(hashmap_);
	ASSERT_EQ(hashmap_.capacity(), Capacity);

	printf("Doubling capacity by rehashing\n");
	hashmap_.rehash(hashmap_.capacity() * 2);
	printf("New size: %u, capacity: %u, load factor: %f\n", hashmap_.size(), hashmap_.capacity(), hashmap_.loadFactor());
	printHashMap(hashmap_);

	ASSERT_EQ(hashmap_.capacity(), Capacity * 2);
	ASSERT_EQ(hashmap_.size(), Size);
	ASSERT_EQ(calcSize(hashmap_), Size);
	ASSERT_FLOAT_EQ(hashmap_.loadFactor(), loadFactor * 0.5f);

	for (unsigned int i = 0; i < Size; i++)
		ASSERT_EQ(hashmap_[i], i + KeyValueDifference);
}

TEST_F(HashMapTest, RehashShrink)
{
	printf("Original size: %u, capacity: %u, load factor: %f\n", hashmap_.size(), hashmap_.capacity(), hashmap_.loadFactor());
	printHashMap(hashmap_);
	ASSERT_EQ(hashmap_.capacity(), Capacity);

	printf("Set capacity to current size by rehashing\n");
	hashmap_.rehash(hashmap_.size());
	printf("New size: %u, capacity: %u, load factor: %f\n", hashmap_.size(), hashmap_.capacity(), hashmap_.loadFactor());
	printHashMap(hashmap_);

	ASSERT_EQ(hashmap_.capacity(), Size);
	ASSERT_EQ(hashmap_.size(), Size);
	ASSERT_EQ(calcSize(hashmap_), Size);
	ASSERT_FLOAT_EQ(hashmap_.loadFactor(), 1.0f);

	for (unsigned int i = 0; i < Size; i++)
		ASSERT_EQ(hashmap_[i], i + KeyValueDifference);
}

TEST_F(HashMapTest, CopyConstruction)
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

TEST_F(HashMapTest, MoveConstruction)
{
	printf("Creating a new hashmap with move construction\n");
	HashMapTestType newHashmap = nctl::move(hashmap_);
	printHashMap(newHashmap);

	ASSERT_EQ(hashmap_.size(), 0);
	ASSERT_EQ(newHashmap.capacity(), Capacity);
	ASSERT_EQ(newHashmap.size(), Size);
	ASSERT_EQ(calcSize(newHashmap), Size);
}

TEST_F(HashMapTest, AssignmentOperator)
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

TEST_F(HashMapTest, MoveAssignmentOperator)
{
	printf("Creating a new hashmap with the move assignment operator\n");
	HashMapTestType newHashmap(Capacity);
	newHashmap = nctl::move(hashmap_);
	printHashMap(newHashmap);

	ASSERT_EQ(hashmap_.size(), 0);
	ASSERT_EQ(newHashmap.capacity(), Capacity);
	ASSERT_EQ(newHashmap.size(), Size);
	ASSERT_EQ(calcSize(newHashmap), Size);
}

TEST_F(HashMapTest, SelfAssignment)
{
	printf("Assigning the hashmap to itself with the assignment operator\n");
	hashmap_ = hashmap_;
	printHashMap(hashmap_);

	ASSERT_EQ(hashmap_.size(), Size);
	ASSERT_EQ(calcSize(hashmap_), Size);
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

TEST_F(HashMapTest, Find)
{
	const int key = 1;
	const int *value = hashmap_.find(key);
	printf("Key %d is in the hashmap: %d - Value: %d\n", key, value != nullptr, *value);

	ASSERT_TRUE(value != nullptr);
	ASSERT_EQ(*value, key + KeyValueDifference);
}

TEST_F(HashMapTest, ConstFind)
{
	const HashMapTestType &constHashmap = hashmap_;
	const int key = 1;
	const int *value = constHashmap.find(key);
	printf("Key %d is in the hashmap: %d - Value: %d\n", key, value != nullptr, *value);

	ASSERT_TRUE(value != nullptr);
	ASSERT_EQ(*value, key + KeyValueDifference);
}

TEST_F(HashMapTest, CannotFind)
{
	const int key = 10;
	const int *value = hashmap_.find(key);
	printf("Key %d is in the hashmap: %d\n", key, value != nullptr);

	ASSERT_FALSE(value != nullptr);
}

TEST_F(HashMapTest, FillCapacity)
{
	printf("Creating a new hashmap to fill up to capacity (%u elements)\n", Capacity);
	HashMapTestType newHashmap(Capacity);

	for (unsigned int i = 0; i < Capacity; i++)
		newHashmap[i] = i + KeyValueDifference;

	ASSERT_EQ(newHashmap.size(), Capacity);
	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_EQ(newHashmap[i], i + KeyValueDifference);
}

TEST_F(HashMapTest, RemoveAllFromFull)
{
	printf("Creating a new hashmap to fill up to capacity (%u elements)\n", Capacity);
	HashMapTestType newHashmap(Capacity);

	for (unsigned int i = 0; i < Capacity; i++)
		newHashmap[i] = i + KeyValueDifference;

	printf("Removing all elements from the hashmap\n");
	for (unsigned int i = 0; i < Capacity; i++)
		newHashmap.remove(i);

	ASSERT_EQ(newHashmap.size(), 0);
	ASSERT_EQ(calcSize(newHashmap), 0);
}

const int BigCapacity = 512;
const int LastElement = BigCapacity / 2;

TEST_F(HashMapTest, StressRemove)
{
	printf("Creating a new hashmap with a capacity of %u and filled up to %u elements\n", BigCapacity, LastElement);
	HashMapTestType newHashmap(BigCapacity);

	for (int i = 0; i < LastElement; i++)
		newHashmap[i] = i + KeyValueDifference;
	ASSERT_EQ(newHashmap.size(), LastElement);

	printf("Removing all elements from the hashmap\n");
	for (int i = 0; i < LastElement; i++)
	{
		newHashmap.remove(i);
		ASSERT_EQ(newHashmap.size(), LastElement - i - 1);

		int value = 0;
		for (int j = i + 1; j < LastElement; j++)
			ASSERT_TRUE(newHashmap.contains(j, value));
		for (int j = 0; j < i + 1; j++)
			ASSERT_FALSE(newHashmap.contains(j, value));
	}

	ASSERT_EQ(newHashmap.size(), 0);
}

TEST_F(HashMapTest, StressReverseRemove)
{
	printf("Creating a new hashmap with a capacity of %u and filled up to %u elements\n", BigCapacity, LastElement);
	HashMapTestType newHashmap(BigCapacity);

	for (int i = 0; i < LastElement; i++)
		newHashmap[i] = i + KeyValueDifference;
	ASSERT_EQ(newHashmap.size(), LastElement);

	printf("Removing all elements from the hashmap\n");
	for (int i = LastElement - 1; i >= 0; i--)
	{
		newHashmap.remove(i);
		ASSERT_EQ(newHashmap.size(), i);

		int value = 0;
		for (int j = i - 1; j >= 0; j--)
			ASSERT_TRUE(newHashmap.contains(j, value));
		for (int j = LastElement; j >= i; j--)
			ASSERT_FALSE(newHashmap.contains(j, value));
	}

	ASSERT_EQ(newHashmap.size(), 0);
}

}
