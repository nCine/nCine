#include "gtest_list.h"

namespace {

class ListTest : public ::testing::Test
{
  public:
	ListTest() { }

  protected:
	void SetUp() { initList(list_); }

	nc::List<int> list_;
};

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
	list_.erase(list_.rBegin());
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
	list_.erase(list_.rBegin());
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

	for (nc::List<int>::ConstIterator i = list_.begin(); i != list_.end(); ++i)
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

}
