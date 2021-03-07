#include "gtest_hashmap_string.h"

namespace {

class HashMapStringTest : public ::testing::Test
{
  public:
	HashMapStringTest()
	    : strHashmap_(Capacity) {}

  protected:
	void SetUp() override { initHashMap(strHashmap_); }

	nctl::HashMap<nctl::String, nctl::String> strHashmap_;
};

TEST_F(HashMapStringTest, Capacity)
{
	const unsigned int capacity = strHashmap_.capacity();
	printf("Capacity: %u\n", capacity);

	ASSERT_EQ(capacity, Capacity);
}

TEST_F(HashMapStringTest, Size)
{
	const unsigned int size = strHashmap_.size();
	printf("Size: %u\n", size);

	ASSERT_EQ(size, Size);
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

TEST_F(HashMapStringTest, InsertElements)
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

TEST_F(HashMapStringTest, EmplaceElements)
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
	nctl::HashMap<nctl::String, nctl::String> newStrHashmap(strHashmap_);
	printHashMap(newStrHashmap);

	assertHashMapsAreEqual(strHashmap_, newStrHashmap);
}

TEST_F(HashMapStringTest, MoveConstruction)
{
	printf("Creating a new hashmap with move construction\n");
	nctl::HashMap<nctl::String, nctl::String> newStrHashmap = nctl::move(strHashmap_);
	printHashMap(newStrHashmap);

	ASSERT_EQ(strHashmap_.size(), 0);
	ASSERT_EQ(newStrHashmap.capacity(), Capacity);
	ASSERT_EQ(newStrHashmap.size(), Size);
	ASSERT_EQ(calcSize(newStrHashmap), Size);
}

TEST_F(HashMapStringTest, AssignmentOperator)
{
	printf("Creating a new hashmap with the assignment operator\n");
	nctl::HashMap<nctl::String, nctl::String> newStrHashmap(Capacity);
	newStrHashmap = strHashmap_;
	printHashMap(newStrHashmap);

	assertHashMapsAreEqual(strHashmap_, newStrHashmap);
}

TEST_F(HashMapStringTest, MoveAssignmentOperator)
{
	printf("Creating a new hashmap with the move assignment operator\n");
	nctl::HashMap<nctl::String, nctl::String> newStrHashmap(Capacity);
	newStrHashmap = nctl::move(strHashmap_);
	printHashMap(newStrHashmap);

	ASSERT_EQ(strHashmap_.size(), 0);
	ASSERT_EQ(newStrHashmap.capacity(), Capacity);
	ASSERT_EQ(newStrHashmap.size(), Size);
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

TEST_F(HashMapStringTest, Find)
{
	const nctl::String *value = strHashmap_.find(Keys[0]);
	printf("Key %s is in the hashmap: %d - Value: %s\n", Keys[0], value != nullptr, value->data());

	ASSERT_TRUE(value != nullptr);
	ASSERT_STREQ(value->data(), Values[0]);
}

TEST_F(HashMapStringTest, CannotFind)
{
	const char *key = "Z";
	const nctl::String *value = strHashmap_.find(key);
	printf("Key %s is in the hashmap: %d\n", key, value != nullptr);

	ASSERT_FALSE(value != nullptr);
}

}
