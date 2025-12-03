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
	char localKeysCopy[Size][KeyCapacity + 2]; // Plus "_2"
	char localValuesCopy[Size][ValueCapacity + 2]; // Plus "_2"
	for (unsigned int i = 0; i < Size; i++)
	{
		strncpy(localKeysCopy[i], Keys[i], KeyCapacity + 2);
		const unsigned int keyLength = strnlen(localKeysCopy[i], KeyCapacity + 2);
		strncpy(&localKeysCopy[i][keyLength], "_2", KeyCapacity + 2 - keyLength);

		strncpy(localValuesCopy[i], Keys[i], ValueCapacity + 2);
		const unsigned int valueLength = strnlen(localValuesCopy[i], ValueCapacity + 2);
		strncpy(&localValuesCopy[i][valueLength], "_2", ValueCapacity + 2 - valueLength);
	}

	printf("Inserting elements\n");
	for (unsigned int i = 0; i < Size; i++)
		cstrHashmap_.insert(localKeysCopy[i], localValuesCopy[i]);

	for (unsigned int i = 0; i < Size; i++)
	{
		ASSERT_STREQ(cstrHashmap_[KeysCopy[i]], Values[i]);
		ASSERT_STREQ(cstrHashmap_[localKeysCopy[i]], localValuesCopy[i]);
	}

	ASSERT_EQ(cstrHashmap_.size(), Size * 2);
	ASSERT_EQ(calcSize(cstrHashmap_), Size * 2);
}

TEST_F(HashMapListCStringTest, EmplaceElements)
{
	char localKeysCopy[Size][KeyCapacity + 2]; // Plus "_2"
	char localValuesCopy[Size][ValueCapacity + 2]; // Plus "_2"
	for (unsigned int i = 0; i < Size; i++)
	{
		strncpy(localKeysCopy[i], Keys[i], KeyCapacity + 2);
		const unsigned int keyLength = strnlen(localKeysCopy[i], KeyCapacity + 2);
		strncpy(&localKeysCopy[i][keyLength], "_2", KeyCapacity + 2 - keyLength);

		strncpy(localValuesCopy[i], Keys[i], ValueCapacity + 2);
		const unsigned int valueLength = strnlen(localValuesCopy[i], ValueCapacity + 2);
		strncpy(&localValuesCopy[i][valueLength], "_2", ValueCapacity + 2 - valueLength);
	}

	printf("Emplacing elements\n");
	for (unsigned int i = 0; i < Size; i++)
		cstrHashmap_.emplace(localKeysCopy[i], localValuesCopy[i]);

	for (unsigned int i = 0; i < Size; i++)
	{
		ASSERT_STREQ(cstrHashmap_[KeysCopy[i]], Values[i]);
		ASSERT_STREQ(cstrHashmap_[localKeysCopy[i]], localValuesCopy[i]);
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
	printf("Key %s is in the hashmap: %d\n", key, found);

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
