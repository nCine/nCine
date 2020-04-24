#include "gtest_hashmap.h"
#define PRINT_ALL_COUNTERS (0)
#include "test_refcounted.h"

#define TEST_STL (0)
#if TEST_STL
	#include <unordered_map>
#endif

int RefCounted::counter_ = 0;
int RefCounted::constructions_ = 0;
int RefCounted::destructions_ = 0;
int RefCounted::copyConstructions_ = 0;
int RefCounted::moveConstructions_ = 0;
int RefCounted::assignments_ = 0;
int RefCounted::moveAssignments_ = 0;

namespace {

using HashMapRefCountedType = nctl::HashMap<int, RefCounted, nctl::FixedHashFunc<int>>;

class HashMapRefCountedTest : public ::testing::Test
{
  public:
	HashMapRefCountedTest()
	    : hashmap_(Capacity) {}

  protected:
	HashMapRefCountedType hashmap_;
#if TEST_STL
	std::unordered_map<int, RefCounted> stdmap_;
#endif
};

TEST_F(HashMapRefCountedTest, InsertLValue)
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

TEST_F(HashMapRefCountedTest, InsertRValue)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Move inserting a new RefCounted object\n");
	hashmap_.insert(0, RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(HashMapRefCountedTest, InsertAndRemove)
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

TEST_F(HashMapRefCountedTest, DoubleInsertClear)
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

TEST_F(HashMapRefCountedTest, Emplace)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Emplacing a RefCounted object\n");
	hashmap_.emplace(0, RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(HashMapRefCountedTest, CopyConstruction)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashmap_.insert(0, RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashmap with copy construction\n");
	HashMapRefCountedType newHashmap(hashmap_);
	printRefCounters();

	ASSERT_EQ(hashmap_.size(), 1);
	ASSERT_EQ(newHashmap.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(HashMapRefCountedTest, MoveConstruction)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashmap_.insert(0, RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashmap with move construction\n");
	HashMapRefCountedType newHashmap(nctl::move(hashmap_));
	printRefCounters();

	ASSERT_EQ(hashmap_.size(), 0);
	ASSERT_EQ(newHashmap.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(HashMapRefCountedTest, CopyAssignmentOperator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashmap_.insert(0, RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashmap with the copy assignment operator\n");
	HashMapRefCountedType newHashmap(Capacity);
	newHashmap = hashmap_;
	printRefCounters();

	ASSERT_EQ(hashmap_.size(), 1);
	ASSERT_EQ(newHashmap.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(HashMapRefCountedTest, CopyAssignmentOperatorFromBigger)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashmap_.insert(0, RefCounted());
	hashmap_.insert(1, RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Creating a new hashmap with the copy assignment operator\n");
	HashMapRefCountedType newHashmap(Capacity);
	newHashmap = hashmap_;
	printRefCounters();

	ASSERT_EQ(hashmap_.size(), 2);
	ASSERT_EQ(newHashmap.size(), 2);
	ASSERT_EQ(RefCounted::counter(), 4);
}

TEST_F(HashMapRefCountedTest, CopyAssignmentOperatorFromSmaller)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashmap_.insert(0, RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashmap with the copy assignment operator\n");
	HashMapRefCountedType newHashmap(Capacity);
	newHashmap.insert(0, RefCounted());
	newHashmap.insert(1, RefCounted());
	ASSERT_EQ(RefCounted::counter(), 3);
	newHashmap = hashmap_;
	printRefCounters();

	ASSERT_EQ(hashmap_.size(), 1);
	ASSERT_EQ(newHashmap.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(HashMapRefCountedTest, MoveAssignmentOperator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashmap_.insert(0, RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashmap with the move assignment operator\n");
	HashMapRefCountedType newHashmap(Capacity);
	newHashmap = nctl::move(hashmap_);
	printRefCounters();

	ASSERT_EQ(hashmap_.size(), 0);
	ASSERT_EQ(newHashmap.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(HashMapRefCountedTest, MoveAssignmentOperatorFromBigger)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashmap_.insert(0, RefCounted());
	hashmap_.insert(1, RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Creating a new hashmap with the move assignment operator\n");
	HashMapRefCountedType newHashmap(Capacity);
	newHashmap = nctl::move(hashmap_);
	printRefCounters();

	ASSERT_EQ(hashmap_.size(), 0);
	ASSERT_EQ(newHashmap.size(), 2);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(HashMapRefCountedTest, MoveAssignmentOperatorFromSmaller)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	hashmap_.insert(0, RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashmap with the move assignment operator\n");
	HashMapRefCountedType newHashmap(Capacity);
	newHashmap.insert(0, RefCounted());
	newHashmap.insert(1, RefCounted());
	ASSERT_EQ(RefCounted::counter(), 3);
	newHashmap = nctl::move(hashmap_);
	printRefCounters();

	ASSERT_EQ(hashmap_.size(), 0);
	ASSERT_EQ(newHashmap.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

#if TEST_STL
TEST_F(HashMapRefCountedTest, StdMap_InsertLValue)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting a new RefCounted object\n");
	{
		const RefCounted rc;
		stdmap_.insert({ 0, rc });
	}

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(HashMapRefCountedTest, StdMap_InsertRValue)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Move inserting a new RefCounted object\n");
	stdmap_.insert({ 0, RefCounted() });

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(HashMapRefCountedTest, StdMap_InsertAndRemove)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting a new RefCounted object\n");
	stdmap_.insert({ 0, RefCounted() });
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Removing the RefCounted object\n");
	stdmap_.erase(0);
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 0);
}

TEST_F(HashMapRefCountedTest, StdMap_DoublePushBackClear)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting two new RefCounted objects\n");
	stdmap_.insert({ 0, RefCounted() });
	stdmap_.insert({ 1, RefCounted() });
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Clearing the hashmap\n");
	stdmap_.clear();
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 0);
}

TEST_F(HashMapRefCountedTest, StdMap_Emplace)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Emplacing a RefCounted object\n");
	stdmap_.emplace(0, RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(HashMapRefCountedTest, StdMap_CopyConstruction)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdmap_.insert({ 0, RefCounted() });
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashmap with copy construction\n");
	std::unordered_map<int, RefCounted> newHashmap(stdmap_);
	printRefCounters();

	ASSERT_EQ(stdmap_.size(), 1);
	ASSERT_EQ(newHashmap.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(HashMapRefCountedTest, StdMap_MoveConstruction)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdmap_.insert({ 0, RefCounted() });
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashmap with move construction\n");
	std::unordered_map<int, RefCounted> newHashmap(std::move(stdmap_));
	printRefCounters();

	ASSERT_EQ(stdmap_.size(), 0);
	ASSERT_EQ(newHashmap.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(HashMapRefCountedTest, StdMap_CopyAssignmentOperator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdmap_.insert({ 0, RefCounted() });
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashmap with the copy assignment operator\n");
	std::unordered_map<int, RefCounted> newHashmap;
	newHashmap = stdmap_;
	printRefCounters();

	ASSERT_EQ(stdmap_.size(), 1);
	ASSERT_EQ(newHashmap.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(HashMapRefCountedTest, StdMap_CopyAssignmentOperatorFromBigger)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdmap_.insert({ 0, RefCounted() });
	stdmap_.insert({ 1, RefCounted() });
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Creating a new hashmap with the copy assignment operator\n");
	std::unordered_map<int, RefCounted> newHashmap;
	newHashmap = stdmap_;
	printRefCounters();

	ASSERT_EQ(stdmap_.size(), 2);
	ASSERT_EQ(newHashmap.size(), 2);
	ASSERT_EQ(RefCounted::counter(), 4);
}

TEST_F(HashMapRefCountedTest, StdMap_CopyAssignmentOperatorFromSmaller)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdmap_.insert({ 0, RefCounted() });
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashmap with the copy assignment operator\n");
	std::unordered_map<int, RefCounted> newHashmap;
	newHashmap.insert({ 0, RefCounted() });
	newHashmap.insert({ 1, RefCounted() });
	ASSERT_EQ(RefCounted::counter(), 3);
	newHashmap = stdmap_;
	printRefCounters();

	ASSERT_EQ(stdmap_.size(), 1);
	ASSERT_EQ(newHashmap.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(HashMapRefCountedTest, StdMap_MoveAssignmentOperator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdmap_.insert({ 0, RefCounted() });
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashmap with the move assignment operator\n");
	std::unordered_map<int, RefCounted> newHashmap;
	newHashmap = std::move(stdmap_);
	printRefCounters();

	ASSERT_EQ(stdmap_.size(), 0);
	ASSERT_EQ(newHashmap.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(HashMapRefCountedTest, StdMap_MoveAssignmentOperatorFromBigger)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdmap_.insert({ 0, RefCounted() });
	stdmap_.insert({ 1, RefCounted() });
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Creating a new hashmap with the move assignment operator\n");
	std::unordered_map<int, RefCounted> newHashmap;
	newHashmap = std::move(stdmap_);
	printRefCounters();

	ASSERT_EQ(stdmap_.size(), 0);
	ASSERT_EQ(newHashmap.size(), 2);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(HashMapRefCountedTest, StdMap_MoveAssignmentOperatorFromSmaller)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdmap_.insert({ 0, RefCounted() });
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new hashmap with the move assignment operator\n");
	std::unordered_map<int, RefCounted> newHashmap;
	newHashmap.insert({ 0, RefCounted() });
	newHashmap.insert({ 1, RefCounted() });
	ASSERT_EQ(RefCounted::counter(), 3);
	newHashmap = std::move(stdmap_);
	printRefCounters();

	ASSERT_EQ(stdmap_.size(), 0);
	ASSERT_EQ(newHashmap.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}
#endif

}
