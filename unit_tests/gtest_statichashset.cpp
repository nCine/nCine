#include "gtest_statichashset.h"

namespace {

class StaticHashSetTest : public ::testing::Test
{
  protected:
	void SetUp() override { initHashSet(hashset_); }

	HashSetTestType hashset_;
};

TEST_F(StaticHashSetTest, Capacity)
{
	const unsigned int capacity = hashset_.capacity();
	printf("Capacity: %u\n", capacity);

	ASSERT_EQ(capacity, Capacity);
}

TEST_F(StaticHashSetTest, Size)
{
	const unsigned int size = hashset_.size();
	printf("Size: %u\n", size);

	ASSERT_EQ(size, Size);
	ASSERT_EQ(calcSize(hashset_), Size);
}

TEST_F(StaticHashSetTest, LoadFactor)
{
	const float loadFactor = hashset_.loadFactor();
	printf("Size: %u, Capacity: %u, Load Factor: %f\n", Size, Capacity, loadFactor);

	ASSERT_FLOAT_EQ(loadFactor, Size / static_cast<float>(Capacity));
}

TEST_F(StaticHashSetTest, Clear)
{
	ASSERT_FALSE(hashset_.isEmpty());
	hashset_.clear();
	printHashSet(hashset_);
	ASSERT_TRUE(hashset_.isEmpty());
	ASSERT_EQ(hashset_.size(), 0u);
	ASSERT_EQ(hashset_.capacity(), Capacity);
}

TEST_F(StaticHashSetTest, InsertElements)
{
	printf("Inserting elements\n");
	for (unsigned int i = Size; i < Size * 2; i++)
		hashset_.insert(i);

	for (unsigned int i = 0; i < Size * 2; i++)
		ASSERT_TRUE(hashset_.contains(i));

	ASSERT_EQ(hashset_.size(), Size * 2);
	ASSERT_EQ(calcSize(hashset_), Size * 2);
}

TEST_F(StaticHashSetTest, FailInsertElements)
{
	printf("Trying to insert elements already in the hashset\n");
	for (unsigned int i = 0; i < Size * 2; i++)
		hashset_.insert(i);

	for (unsigned int i = 0; i < Size * 2; i++)
		ASSERT_TRUE(hashset_.contains(i));

	ASSERT_EQ(hashset_.size(), Size * 2);
	ASSERT_EQ(calcSize(hashset_), Size * 2);
}

TEST_F(StaticHashSetTest, RemoveElements)
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

TEST_F(StaticHashSetTest, CopyConstruction)
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

TEST_F(StaticHashSetTest, MoveConstruction)
{
	printf("Creating a new hashset with move construction\n");
	HashSetTestType newHashset = nctl::move(hashset_);
	printHashSet(newHashset);

	ASSERT_EQ(hashset_.size(), 0);
	ASSERT_EQ(newHashset.capacity(), Capacity);
	ASSERT_EQ(newHashset.size(), Size);
	ASSERT_EQ(calcSize(newHashset), Size);
}

TEST_F(StaticHashSetTest, AssignmentOperator)
{
	printf("Creating a new hashset with the assignment operator\n");
	HashSetTestType newHashset;
	newHashset = hashset_;
	printHashSet(newHashset);

	assertHashSetsAreEqual(hashset_, newHashset);
	ASSERT_EQ(hashset_.size(), Size);
	ASSERT_EQ(calcSize(hashset_), Size);
	ASSERT_EQ(newHashset.size(), Size);
	ASSERT_EQ(calcSize(newHashset), Size);
}

TEST_F(StaticHashSetTest, MoveAssignmentOperator)
{
	printf("Creating a new hashset with the move assignment operator\n");
	HashSetTestType newHashset;
	newHashset = nctl::move(hashset_);
	printHashSet(newHashset);

	ASSERT_EQ(hashset_.size(), 0);
	ASSERT_EQ(newHashset.capacity(), Capacity);
	ASSERT_EQ(newHashset.size(), Size);
	ASSERT_EQ(calcSize(newHashset), Size);
}

TEST_F(StaticHashSetTest, SelfAssignment)
{
	printf("Assigning the hashset to itself with the assignment operator\n");
	hashset_ = hashset_;
	printHashSet(hashset_);

	ASSERT_EQ(hashset_.size(), Size);
	ASSERT_EQ(calcSize(hashset_), Size);
}

TEST_F(StaticHashSetTest, Contains)
{
	const int key = 1;
	const bool found = hashset_.contains(key);
	printf("Key %d is in the hashset: %d\n", key, found);

	ASSERT_TRUE(found);
}

TEST_F(StaticHashSetTest, DoesNotContain)
{
	const int key = 10;
	const bool found = hashset_.contains(key);
	printf("Key %d is in the hashset: %d\n", key, found);

	ASSERT_FALSE(found);
}

TEST_F(StaticHashSetTest, Find)
{
	const int key = 1;
	const int *value = hashset_.find(key);
	printf("Key %d is in the hashset: %d - Value: %d\n", key, value != nullptr, *value);

	ASSERT_TRUE(value != nullptr);
	ASSERT_EQ(*value, key);
}

TEST_F(StaticHashSetTest, ConstFind)
{
	const HashSetTestType &constHashset = hashset_;
	const int key = 1;
	const int *value = constHashset.find(key);
	printf("Key %d is in the hashset: %d - Value: %d\n", key, value != nullptr, *value);

	ASSERT_TRUE(value != nullptr);
	ASSERT_EQ(*value, key);
}

TEST_F(StaticHashSetTest, CannotFind)
{
	const int key = 10;
	const int *value = hashset_.find(key);
	printf("Key %d is in the hashset: %d\n", key, value != nullptr);

	ASSERT_FALSE(value != nullptr);
}

TEST_F(StaticHashSetTest, FillCapacity)
{
	printf("Creating a new hashset to fill up to capacity (%u elements)\n", Capacity);
	HashSetTestType newHashset;

	for (unsigned int i = 0; i < Capacity; i++)
		newHashset.insert(i);

	ASSERT_EQ(newHashset.size(), Capacity);
	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_TRUE(newHashset.contains(i));
}

TEST_F(StaticHashSetTest, RemoveAllFromFull)
{
	printf("Creating a new hashset to fill up to capacity (%u elements)\n", Capacity);
	HashSetTestType newHashset;

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
using HashSetStressTestType = nctl::StaticHashSet<int, BigCapacity, nctl::FNV1aHashFunc<int>>;

TEST_F(StaticHashSetTest, StressRemove)
{
	printf("Creating a new hashset with a capacity of %u and filled up to %u elements\n", BigCapacity, LastElement);
	HashSetStressTestType newHashset;

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

TEST_F(StaticHashSetTest, StressReverseRemove)
{
	printf("Creating a new hashset with a capacity of %u and filled up to %u elements\n", BigCapacity, LastElement);
	HashSetStressTestType newHashset;

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
