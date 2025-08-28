#include "gtest_array.h"
#include "test_object_policy_tag.h"

namespace {

class ArrayObjectPolicyTagTest : public ::testing::Test
{
  public:
	static constexpr unsigned int Size = 6;
};

TEST_F(ArrayObjectPolicyTagTest, MovableSetCapacityOnEmpty)
{
	printf("Setting capacity of an empty array of movable and copyable objects\n");
	nctl::Array<Movable> array;

	array.setCapacity(Capacity);
	ASSERT_EQ(array.capacity(), Capacity);
}

TEST_F(ArrayObjectPolicyTagTest, MoveOnlySetCapacityOnEmpty)
{
	printf("Setting capacity of an empty array of movable only objects\n");
	nctl::Array<MoveOnly> array;

	array.setCapacity(Capacity);
	ASSERT_EQ(array.capacity(), Capacity);
}

TEST_F(ArrayObjectPolicyTagTest, CopyableSetCapacityOnEmpty)
{
	printf("Setting capacity of an empty array of copyable but non-movable objects\n");
	nctl::Array<Copyable> array;

	array.setCapacity(Capacity);
	ASSERT_EQ(array.capacity(), Capacity);
}

TEST_F(ArrayObjectPolicyTagTest, TrivialSetCapacityOnEmpty)
{
	printf("Setting capacity of an empty array of trivially copyable objects\n");
	nctl::Array<Trivial> array;

	array.setCapacity(Capacity);
	ASSERT_EQ(array.capacity(), Capacity);
}

TEST_F(ArrayObjectPolicyTagTest, NonMovableSetCapacityOnEmpty)
{
	printf("Setting capacity of an empty array of non-movable but trivially copyable objects\n");
	nctl::Array<NonMovable> array;

	array.setCapacity(Capacity);
	ASSERT_EQ(array.capacity(), Capacity);
}

TEST_F(ArrayObjectPolicyTagTest, MovableSetCapacity)
{
	printf("Setting capacity of a non-empty array of movable and copyable objects\n");
	nctl::Array<Movable> array(Size);
	for (unsigned int i = 0; i < Size; i++)
		array.emplaceBack();

	array.setCapacity(Capacity);
	ASSERT_EQ(array.capacity(), Capacity);
	ASSERT_EQ(array.size(), Size);
}

TEST_F(ArrayObjectPolicyTagTest, MoveOnlySetCapacity)
{
	printf("Setting capacity of a non-empty array of movable only objects\n");
	nctl::Array<MoveOnly> array(Size);
	for (unsigned int i = 0; i < Size; i++)
		array.emplaceBack();

	array.setCapacity(Capacity);
	ASSERT_EQ(array.capacity(), Capacity);
	ASSERT_EQ(array.size(), Size);
}

TEST_F(ArrayObjectPolicyTagTest, CopyableSetCapacity)
{
	printf("Setting capacity of a non-empty array of copyable but non-movable objects\n");
	nctl::Array<Copyable> array(Size);
	for (unsigned int i = 0; i < Size; i++)
		array.emplaceBack();

	array.setCapacity(Capacity);
	ASSERT_EQ(array.capacity(), Capacity);
	ASSERT_EQ(array.size(), Size);
}

TEST_F(ArrayObjectPolicyTagTest, TrivialSetCapacity)
{
	printf("Setting capacity of a non-empty array of trivially copyable objects\n");
	nctl::Array<Trivial> array(Size);
	for (unsigned int i = 0; i < Size; i++)
		array.emplaceBack();

	array.setCapacity(Capacity);
	ASSERT_EQ(array.capacity(), Capacity);
	ASSERT_EQ(array.size(), Size);
}

#ifndef __EMSCRIPTEN__
/// This test crashes because capacity is being changed on a non empty array of non-movable and non-copyable objects
TEST_F(ArrayObjectPolicyTagTest, NonMovableSetCapacity)
{
	printf("Setting capacity of a non-empty array of non-movable and non-copyable objects\n");
	nctl::Array<NonMovable> array(Size);
	for (unsigned int i = 0; i < Size; i++)
		array.emplaceBack();

	ASSERT_EQ(array.size(), Size);
	ASSERT_DEATH(array.setCapacity(Capacity), "");
}
#endif

TEST_F(ArrayObjectPolicyTagTest, MovableInsertAt)
{
	printf("Inserting in the middle of an array of movable and copyable objects\n");
	nctl::Array<Movable> array(Capacity);
	for (unsigned int i = 0; i < Size; i++)
		array.emplaceBack();
	ASSERT_EQ(array.size(), Size);

	array.insertAt(Size / 2, Movable());
	ASSERT_EQ(array.size(), Size + 1);
}

TEST_F(ArrayObjectPolicyTagTest, MoveOnlyInsertAt)
{
	printf("Inserting in the middle of an array of movable only objects\n");
	nctl::Array<MoveOnly> array(Capacity);
	for (unsigned int i = 0; i < Size; i++)
		array.emplaceBack();
	ASSERT_EQ(array.size(), Size);

	array.insertAt(Size / 2, MoveOnly());
	ASSERT_EQ(array.size(), Size + 1);
}

TEST_F(ArrayObjectPolicyTagTest, CopyableInsertAt)
{
	printf("Inserting in the middle of an array of copyable but non-movable objects\n");
	nctl::Array<Copyable> array(Capacity);
	for (unsigned int i = 0; i < Size; i++)
		array.emplaceBack();
	ASSERT_EQ(array.size(), Size);

	array.insertAt(Size / 2, Copyable());
	ASSERT_EQ(array.size(), Size + 1);
}

TEST_F(ArrayObjectPolicyTagTest, TrivialInsertAt)
{
	printf("Inserting in the middle of an array of trivially copyable objects\n");
	nctl::Array<Trivial> array(Capacity);
	for (unsigned int i = 0; i < Size; i++)
		array.emplaceBack();
	ASSERT_EQ(array.size(), Size);

	array.insertAt(Size / 2, Trivial());
	ASSERT_EQ(array.size(), Size + 1);
}

// A NonMovableInsertAt test would not compile.

TEST_F(ArrayObjectPolicyTagTest, NonMovableEmplaceAndPop)
{
	printf("Emplacing at the back\n");
	nctl::Array<NonMovable> array(Capacity);
	for (unsigned int i = 0; i < Size; i++)
		array.emplaceBack();
	ASSERT_EQ(array.size(), Size);

	printf("Removing at the back\n");
	for (unsigned int i = 0; i < Size; i++)
		array.popBack();
	ASSERT_EQ(array.size(), 0);

	printf("Changing capacity of an empty array of copyable but non-movable objects\n");
	array.setCapacity(Size);
	ASSERT_EQ(array.capacity(), Size);
}

}
