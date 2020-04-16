#include "gtest_list.h"

namespace {

class ListOperationsTest : public ::testing::Test
{
  public:
	ListOperationsTest() {}

  protected:
	void SetUp() override { initList(list_); }

	nctl::List<int> list_;
};

TEST_F(ListOperationsTest, Clear)
{
	ASSERT_FALSE(list_.isEmpty());
	ASSERT_EQ(list_.size(), Length);
	ASSERT_TRUE(isUnmodified(list_));

	printf("Clearing the list\n");
	list_.clear();

	ASSERT_TRUE(list_.isEmpty());
	ASSERT_EQ(list_.size(), 0);
}

TEST_F(ListOperationsTest, ClearEmpty)
{
	nctl::List<int> newList;

	ASSERT_TRUE(newList.isEmpty());
	ASSERT_EQ(newList.size(), 0);

	printf("Clearing an empty list\n");
	list_.clear();

	ASSERT_TRUE(newList.isEmpty());
	ASSERT_EQ(newList.size(), 0);
}

TEST_F(ListOperationsTest, CopyConstruction)
{
	printf("Creating a new list with copy construction\n");
	nctl::List<int> newList(list_);
	printList(newList);

	assertListsAreEqual(list_, newList);
}

TEST_F(ListOperationsTest, MoveConstruction)
{
	printf("Creating a new list with move construction\n");
	nctl::List<int> newList(nctl::move(list_));
	printList(newList);

	ASSERT_EQ(list_.size(), 0);
	ASSERT_EQ(calcLength(list_), 0);
	ASSERT_EQ(newList.size(), Length);
	int array[Length] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	assertListMatchesArray(newList, array);
}

TEST_F(ListOperationsTest, MoveConstructionFromEmpty)
{
	printf("Creating a new list from an empty one with move construction\n");
	nctl::List<int> emptyList;
	nctl::List<int> newList(nctl::move(emptyList));
	printList(newList);

	ASSERT_EQ(emptyList.size(), 0);
	ASSERT_EQ(calcLength(emptyList), 0);
	ASSERT_EQ(newList.size(), 0);
	ASSERT_EQ(calcLength(newList), 0);
}

TEST_F(ListOperationsTest, AssignmentOperator)
{
	printf("Creating a new list with the assignment operator\n");
	nctl::List<int> newList;
	newList = list_;
	printList(newList);

	assertListsAreEqual(list_, newList);
}

TEST_F(ListOperationsTest, MoveAssignmentOperator)
{
	printf("Creating a new list with the move assignment operator\n");
	nctl::List<int> newList;
	newList = nctl::move(list_);
	printList(newList);

	ASSERT_EQ(list_.size(), 0);
	ASSERT_EQ(calcLength(list_), 0);
	ASSERT_EQ(newList.size(), Length);
	int array[Length] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	assertListMatchesArray(newList, array);
}

TEST_F(ListOperationsTest, SelfAssignment)
{
	printf("Assigning the list to itself with the assignment operator\n");
	list_ = list_;
	printList(list_);

	ASSERT_EQ(list_.size(), Length);
	ASSERT_EQ(calcLength(list_), Length);
	int array[Length] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	assertListMatchesArray(list_, array);
}

TEST_F(ListOperationsTest, RemoveOddNumbers)
{
	printf("Removing all odd numbers from the list\n");
	list_.removeIf(isOdd);
	printList(list_);

	ASSERT_FALSE(list_.isEmpty());
	ASSERT_EQ(list_.size(), 6);

	for (nctl::List<int>::ConstIterator i = list_.begin(); i != list_.end(); ++i)
		ASSERT_EQ(*i % 2, 0);
}

TEST_F(ListOperationsTest, RemoveEvenNumbers)
{
	printf("Removing all even numbers from the list\n");
	list_.removeIf(isEven);
	printList(list_);

	ASSERT_FALSE(list_.isEmpty());
	ASSERT_EQ(list_.size(), 5);

	for (nctl::List<int>::ConstIterator i = list_.begin(); i != list_.end(); ++i)
		ASSERT_EQ(*i % 2, 1);
}

TEST_F(ListOperationsTest, RemoveAllWithIterators)
{
	printf("Removing all list nodes through iterators\n");
	list_.erase(list_.begin(), list_.end());
	printList(list_);

	ASSERT_TRUE(list_.isEmpty());
	ASSERT_EQ(list_.size(), 0);
}

TEST_F(ListOperationsTest, RemoveHeadWithIterators)
{
	printf("Removing first node through iterator\n");
	list_.erase(list_.begin());
	printList(list_);

	ASSERT_EQ(list_.size(), Length - 1);
	int array[Length - 1] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	assertListMatchesArray(list_, array);
}

TEST_F(ListOperationsTest, RemoveTailWithIterators)
{
	printf("Removing last node through iterator\n");
	list_.erase(--list_.end());
	printList(list_);

	ASSERT_EQ(list_.size(), Length - 1);
	int array[Length - 1] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	assertListMatchesArray(list_, array);
}

TEST_F(ListOperationsTest, CopyAllWithIterators)
{
	printf("Inserting all list nodes into a new one through iterators\n");
	nctl::List<int> newList;
	newList.insert(newList.begin(), list_.begin(), list_.end());
	printList(newList);
	printf("\n");

	assertListsAreEqual(list_, newList);
}

TEST_F(ListOperationsTest, SpliceOneNodeToEmptyList)
{
	printf("Splicing the sixth node of the list into a new empty one\n");
	nctl::List<int> newList;
	newList.splice(newList.begin(), list_, nctl::next(list_.begin(), 5));
	printList("List 1 - ", list_);
	printList("List 2 - ", newList);
	printf("\n");

	ASSERT_EQ(list_.size(), Length - 1);
	ASSERT_EQ(newList.size(), 1);
	ASSERT_EQ(list_.size(), calcLength(list_));
	ASSERT_EQ(newList.size(), calcLength(newList));

	int array[Length - 1] = { 0, 1, 2, 3, 4, 6, 7, 8, 9, 10 };
	assertListMatchesArray(list_, array);
	int newArray[1] = { 5 };
	assertListMatchesArray(newList, newArray);
}

TEST_F(ListOperationsTest, SpliceOneNodeToListBeginning)
{
	printf("Splicing the sixth node of the list at the beginning of a new non-empty one\n");
	nctl::List<int> newList;
	newList.pushBack(11);
	newList.pushBack(12);
	newList.splice(newList.begin(), list_, nctl::next(list_.begin(), 5));
	printList("List 1 - ", list_);
	printList("List 2 - ", newList);
	printf("\n");

	ASSERT_EQ(list_.size(), Length - 1);
	ASSERT_EQ(newList.size(), 3);
	ASSERT_EQ(list_.size(), calcLength(list_));
	ASSERT_EQ(newList.size(), calcLength(newList));

	int array[Length - 1] = { 0, 1, 2, 3, 4, 6, 7, 8, 9, 10 };
	assertListMatchesArray(list_, array);
	int newArray[3] = { 5, 11, 12 };
	assertListMatchesArray(newList, newArray);
}

TEST_F(ListOperationsTest, SpliceOneNodeToListEnd)
{
	printf("Splicing the sixth node of the list at the end of a new non-empty one\n");
	nctl::List<int> newList;
	newList.pushBack(-2);
	newList.pushBack(-1);
	newList.splice(newList.end(), list_, nctl::next(list_.begin(), 5));
	printList("List 1 - ", list_);
	printList("List 2 - ", newList);
	printf("\n");

	ASSERT_EQ(list_.size(), Length - 1);
	ASSERT_EQ(newList.size(), 3);
	ASSERT_EQ(list_.size(), calcLength(list_));
	ASSERT_EQ(newList.size(), calcLength(newList));

	int array[Length - 1] = { 0, 1, 2, 3, 4, 6, 7, 8, 9, 10 };
	assertListMatchesArray(list_, array);
	int newArray[3] = { -2, -1, 5 };
	assertListMatchesArray(newList, newArray);
}

TEST_F(ListOperationsTest, SpliceOneNodeToList)
{
	printf("Splicing the sixth node of the list into a new non-empty one\n");
	nctl::List<int> newList;
	newList.pushBack(0);
	newList.pushBack(1);
	newList.splice(nctl::next(newList.begin()), list_, nctl::next(list_.begin(), 5));
	printList("List 1 - ", list_);
	printList("List 2 - ", newList);
	printf("\n");

	ASSERT_EQ(list_.size(), Length - 1);
	ASSERT_EQ(newList.size(), 3);
	ASSERT_EQ(list_.size(), calcLength(list_));
	ASSERT_EQ(newList.size(), calcLength(newList));

	int array[Length - 1] = { 0, 1, 2, 3, 4, 6, 7, 8, 9, 10 };
	assertListMatchesArray(list_, array);
	int newArray[3] = { 0, 5, 1 };
	assertListMatchesArray(newList, newArray);
}

TEST_F(ListOperationsTest, SpliceOneNodeFromEmptyList)
{
	printf("Splicing a node from a new empty list into the first one\n");
	nctl::List<int> newList;
	list_.splice(list_.begin(), newList, newList.begin());
	printList("List 1 - ", list_);
	printList("List 2 - ", newList);
	printf("\n");

	ASSERT_EQ(list_.size(), Length);
	ASSERT_EQ(newList.size(), 0);
	ASSERT_TRUE(newList.isEmpty());
	ASSERT_EQ(list_.size(), calcLength(list_));
	ASSERT_EQ(newList.size(), calcLength(newList));

	int array[Length] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	assertListMatchesArray(list_, array);
}

TEST_F(ListOperationsTest, SpliceAllNodesToEmptyList)
{
	printf("Splicing all list nodes into a new empty one\n");
	nctl::List<int> newList;
	newList.splice(newList.begin(), list_);
	printList("List 1 - ", list_);
	printList("List 2 - ", newList);
	printf("\n");

	ASSERT_TRUE(list_.isEmpty());
	ASSERT_EQ(newList.size(), Length);
	ASSERT_EQ(list_.size(), calcLength(list_));
	ASSERT_EQ(newList.size(), calcLength(newList));

	int array[Length] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	assertListMatchesArray(newList, array);
}

TEST_F(ListOperationsTest, SpliceAllNodesToListBeginning)
{
	printf("Splicing all list nodes at the beginning of a new non-empty one\n");
	nctl::List<int> newList;
	newList.pushBack(11);
	newList.pushBack(12);
	newList.splice(newList.begin(), list_);
	printList("List 1 - ", list_);
	printList("List 2 - ", newList);
	printf("\n");

	ASSERT_TRUE(list_.isEmpty());
	ASSERT_EQ(newList.size(), Length + 2);
	ASSERT_EQ(list_.size(), calcLength(list_));
	ASSERT_EQ(newList.size(), calcLength(newList));

	int array[Length + 2] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
	assertListMatchesArray(newList, array);
}

TEST_F(ListOperationsTest, SpliceAllNodesToListEnd)
{
	printf("Splicing all list nodes at the end of a new non-empty one\n");
	nctl::List<int> newList;
	newList.pushBack(-2);
	newList.pushBack(-1);
	newList.splice(newList.end(), list_);
	printList("List 1 - ", list_);
	printList("List 2 - ", newList);
	printf("\n");

	ASSERT_TRUE(list_.isEmpty());
	ASSERT_EQ(newList.size(), Length + 2);
	ASSERT_EQ(list_.size(), calcLength(list_));
	ASSERT_EQ(newList.size(), calcLength(newList));

	int array[Length + 2] = { -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	assertListMatchesArray(newList, array);
}

TEST_F(ListOperationsTest, SpliceAllNodesToList)
{
	printf("Splicing all list nodes into a new non-empty one\n");
	nctl::List<int> newList;
	newList.pushBack(-1);
	newList.pushBack(11);
	newList.splice(nctl::next(newList.begin()), list_);
	printList("List 1 - ", list_);
	printList("List 2 - ", newList);
	printf("\n");

	ASSERT_TRUE(list_.isEmpty());
	ASSERT_EQ(newList.size(), Length + 2);
	ASSERT_EQ(list_.size(), calcLength(list_));
	ASSERT_EQ(newList.size(), calcLength(newList));

	int array[Length + 2] = { -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
	assertListMatchesArray(newList, array);
}

TEST_F(ListOperationsTest, SpliceAllNodesFromEmptyList)
{
	printf("Splicing all nodes from a new empty list into the first one\n");
	nctl::List<int> newList;
	list_.splice(list_.begin(), newList);
	printList("List 1 - ", list_);
	printList("List 2 - ", newList);
	printf("\n");

	ASSERT_EQ(list_.size(), Length);
	ASSERT_EQ(newList.size(), 0);
	ASSERT_TRUE(newList.isEmpty());
	ASSERT_EQ(list_.size(), calcLength(list_));
	ASSERT_EQ(newList.size(), calcLength(newList));

	int array[Length] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	assertListMatchesArray(list_, array);
}

TEST_F(ListOperationsTest, SpliceRangeToEmptyList)
{
	printf("Splicing a range of nodes into a new empty one\n");
	nctl::List<int> newList;
	newList.splice(newList.begin(), list_, nctl::next(list_.begin(), 2), nctl::prev(list_.end(), 4));
	printList("List 1 - ", list_);
	printList("List 2 - ", newList);
	printf("\n");

	ASSERT_EQ(list_.size(), Length - 5);
	ASSERT_EQ(newList.size(), 5);
	ASSERT_EQ(list_.size(), calcLength(list_));
	ASSERT_EQ(newList.size(), calcLength(newList));

	int array[Length - 5] = { 0, 1, 7, 8, 9, 10 };
	assertListMatchesArray(list_, array);
	int newArray[5] = { 2, 3, 4, 5, 6 };
	assertListMatchesArray(newList, newArray);
}

TEST_F(ListOperationsTest, SpliceRangeToListBeginning)
{
	printf("Splicing a range of nodes at the beginning of a new non-empty one\n");
	nctl::List<int> newList;
	newList.pushBack(11);
	newList.pushBack(12);
	newList.splice(newList.begin(), list_, nctl::next(list_.begin(), 2), nctl::prev(list_.end(), 4));
	printList("List 1 - ", list_);
	printList("List 2 - ", newList);
	printf("\n");

	ASSERT_EQ(list_.size(), Length - 5);
	ASSERT_EQ(newList.size(), 5 + 2);
	ASSERT_EQ(list_.size(), calcLength(list_));
	ASSERT_EQ(newList.size(), calcLength(newList));

	int array[Length - 5] = { 0, 1, 7, 8, 9, 10 };
	assertListMatchesArray(list_, array);
	int newArray[5 + 2] = { 2, 3, 4, 5, 6, 11, 12 };
	assertListMatchesArray(newList, newArray);
}

TEST_F(ListOperationsTest, SpliceRangeToListEnd)
{
	printf("Splicing a range of nodes at the end of a new non-empty one\n");
	nctl::List<int> newList;
	newList.pushBack(-2);
	newList.pushBack(-1);
	newList.splice(newList.end(), list_, nctl::next(list_.begin(), 2), nctl::prev(list_.end(), 4));
	printList("List 1 - ", list_);
	printList("List 2 - ", newList);
	printf("\n");

	ASSERT_EQ(list_.size(), Length - 5);
	ASSERT_EQ(newList.size(), 5 + 2);
	ASSERT_EQ(list_.size(), calcLength(list_));
	ASSERT_EQ(newList.size(), calcLength(newList));

	int array[Length - 5] = { 0, 1, 7, 8, 9, 10 };
	assertListMatchesArray(list_, array);
	int newArray[5 + 2] = { -2, -1, 2, 3, 4, 5, 6 };
	assertListMatchesArray(newList, newArray);
}

TEST_F(ListOperationsTest, SpliceRangeToList)
{
	printf("Splicing a range of nodes into a new non-empty one\n");
	nctl::List<int> newList;
	newList.pushBack(-1);
	newList.pushBack(11);
	newList.splice(nctl::next(newList.begin()), list_, nctl::next(list_.begin(), 2), nctl::prev(list_.end(), 4));
	printList("List 1 - ", list_);
	printList("List 2 - ", newList);
	printf("\n");

	ASSERT_EQ(list_.size(), Length - 5);
	ASSERT_EQ(newList.size(), 5 + 2);
	ASSERT_EQ(list_.size(), calcLength(list_));
	ASSERT_EQ(newList.size(), calcLength(newList));

	int array[Length - 5] = { 0, 1, 7, 8, 9, 10 };
	assertListMatchesArray(list_, array);
	int newArray[5 + 2] = { -1, 2, 3, 4, 5, 6, 11 };
	assertListMatchesArray(newList, newArray);
}

}
