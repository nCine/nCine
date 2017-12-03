#include "gtest_staticarray.h"

namespace {

class StaticArrayOperationsTest : public ::testing::Test
{
  protected:
	void SetUp() override { initArray(array_); }

	nctl::StaticArray<int, Capacity> array_;
};

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

TEST_F(StaticArrayOperationsTest, AssignmentOperator)
{
	printf("Creating a new array with the assignment operator\n");
	nctl::StaticArray<int, Capacity> newArray = array_;
	printArray(newArray);

	ASSERT_EQ(array_.size(), newArray.size());
	for (unsigned int i = 0; i < array_.size(); i++)
		ASSERT_EQ(array_[i], newArray[i]);
}

TEST_F(StaticArrayOperationsTest, InsertAtBackWithIterator)
{
	nctl::StaticArray<int, Capacity> newArray;

	printf("Inserting twice at the back\n");
	newArray.insertBack(1);
	newArray.insertBack(2);
	printArray(newArray);

	ASSERT_EQ(newArray.size(), 2);
	ASSERT_EQ(newArray[0], 1);
	ASSERT_EQ(newArray[1], 2);
}

}
