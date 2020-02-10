#include "gtest_hashsetlist_cstring.h"

namespace {

class HashSetListCStringTest : public ::testing::Test
{
  public:
	HashSetListCStringTest()
	    : cstrHashset_(Capacity) {}

  protected:
	void SetUp() override { initHashSet(cstrHashset_); }

	nctl::CStringHashSetList cstrHashset_;
};

TEST_F(HashSetListCStringTest, InsertElements)
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

TEST_F(HashSetListCStringTest, RemoveElements)
{
	printf("Removing a couple elements\n");
	cstrHashset_.remove(KeysCopy[0]);
	cstrHashset_.remove(KeysCopy[3]);
	printHashMap(cstrHashset_);

	ASSERT_FALSE(cstrHashset_.contains(Keys[0]));
	ASSERT_FALSE(cstrHashset_.contains(Keys[3]));
}

TEST_F(HashSetListCStringTest, Contains)
{
	const bool found = cstrHashset_.contains(KeysCopy[0]);
	printf("Key %s is in the hashmap: %d\n", KeysCopy[0], found);

	ASSERT_TRUE(found);
}

}
