#include "gtest_list.h"
#define PRINT_ALL_COUNTERS (0)
#include "test_refcounted.h"

#define TEST_STL (0)
#if TEST_STL
	#include <list>
#endif

int RefCounted::counter_ = 0;
int RefCounted::constructions_ = 0;
int RefCounted::destructions_ = 0;
int RefCounted::copyConstructions_ = 0;
int RefCounted::moveConstructions_ = 0;
int RefCounted::assignments_ = 0;
int RefCounted::moveAssignments_ = 0;

namespace {

class ListRefCountedTest : public ::testing::Test
{
  public:
	ListRefCountedTest() {}

  protected:
	nctl::List<RefCounted> list_;
#if TEST_STL
	std::list<RefCounted> stdlist_;
#endif
};

TEST_F(ListRefCountedTest, PushBackLValue)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting a new RefCounted object at the back\n");
	{
		const RefCounted rc;
		list_.pushBack(rc);
	}

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ListRefCountedTest, PushBackRValue)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Move inserting a new RefCounted object at the back\n");
	list_.pushBack(RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ListRefCountedTest, PushFrontLValue)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting a new RefCounted object to the front\n");
	{
		const RefCounted rc;
		list_.pushFront(rc);
	}

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ListRefCountedTest, PushFrontRValue)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Move inserting a new RefCounted object to the front\n");
	list_.pushFront(RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ListRefCountedTest, PushAndPopBack)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting a new RefCounted object at the back\n");
	list_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Removing the RefCounted object from the back\n");
	list_.popBack();
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 0);
}

TEST_F(ListRefCountedTest, PushAndPopFront)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting a new RefCounted object at the back\n");
	list_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Removing the RefCounted object from the front\n");
	list_.popFront();
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 0);
}

TEST_F(ListRefCountedTest, DoublePushBackClear)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting two new RefCounted objects at the back\n");
	list_.pushBack(RefCounted());
	list_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Clearing the list\n");
	list_.clear();
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 0);
}

