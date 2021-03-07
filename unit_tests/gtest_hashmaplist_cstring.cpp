#include "gtest_hashmaplist_cstring.h"

namespace {

class HashMapListCStringTest : public ::testing::Test
{
  public:
	HashMapListCStringTest()
	    : cstrHashmap_(Capacity) {}

  protected:
	void SetUp() override { initHashMap(cstrHashmap_); }

	nctl::HashMapList<const char *, const char *> cstrHashmap_;
};

TEST_F(HashMapListCStringTest, RetrieveElements)
{
	printf("Retrieving the elements\n");
	for (unsigned int i = 0; i < Size; i++)
	{
		const char *value = cstrHashmap_[KeysCopy[i]];
		printf("key: %s, value: %s\n", KeysCopy[i], value);
		ASSERT_STREQ(value, Values[i]);
	}
}

TEST_F(HashMapListCStringTest, InsertElements)
{
	printf("Inserting elements\n");
	nctl::String newKey(32);
	nctl::String newValue(32);
	for (unsigned int i = Size; i < Size * 2; i++)
	{
		newKey.format("%s_2", KeysCopy[i % Size]);
		newValue.format("%s_2", Values[i % Size]);
		cstrHashmap_.insert(newKey.data(), newValue.data());
	}

	for (unsigned int i = 0; i < Size; i++)
		ASSERT_STREQ(cstrHashmap_[KeysCopy[i]], Values[i]);
	for (unsigned int i = Size; i < Size * 2; i++)
	{
		newKey.format("%s_2", KeysCopy[i % Size]);
		newValue.format("%s_2", Values[i % Size]);
		ASSERT_STREQ(cstrHashmap_[newKey.data()], newValue.data());
	}

	ASSERT_EQ(cstrHashmap_.size(), Size * 2);
	ASSERT_EQ(calcSize(cstrHashmap_), Size * 2);
}

TEST_F(HashMapListCStringTest, EmplaceElements)
{
	printf("Emplacing elements\n");
	nctl::String newKey(32);
	nctl::String newValue(32);
	for (unsigned int i = Size; i < Size * 2; i++)
	{
		newKey.format("%s_2", KeysCopy[i % Size]);
		newValue.format("%s_2", Values[i % Size]);
		cstrHashmap_.emplace(newKey.data(), newValue.data());
	}

	for (unsigned int i = 0; i < Size; i++)
		ASSERT_STREQ(cstrHashmap_[KeysCopy[i]], Values[i]);
	for (unsigned int i = Size; i < Size * 2; i++)
	{
		newKey.format("%s_2", KeysCopy[i % Size]);
		newValue.format("%s_2", Values[i % Size]);
		ASSERT_STREQ(cstrHashmap_[newKey.data()], newValue.data());
	}

	ASSERT_EQ(cstrHashmap_.size(), Size * 2);
	ASSERT_EQ(calcSize(cstrHashmap_), Size * 2);
}

TEST_F(HashMapListCStringTest, RemoveElements)
{
	printf("Removing a couple elements\n");
	cstrHashmap_.remove(KeysCopy[0]);
	cstrHashmap_.remove(KeysCopy[3]);
	printHashMap(cstrHashmap_);

	const char *value = nullptr;
	ASSERT_FALSE(cstrHashmap_.contains(Keys[0], value));
	ASSERT_FALSE(cstrHashmap_.contains(Keys[3], value));
}

TEST_F(HashMapListCStringTest, Contains)
{
	const char *value = nullptr;
	const bool found = cstrHashmap_.contains(KeysCopy[0], value);
	printf("Key %s is in the hashmap: %d - Value: %s\n", KeysCopy[0], found, value);

	ASSERT_TRUE(found);
	ASSERT_STREQ(value, Values[0]);
}

TEST_F(HashMapListCStringTest, DoesNotContain)
{
	const char *key = "Z";
	const char *value = nullptr;
	const bool found = cstrHashmap_.contains(key, value);
	printf("Key %s is in the hashmap: %d - Value: %s\n", key, found, value);

	ASSERT_FALSE(found);
}

TEST_F(HashMapListCStringTest, Find)
{
	const char **value = cstrHashmap_.find(KeysCopy[0]);
	printf("Key %s is in the hashmap: %d - Value: %s\n", KeysCopy[0], value != nullptr, *value);

	ASSERT_TRUE(value != nullptr);
	ASSERT_STREQ(*value, Values[0]);
}

TEST_F(HashMapListCStringTest, CannotFind)
{
	const char *key = "Z";
	const char **value = cstrHashmap_.find(key);
	printf("Key %s is in the hashmap: %d\n", key, value != nullptr);

	ASSERT_FALSE(value != nullptr);
}

}
