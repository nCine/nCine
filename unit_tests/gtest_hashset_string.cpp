#include "gtest_hashset_string.h"

namespace {

class HashSetStringTest : public ::testing::Test
{
  public:
	HashSetStringTest()
	    : strHashset_(Capacity) {}

  protected:
	void SetUp() override { initHashSet(strHashset_); }

	nctl::StringHashSet strHashset_;
};

TEST_F(HashSetStringTest, Capacity)
{
	const unsigned int capacity = strHashset_.capacity();
	printf("Capacity: %u\n", capacity);

	ASSERT_EQ(capacity, Capacity);
}

TEST_F(HashSetStringTest, Size)
{
	const unsigned int size = strHashset_.size();
	printf("Size: %u\n", size);

	ASSERT_EQ(size, Size);
}

TEST_F(HashSetStringTest, InsertElements)
{
	printf("Inserting elements\n");
	nctl::String newKey(32);
	for (unsigned int i = Size; i < Size * 2; i++)
	{
		newKey.format("%s_2", Keys[i % Size]);
		strHashset_.insert(newKey);
	}

	for (unsigned int i = 0; i < Size; i++)
		ASSERT_TRUE(strHashset_.contains(Keys[i]));
	for (unsigned int i = Size; i < Size * 2; i++)
	{
		newKey.format("%s_2", Keys[i % Size]);
		ASSERT_TRUE(strHashset_.contains(newKey));
	}

	ASSERT_EQ(strHashset_.size(), Size * 2);
	ASSERT_EQ(calcSize(strHashset_), Size * 2);
}

TEST_F(HashSetStringTest, RemoveElements)
{
	printf("Removing a couple elements\n");
	strHashset_.remove(Keys[0]);
	strHashset_.remove(Keys[3]);
	printHashSet(strHashset_);

	ASSERT_FALSE(strHashset_.contains(Keys[0]));
	ASSERT_FALSE(strHashset_.contains(Keys[3]));
}

TEST_F(HashSetStringTest, CopyConstruction)
{
	printf("Creating a new hashset with copy construction\n");
	nctl::StringHashSet newstrHashset(strHashset_);
	printHashSet(newstrHashset);

	assertHashSetsAreEqual(strHashset_, newstrHashset);
}

TEST_F(HashSetStringTest, MoveConstruction)
{
	printf("Creating a new hashset with move construction\n");
	nctl::StringHashSet newStrHashset = nctl::move(strHashset_);
	printHashSet(newStrHashset);

	ASSERT_EQ(strHashset_.size(), 0);
	ASSERT_EQ(newStrHashset.capacity(), Capacity);
	ASSERT_EQ(newStrHashset.size(), Size);
	ASSERT_EQ(calcSize(newStrHashset), Size);
}

TEST_F(HashSetStringTest, AssignmentOperator)
{
	printf("Creating a new hashset with the assignment operator\n");
	nctl::StringHashSet newstrHashset(Capacity);
	newstrHashset = strHashset_;
	printHashSet(newstrHashset);

	assertHashSetsAreEqual(strHashset_, newstrHashset);
}

TEST_F(HashSetStringTest, MoveAssignmentOperator)
{
	printf("Creating a new hashset with the move assignment operator\n");
	nctl::StringHashSet newStrHashset(Capacity);
	newStrHashset = nctl::move(strHashset_);
	printHashSet(newStrHashset);

	ASSERT_EQ(strHashset_.size(), 0);
	ASSERT_EQ(newStrHashset.capacity(), Capacity);
	ASSERT_EQ(newStrHashset.size(), Size);
	ASSERT_EQ(calcSize(newStrHashset), Size);
}

TEST_F(HashSetStringTest, Contains)
{
	const bool found = strHashset_.contains(Keys[0]);
	printf("Key %s is in the hashset: %d\n", Keys[0], found);

	ASSERT_TRUE(found);
}

TEST_F(HashSetStringTest, DoesNotContain)
{
	const char *key = "Z";
	const bool found = strHashset_.contains(key);
	printf("Key %s is in the hashset: %d\n", key, found);

	ASSERT_FALSE(found);
}

}
