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
TEST(StaticArrayOperationsDeathTest, UnorderedRemoveRangeBeyondSize)
{
	printf("Trying to remove a range beyond size\n");
	nctl::StaticArray<int, Capacity> array;
	array.pushBack(0);

	ASSERT_DEATH(array.unorderedRemoveRange(1, 5), "");
}

TEST(StaticArrayOperationsDeathTest, UnorderedRemoveAtBeyondSize)
{
	printf("Trying to remove an element beyond size\n");
	nctl::StaticArray<int, Capacity> array;
	array.pushBack(0);

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

TEST_F(StaticArrayOperationsTest, SelfAssignment)
{
	printf("Assigning the array to itself with the assignment operator\n");
	array_ = array_;
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	ASSERT_TRUE(isUnmodified(array_));
}

TEST_F(StaticArrayOperationsTest, RemoveFirstWithIterator)
{
	printf("Removing the first element with an iterator\n");
	array_.erase(array_.begin());
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity - 1);
	ASSERT_EQ(array_[0], 1);
}

TEST_F(StaticArrayOperationsTest, UnorderedRemoveFirstWithIterator)
{
	printf("Removing the first element with an iterator (unordered)\n");
	array_.unorderedErase(array_.begin());
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity - 1);
	ASSERT_EQ(array_[0], 9);
}

TEST_F(StaticArrayOperationsTest, RemoveLastWithIterator)
{
	printf("Removing the last element with an iterator\n");
	array_.erase(array_.end() - 1);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity - 1);
	ASSERT_EQ(array_[array_.size() - 1], 8);
}

TEST_F(StaticArrayOperationsTest, UnorderedRemoveLastWithIterator)
{
	printf("Removing the last element with an iterator (unordered)\n");
	array_.unorderedErase(array_.end() - 1);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity - 1);
	ASSERT_EQ(array_[array_.size() - 1], 8);
}

TEST_F(StaticArrayOperationsTest, RemoveFirstHalfWithIterator)
{
	printf("Removing the first half of the array\n");
	array_.erase(array_.begin(), array_.begin() + array_.size() / 2);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity / 2);
	ASSERT_EQ(array_[0], 5);
	ASSERT_EQ(array_[array_.size() - 1], 9);
}

TEST_F(StaticArrayOperationsTest, UnorderedRemoveFirstHalfWithIterator)
{
	printf("Removing the first half of the array (unordered)\n");
	array_.unorderedErase(array_.begin(), array_.begin() + array_.size() / 2);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity / 2);
	ASSERT_EQ(array_[0], 9);
	ASSERT_EQ(array_[array_.size() - 1], 5);
}

TEST_F(StaticArrayOperationsTest, RemoveSecondHalfWithIterator)
{
	printf("Removing the second half of the array\n");
	array_.erase(array_.begin() + array_.size() / 2, array_.end());
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity / 2);
	ASSERT_EQ(array_[0], 0);
	ASSERT_EQ(array_[array_.size() - 1], 4);
}

TEST_F(StaticArrayOperationsTest, UnorderedRemoveSecondHalfWithIterator)
{
	printf("Removing the second half of the array (unordered)\n");
	array_.unorderedErase(array_.begin() + array_.size() / 2, array_.end());
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity / 2);
	ASSERT_EQ(array_[0], 0);
	ASSERT_EQ(array_[array_.size() - 1], 4);
}

TEST_F(StaticArrayOperationsTest, RemoveFirstHalfWithReturnedIterator)
{
	nctl::StaticArray<int, Capacity>::Iterator it = array_.begin();

	printf("Removing the first half of the array with the returned iterator\n");
	for (unsigned int i = 0; i < Capacity / 2; i++)
		it = array_.erase(it);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity / 2);
	ASSERT_EQ(array_[0], 5);
	ASSERT_EQ(array_[array_.size() - 1], 9);
}

TEST_F(StaticArrayOperationsTest, UnorderedRemoveFirstHalfWithReturnedIterator)
{
	nctl::StaticArray<int, Capacity>::Iterator it = array_.begin();

	printf("Removing the first half of the array with the returned iterator (unordered)\n");
	for (unsigned int i = 0; i < Capacity / 2; i++)
		it = array_.unorderedErase(it);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity / 2);
	ASSERT_EQ(array_[0], 9);
	ASSERT_EQ(array_[array_.size() - 1], 5);
}

TEST_F(StaticArrayOperationsTest, RemoveRangeWithReturnedIterator)
{
	nctl::StaticArray<int, Capacity>::Iterator it = array_.begin();

	printf("Removing a range of elements of the array with the returned iterator\n");
	for (unsigned int i = 0; i < Capacity / 2; i++)
		it = array_.erase(it, it + 1);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity / 2);
	ASSERT_EQ(array_[0], 5);
	ASSERT_EQ(array_[array_.size() - 1], 9);
}

