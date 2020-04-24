#include "gtest_statichashmap.h"
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

using StaticHashMapRefCountedType = nctl::StaticHashMap<int, RefCounted, Capacity, nctl::FixedHashFunc<int>>;

class StaticHashMapRefCountedTest : public ::testing::Test
{
  public:
	StaticHashMapRefCountedTest() {}

  protected:
	StaticHashMapRefCountedType hashmap_;
};

TEST_F(StaticHashMapRefCountedTest, InsertLValue)
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

TEST_F(StaticHashMapRefCountedTest, InsertRValue)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Move inserting a new RefCounted object\n");
	hashmap_.insert(0, RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(StaticHashMapRefCountedTest, InsertAndRemove)
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

TEST_F(StaticHashMapRefCountedTest, DoubleInsertClear)
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

TEST_F(StaticHashMapRefCountedTest, Emplace)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Emplacing a RefCounted object\n");
	hashmap_.emplace(0, RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(StaticHashMapRefCountedTest, CopyConstruction)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashmap_.insert(0, RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashmap with copy construction\n");
	StaticHashMapRefCountedType newHashmap(hashmap_);
	printRefCounters();

	ASSERT_EQ(hashmap_.size(), 1);
	ASSERT_EQ(newHashmap.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(StaticHashMapRefCountedTest, MoveConstruction)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashmap_.insert(0, RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashmap with move construction\n");
	StaticHashMapRefCountedType newHashmap(nctl::move(hashmap_));
	printRefCounters();

	ASSERT_EQ(hashmap_.size(), 0);
	ASSERT_EQ(newHashmap.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(StaticHashMapRefCountedTest, CopyAssignmentOperator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashmap_.insert(0, RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashmap with the copy assignment operator\n");
	StaticHashMapRefCountedType newHashmap;
	newHashmap = hashmap_;
	printRefCounters();

	ASSERT_EQ(hashmap_.size(), 1);
	ASSERT_EQ(newHashmap.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(StaticHashMapRefCountedTest, CopyAssignmentOperatorFromBigger)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashmap_.insert(0, RefCounted());
	hashmap_.insert(1, RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Creating a new hashmap with the copy assignment operator\n");
	StaticHashMapRefCountedType newHashmap;
	newHashmap = hashmap_;
	printRefCounters();

	ASSERT_EQ(hashmap_.size(), 2);
	ASSERT_EQ(newHashmap.size(), 2);
	ASSERT_EQ(RefCounted::counter(), 4);
}

TEST_F(StaticHashMapRefCountedTest, CopyAssignmentOperatorFromSmaller)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashmap_.insert(0, RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashmap with the copy assignment operator\n");
	StaticHashMapRefCountedType newHashmap;
	newHashmap.insert(0, RefCounted());
	newHashmap.insert(1, RefCounted());
	ASSERT_EQ(RefCounted::counter(), 3);
	newHashmap = hashmap_;
	printRefCounters();

	ASSERT_EQ(hashmap_.size(), 1);
	ASSERT_EQ(newHashmap.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(StaticHashMapRefCountedTest, MoveAssignmentOperator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashmap_.insert(0, RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashmap with the move assignment operator\n");
	StaticHashMapRefCountedType newHashmap;
	newHashmap = nctl::move(hashmap_);
	printRefCounters();

	ASSERT_EQ(hashmap_.size(), 0);
	ASSERT_EQ(newHashmap.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(StaticHashMapRefCountedTest, MoveAssignmentOperatorFromBigger)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashmap_.insert(0, RefCounted());
	hashmap_.insert(1, RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Creating a new hashmap with the move assignment operator\n");
	StaticHashMapRefCountedType newHashmap;
	newHashmap = nctl::move(hashmap_);
	printRefCounters();

	ASSERT_EQ(hashmap_.size(), 0);
	ASSERT_EQ(newHashmap.size(), 2);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(StaticHashMapRefCountedTest, MoveAssignmentOperatorFromSmaller)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashmap_.insert(0, RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashmap with the move assignment operator\n");
	StaticHashMapRefCountedType newHashmap;
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
