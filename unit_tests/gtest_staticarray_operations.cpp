#include "gtest_staticarray.h"

namespace {

class StaticArrayOperationsTest : public ::testing::Test
{
  protected:
	void SetUp() override { initArray(array_); }

	nctl::StaticArray<int, Capacity> array_;
};

#ifndef __EMSCRIPTEN__
	#ifdef NCINE_DEBUG
TEST(StaticArrayOperationsDeathTest, UnorderedRemoveBeyondSize)
{
	printf("Trying to remove an element beyond size\n");
	nctl::StaticArray<int, Capacity> array;
	array[0] = 0;

	ASSERT_DEATH(array.unorderedRemoveAt(5), "");
}
	#endif
#endif

TEST_F(StaticArrayOperationsTest, Clear)
{
	ASSERT_FALSE(array_.isEmpty());
	array_.clear();
	printArray(array_);
	ASSERT_TRUE(array_.isEmpty());
	ASSERT_EQ(array_.size(), 0u);
	ASSERT_EQ(array_.capacity(), Capacity);
}

TEST_F(StaticArrayOperationsTest, CopyConstruction)
{
	printf("Creating a new array with copy construction\n");
	nctl::StaticArray<int, Capacity> newArray(array_);
	printArray(newArray);

	ASSERT_EQ(array_.size(), newArray.size());
	for (unsigned int i = 0; i < array_.size(); i++)
		ASSERT_EQ(array_[i], newArray[i]);
}

TEST_F(StaticArrayOperationsTest, MoveConstruction)
{
	printf("Creating a new array with move construction\n");
	nctl::StaticArray<int, Capacity> newArray(std::move(array_));
	printArray(newArray);

	ASSERT_EQ(array_.size(), 0);
	ASSERT_EQ(newArray.size(), Capacity);
	for (unsigned int i = 0; i < newArray.size(); i++)
		ASSERT_EQ(newArray[i], i);
}

TEST_F(StaticArrayOperationsTest, AssignmentOperator)
{
	printf("Creating a new array with the assignment operator\n");
	nctl::StaticArray<int, Capacity> newArray;
	newArray = array_;
	printArray(newArray);

	ASSERT_EQ(array_.size(), newArray.size());
	for (unsigned int i = 0; i < array_.size(); i++)
		ASSERT_EQ(array_[i], newArray[i]);
}

TEST_F(StaticArrayOperationsTest, MoveAssignmentOperator)
{
	printf("Creating a new array with the move assignment operator\n");
	nctl::StaticArray<int, Capacity> newArray;
	newArray = nctl::move(array_);
	printArray(newArray);

	ASSERT_EQ(newArray.size(), Capacity);
	ASSERT_EQ(array_.size(), 0);
	for (unsigned int i = 0; i < newArray.size(); i++)
		ASSERT_EQ(newArray[i], i);
}

TEST_F(StaticArrayOperationsTest, PushBack)
{
	nctl::StaticArray<int, Capacity> newArray;

	printf("Inserting twice at the back\n");
	newArray.pushBack(1);
	newArray.pushBack(2);
	printArray(newArray);

	ASSERT_EQ(newArray.size(), 2);
	ASSERT_EQ(newArray[0], 1);
	ASSERT_EQ(newArray[1], 2);
}

TEST_F(StaticArrayOperationsTest, UnorderedRemoveFirst)
{
	printf("Removing the first element (unordered)\n");
	array_.unorderedRemoveAt(0);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity - 1);
	ASSERT_EQ(array_[0], 9);
}

TEST_F(StaticArrayOperationsTest, UnorderedRemoveLast)
{
	printf("Removing the last element (unordered)\n");
	array_.unorderedRemoveAt(array_.size() - 1);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity - 1);
	ASSERT_EQ(array_[array_.size() - 1], 8);
}

TEST_F(StaticArrayOperationsTest, UnorderedRemoveFirstHalfWithReturnedIndex)
{
	int index = 0;

	printf("Removing the first half of the array with the returned index (unordered)\n");
	for (unsigned int i = 0; i < Capacity / 2; i++)
		index = array_.unorderedRemoveAt(index);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity / 2);
	ASSERT_EQ(array_[0], 9);
	ASSERT_EQ(array_[array_.size() - 1], 5);
}

}