TEST_F(StaticArrayOperationsTest, UnorderedRemoveRangeWithReturnedIterator)
{
	nctl::StaticArray<int, Capacity>::Iterator it = array_.begin();

	printf("Removing a range of elements of the array with the returned iterator (unordered)\n");
	for (unsigned int i = 0; i < Capacity / 2; i++)
		it = array_.unorderedErase(it, it + 1);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity / 2);
	ASSERT_EQ(array_[0], 9);
	ASSERT_EQ(array_[array_.size() - 1], 5);
}

TEST_F(StaticArrayOperationsTest, RemoveEmptyBeginRange)
{
	printf("Removing an empty begin-to-begin range of the array\n");
	array_.erase(array_.begin(), array_.begin());
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	ASSERT_TRUE(isUnmodified(array_));
}

TEST_F(StaticArrayOperationsTest, UnorderedRemoveEmptyBeginRange)
{
	printf("Removing an empty begin-to-begin range of the array (unordered)\n");
	array_.unorderedErase(array_.begin(), array_.begin());
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	ASSERT_TRUE(isUnmodified(array_));
}

TEST_F(StaticArrayOperationsTest, RemoveEmptyEndRange)
{
	printf("Removing an empty end-to-end range of the array\n");
	array_.erase(array_.end() - 1, array_.end() - 1);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	ASSERT_TRUE(isUnmodified(array_));
}

TEST_F(StaticArrayOperationsTest, UnorderedRemoveEmptyEndRange)
{
	printf("Removing an empty end-to-end range of the array (unordered)\n");
	array_.unorderedErase(array_.end() - 1, array_.end() - 1);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	ASSERT_TRUE(isUnmodified(array_));
}

TEST_F(StaticArrayOperationsTest, InsertValueAtFrontWithIterator)
{
	// Making space for the element to insert
	array_.popBack();
	printf("Inserting a single value at the beginning of the array\n");
	array_.insert(array_.begin(), -1);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	ASSERT_EQ(array_[0], -1);
	ASSERT_EQ(array_[1], 0);
	ASSERT_EQ(array_[array_.size() - 1], 8);
}

TEST_F(StaticArrayOperationsTest, InsertAtFrontWithIterator)
{
	nctl::StaticArray<int, Capacity> newArray;
	initArray(newArray);

	// Making space for the elements to insert
	array_.erase(array_.begin(), array_.begin() + array_.size() / 2);
	printf("Inserting the first half of a new array at the beginning of the first one\n");
	array_.insert(array_.begin(), newArray.begin(), newArray.begin() + newArray.size() / 2);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	ASSERT_TRUE(isUnmodified(array_));
}

TEST_F(StaticArrayOperationsTest, InsertAtBackWithIterator)
{
	nctl::StaticArray<int, Capacity> newArray;
	initArray(newArray);

	// Making space for the elements to insert
	array_.erase(array_.begin() + array_.size() / 2, array_.end());
	printf("Inserting the second half of a new array at the end of the first one\n");
	array_.insert(array_.end(), newArray.begin() + newArray.size() / 2, newArray.end());
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	ASSERT_TRUE(isUnmodified(array_));
}

TEST_F(StaticArrayOperationsTest, InsertWithReturnedIterator)
{
	nctl::StaticArray<int, Capacity> newArray;
	nctl::StaticArray<int, Capacity>::Iterator it = newArray.begin();

	printf("Inserting elements in a new array with the returned iterator\n");
	for (unsigned int i = 0; i < Capacity; i++)
		it = newArray.insert(it, static_cast<int>(i));
	printArray(newArray);

	ASSERT_EQ(newArray.size(), Capacity);
	for (unsigned int i = 0; i < newArray.size(); i++)
		ASSERT_EQ(newArray[i], static_cast<int>(i));
}

TEST_F(StaticArrayOperationsTest, InsertRangeWithReturnedIterator)
{
	nctl::StaticArray<int, Capacity> newArray;
	nctl::StaticArray<int, Capacity>::Iterator it = newArray.begin();

	printf("Inserting a range of elements in a new array with the returned iterator\n");
	for (unsigned int i = 0; i < Capacity; i += 2)
		it = newArray.insert(it, array_.begin() + i, array_.begin() + i + 2);
	printArray(newArray);

	ASSERT_EQ(newArray.size(), Capacity);
	for (unsigned int i = 0; i < newArray.size(); i++)
		ASSERT_EQ(newArray[i], static_cast<int>(i));
}

TEST_F(StaticArrayOperationsTest, InsertEmptyBeginRangeAtFrontWithIterator)
{
	nctl::StaticArray<int, Capacity> newArray;
	initArray(newArray);

	printf("Inserting an empty begin-to-begin range at the beginning of the array\n");
	array_.insert(array_.begin(), newArray.begin(), newArray.begin());
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	ASSERT_TRUE(isUnmodified(array_));
}

TEST_F(StaticArrayOperationsTest, InsertEmptyEndRangeAtBackWithIterator)
{
	nctl::StaticArray<int, Capacity> newArray;
	initArray(newArray);

	printf("Inserting an empty end-to-end range at the end of the array\n");
	array_.insert(array_.end(), newArray.end(), newArray.end());
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	ASSERT_TRUE(isUnmodified(array_));
}

}
