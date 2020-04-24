#include "gtest_hashmaplist.h"
#define PRINT_ALL_COUNTERS (0)
#include "test_refcounted.h"

int RefCounted::counter_ = 0;
int RefCounted::constructions_ = 0;
int RefCounted::destructions_ = 0;
int RefCounted::copyConstructions_ = 0;
int RefCounted::moveConstructions_ = 0;
int RefCounted::assignments_ = 0;
int RefCounted::moveAssignments_ = 0;

namespace {

using HashMapListRefCountedType = nctl::HashMapList<int, RefCounted, nctl::FixedHashFunc<int>>;

class HashMapListRefCountedTest : public ::testing::Test
{
  public:
	HashMapListRefCountedTest()
	    : hashmap_(Capacity) {}

  protected:
	HashMapListRefCountedType hashmap_;
};

TEST_F(HashMapListRefCountedTest, InsertLValue)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting a new RefCounted object\n");
	{
		const RefCounted rc;
		hashmap_.insert(0, rc);
	}

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(HashMapListRefCountedTest, InsertRValue)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Move inserting a new RefCounted object\n");
	hashmap_.insert(0, RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(HashMapListRefCountedTest, InsertAndRemove)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting a new RefCounted object\n");
	hashmap_.insert(0, RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Removing the RefCounted object\n");
	hashmap_.remove(0);
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 0);
}

TEST_F(HashMapListRefCountedTest, DoubleInsertClear)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting two new RefCounted objects\n");
	hashmap_.insert(0, RefCounted());
	hashmap_.insert(1, RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Clearing the hashmap\n");
	hashmap_.clear();
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 0);
}

TEST_F(HashMapListRefCountedTest, Emplace)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Emplacing a RefCounted object\n");
	hashmap_.emplace(0, RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(HashMapListRefCountedTest, CopyConstruction)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashmap_.insert(0, RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashmap with copy construction\n");
	HashMapListRefCountedType newHashmap(hashmap_);
	printRefCounters();

	ASSERT_EQ(hashmap_.size(), 1);
	ASSERT_EQ(newHashmap.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(HashMapListRefCountedTest, MoveConstruction)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashmap_.insert(0, RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashmap with move construction\n");
	HashMapListRefCountedType newHashmap(nctl::move(hashmap_));
	printRefCounters();

	ASSERT_EQ(hashmap_.size(), 0);
	ASSERT_EQ(newHashmap.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(HashMapListRefCountedTest, CopyAssignmentOperator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashmap_.insert(0, RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashmap with the copy assignment operator\n");
	HashMapListRefCountedType newHashmap(Capacity);
	newHashmap = hashmap_;
	printRefCounters();

	ASSERT_EQ(hashmap_.size(), 1);
	ASSERT_EQ(newHashmap.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(HashMapListRefCountedTest, CopyAssignmentOperatorFromBigger)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashmap_.insert(0, RefCounted());
	hashmap_.insert(1, RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Creating a new hashmap with the copy assignment operator\n");
	HashMapListRefCountedType newHashmap(Capacity);
	newHashmap = hashmap_;
	printRefCounters();

	ASSERT_EQ(hashmap_.size(), 2);
	ASSERT_EQ(newHashmap.size(), 2);
	ASSERT_EQ(RefCounted::counter(), 4);
}

TEST_F(HashMapListRefCountedTest, CopyAssignmentOperatorFromSmaller)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashmap_.insert(0, RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashmap with the copy assignment operator\n");
	HashMapListRefCountedType newHashmap(Capacity);
	newHashmap.insert(0, RefCounted());
	newHashmap.insert(1, RefCounted());
	ASSERT_EQ(RefCounted::counter(), 3);
	newHashmap = hashmap_;
	printRefCounters();

	ASSERT_EQ(hashmap_.size(), 1);
	ASSERT_EQ(newHashmap.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(HashMapListRefCountedTest, MoveAssignmentOperator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashmap_.insert(0, RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashmap with the move assignment operator\n");
	HashMapListRefCountedType newHashmap(Capacity);
	newHashmap = nctl::move(hashmap_);
	printRefCounters();

	ASSERT_EQ(hashmap_.size(), 0);
	ASSERT_EQ(newHashmap.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(HashMapListRefCountedTest, MoveAssignmentOperatorFromBigger)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashmap_.insert(0, RefCounted());
	hashmap_.insert(1, RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Creating a new hashmap with the move assignment operator\n");
	HashMapListRefCountedType newHashmap(Capacity);
	newHashmap = nctl::move(hashmap_);
	printRefCounters();

	ASSERT_EQ(hashmap_.size(), 0);
	ASSERT_EQ(newHashmap.size(), 2);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(HashMapListRefCountedTest, MoveAssignmentOperatorFromSmaller)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashmap_.insert(0, RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashmap with the move assignment operator\n");
	HashMapListRefCountedType newHashmap(Capacity);
	newHashmap.insert(0, RefCounted());
	newHashmap.insert(1, RefCounted());
	ASSERT_EQ(RefCounted::counter(), 3);
	newHashmap = nctl::move(hashmap_);
	printRefCounters();

	ASSERT_EQ(hashmap_.size(), 0);
	ASSERT_EQ(newHashmap.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

}
