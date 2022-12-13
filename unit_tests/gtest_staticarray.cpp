#include "gtest_staticarray.h"

namespace {

class StaticArrayTest : public ::testing::Test
{
  protected:
	void SetUp() override { initArray(array_); }

	nctl::StaticArray<int, Capacity> array_;
};

#ifndef __EMSCRIPTEN__
	#ifdef NCINE_DEBUG
TEST(StaticArrayDeathTest, SubscriptAccessBeyondSize)
{
	printf("Trying to access an element within capacity but beyond size\n");
	nctl::StaticArray<int, Capacity> array;
	array.pushBack(0);

	ASSERT_DEATH(array[5] = 1, "");
}

TEST(StaticArrayDeathTest, SubscriptAccessConstBeyondSize)
{
	printf("Trying to access an element of a const array within capacity but beyond size\n");
	nctl::StaticArray<int, Capacity> array;
	array.pushBack(0);

	const nctl::StaticArray<int, Capacity> &constArray = array;

	ASSERT_DEATH(constArray[5], "");
}

TEST(StaticArrayDeathTest, SubscriptAccessConstAtSize)
{
	printf("Trying to access an element of a const array within capacity but at size\n");
	nctl::StaticArray<int, Capacity> array;
	array.pushBack(0);

	const nctl::StaticArray<int, Capacity> &constArray = array;

	ASSERT_DEATH(constArray[1], "");
}
	#endif

TEST(StaticArrayDeathTest, AccessBeyondSize)
{
	printf("Trying to access an element within capacity but beyond size\n");
	nctl::StaticArray<int, Capacity> array;
	array.pushBack(0);

	ASSERT_DEATH(array.at(5) = 1, "");
}

TEST(StaticArrayDeathTest, AccessConstBeyondSize)
{
	printf("Trying to access an element of a const array within capacity but beyond size\n");
	nctl::StaticArray<int, Capacity> array;
	array.pushBack(0);

	const nctl::StaticArray<int, Capacity> &constArray = array;

	ASSERT_DEATH(constArray.at(5), "");
}

TEST(StaticArrayDeathTest, AccessConstAtSize)
{
	printf("Trying to access an element of a const array at size\n");
	nctl::StaticArray<int, Capacity> array;
	array.pushBack(0);

	const nctl::StaticArray<int, Capacity> &constArray = array;

	ASSERT_DEATH(constArray.at(1), "");
}

TEST(StaticArrayDeathTest, FrontElementFromEmptyArray)
{
	nctl::StaticArray<int, Capacity> array;
	printf("Retrieving the front element from an empty array\n");

	ASSERT_EQ(array.size(), 0);
	ASSERT_DEATH(array.front(), "");
}

TEST(StaticArrayDeathTest, FrontConstElementFromEmptyArray)
{
	const nctl::StaticArray<int, Capacity> array;
	printf("Retrieving the front element from an empty const array\n");

	ASSERT_EQ(array.size(), 0);
	ASSERT_DEATH(array.front(), "");
}

TEST(StaticArrayDeathTest, BackElementFromEmptyArray)
{
	nctl::StaticArray<int, Capacity> array;
	printf("Retrieving the back element from an empty array\n");

	ASSERT_EQ(array.size(), 0);
	ASSERT_DEATH(array.back(), "");
}

TEST(StaticArrayDeathTest, BackConstElementFromEmptyArray)
{
	const nctl::StaticArray<int, Capacity> array;
	printf("Retrieving the back element from an empty const array\n");

	ASSERT_EQ(array.size(), 0);
	ASSERT_DEATH(array.back(), "");
}

TEST(StaticArrayDeathTest, PushBackBeyondCapacity)
{
	printf("Trying to push back an element beyond the capacity of a static array\n");
	nctl::StaticArray<int, Capacity> array;
	for (unsigned int i = 0; i < Capacity; i++)
		array.pushBack(i);

	ASSERT_EQ(array.size(), Capacity);
	ASSERT_DEATH(array.pushBack(0), "");
}

TEST(StaticArrayDeathTest, EmplaceBackBeyondCapacity)
{
	printf("Trying to emplace back an element beyond the capacity of a static array\n");
	nctl::StaticArray<int, Capacity> array;
	for (unsigned int i = 0; i < Capacity; i++)
		array.emplaceBack(i);

	ASSERT_EQ(array.size(), Capacity);
	ASSERT_DEATH(array.emplaceBack(0), "");
}

TEST(StaticArrayDeathTest, PopBackEmpty)
{
	printf("Removing at the back of an empty array\n");
	nctl::StaticArray<int, Capacity> array;

	ASSERT_EQ(array.size(), 0);
	ASSERT_DEATH(array.popBack(), "");
}
#endif

TEST_F(StaticArrayTest, AccessWithinSize)
{
	printf("Accessing an element within the array size\n");

	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_EQ(array_.at(i), static_cast<int>(i));
}

TEST_F(StaticArrayTest, AccessConstWithinSize)
{
	printf("Accessing an element within the const array size\n");

	const nctl::StaticArray<int, Capacity> &constArray = array_;
	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_EQ(constArray.at(i), static_cast<int>(i));
}

TEST_F(StaticArrayTest, AccessWithPointer)
{
	printf("Accessing an array element with a pointer\n");

	int *ptr = array_.data();
	for (unsigned int i = 0; i < Capacity; i++)
		ASSERT_EQ(ptr[i], static_cast<int>(i));
}

TEST_F(StaticArrayTest, SetSizeOnStatic)
{
	printf("Trying to extend the size of a static array\n");
	nctl::StaticArray<int, 1> array;
	array.pushBack(0);
	array.setSize(Capacity);

	ASSERT_EQ(array.capacity(), 1);
	ASSERT_EQ(array.size(), 1);
}

TEST_F(StaticArrayTest, SetSizeToShrink)
{
	nctl::StaticArray<int, Capacity> array;
	array.pushBack(0);
	array.pushBack(1);
	printf("Trying to shrink the size of a static array\n");
	array.setSize(1);

	ASSERT_EQ(array.capacity(), Capacity);
	ASSERT_EQ(array.size(), 1);
}

TEST_F(StaticArrayTest, SetSizeAsCapacity)
{
	printf("Trying to extend the size of a static array to its capacity\n");
	nctl::StaticArray<int, Capacity> array;
	array.setSize(Capacity);

	ASSERT_EQ(array.capacity(), Capacity);
	ASSERT_EQ(array.size(), Capacity);
}

TEST_F(StaticArrayTest, FrontElement)
{
	const int front = array_.front();
	printf("Retrieving the front element: %d\n", front);

	ASSERT_TRUE(isUnmodified(array_));
	ASSERT_EQ(array_.size(), Capacity);
	ASSERT_EQ(front, array_[0]);
}

TEST_F(StaticArrayTest, FrontConstElement)
{
	const nctl::StaticArray<int, Capacity> &constArray = array_;
	const int front = constArray.front();
	printf("Retrieving the front element from a const array: %d\n", front);

	ASSERT_TRUE(isUnmodified(constArray));
	ASSERT_EQ(constArray.size(), Capacity);
	ASSERT_EQ(front, constArray[0]);
}

TEST_F(StaticArrayTest, BackElement)
{
	const int back = array_.back();
	printf("Retrieving the back element: %d\n", back);

	ASSERT_TRUE(isUnmodified(array_));
	ASSERT_EQ(array_.size(), Capacity);
	ASSERT_EQ(back, array_[array_.size() - 1]);
}

TEST_F(StaticArrayTest, BackConstElement)
{
	const nctl::StaticArray<int, Capacity> &constArray = array_;
	const int back = constArray.back();
	printf("Retrieving the back element from a const array: %d\n", back);

	ASSERT_TRUE(isUnmodified(constArray));
	ASSERT_EQ(constArray.size(), Capacity);
	ASSERT_EQ(back, constArray[constArray.size() - 1]);
}

TEST_F(StaticArrayTest, PushBack)
{
	nctl::StaticArray<int, Capacity> newArray;

	printf("Inserting twice at the back\n");
	newArray.pushBack(0);
	newArray.pushBack(1);
	printArray(newArray);

	ASSERT_EQ(newArray.size(), 2);
	ASSERT_EQ(newArray[0], 0);
	ASSERT_EQ(newArray[1], 1);
}

TEST_F(StaticArrayTest, EmplaceBack)
{
	nctl::StaticArray<int, Capacity> newArray;

	printf("Emplacing twice at the back\n");
	newArray.emplaceBack(0);
	newArray.emplaceBack(1);
	printArray(newArray);

	ASSERT_EQ(newArray.size(), 2);
	ASSERT_EQ(newArray[0], 0);
	ASSERT_EQ(newArray[1], 1);
}

TEST_F(StaticArrayTest, PopBack)
{
	printf("Removing at the back\n");
	array_.popBack();
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity - 1);
	ASSERT_EQ(array_[array_.size() - 1], 8);
}

