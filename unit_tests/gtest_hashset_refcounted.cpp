#include "gtest_hashset.h"
#define PRINT_ALL_COUNTERS (0)
#include "test_refcounted.h"

#define TEST_STL (0)
#if TEST_STL
	#include <unordered_set>
#endif

int RefCounted::counter_ = 0;
int RefCounted::constructions_ = 0;
int RefCounted::destructions_ = 0;
int RefCounted::copyConstructions_ = 0;
int RefCounted::moveConstructions_ = 0;
int RefCounted::assignments_ = 0;
int RefCounted::moveAssignments_ = 0;

namespace {

using HashSetRefCountedType = nctl::HashSet<RefCounted, RefCountedClassHashFunc>;

class HashSetRefCountedTest : public ::testing::Test
{
  public:
	HashSetRefCountedTest()
	    : hashset_(Capacity)
#if TEST_STL
	      ,
	      stdset_(Capacity)
#endif
	{}

  protected:
	HashSetRefCountedType hashset_;
#if TEST_STL
	std::unordered_set<RefCounted, RefCountedClassHashFunc> stdset_;
#endif
};

TEST_F(HashSetRefCountedTest, InsertLValue)
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

TEST_F(HashSetRefCountedTest, InsertRValue)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Move inserting a new RefCounted object\n");
	hashset_.insert(RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(HashSetRefCountedTest, DoubleInsertClear)
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

TEST_F(HashSetRefCountedTest, CopyConstruction)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashset with copy construction\n");
	HashSetRefCountedType newHashset(hashset_);
	printRefCounters();

	ASSERT_EQ(hashset_.size(), 1);
	ASSERT_EQ(newHashset.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(HashSetRefCountedTest, MoveConstruction)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashset with move construction\n");
	HashSetRefCountedType newHashset(nctl::move(hashset_));
	printRefCounters();

	ASSERT_EQ(hashset_.size(), 0);
	ASSERT_EQ(newHashset.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(HashSetRefCountedTest, CopyAssignmentOperator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashset with the copy assignment operator\n");
	HashSetRefCountedType newHashset(Capacity);
	newHashset = hashset_;
	printRefCounters();

	ASSERT_EQ(hashset_.size(), 1);
	ASSERT_EQ(newHashset.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(HashSetRefCountedTest, CopyAssignmentOperatorFromBigger)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashset_.insert(RefCounted());
	hashset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Creating a new hashset with the copy assignment operator\n");
	HashSetRefCountedType newHashset(Capacity);
	newHashset = hashset_;
	printRefCounters();

	ASSERT_EQ(hashset_.size(), 2);
	ASSERT_EQ(newHashset.size(), 2);
	ASSERT_EQ(RefCounted::counter(), 4);
}

TEST_F(HashSetRefCountedTest, CopyAssignmentOperatorFromSmaller)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashset with the copy assignment operator\n");
	HashSetRefCountedType newHashset(Capacity);
	newHashset.insert(RefCounted());
	newHashset.insert(RefCounted());
	ASSERT_EQ(RefCounted::counter(), 3);
	newHashset = hashset_;
	printRefCounters();

	ASSERT_EQ(hashset_.size(), 1);
	ASSERT_EQ(newHashset.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(HashSetRefCountedTest, MoveAssignmentOperator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashset with the move assignment operator\n");
	HashSetRefCountedType newHashset(Capacity);
	newHashset = nctl::move(hashset_);
	printRefCounters();

	ASSERT_EQ(hashset_.size(), 0);
	ASSERT_EQ(newHashset.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(HashSetRefCountedTest, MoveAssignmentOperatorFromBigger)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashset_.insert(RefCounted());
	hashset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Creating a new hashset with the move assignment operator\n");
	HashSetRefCountedType newHashset(Capacity);
	newHashset = nctl::move(hashset_);
	printRefCounters();

	ASSERT_EQ(hashset_.size(), 0);
	ASSERT_EQ(newHashset.size(), 2);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(HashSetRefCountedTest, MoveAssignmentOperatorFromSmaller)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashset with the move assignment operator\n");
	HashSetRefCountedType newHashset(Capacity);
	newHashset.insert(RefCounted());
	newHashset.insert(RefCounted());
	ASSERT_EQ(RefCounted::counter(), 3);
	newHashset = nctl::move(hashset_);
	printRefCounters();

	ASSERT_EQ(hashset_.size(), 0);
	ASSERT_EQ(newHashset.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

#if TEST_STL
TEST_F(HashSetRefCountedTest, StdSet_InsertLValue)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting a new RefCounted object\n");
	{
		const RefCounted rc;
		stdset_.insert(rc);
	}

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(HashSetRefCountedTest, StdSet_InsertRValue)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Move inserting a new RefCounted object\n");
	stdset_.insert(RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(HashSetRefCountedTest, StdSet_DoublePushBackClear)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting two new RefCounted objects\n");
	stdset_.insert(RefCounted());
	stdset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Clearing the hashset\n");
	stdset_.clear();
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 0);
}

TEST_F(HashSetRefCountedTest, StdSet_Emplace)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Emplacing a RefCounted object\n");
	stdset_.emplace(RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(HashSetRefCountedTest, StdSet_CopyConstruction)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashset with copy construction\n");
	std::unordered_set<RefCounted, RefCountedClassHashFunc> newHashset(stdset_);
	printRefCounters();

	ASSERT_EQ(stdset_.size(), 1);
	ASSERT_EQ(newHashset.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(HashSetRefCountedTest, StdSet_MoveConstruction)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashset with move construction\n");
	std::unordered_set<RefCounted, RefCountedClassHashFunc> newHashset(std::move(stdset_));
	printRefCounters();

	ASSERT_EQ(stdset_.size(), 0);
	ASSERT_EQ(newHashset.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(HashSetRefCountedTest, StdSet_CopyAssignmentOperator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashset with the copy assignment operator\n");
	std::unordered_set<RefCounted, RefCountedClassHashFunc> newHashset(Capacity);
	newHashset = stdset_;
	printRefCounters();

	ASSERT_EQ(stdset_.size(), 1);
	ASSERT_EQ(newHashset.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(HashSetRefCountedTest, StdSet_CopyAssignmentOperatorFromBigger)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdset_.insert(RefCounted());
	stdset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Creating a new hashset with the copy assignment operator\n");
	std::unordered_set<RefCounted, RefCountedClassHashFunc> newHashset(Capacity);
	newHashset = stdset_;
	printRefCounters();

	ASSERT_EQ(stdset_.size(), 2);
	ASSERT_EQ(newHashset.size(), 2);
	ASSERT_EQ(RefCounted::counter(), 4);
}

TEST_F(HashSetRefCountedTest, StdSet_CopyAssignmentOperatorFromSmaller)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashset with the copy assignment operator\n");
	std::unordered_set<RefCounted, RefCountedClassHashFunc> newHashset(Capacity);
	newHashset.insert(RefCounted());
	newHashset.insert(RefCounted());
	ASSERT_EQ(RefCounted::counter(), 3);
	newHashset = stdset_;
	printRefCounters();

	ASSERT_EQ(stdset_.size(), 1);
	ASSERT_EQ(newHashset.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(HashSetRefCountedTest, StdSet_MoveAssignmentOperator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashset with the move assignment operator\n");
	std::unordered_set<RefCounted, RefCountedClassHashFunc> newHashset(Capacity);
	newHashset = std::move(stdset_);
	printRefCounters();

	ASSERT_EQ(stdset_.size(), 0);
	ASSERT_EQ(newHashset.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(HashSetRefCountedTest, StdSet_MoveAssignmentOperatorFromBigger)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdset_.insert(RefCounted());
	stdset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Creating a new hashset with the move assignment operator\n");
	std::unordered_set<RefCounted, RefCountedClassHashFunc> newHashset(Capacity);
	newHashset = std::move(stdset_);
	printRefCounters();

	ASSERT_EQ(stdset_.size(), 0);
	ASSERT_EQ(newHashset.size(), 2);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(HashSetRefCountedTest, StdSet_MoveAssignmentOperatorFromSmaller)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdset_.insert(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashset with the move assignment operator\n");
	std::unordered_set<RefCounted, RefCountedClassHashFunc> newHashset(Capacity);
	newHashset.insert(RefCounted());
	newHashset.insert(RefCounted());
	ASSERT_EQ(RefCounted::counter(), 3);
	newHashset = std::move(stdset_);
	printRefCounters();

	ASSERT_EQ(stdset_.size(), 0);
	ASSERT_EQ(newHashset.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}
#endif

}
