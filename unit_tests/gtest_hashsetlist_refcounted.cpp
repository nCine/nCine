#include "gtest_hashsetlist.h"
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

using HashSetListRefCountedType = nctl::HashSetList<RefCounted, RefCountedClassHashFunc>;

class HashSetListRefCountedTest : public ::testing::Test
{
  public:
	HashSetListRefCountedTest()
	    : hashset_(Capacity) {}

  protected:
	HashSetListRefCountedType hashset_;
};

TEST_F(HashSetListRefCountedTest, InsertLValue)
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

TEST_F(HashSetListRefCountedTest, InsertRValue)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Move inserting a new RefCounted object\n");
	hashset_.insert(RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(HashSetListRefCountedTest, DoubleInsertClear)
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

TEST_F(HashSetListRefCountedTest, CopyConstruction)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashset with copy construction\n");
	HashSetListRefCountedType newHashset(hashset_);
	printRefCounters();

	ASSERT_EQ(hashset_.size(), 1);
	ASSERT_EQ(newHashset.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(HashSetListRefCountedTest, MoveConstruction)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashset with move construction\n");
	HashSetListRefCountedType newHashset(nctl::move(hashset_));
	printRefCounters();

	ASSERT_EQ(hashset_.size(), 0);
	ASSERT_EQ(newHashset.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(HashSetListRefCountedTest, CopyAssignmentOperator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashset with the copy assignment operator\n");
	HashSetListRefCountedType newHashset(Capacity);
	newHashset = hashset_;
	printRefCounters();

	ASSERT_EQ(hashset_.size(), 1);
	ASSERT_EQ(newHashset.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(HashSetListRefCountedTest, CopyAssignmentOperatorFromBigger)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashset_.insert(RefCounted());
	hashset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Creating a new hashset with the copy assignment operator\n");
	HashSetListRefCountedType newHashset(Capacity);
	newHashset = hashset_;
	printRefCounters();

	ASSERT_EQ(hashset_.size(), 2);
	ASSERT_EQ(newHashset.size(), 2);
	ASSERT_EQ(RefCounted::counter(), 4);
}

TEST_F(HashSetListRefCountedTest, CopyAssignmentOperatorFromSmaller)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashset with the copy assignment operator\n");
	HashSetListRefCountedType newHashset(Capacity);
	newHashset.insert(RefCounted());
	newHashset.insert(RefCounted());
	ASSERT_EQ(RefCounted::counter(), 3);
	newHashset = hashset_;
	printRefCounters();

	ASSERT_EQ(hashset_.size(), 1);
	ASSERT_EQ(newHashset.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(HashSetListRefCountedTest, MoveAssignmentOperator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashset with the move assignment operator\n");
	HashSetListRefCountedType newHashset(Capacity);
	newHashset = nctl::move(hashset_);
	printRefCounters();

	ASSERT_EQ(hashset_.size(), 0);
	ASSERT_EQ(newHashset.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(HashSetListRefCountedTest, MoveAssignmentOperatorFromBigger)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashset_.insert(RefCounted());
	hashset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Creating a new hashset with the move assignment operator\n");
	HashSetListRefCountedType newHashset(Capacity);
	newHashset = nctl::move(hashset_);
	printRefCounters();

	ASSERT_EQ(hashset_.size(), 0);
	ASSERT_EQ(newHashset.size(), 2);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(HashSetListRefCountedTest, MoveAssignmentOperatorFromSmaller)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashset with the move assignment operator\n");
	HashSetListRefCountedType newHashset(Capacity);
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