TEST_F(StaticArrayTest, OverwriteMiddle)
{
	printf("Overwriting in the middle\n");
	array_[2] = 22;
	printArray(array_);

	ASSERT_EQ(array_[2], 22);
	ASSERT_EQ(array_.size(), Capacity);
}

TEST_F(StaticArrayTest, InsertMiddle)
{
	// Making space for the element to insert
	array_.popBack();
	printf("Inserting in the middle\n");
	array_.insertAt(3, 22);
	printArray(array_);

	ASSERT_EQ(array_[2], 2);
	ASSERT_EQ(array_[3], 22);
	ASSERT_EQ(array_[4], 3);
	ASSERT_EQ(array_.size(), Capacity);
}

TEST_F(StaticArrayTest, EmplaceMiddle)
{
	// Making space for the element to emplace
	array_.popBack();
	printf("Emplacing in the middle\n");
	array_.emplaceAt(3, 22);
	printArray(array_);

	ASSERT_EQ(array_[2], 2);
	ASSERT_EQ(array_[3], 22);
	ASSERT_EQ(array_[4], 3);
	ASSERT_EQ(array_.size(), Capacity);
}

TEST_F(StaticArrayTest, RemoveMiddle)
{
	printf("Removing from the middle\n");
	array_.removeAt(3);
	printArray(array_);

	ASSERT_EQ(array_[2], 2);
	ASSERT_EQ(array_[3], 4);
	ASSERT_EQ(array_[4], 5);
	ASSERT_EQ(array_.size(), Capacity - 1);
}

