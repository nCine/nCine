#include "gtest_hashmap_string.h"

namespace {

class HashMapStringTest : public ::testing::Test
{
  public:
	HashMapStringTest()
	    : strHashmap_(Capacity) {}

  protected:
	void SetUp() override { initHashMap(strHashmap_); }

	nctl::StringHashMap<nctl::String> strHashmap_;
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
	nctl::StringHashMap<nctl::String> newStrHashmap(strHashmap_);
	printHashMap(newStrHashmap);

	assertHashMapsAreEqual(strHashmap_, newStrHashmap);
}

TEST_F(HashMapStringTest, AssignmentOperator)
{
	printf("Creating a new hashmap with the assignment operator\n");
	nctl::StringHashMap<nctl::String> newStrHashmap(Capacity);
	newStrHashmap = strHashmap_;
	printHashMap(newStrHashmap);

	assertHashMapsAreEqual(strHashmap_, newStrHashmap);
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

}
