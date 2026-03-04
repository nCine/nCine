#include "gtest_statichashmap_string.h"

namespace {

class StaticHashMapStringTest : public ::testing::Test
{
  protected:
	void SetUp() override { initHashMap(strHashmap_); }

	HashMapStrType strHashmap_;
};

TEST_F(StaticHashMapStringTest, Capacity)
{
	const unsigned int capacity = strHashmap_.capacity();
	printf("Capacity: %u\n", capacity);

	ASSERT_EQ(capacity, Capacity);
}

TEST_F(StaticHashMapStringTest, Size)
{
	const unsigned int size = strHashmap_.size();
	printf("Size: %u\n", size);

	ASSERT_EQ(size, Size);
}

TEST_F(StaticHashMapStringTest, RetrieveElements)
{
	printf("Retrieving the elements\n");
	for (unsigned int i = 0; i < Size; i++)
	{
		const nctl::String value = strHashmap_[Keys[i]];
		printf("key: %s, value: %s\n", Keys[i], value.data());
		ASSERT_STREQ(value.data(), Values[i]);
	}
}

TEST_F(StaticHashMapStringTest, InsertElements)
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

TEST_F(StaticHashMapStringTest, InsertPairs)
{
	printf("Inserting elements as pairs\n");
	nctl::String newKey(32);
	nctl::String newValue(32);
	for (unsigned int i = Size; i < Size * 2; i++)
	{
		newKey.format("%s_2", Keys[i % Size]);
		newValue.format("%s_2", Values[i % Size]);
		PairStrType pair(newKey, newValue);
		strHashmap_.insert(pair);
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

TEST_F(StaticHashMapStringTest, EmplaceElements)
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

TEST_F(StaticHashMapStringTest, RemoveElements)
{
	printf("Removing a couple elements\n");
	strHashmap_.remove(Keys[0]);
	strHashmap_.remove(Keys[3]);
	printHashMap(strHashmap_);

	ASSERT_FALSE(strHashmap_.contains(Keys[0]));
	ASSERT_FALSE(strHashmap_.contains(Keys[3]));
}

TEST_F(StaticHashMapStringTest, CopyConstruction)
{
	printf("Creating a new hashmap with copy construction\n");
	HashMapStrType newStrHashmap(strHashmap_);
	printHashMap(newStrHashmap);

	assertHashMapsAreEqual(strHashmap_, newStrHashmap);
}

TEST_F(StaticHashMapStringTest, MoveConstruction)
{
	printf("Creating a new hashmap with move construction\n");
	HashMapStrType newStrHashmap = nctl::move(strHashmap_);
	printHashMap(newStrHashmap);

	ASSERT_EQ(strHashmap_.size(), 0);
	ASSERT_EQ(newStrHashmap.capacity(), Capacity);
	ASSERT_EQ(newStrHashmap.size(), Size);
	ASSERT_EQ(calcSize(newStrHashmap), Size);
}

TEST_F(StaticHashMapStringTest, AssignmentOperator)
{
	printf("Creating a new hashmap with the assignment operator\n");
	HashMapStrType newStrHashmap;
	newStrHashmap = strHashmap_;
	printHashMap(newStrHashmap);

	assertHashMapsAreEqual(strHashmap_, newStrHashmap);
}

TEST_F(StaticHashMapStringTest, MoveAssignmentOperator)
{
	printf("Creating a new hashmap with the move assignment operator\n");
	HashMapStrType newStrHashmap;
	newStrHashmap = nctl::move(strHashmap_);
	printHashMap(newStrHashmap);

	ASSERT_EQ(strHashmap_.size(), 0);
	ASSERT_EQ(newStrHashmap.capacity(), Capacity);
	ASSERT_EQ(newStrHashmap.size(), Size);
	ASSERT_EQ(calcSize(newStrHashmap), Size);
}

TEST_F(StaticHashMapStringTest, Contains)
{
	const bool found = strHashmap_.contains(Keys[0]);
	printf("Key %s is in the hashmap: %d\n", Keys[0], found);

	ASSERT_TRUE(found);
}

TEST_F(StaticHashMapStringTest, DoesNotContain)
{
	const char *key = "Z";
	nctl::String value;
	const bool found = strHashmap_.contains(key);
	printf("Key %s is in the hashmap: %d\n", key, found);

	ASSERT_FALSE(found);
}

TEST_F(StaticHashMapStringTest, Find)
{
	const nctl::String *value = strHashmap_.find(Keys[0]);
	printf("Key %s is in the hashmap: %d - Value: %s\n", Keys[0], value != nullptr, value->data());

	ASSERT_TRUE(value != nullptr);
	ASSERT_STREQ(value->data(), Values[0]);
}

TEST_F(StaticHashMapStringTest, CannotFind)
{
	const char *key = "Z";
	const nctl::String *value = strHashmap_.find(key);
	printf("Key %s is in the hashmap: %d\n", key, value != nullptr);

	ASSERT_FALSE(value != nullptr);
}

}
