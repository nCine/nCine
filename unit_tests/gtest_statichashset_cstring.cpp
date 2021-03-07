#include "gtest_statichashset_cstring.h"

namespace {

class StaticHashSetCStringTest : public ::testing::Test
{
  protected:
	void SetUp() override { initHashSet(cstrHashset_); }

	nctl::StaticHashSet<const char *, Capacity> cstrHashset_;
};

TEST_F(StaticHashSetCStringTest, InsertElements)
{
	printf("Inserting elements\n");
	nctl::String newKey(32);
	for (unsigned int i = Size; i < Size * 2; i++)
	{
		newKey.format("%s_2", KeysCopy[i % Size]);
		cstrHashset_.insert(newKey.data());
	}

	for (unsigned int i = 0; i < Size; i++)
		ASSERT_TRUE(cstrHashset_.contains(KeysCopy[i]));
	for (unsigned int i = Size; i < Size * 2; i++)
	{
		newKey.format("%s_2", KeysCopy[i % Size]);
		ASSERT_TRUE(cstrHashset_.contains(newKey.data()));
	}

	ASSERT_EQ(cstrHashset_.size(), Size * 2);
	ASSERT_EQ(calcSize(cstrHashset_), Size * 2);
}

TEST_F(StaticHashSetCStringTest, RemoveElements)
{
	printf("Removing a couple elements\n");
	cstrHashset_.remove(KeysCopy[0]);
	cstrHashset_.remove(KeysCopy[3]);
	printHashSet(cstrHashset_);

	ASSERT_FALSE(cstrHashset_.contains(Keys[0]));
	ASSERT_FALSE(cstrHashset_.contains(Keys[3]));
}

TEST_F(StaticHashSetCStringTest, Contains)
{
	const bool found = cstrHashset_.contains(KeysCopy[0]);
	printf("Key %s is in the hashset: %d\n", KeysCopy[0], found);

	ASSERT_TRUE(found);
}

TEST_F(StaticHashSetCStringTest, DoesNotContain)
{
	const char *key = "Z";
	const bool found = cstrHashset_.contains(key);
	printf("Key %s is in the hashset: %d\n", key, found);

	ASSERT_FALSE(found);
}

TEST_F(StaticHashSetCStringTest, Find)
{
	const char **value = cstrHashset_.find(KeysCopy[0]);
	printf("Key %s is in the hashset: %d\n", KeysCopy[0], value != nullptr);

	ASSERT_TRUE(value != nullptr);
}

TEST_F(StaticHashSetCStringTest, CannotFind)
{
	const char *key = "Z";
	const char **value = cstrHashset_.find(key);
	printf("Key %s is in the hashset: %d\n", key, value != nullptr);

	ASSERT_FALSE(value != nullptr);
}

}
