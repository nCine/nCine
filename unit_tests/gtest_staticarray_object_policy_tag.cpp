#include "gtest_staticarray.h"
#include "test_object_policy_tag.h"

namespace {

class StaticArrayObjectPolicyTagTest : public ::testing::Test
{
  public:
	static constexpr unsigned int Size = 6;

  protected:
	void SetUp() override { initArray(array_); }

	nctl::StaticArray<int, Capacity> array_;
};

TEST_F(StaticArrayObjectPolicyTagTest, MovableInsertAt)
{
	printf("Inserting in the middle of an array of movable and copyable objects\n");
	nctl::StaticArray<Movable, Capacity> array;
	for (unsigned int i = 0; i < Size; i++)
		array.emplaceBack();
	ASSERT_EQ(array.size(), Size);

	array.insertAt(Size / 2, Movable());
	ASSERT_EQ(array.size(), Size + 1);
}

TEST_F(StaticArrayObjectPolicyTagTest, MoveOnlyInsertAt)
{
	printf("Inserting in the middle of an array of movable only objects\n");
	nctl::StaticArray<MoveOnly, Capacity> array;
	for (unsigned int i = 0; i < Size; i++)
		array.emplaceBack();
	ASSERT_EQ(array.size(), Size);

	array.insertAt(Size / 2, MoveOnly());
	ASSERT_EQ(array.size(), Size + 1);
}

TEST_F(StaticArrayObjectPolicyTagTest, CopyableInsertAt)
{
	printf("Inserting in the middle of an array of copyable but non-movable objects\n");
	nctl::StaticArray<Copyable, Capacity> array;
	for (unsigned int i = 0; i < Size; i++)
		array.emplaceBack();
	ASSERT_EQ(array.size(), Size);

	array.insertAt(Size / 2, Copyable());
	ASSERT_EQ(array.size(), Size + 1);
}

TEST_F(StaticArrayObjectPolicyTagTest, TrivialInsertAt)
{
	printf("Inserting in the middle of an array of trivially copyable objects\n");
	nctl::StaticArray<Trivial, Capacity> array;
	for (unsigned int i = 0; i < Size; i++)
		array.emplaceBack();
	ASSERT_EQ(array.size(), Size);

	array.insertAt(Size / 2, Trivial());
	ASSERT_EQ(array.size(), Size + 1);
}

// A NonMovableInsertAt test would not compile.

TEST_F(StaticArrayObjectPolicyTagTest, NonMovableEmplaceBack)
{
	printf("Emplacing back in an empty array of non-movable objects\n");
	nctl::StaticArray<NonMovable, Capacity> array;
	for (unsigned int i = 0; i < Size; i++)
		array.emplaceBack();
	ASSERT_EQ(array.size(), Size);
}

}
