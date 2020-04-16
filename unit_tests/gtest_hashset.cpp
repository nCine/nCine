#include "gtest_hashset.h"

namespace {

class HashSetTest : public ::testing::Test
{
  public:
	HashSetTest()
	    : hashset_(Capacity) {}

  protected:
	void SetUp() override { initHashSet(hashset_); }

	HashSetTestType hashset_;
};

#ifndef __EMSCRIPTEN__
TEST(HashSetDeathTest, ZeroCapacity)
{
	printf("Creating an hashset of zero capacity\n");
	ASSERT_DEATH(HashSetTestType newHashset(0), "");
}
#endif

TEST_F(HashSetTest, Capacity)
{
	const unsigned int capacity = hashset_.capacity();
	printf("Capacity: %u\n", capacity);

	ASSERT_EQ(capacity, Capacity);
}

TEST_F(HashSetTest, Size)
{
	const unsigned int size = hashset_.size();
	printf("Size: %u\n", size);

	ASSERT_EQ(size, Size);
	ASSERT_EQ(calcSize(hashset_), Size);
}

TEST_F(HashSetTest, LoadFactor)
{
	const float loadFactor = hashset_.loadFactor();
	printf("Size: %u, Capacity: %u, Load Factor: %f\n", Size, Capacity, loadFactor);

	ASSERT_FLOAT_EQ(loadFactor, Size / static_cast<float>(Capacity));
}

TEST_F(HashSetTest, Clear)
{
	ASSERT_FALSE(hashset_.isEmpty());
	hashset_.clear();
	printHashSet(hashset_);
	ASSERT_TRUE(hashset_.isEmpty());
	ASSERT_EQ(hashset_.size(), 0u);
	ASSERT_EQ(hashset_.capacity(), Capacity);
}

TEST_F(HashSetTest, InsertElements)
{
	printf("Inserting elements\n");
	for (unsigned int i = Size; i < Size * 2; i++)
		hashset_.insert(i);

	for (unsigned int i = 0; i < Size * 2; i++)
		ASSERT_TRUE(hashset_.contains(i));

	ASSERT_EQ(hashset_.size(), Size * 2);
	ASSERT_EQ(calcSize(hashset_), Size * 2);
}

TEST_F(HashSetTest, InsertConstElements)
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

TEST_F(HashSetTest, FailInsertElements)
{
	printf("Trying to insert elements already in the hashset\n");
	for (unsigned int i = 0; i < Size * 2; i++)
		hashset_.insert(i);

	for (unsigned int i = 0; i < Size * 2; i++)
		ASSERT_TRUE(hashset_.contains(i));

	ASSERT_EQ(hashset_.size(), Size * 2);
	ASSERT_EQ(calcSize(hashset_), Size * 2);
}

TEST_F(HashSetTest, FailInsertConstElements)
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

TEST_F(HashSetTest, RemoveElements)
{
	printf("Original size: %u\n", hashset_.size());
	printf("Removing a couple elements\n");
	printf("New size: %u\n", hashset_.size());
	hashset_.remove(5);
	hashset_.remove(7);
	printHashSet(hashset_);

	ASSERT_FALSE(hashset_.contains(5));
	ASSERT_FALSE(hashset_.contains(7));
	ASSERT_EQ(hashset_.size(), Size - 2);
	ASSERT_EQ(calcSize(hashset_), Size - 2);
}

TEST_F(HashSetTest, RehashExtend)
{
	const float loadFactor = hashset_.loadFactor();
	printf("Original size: %u, capacity: %u, load factor: %f\n", hashset_.size(), hashset_.capacity(), hashset_.loadFactor());
	printHashSet(hashset_);
	ASSERT_EQ(hashset_.capacity(), Capacity);

	printf("Doubling capacity by rehashing\n");
	hashset_.rehash(hashset_.capacity() * 2);
	printf("New size: %u, capacity: %u, load factor: %f\n", hashset_.size(), hashset_.capacity(), hashset_.loadFactor());
	printHashSet(hashset_);

	ASSERT_EQ(hashset_.capacity(), Capacity * 2);
	ASSERT_EQ(hashset_.size(), Size);
	ASSERT_EQ(calcSize(hashset_), Size);
	ASSERT_FLOAT_EQ(hashset_.loadFactor(), loadFactor * 0.5f);

	for (unsigned int i = 0; i < Size; i++)
		ASSERT_TRUE(hashset_.contains(i));
}

TEST_F(HashSetTest, RehashShrink)
{
	printf("Original size: %u, capacity: %u, load factor: %f\n", hashset_.size(), hashset_.capacity(), hashset_.loadFactor());
	printHashSet(hashset_);
	ASSERT_EQ(hashset_.capacity(), Capacity);

	printf("Set capacity to current size by rehashing\n");
	hashset_.rehash(hashset_.size());
	printf("New size: %u, capacity: %u, load factor: %f\n", hashset_.size(), hashset_.capacity(), hashset_.loadFactor());
	printHashSet(hashset_);

	ASSERT_EQ(hashset_.capacity(), Size);
	ASSERT_EQ(hashset_.size(), Size);
	ASSERT_EQ(calcSize(hashset_), Size);
	ASSERT_FLOAT_EQ(hashset_.loadFactor(), 1.0f);

	for (unsigned int i = 0; i < Size; i++)
		ASSERT_TRUE(hashset_.contains(i));
}

TEST_F(HashSetTest, CopyConstruction)
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

TEST_F(HashSetTest, MoveConstruction)
{
	printf("Creating a new hashset with move construction\n");
	HashSetTestType newHashset = nctl::move(hashset_);
	printHashSet(newHashset);

	ASSERT_EQ(hashset_.size(), 0);
	ASSERT_EQ(newHashset.capacity(), Capacity);
	ASSERT_EQ(newHashset.size(), Size);
	ASSERT_EQ(calcSize(newHashset), Size);
}

TEST_F(HashSetTest, AssignmentOperator)
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

TEST_F(HashSetTest, MoveAssignmentOperator)
{
	printf("Creating a new hashset with the move assignment operator\n");
	HashSetTestType newHashset(Capacity);
	newHashset = nctl::move(hashset_);
	printHashSet(newHashset);

	ASSERT_EQ(hashset_.size(), 0);
	ASSERT_EQ(newHashset.capacity(), Capacity);
	ASSERT_EQ(newHashset.size(), Size);
	ASSERT_EQ(calcSize(newHashset), Size);
}

TEST_F(HashSetTest, SelfAssignment)
{
	printf("Assigning the hashset to itself with the assignment operator\n");
	hashset_ = hashset_;
	printHashSet(hashset_);

	ASSERT_EQ(hashset_.size(), Size);
	ASSERT_EQ(calcSize(hashset_), Size);
}

TEST_F(HashSetTest, Contains)
{
	const int key = 1;
	const bool found = hashset_.contains(key);
	printf("Key %d is in the hashset: %d\n", key, found);

	ASSERT_TRUE(found);
}

TEST_F(HashSetTest, DoesNotContain)
{
	const int key = 10;
	const bool found = hashset_.contains(key);
	printf("Key %d is in the hashset: %d\n", key, found);

	ASSERT_FALSE(found);
}

TEST_F(HashSetTest, Find)
{
	const int key = 1;
	const int *value = hashset_.find(key);
	printf("Key %d is in the hashset: %d - Value: %d\n", key, value != nullptr, *value);

	ASSERT_TRUE(value != nullptr);
	ASSERT_EQ(*value, key);
}

TEST_F(HashSetTest, ConstFind)
{
	const HashSetTestType &constHashset = hashset_;
	const int key = 1;
	const int *value = constHashset.find(key);
	printf("Key %d is in the hashset: %d - Value: %d\n", key, value != nullptr, *value);

	ASSERT_TRUE(value != nullptr);
	ASSERT_EQ(*value, key);
}

TEST_F(HashSetTest, CannotFind)
{
	const int key = 10;
	const int *value = hashset_.find(key);
	printf("Key %d is in the hashset: %d\n", key, value != nullptr);

	ASSERT_FALSE(value != nullptr);
}

TEST_F(HashSetTest, FillCapacity)
{
	printf("Creating a new hashset to fill up to capacity (%u elements)\n", Capacity);
	HashSetTestType newHashset(Capacity);

	for (unsigned int i = 0; i < Capacity; i++)
		newHashset.insert(i);

	ASSERT_EQ(newHashset.size(), Capacity);
	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_TRUE(newHashset.contains(i));
}

TEST_F(HashSetTest, RemoveAllFromFull)
{
	printf("Creating a new hashset to fill up to capacity (%u elements)\n", Capacity);
	HashSetTestType newHashset(Capacity);

	for (unsigned int i = 0; i < Capacity; i++)
		newHashset.insert(i);

	printf("Removing all elements from the hashset\n");
	for (unsigned int i = 0; i < Capacity; i++)
		newHashset.remove(i);

	ASSERT_EQ(newHashset.size(), 0);
	ASSERT_EQ(calcSize(newHashset), 0);
}

const int BigCapacity = 512;
const int LastElement = BigCapacity / 2;

TEST_F(HashSetTest, StressRemove)
{
	printf("Creating a new hashset with a capacity of %u and filled up to %u elements\n", BigCapacity, LastElement);
	HashSetTestType newHashset(BigCapacity);

	for (int i = 0; i < LastElement; i++)
		newHashset.insert(i);
	ASSERT_EQ(newHashset.size(), LastElement);

	printf("Removing all elements from the hashset\n");
	for (int i = 0; i < LastElement; i++)
	{
		newHashset.remove(i);
		ASSERT_EQ(newHashset.size(), LastElement - i - 1);

		for (int j = i + 1; j < LastElement; j++)
			ASSERT_TRUE(newHashset.contains(j));
		for (int j = 0; j < i + 1; j++)
			ASSERT_FALSE(newHashset.contains(j));
	}

	ASSERT_EQ(newHashset.size(), 0);
}

TEST_F(HashSetTest, StressReverseRemove)
{
	printf("Creating a new hashset with a capacity of %u and filled up to %u elements\n", BigCapacity, LastElement);
	HashSetTestType newHashset(BigCapacity);

	for (int i = 0; i < LastElement; i++)
		newHashset.insert(i);
	ASSERT_EQ(newHashset.size(), LastElement);

	printf("Removing all elements from the hashset\n");
	for (int i = LastElement - 1; i >= 0; i--)
	{
		newHashset.remove(i);
		ASSERT_EQ(newHashset.size(), i);

		for (int j = i - 1; j >= 0; j--)
			ASSERT_TRUE(newHashset.contains(j));
		for (int j = LastElement; j >= i; j--)
			ASSERT_FALSE(newHashset.contains(j));
	}

	ASSERT_EQ(newHashset.size(), 0);
}

}