TEST_F(StaticArrayTest, UnorderedRemoveMiddle)
{
	printf("Removing from the middle (unordered)\n");
	array_.unorderedRemoveAt(3);
	printArray(array_);

	ASSERT_EQ(array_[2], 2);
	ASSERT_EQ(array_[3], 9);
	ASSERT_EQ(array_[4], 4);
	ASSERT_EQ(array_.size(), Capacity - 1);
}

TEST_F(StaticArrayTest, InsertFirstAndLast)
{
	// Making space for the elements to insert
	array_.popBack();
	array_.popBack();
	printf("Inserting as first and last\n");
	array_.insertAt(0, -1);
	array_.insertAt(array_.size(), 8);
	printArray(array_);

	ASSERT_EQ(array_[0], -1);
	ASSERT_EQ(array_[1], 0);
	ASSERT_EQ(array_[8], 7);
	ASSERT_EQ(array_[9], 8);
	ASSERT_EQ(array_.size(), Capacity);
}

TEST_F(StaticArrayTest, EmplaceFirstAndLast)
{
	// Making space for the elements to insert
	array_.popBack();
	array_.popBack();
	printf("Emplacing as first and last\n");
	array_.emplaceAt(0, -1);
	array_.emplaceAt(array_.size(), 8);
	printArray(array_);

	ASSERT_EQ(array_[0], -1);
	ASSERT_EQ(array_[1], 0);
	ASSERT_EQ(array_[8], 7);
	ASSERT_EQ(array_[9], 8);
	ASSERT_EQ(array_.size(), Capacity);
}

TEST_F(StaticArrayTest, RemoveFirstAndTwiceLast)
{
	printf("Removing the first and twice the last\n");
	array_.removeAt(0);
	array_.removeAt(array_.size() - 1);
	array_.removeAt(array_.size() - 1);
	printArray(array_);

	ASSERT_EQ(array_[0], 1);
	ASSERT_EQ(array_[1], 2);
	ASSERT_EQ(array_[5], 6);
	ASSERT_EQ(array_[6], 7);
	ASSERT_EQ(array_.size(), Capacity - 3);
}

TEST_F(StaticArrayTest, UnorderedRemoveFirstAndTwiceLast)
{
	printf("Removing the first and twice the last (unordered)\n");
	array_.unorderedRemoveAt(0);
	array_.unorderedRemoveAt(array_.size() - 1);
	array_.unorderedRemoveAt(array_.size() - 1);
	printArray(array_);

	ASSERT_EQ(array_[0], 9);
	ASSERT_EQ(array_[1], 1);
	ASSERT_EQ(array_[5], 5);
	ASSERT_EQ(array_[6], 6);
	ASSERT_EQ(array_.size(), Capacity - 3);
}

TEST_F(StaticArrayTest, ShrinkAndExtendSize)
{
	printf("Shriking the size by two elements\n");
	const unsigned int oldSize = array_.size();
	array_.setSize(oldSize - 2);
	printArray(array_);
	ASSERT_EQ(array_.size(), oldSize - 2);

	printf("Restoring previous size\n");
	array_.setSize(oldSize);
	printArray(array_);
	ASSERT_TRUE(isUnmodified(array_));
	ASSERT_EQ(array_.size(), oldSize);
}

}
