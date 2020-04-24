#include "gtest_statichashset.h"
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

using StaticHashSetRefCountedType = nctl::StaticHashSet<RefCounted, Capacity, RefCountedClassHashFunc>;

class StaticHashSetRefCountedTest : public ::testing::Test
{
  public:
	StaticHashSetRefCountedTest() {}

  protected:
	StaticHashSetRefCountedType hashset_;
};

TEST_F(StaticHashSetRefCountedTest, InsertLValue)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting a new RefCounted object\n");
	{
		const RefCounted rc;
		hashset_.insert(rc);
	}

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(StaticHashSetRefCountedTest, InsertRValue)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Move inserting a new RefCounted object\n");
	hashset_.insert(RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(StaticHashSetRefCountedTest, DoubleInsertClear)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting two new RefCounted objects\n");
	hashset_.insert(RefCounted());
	hashset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Clearing the hashset\n");
	hashset_.clear();
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 0);
}

TEST_F(StaticHashSetRefCountedTest, CopyConstruction)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashset with copy construction\n");
	StaticHashSetRefCountedType newHashset(hashset_);
	printRefCounters();

	ASSERT_EQ(hashset_.size(), 1);
	ASSERT_EQ(newHashset.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(StaticHashSetRefCountedTest, MoveConstruction)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashset with move construction\n");
	StaticHashSetRefCountedType newHashset(nctl::move(hashset_));
	printRefCounters();

	ASSERT_EQ(hashset_.size(), 0);
	ASSERT_EQ(newHashset.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(StaticHashSetRefCountedTest, CopyAssignmentOperator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashset with the copy assignment operator\n");
	StaticHashSetRefCountedType newHashset;
	newHashset = hashset_;
	printRefCounters();

	ASSERT_EQ(hashset_.size(), 1);
	ASSERT_EQ(newHashset.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(StaticHashSetRefCountedTest, CopyAssignmentOperatorFromBigger)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashset_.insert(RefCounted());
	hashset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Creating a new hashset with the copy assignment operator\n");
	StaticHashSetRefCountedType newHashset;
	newHashset = hashset_;
	printRefCounters();

	ASSERT_EQ(hashset_.size(), 2);
	ASSERT_EQ(newHashset.size(), 2);
	ASSERT_EQ(RefCounted::counter(), 4);
}

TEST_F(StaticHashSetRefCountedTest, CopyAssignmentOperatorFromSmaller)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashset with the copy assignment operator\n");
	StaticHashSetRefCountedType newHashset;
	newHashset.insert(RefCounted());
	newHashset.insert(RefCounted());
	ASSERT_EQ(RefCounted::counter(), 3);
	newHashset = hashset_;
	printRefCounters();

	ASSERT_EQ(hashset_.size(), 1);
	ASSERT_EQ(newHashset.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(StaticHashSetRefCountedTest, MoveAssignmentOperator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashset with the move assignment operator\n");
	StaticHashSetRefCountedType newHashset;
	newHashset = nctl::move(hashset_);
	printRefCounters();

	ASSERT_EQ(hashset_.size(), 0);
	ASSERT_EQ(newHashset.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(StaticHashSetRefCountedTest, MoveAssignmentOperatorFromBigger)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashset_.insert(RefCounted());
	hashset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Creating a new hashset with the move assignment operator\n");
	StaticHashSetRefCountedType newHashset;
	newHashset = nctl::move(hashset_);
	printRefCounters();

	ASSERT_EQ(hashset_.size(), 0);
	ASSERT_EQ(newHashset.size(), 2);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(StaticHashSetRefCountedTest, MoveAssignmentOperatorFromSmaller)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashset with the move assignment operator\n");
	StaticHashSetRefCountedType newHashset;
	newHashset.insert(RefCounted());
	newHashset.insert(RefCounted());
	ASSERT_EQ(RefCounted::counter(), 3);
	newHashset = nctl::move(hashset_);
	printRefCounters();

	ASSERT_EQ(hashset_.size(), 0);
	ASSERT_EQ(newHashset.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

}