TEST_F(ListRefCountedTest, EmplaceBack)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Emplacing a RefCounted object at the back\n");
	list_.emplaceBack(RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ListRefCountedTest, InsertLValueAtBackWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting a RefCounted object at the back\n");
	{
		const RefCounted rc;
		list_.insertBefore(list_.end(), rc);
	}

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ListRefCountedTest, InsertRValueAtBackWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Move inserting a RefCounted object at the back\n");
	list_.insertBefore(list_.end(), RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ListRefCountedTest, InsertLValueAtFrontWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	list_.pushBack(RefCounted());
	ASSERT_EQ(RefCounted::counter(), 1);
	printf("Inserting a RefCounted object at the beginning\n");
	{
		const RefCounted rc;
		list_.insertAfter(list_.begin(), rc);
	}

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ListRefCountedTest, InsertRValueAtFrontWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	list_.pushBack(RefCounted());
	ASSERT_EQ(RefCounted::counter(), 1);
	printf("Move inserting a RefCounted object at the beginning\n");
	list_.insertAfter(list_.begin(), RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ListRefCountedTest, EmplaceAtBackWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Emplacing a RefCounted object at the back\n");
	list_.emplaceBefore(list_.end(), RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ListRefCountedTest, EmplaceAtFrontWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	list_.pushBack(RefCounted());
	ASSERT_EQ(RefCounted::counter(), 1);
	printf("Emplacing a RefCounted object at the beginning\n");
	list_.emplaceAfter(list_.begin(), RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ListRefCountedTest, CopyConstruction)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	list_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new list with copy construction\n");
	nctl::List<RefCounted> newList(list_);
	printRefCounters();

	ASSERT_EQ(list_.size(), 1);
	ASSERT_EQ(newList.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ListRefCountedTest, MoveConstruction)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	list_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new list with move construction\n");
	nctl::List<RefCounted> newList(nctl::move(list_));
	printRefCounters();

	ASSERT_EQ(list_.size(), 0);
	ASSERT_EQ(newList.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ListRefCountedTest, CopyAssignmentOperator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	list_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new array with the copy assignment operator\n");
	nctl::List<RefCounted> newList;
	newList = list_;
	printRefCounters();

	ASSERT_EQ(list_.size(), 1);
	ASSERT_EQ(newList.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ListRefCountedTest, CopyAssignmentOperatorFromBigger)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	list_.pushBack(RefCounted());
	list_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Creating a new list with the copy assignment operator\n");
	nctl::List<RefCounted> newList;
	newList = list_;
	printRefCounters();

	ASSERT_EQ(list_.size(), 2);
	ASSERT_EQ(newList.size(), 2);
	ASSERT_EQ(RefCounted::counter(), 4);
}

TEST_F(ListRefCountedTest, CopyAssignmentOperatorFromSmaller)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	list_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new list with the copy assignment operator\n");
	nctl::List<RefCounted> newList;
	newList.pushBack(RefCounted());
	newList.pushBack(RefCounted());
	ASSERT_EQ(RefCounted::counter(), 3);
	newList = list_;
	printRefCounters();

	ASSERT_EQ(list_.size(), 1);
	ASSERT_EQ(newList.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ListRefCountedTest, MoveAssignmentOperator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	list_.pushBack(RefCounted());
	printRefCounters();

	printf("Creating a new list with the move assignment operator\n");
	nctl::List<RefCounted> newList;
	newList = nctl::move(list_);
	printRefCounters();

	ASSERT_EQ(list_.size(), 0);
	ASSERT_EQ(newList.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ListRefCountedTest, MoveAssignmentOperatorFromBigger)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	list_.pushBack(RefCounted());
	list_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Creating a new list with the move assignment operator\n");
	nctl::List<RefCounted> newList;
	newList = nctl::move(list_);
	printRefCounters();

	ASSERT_EQ(list_.size(), 0);
	ASSERT_EQ(newList.size(), 2);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ListRefCountedTest, MoveAssignmentOperatorFromSmaller)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	list_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new list with the move assignment operator\n");
	nctl::List<RefCounted> newList;
	newList.pushBack(RefCounted());
	newList.pushBack(RefCounted());
	ASSERT_EQ(RefCounted::counter(), 3);
	newList = nctl::move(list_);
	printRefCounters();

	ASSERT_EQ(list_.size(), 0);
	ASSERT_EQ(newList.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ListRefCountedTest, RemoveLastWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Insert three elements)\n");
	list_.pushBack(RefCounted());
	list_.pushBack(RefCounted());
	list_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 3);

	printf("Removing last element\n");
	list_.erase(--list_.end());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ListRefCountedTest, RemoveAllWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Fill the list with %u elements\n", Length);
	for (unsigned int i = 0; i < Length; i++)
		list_.pushBack(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Length);

	printf("Removing the second half of the list\n");
	list_.erase(list_.begin(), list_.end());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 0);
}

#if TEST_STL
TEST_F(ListRefCountedTest, StdList_PushBackLValue)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting a new RefCounted object at the back\n");
	{
		const RefCounted rc;
		stdlist_.push_back(rc);
	}

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ListRefCountedTest, StdList_PushBackRValue)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Move inserting a new RefCounted object at the back\n");
	stdlist_.push_back(RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ListRefCountedTest, StdList_PushFrontLValue)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting a new RefCounted object to the front\n");
	{
		const RefCounted rc;
		stdlist_.push_front(rc);
	}

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ListRefCountedTest, StdList_PushFrontRValue)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Move inserting a new RefCounted object to the front\n");
	stdlist_.push_front(RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ListRefCountedTest, StdList_PushAndPopBack)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting a new RefCounted object at the back\n");
	stdlist_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Removing the RefCounted object from the back\n");
	stdlist_.pop_back();
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 0);
}

TEST_F(ListRefCountedTest, StdList_PushAndPopFront)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting a new RefCounted object at the back\n");
	stdlist_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Removing the RefCounted object from the front\n");
	stdlist_.pop_front();
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 0);
}

TEST_F(ListRefCountedTest, StdList_DoublePushBackClear)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting two new RefCounted objects at the back\n");
	stdlist_.push_back(RefCounted());
	stdlist_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Clearing the list\n");
	stdlist_.clear();
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 0);
}

