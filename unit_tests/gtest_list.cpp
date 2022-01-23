#include "gtest_list.h"

namespace {

class ListTest : public ::testing::Test
{
  public:
	ListTest() {}

  protected:
	void SetUp() override { initList(list_); }

	nctl::List<int> list_;
};

#ifndef __EMSCRIPTEN__
TEST(ListDeathTest, FrontElementFromEmptyArray)
{
	nctl::List<int> list;
	printf("Retrieving the front element from an empty list\n");

	ASSERT_EQ(list.size(), 0);
	ASSERT_DEATH(list.front(), "");
}

TEST(ListDeathTest, FrontConstElementFromEmptyArray)
{
	const nctl::List<int> list;
	printf("Retrieving the front element from an empty const list\n");

	ASSERT_EQ(list.size(), 0);
	ASSERT_DEATH(list.front(), "");
}

TEST(ListDeathTest, BackElementFromEmptyArray)
{
	nctl::List<int> list;
	printf("Retrieving the back element from an empty list\n");

	ASSERT_EQ(list.size(), 0);
	ASSERT_DEATH(list.back(), "");
}

TEST(ListDeathTest, BackConstElementFromEmptyArray)
{
	const nctl::List<int> list;
	printf("Retrieving the back element from an empty const list\n");

	ASSERT_EQ(list.size(), 0);
	ASSERT_DEATH(list.back(), "");
}
#endif

TEST_F(ListTest, PushFront)
{
	printf("Clearing the list\n");
	list_.clear();

	printf("Inserting some nodes to the front\n");
	for (int i = LastElement; i > -1; i--)
		list_.pushFront(i);
	printList(list_);

	ASSERT_FALSE(list_.isEmpty());
	ASSERT_EQ(list_.size(), Length);
	ASSERT_TRUE(isUnmodified(list_));
}

TEST_F(ListTest, EmplaceFront)
{
	printf("Clearing the list\n");
	list_.clear();

	printf("Emplacing some nodes to the front\n");
	for (int i = LastElement; i > -1; i--)
		list_.emplaceFront(i);
	printList(list_);

	ASSERT_FALSE(list_.isEmpty());
	ASSERT_EQ(list_.size(), Length);
	ASSERT_TRUE(isUnmodified(list_));
}

TEST_F(ListTest, PushBack)
{
	printf("Clearing the list\n");
	list_.clear();

	printf("Inserting some nodes to the back\n");
	for (unsigned int i = 0; i < Length; i++)
		list_.pushBack(i);
	printList(list_);

	ASSERT_FALSE(list_.isEmpty());
	ASSERT_EQ(list_.size(), Length);
	ASSERT_TRUE(isUnmodified(list_));
}

TEST_F(ListTest, EmplaceBack)
{
	printf("Clearing the list\n");
	list_.clear();

	printf("Emplacing some nodes to the back\n");
	for (unsigned int i = 0; i < Length; i++)
		list_.emplaceBack(i);
	printList(list_);

	ASSERT_FALSE(list_.isEmpty());
	ASSERT_EQ(list_.size(), Length);
	ASSERT_TRUE(isUnmodified(list_));
}

TEST_F(ListTest, FrontAndBack)
{
	printf("Retrieving the value of the node at the front and the node in the back\n");
	const int front = list_.front();
	const int back = list_.back();
	printList(list_);

	ASSERT_FALSE(list_.isEmpty());
	ASSERT_EQ(list_.size(), Length);
	ASSERT_TRUE(isUnmodified(list_));

	ASSERT_EQ(front, 0);
	ASSERT_EQ(back, LastElement);
	ASSERT_EQ(front, list_.front());
	ASSERT_EQ(back, list_.back());
}

TEST_F(ListTest, PushFrontAndBack)
{
	printf("Inserting one node at the front and one at the back\n");
	list_.pushFront(-1);
	list_.pushBack(-1);
	printList(list_);

	ASSERT_FALSE(list_.isEmpty());
	ASSERT_EQ(list_.size(), Length + 2);
}

TEST_F(ListTest, EmplaceFrontAndBack)
{
	printf("Emplacing one node at the front and one at the back\n");
	list_.emplaceFront(-1);
	list_.emplaceBack(-1);
	printList(list_);

	ASSERT_FALSE(list_.isEmpty());
	ASSERT_EQ(list_.size(), Length + 2);
}

TEST_F(ListTest, PopFrontAndBack)
{
	printf("Popping a node from the front and one from the back\n");
	list_.popFront();
	list_.popBack();
	printList(list_);

	ASSERT_FALSE(list_.isEmpty());
	ASSERT_EQ(list_.size(), Length - 2);

	ASSERT_EQ(list_.front(), 1);
	ASSERT_EQ(list_.back(), LastElement - 1);
}

TEST_F(ListTest, RemoveFrontAndBackWithIterators)
{
	printf("Removing first and last node with iterators\n");
	list_.erase(list_.begin());
	list_.erase(--list_.end());
	printList(list_);

	ASSERT_FALSE(list_.isEmpty());
	ASSERT_EQ(list_.size(), Length - 2);

	ASSERT_EQ(list_.front(), 1);
	ASSERT_EQ(list_.back(), LastElement - 1);
}

TEST_F(ListTest, EraseFirstNodeOnEmpty)
{
	printf("Clearing the list\n");
	list_.clear();

	printf("Removing the first node of an empty list with iterators\n");
	list_.erase(list_.begin());
	printList(list_);

	ASSERT_TRUE(list_.isEmpty());
	ASSERT_EQ(list_.size(), 0);
}

TEST_F(ListTest, EraseLastNodeOnEmpty)
{
	printf("Clearing the list\n");
	list_.clear();

	printf("Removing the last node of an empty list with iterators\n");
	list_.erase(--list_.end());
	printList(list_);

	ASSERT_TRUE(list_.isEmpty());
	ASSERT_EQ(list_.size(), 0);
}

TEST_F(ListTest, RemoveTwoElements)
{
	printf("Removing a couple of nodes\n");
	list_.remove(3);
	list_.remove(5);
	printList(list_);

	ASSERT_FALSE(list_.isEmpty());
	ASSERT_EQ(list_.size(), Length - 2);
	ASSERT_EQ(list_.front(), FirstElement);
	ASSERT_EQ(list_.back(), LastElement);

	for (nctl::List<int>::ConstIterator i = list_.begin(); i != list_.end(); ++i)
	{
		ASSERT_NE(*i, 3);
		ASSERT_NE(*i, 5);
	}
}

TEST_F(ListTest, RemoveNonExistantElement)
{
	printf("Removing a non-existant node\n");
	list_.remove(100);
	printList(list_);

	ASSERT_FALSE(list_.isEmpty());
	ASSERT_EQ(list_.size(), Length);
	ASSERT_TRUE(isUnmodified(list_));
}

TEST_F(ListTest, InsertAfterBeginning)
{
	printf("Insert a node after the beginning of the list\n");
	list_.insertAfter(list_.begin(), 11);
	printList(list_);

	ASSERT_EQ(list_.size(), Length + 1);
	ASSERT_EQ(list_.size(), calcLength(list_));

	int array[Length + 1] = { 0, 11, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	assertListMatchesArray(list_, array);
}

TEST_F(ListTest, EmplaceAfterBeginning)
{
	printf("Emplace a node after the beginning of the list\n");
	list_.emplaceAfter(list_.begin(), 11);
	printList(list_);

	ASSERT_EQ(list_.size(), Length + 1);
	ASSERT_EQ(list_.size(), calcLength(list_));

	int array[Length + 1] = { 0, 11, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	assertListMatchesArray(list_, array);
}

TEST_F(ListTest, InsertBeforeBeginning)
{
	printf("Insert a node before the beginning of the list\n");
	list_.insertBefore(list_.begin(), -1);
	printList(list_);

	ASSERT_EQ(list_.size(), Length + 1);
	ASSERT_EQ(list_.size(), calcLength(list_));

	int array[Length + 1] = { -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	assertListMatchesArray(list_, array);
}

TEST_F(ListTest, EmplaceBeforeBeginning)
{
	printf("Emplace a node before the beginning of the list\n");
	list_.emplaceBefore(list_.begin(), -1);
	printList(list_);

	ASSERT_EQ(list_.size(), Length + 1);
	ASSERT_EQ(list_.size(), calcLength(list_));

	int array[Length + 1] = { -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	assertListMatchesArray(list_, array);
}

TEST_F(ListTest, InsertAfterEnd)
{
	printf("Insert a node after the end of the list\n");
	list_.insertAfter(list_.end(), -1);
	printList(list_);

	ASSERT_EQ(list_.size(), Length + 1);
	ASSERT_EQ(list_.size(), calcLength(list_));

	int array[Length + 1] = { -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	assertListMatchesArray(list_, array);
}

TEST_F(ListTest, EmplaceAfterEnd)
{
	printf("Emplace a node after the end of the list\n");
	list_.emplaceAfter(list_.end(), -1);
	printList(list_);

	ASSERT_EQ(list_.size(), Length + 1);
	ASSERT_EQ(list_.size(), calcLength(list_));

	int array[Length + 1] = { -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	assertListMatchesArray(list_, array);
}

TEST_F(ListTest, InsertBeforeEnd)
{
	printf("Insert a node before the end of the list\n");
	list_.insertBefore(list_.end(), 11);
	printList(list_);

	ASSERT_EQ(list_.size(), Length + 1);
	ASSERT_EQ(list_.size(), calcLength(list_));

	int array[Length + 1] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
	assertListMatchesArray(list_, array);
}

TEST_F(ListTest, EmplaceBeforeEnd)
{
	printf("Emplace a node before the end of the list\n");
	list_.emplaceBefore(list_.end(), 11);
	printList(list_);

	ASSERT_EQ(list_.size(), Length + 1);
	ASSERT_EQ(list_.size(), calcLength(list_));

	int array[Length + 1] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
	assertListMatchesArray(list_, array);
}

}
