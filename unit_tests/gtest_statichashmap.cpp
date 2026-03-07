#include "gtest_statichashmap.h"

namespace {

class StaticHashMapTest : public ::testing::Test
{
  protected:
	void SetUp() override { initHashMap(hashmap_); }

	HashMapTestType hashmap_;
};

TEST_F(StaticHashMapTest, Capacity)
{
	const unsigned int capacity = hashmap_.capacity();
	printf("Capacity: %u\n", capacity);

	ASSERT_EQ(capacity, Capacity);
}

TEST_F(StaticHashMapTest, Size)
{
	const unsigned int size = hashmap_.size();
	printf("Size: %u\n", size);

	ASSERT_EQ(size, Size);
	ASSERT_EQ(calcSize(hashmap_), Size);
}

TEST_F(StaticHashMapTest, LoadFactor)
{
	const float loadFactor = hashmap_.loadFactor();
	printf("Size: %u, Capacity: %u, Load Factor: %f\n", Size, Capacity, loadFactor);

	ASSERT_FLOAT_EQ(loadFactor, Size / static_cast<float>(Capacity));
}

TEST_F(StaticHashMapTest, Clear)
{
	ASSERT_FALSE(hashmap_.isEmpty());
	hashmap_.clear();
	printHashMap(hashmap_);
	ASSERT_TRUE(hashmap_.isEmpty());
	ASSERT_EQ(hashmap_.size(), 0u);
	ASSERT_EQ(hashmap_.capacity(), Capacity);
}

TEST_F(StaticHashMapTest, RetrieveElements)
{
	printf("Retrieving the elements\n");
	for (unsigned int i = 0; i < Size; i++)
	{
		printf("key: %u, value: %d\n", i, hashmap_[i]);
		ASSERT_EQ(hashmap_[i], KeyValueDifference + i);
	}

	ASSERT_EQ(hashmap_.size(), Size);
	ASSERT_EQ(calcSize(hashmap_), Size);
}

TEST_F(StaticHashMapTest, InsertElements)
{
	printf("Inserting elements\n");
	for (unsigned int i = Size; i < Size * 2; i++)
		hashmap_.insert(i, i + KeyValueDifference);

	for (unsigned int i = 0; i < Size * 2; i++)
		ASSERT_EQ(hashmap_[i], i + KeyValueDifference);

	ASSERT_EQ(hashmap_.size(), Size * 2);
	ASSERT_EQ(calcSize(hashmap_), Size * 2);
}

TEST_F(StaticHashMapTest, InsertConstElements)
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

TEST_F(StaticHashMapTest, InsertPairs)
{
	printf("Inserting elements as pairs\n");
	for (unsigned int i = Size; i < Size * 2; i++)
	{
		PairType pair(i, i + KeyValueDifference);
		hashmap_.insert(pair);
	}

	for (unsigned int i = 0; i < Size * 2; i++)
		ASSERT_EQ(hashmap_[i], i + KeyValueDifference);

	ASSERT_EQ(hashmap_.size(), Size * 2);
	ASSERT_EQ(calcSize(hashmap_), Size * 2);
}

TEST_F(StaticHashMapTest, InsertInitializerList)
{
	const unsigned int InitializerListSize = 5;
	printf("Inserting elements with an initializer list\n");
	const bool allInserted = hashmap_.insert({
		{ 10, 10 + KeyValueDifference },
		{ 11, 11 + KeyValueDifference },
		{ 12, 12 + KeyValueDifference },
		{ 13, 13 + KeyValueDifference },
		{ 14, 14 + KeyValueDifference }
	});
	printHashMap(hashmap_);

	for (unsigned int i = 0; i < hashmap_.size(); i++)
		ASSERT_EQ(hashmap_[i], i + KeyValueDifference);

	ASSERT_TRUE(allInserted);
	ASSERT_EQ(hashmap_.size(), Size + InitializerListSize);
	ASSERT_EQ(calcSize(hashmap_), Size + InitializerListSize);
}

TEST_F(StaticHashMapTest, InsertInitializerListTruncate)
{
	const unsigned int SmallCapacity = 4;
	nctl::StaticHashMap<int, int, SmallCapacity, nctl::FixedHashFunc<int>> newHashmap;

	printf("Inserting elements with an initializer list longer than its capacity\n");
	const bool allInserted = newHashmap.insert({
		{ 0, 0 + KeyValueDifference },
		{ 1, 1 + KeyValueDifference },
		{ 2, 2 + KeyValueDifference },
		{ 3, 3 + KeyValueDifference },
		{ 4, 4 + KeyValueDifference }
	});

	for (unsigned int i = 0; i < newHashmap.size(); i++)
		ASSERT_EQ(newHashmap[i], i + KeyValueDifference);

	ASSERT_FALSE(allInserted);
	ASSERT_FALSE(newHashmap.contains(4));
	ASSERT_EQ(newHashmap.size(), SmallCapacity);
}

TEST_F(StaticHashMapTest, FailInsertElements)
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

TEST_F(StaticHashMapTest, FailInsertConstElements)
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

TEST_F(StaticHashMapTest, EmplaceElements)
{
	printf("Emplacing elements\n");
	for (unsigned int i = Size; i < Size * 2; i++)
		hashmap_.emplace(i, i + KeyValueDifference);

	for (unsigned int i = 0; i < Size * 2; i++)
		ASSERT_EQ(hashmap_[i], i + KeyValueDifference);

	ASSERT_EQ(hashmap_.size(), Size * 2);
	ASSERT_EQ(calcSize(hashmap_), Size * 2);
}

TEST_F(StaticHashMapTest, FailEmplaceElements)
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

TEST_F(StaticHashMapTest, RemoveElements)
{
	printf("Original size: %u\n", hashmap_.size());
	printf("Removing a couple elements\n");
	printf("New size: %u\n", hashmap_.size());
	hashmap_.remove(5);
	hashmap_.remove(7);
	printHashMap(hashmap_);

	ASSERT_FALSE(hashmap_.contains(5));
	ASSERT_FALSE(hashmap_.contains(7));
	ASSERT_EQ(hashmap_.size(), Size - 2);
	ASSERT_EQ(calcSize(hashmap_), Size - 2);
}

TEST_F(StaticHashMapTest, InitializerListConstruction)
{
	const unsigned int InitializerListSize = 5;
	printf("Creating a new hashmap with an initializer list\n");
	HashMapTestType newHashmap{
		{ 0, 0 + KeyValueDifference },
		{ 1, 1 + KeyValueDifference },
		{ 2, 2 + KeyValueDifference },
		{ 3, 3 + KeyValueDifference },
		{ 4, 4 + KeyValueDifference }
	};
	printHashMap(newHashmap);

	for (unsigned int i = 0; i < newHashmap.size(); i++)
		ASSERT_EQ(newHashmap[i], i + KeyValueDifference);

	ASSERT_EQ(newHashmap.size(), InitializerListSize);
	ASSERT_EQ(calcSize(newHashmap), InitializerListSize);
}

TEST_F(StaticHashMapTest, InitializerListConstructionTruncate)
{
	const unsigned int SmallCapacity = 4;
	printf("Creating a new hashmap with an initializer list longer than its capacity\n");
	nctl::StaticHashMap<int, int, SmallCapacity, nctl::FixedHashFunc<int>> newHashmap{
		{ 0, 0 + KeyValueDifference },
		{ 1, 1 + KeyValueDifference },
		{ 2, 2 + KeyValueDifference },
		{ 3, 3 + KeyValueDifference },
		{ 4, 4 + KeyValueDifference }
	};

	for (unsigned int i = 0; i < newHashmap.size(); i++)
		ASSERT_EQ(newHashmap[i], i + KeyValueDifference);

	ASSERT_FALSE(newHashmap.contains(4));
	ASSERT_EQ(newHashmap.size(), SmallCapacity);
}

TEST_F(StaticHashMapTest, CopyConstruction)
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

TEST_F(StaticHashMapTest, MoveConstruction)
{
	printf("Creating a new hashmap with move construction\n");
	HashMapTestType newHashmap = nctl::move(hashmap_);
	printHashMap(newHashmap);

	ASSERT_EQ(hashmap_.size(), 0);
	ASSERT_EQ(newHashmap.capacity(), Capacity);
	ASSERT_EQ(newHashmap.size(), Size);
	ASSERT_EQ(calcSize(newHashmap), Size);
}

TEST_F(StaticHashMapTest, AssignmentOperator)
{
	printf("Creating a new hashmap with the assignment operator\n");
	HashMapTestType newHashmap;
	newHashmap = hashmap_;
	printHashMap(newHashmap);

	assertHashMapsAreEqual(hashmap_, newHashmap);
	ASSERT_EQ(hashmap_.size(), Size);
	ASSERT_EQ(calcSize(hashmap_), Size);
	ASSERT_EQ(newHashmap.size(), Size);
	ASSERT_EQ(calcSize(newHashmap), Size);
}

TEST_F(StaticHashMapTest, MoveAssignmentOperator)
{
	printf("Creating a new hashmap with the move assignment operator\n");
	HashMapTestType newHashmap;
	newHashmap = nctl::move(hashmap_);
	printHashMap(newHashmap);

	ASSERT_EQ(hashmap_.size(), 0);
	ASSERT_EQ(newHashmap.capacity(), Capacity);
	ASSERT_EQ(newHashmap.size(), Size);
	ASSERT_EQ(calcSize(newHashmap), Size);
}

TEST_F(StaticHashMapTest, SelfAssignment)
{
	printf("Assigning the hashmap to itself with the assignment operator\n");
	hashmap_ = hashmap_;
	printHashMap(hashmap_);

	ASSERT_EQ(hashmap_.size(), Size);
	ASSERT_EQ(calcSize(hashmap_), Size);
}

TEST_F(StaticHashMapTest, Contains)
{
	const int key = 1;
	const bool found = hashmap_.contains(key);
	printf("Key %d is in the hashmap: %d\n", key, found);

	ASSERT_TRUE(found);
}

TEST_F(StaticHashMapTest, DoesNotContain)
{
	const int key = 10;
	const bool found = hashmap_.contains(key);
	printf("Key %d is in the hashmap: %d\n", key, found);

	ASSERT_FALSE(found);
}

TEST_F(StaticHashMapTest, Find)
{
	const int key = 1;
	const int *value = hashmap_.find(key);
	printf("Key %d is in the hashmap: %d - Value: %d\n", key, value != nullptr, *value);

	ASSERT_TRUE(value != nullptr);
	ASSERT_EQ(*value, key + KeyValueDifference);
}

TEST_F(StaticHashMapTest, ConstFind)
{
	const HashMapTestType &constHashmap = hashmap_;
	const int key = 1;
	const int *value = constHashmap.find(key);
	printf("Key %d is in the hashmap: %d - Value: %d\n", key, value != nullptr, *value);

	ASSERT_TRUE(value != nullptr);
	ASSERT_EQ(*value, key + KeyValueDifference);
}

TEST_F(StaticHashMapTest, CannotFind)
{
	const int key = 10;
	const int *value = hashmap_.find(key);
	printf("Key %d is in the hashmap: %d\n", key, value != nullptr);

	ASSERT_FALSE(value != nullptr);
}

TEST_F(StaticHashMapTest, FillCapacity)
{
	printf("Creating a new hashmap to fill up to capacity (%u elements)\n", Capacity);
	HashMapTestType newHashmap;

	for (unsigned int i = 0; i < Capacity; i++)
		newHashmap[i] = i + KeyValueDifference;

	ASSERT_EQ(newHashmap.size(), Capacity);
	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_EQ(newHashmap[i], i + KeyValueDifference);
}

TEST_F(StaticHashMapTest, RemoveAllFromFull)
{
	printf("Creating a new hashmap to fill up to capacity (%u elements)\n", Capacity);
	HashMapTestType newHashmap;

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
using HashMapStressTestType = nctl::StaticHashMap<int, int, BigCapacity, nctl::FNV1aHashFunc<int>>;

TEST_F(StaticHashMapTest, StressRemove)
{
	printf("Creating a new hashmap with a capacity of %u and filled up to %u elements\n", BigCapacity, LastElement);
	HashMapStressTestType newHashmap;

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
			ASSERT_TRUE(newHashmap.contains(j));
		for (int j = 0; j < i + 1; j++)
			ASSERT_FALSE(newHashmap.contains(j));
	}

	ASSERT_EQ(newHashmap.size(), 0);
}

TEST_F(StaticHashMapTest, StressReverseRemove)
{
	printf("Creating a new hashmap with a capacity of %u and filled up to %u elements\n", BigCapacity, LastElement);
	HashMapStressTestType newHashmap;

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
			ASSERT_TRUE(newHashmap.contains(j));
		for (int j = LastElement; j >= i; j--)
			ASSERT_FALSE(newHashmap.contains(j));
	}

	ASSERT_EQ(newHashmap.size(), 0);
}

}