TEST_F(ListRefCountedTest, StdList_EmplaceBack)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Emplacing a RefCounted object at the back\n");
	stdlist_.emplace_back(RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ListRefCountedTest, StdList_InsertLValueAtBackWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Inserting a RefCounted object at the back\n");
	{
		const RefCounted rc;
		stdlist_.insert(stdlist_.end(), rc);
	}

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ListRefCountedTest, StdList_InsertRValueAtBackWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Move inserting a RefCounted object at the back\n");
	stdlist_.insert(stdlist_.end(), RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ListRefCountedTest, StdList_InsertLValueAtFrontWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdlist_.push_back(RefCounted());
	ASSERT_EQ(RefCounted::counter(), 1);
	printf("Inserting a RefCounted object at the beginning\n");
	{
		const RefCounted rc;
		stdlist_.insert(stdlist_.begin(), rc);
	}

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ListRefCountedTest, StdList_InsertRValueAtFrontWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdlist_.push_back(RefCounted());
	ASSERT_EQ(RefCounted::counter(), 1);
	printf("Move inserting a RefCounted object at the beginning\n");
	stdlist_.insert(stdlist_.begin(), RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ListRefCountedTest, StdList_EmplaceAtBackWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Emplacing a RefCounted object at the back\n");
	stdlist_.emplace(stdlist_.end(), RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ListRefCountedTest, StdList_EmplaceAtFrontWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdlist_.push_back(RefCounted());
	ASSERT_EQ(RefCounted::counter(), 1);
	printf("Emplacing a RefCounted object at the beginning\n");
	stdlist_.emplace(stdlist_.begin(), RefCounted());

	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ListRefCountedTest, StdList_CopyConstruction)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdlist_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new list with copy construction\n");
	std::list<RefCounted> newList(stdlist_);
	printRefCounters();

	ASSERT_EQ(stdlist_.size(), 1);
	ASSERT_EQ(stdlist_.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ListRefCountedTest, StdList_MoveConstruction)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdlist_.push_back(RefCounted());
	printRefCounters();

	printf("Creating a new list with move construction\n");
	std::list<RefCounted> newList(std::move(stdlist_));
	printRefCounters();

	ASSERT_EQ(stdlist_.size(), 0);
	ASSERT_EQ(newList.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ListRefCountedTest, StdList_CopyAssignmentOperator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdlist_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new list with the copy assignment operator\n");
	std::list<RefCounted> newList;
	newList = stdlist_;
	printRefCounters();

	ASSERT_EQ(stdlist_.size(), 1);
	ASSERT_EQ(newList.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ListRefCountedTest, StdList_CopyAssignmentOperatorFromBigger)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdlist_.push_back(RefCounted());
	stdlist_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Creating a new list with the copy assignment operator\n");
	std::list<RefCounted> newList;
	newList = stdlist_;
	printRefCounters();

	ASSERT_EQ(stdlist_.size(), 2);
	ASSERT_EQ(newList.size(), 2);
	ASSERT_EQ(RefCounted::counter(), 4);
}

TEST_F(ListRefCountedTest, StdList_CopyAssignmentOperatorFromSmaller)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdlist_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new list with the copy assignment operator\n");
	std::list<RefCounted> newList;
	newList.push_back(RefCounted());
	newList.push_back(RefCounted());
	ASSERT_EQ(RefCounted::counter(), 3);
	newList = stdlist_;
	printRefCounters();

	ASSERT_EQ(stdlist_.size(), 1);
	ASSERT_EQ(newList.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ListRefCountedTest, StdList_MoveAssignmentOperator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdlist_.push_back(RefCounted());
	printRefCounters();

	printf("Creating a new list with the move assignment operator\n");
	std::list<RefCounted> newList;
	newList = std::move(stdlist_);
	printRefCounters();

	ASSERT_EQ(stdlist_.size(), 0);
	ASSERT_EQ(newList.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ListRefCountedTest, StdList_MoveAssignmentOperatorFromBigger)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdlist_.push_back(RefCounted());
	stdlist_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);

	printf("Creating a new list with the move assignment operator\n");
	std::list<RefCounted> newList;
	newList = std::move(stdlist_);
	printRefCounters();

	ASSERT_EQ(stdlist_.size(), 0);
	ASSERT_EQ(newList.size(), 2);
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ListRefCountedTest, StdList_MoveAssignmentOperatorFromSmaller)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	stdlist_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 1);

	printf("Creating a new list with the move assignment operator\n");
	std::list<RefCounted> newList;
	newList.push_back(RefCounted());
	newList.push_back(RefCounted());
	ASSERT_EQ(RefCounted::counter(), 3);
	newList = std::move(stdlist_);
	printRefCounters();

	ASSERT_EQ(stdlist_.size(), 0);
	ASSERT_EQ(newList.size(), 1);
	ASSERT_EQ(RefCounted::counter(), 1);
}

TEST_F(ListRefCountedTest, StdList_RemoveLastWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Insert three elements)\n");
	stdlist_.push_back(RefCounted());
	stdlist_.push_back(RefCounted());
	stdlist_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 3);

	printf("Removing last element\n");
	stdlist_.erase(--stdlist_.end());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 2);
}

TEST_F(ListRefCountedTest, StdList_RemoveAllWithIterator)
{
	ASSERT_EQ(RefCounted::counter(), 0);
	printf("Fill the list with %u elements\n", Length);
	for (unsigned int i = 0; i < Length; i++)
		stdlist_.push_back(RefCounted());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), Length);

	printf("Removing the second half of the list\n");
	stdlist_.erase(stdlist_.begin(), stdlist_.end());
	printRefCounters();
	ASSERT_EQ(RefCounted::counter(), 0);
}
#endif

}
