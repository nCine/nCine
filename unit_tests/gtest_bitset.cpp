#include "gtest_bitset.h"

namespace {

class BitSetTest : public ::testing::Test
{
};

TEST_F(BitSetTest, Constructor)
{
	printf("Constructing a new bitset with the default constructor: \n");
	const nctl::BitSet<uint32_t> bitset;
	printBitSet(bitset);

	ASSERT_TRUE(bitset.none());
	for (unsigned int i = 0; i < bitset.size(); i++)
		ASSERT_FALSE(bitset.test(i));
}

TEST_F(BitSetTest, ConstructorValue)
{
	printf("Constructing a new bitset with a specified initial value: \n");
	const nctl::BitSet<uint32_t> bitset(AllOne);
	printBitSet(bitset);

	ASSERT_TRUE(bitset.all());
	for (unsigned int i = 0; i < bitset.size(); i++)
		ASSERT_TRUE(bitset.test(i));
}

TEST_F(BitSetTest, EqualityOperator)
{
	printf("Constructing a bitset with no bit set: \n");
	const nctl::BitSet<uint32_t> firstBitset;
	printBitSet(firstBitset);

	printf("Constructing a second bitset with no bit set: \n");
	const nctl::BitSet<uint32_t> secondBitset;
	printBitSet(secondBitset);

	printf("Comparing the two bitsets: %d\n", firstBitset == secondBitset);

	ASSERT_TRUE(firstBitset == secondBitset);
	ASSERT_FALSE(firstBitset != secondBitset);
}

TEST_F(BitSetTest, InequalityOperator)
{
	printf("Constructing a bitset with no bit set: \n");
	const nctl::BitSet<uint32_t> firstBitset;
	printBitSet(firstBitset);

	printf("Constructing a second bitset with all bits set: \n");
	const nctl::BitSet<uint32_t> secondBitset(AllOne);
	printBitSet(secondBitset);

	printf("Comparing the two bitsets: %d\n", firstBitset == secondBitset);

	ASSERT_FALSE(firstBitset == secondBitset);
	ASSERT_TRUE(firstBitset != secondBitset);
}

TEST_F(BitSetTest, TestBit)
{
	printf("Constructing a bitset with the fifth bit set: \n");
	const nctl::BitSet<uint32_t> bitset(FifthBit);
	printBitSet(bitset);

	for (unsigned int i = 0; i < bitset.size(); i++)
	{
		// Fifth bit is in position four
		ASSERT_EQ(bitset.test(i), (i == 4) ? true : false);
	}
}

TEST_F(BitSetTest, AllBitsSet)
{
	printf("Constructing a bitset with all bits set: \n");
	const nctl::BitSet<uint32_t> bitset(AllOne);
	printBitSet(bitset);

	ASSERT_TRUE(bitset.all());
	ASSERT_TRUE(bitset.any());
	ASSERT_FALSE(bitset.none());

	for (unsigned int i = 0; i < bitset.size(); i++)
		ASSERT_TRUE(bitset.test(i));
}

TEST_F(BitSetTest, AnyBitsSet)
{
	printf("Constructing a bitset with some bits set: \n");
	const nctl::BitSet<uint32_t> bitset(nc::random().fastInteger(1, UINT32_MAX - 1));
	printBitSet(bitset);

	ASSERT_FALSE(bitset.all());
	ASSERT_TRUE(bitset.any());
	ASSERT_FALSE(bitset.none());

	unsigned int popCount = 0;
	for (unsigned int i = 0; i < bitset.size(); i++)
		popCount += bitset.test(i);
	ASSERT_TRUE(popCount > 0 && popCount < bitset.size());
}

TEST_F(BitSetTest, NoBitSet)
{
	printf("Constructing a bitset with no bit set: \n");
	const nctl::BitSet<uint32_t> bitset;
	printBitSet(bitset);

	ASSERT_FALSE(bitset.all());
	ASSERT_FALSE(bitset.any());
	ASSERT_TRUE(bitset.none());

	for (unsigned int i = 0; i < bitset.size(); i++)
		ASSERT_FALSE(bitset.test(i));
}

TEST_F(BitSetTest, Count8)
{
	printf("Constructing a random 8-bit bitset: \n");
	const nctl::BitSet<uint8_t> bitset(nc::random().fastInteger(0, UINT8_MAX));
	printBitSet(bitset);

	unsigned int popCount = 0;
	for (unsigned int i = 0; i < bitset.size(); i++)
		popCount += bitset.test(i);
	ASSERT_EQ(bitset.count(), popCount);
}

TEST_F(BitSetTest, Count16)
{
	printf("Constructing a random 16-bit bitset: \n");
	const nctl::BitSet<uint16_t> bitset(nc::random().fastInteger(0, UINT16_MAX));
	printBitSet(bitset);

	unsigned int popCount = 0;
	for (unsigned int i = 0; i < bitset.size(); i++)
		popCount += bitset.test(i);
	ASSERT_EQ(bitset.count(), popCount);
}

TEST_F(BitSetTest, Count32)
{
	printf("Constructing a random 32-bit bitset: \n");
	const nctl::BitSet<uint32_t> bitset(nc::random().fastInteger(0, UINT32_MAX));
	printBitSet(bitset);

	unsigned int popCount = 0;
	for (unsigned int i = 0; i < bitset.size(); i++)
		popCount += bitset.test(i);
	ASSERT_EQ(bitset.count(), popCount);
}

TEST_F(BitSetTest, Count64)
{
	printf("Constructing a random 64-bit bitset: \n");
	const uint64_t upper = static_cast<uint64_t>(nc::random().integer(0, UINT32_MAX)) << 32;
	const nctl::BitSet<uint64_t> bitset(upper + nc::random().fastInteger(0, UINT32_MAX));
	printBitSet(bitset);

	unsigned int popCount = 0;
	for (unsigned int i = 0; i < bitset.size(); i++)
		popCount += bitset.test(i);
	ASSERT_EQ(bitset.count(), popCount);
}

TEST_F(BitSetTest, Size8)
{
	printf("Constructing a default 8-bit bitset: \n");
	const nctl::BitSet<uint8_t> bitset;
	printBitSet(bitset);

	ASSERT_EQ(bitset.size(), 8);
}

TEST_F(BitSetTest, Size16)
{
	printf("Constructing a default 16-bit bitset: \n");
	const nctl::BitSet<uint16_t> bitset;
	printBitSet(bitset);

	ASSERT_EQ(bitset.size(), 16);
}

TEST_F(BitSetTest, Size32)
{
	printf("Constructing a default 32-bit bitset: \n");
	const nctl::BitSet<uint32_t> bitset;
	printBitSet(bitset);

	ASSERT_EQ(bitset.size(), 32);
}

TEST_F(BitSetTest, Size64)
{
	printf("Constructing a default 64-bit bitset: \n");
	const nctl::BitSet<uint64_t> bitset;
	printBitSet(bitset);

	ASSERT_EQ(bitset.size(), 64);
}

TEST_F(BitSetTest, OperatorAndInPlace)
{
	printf("Constructing a bitset with the first bit set: \n");
	nctl::BitSet<uint32_t> firstBitset(1U);
	printBitSet(firstBitset);

	printf("Constructing a second bitset with the fifth bit set: \n");
	const nctl::BitSet<uint32_t> secondBitset(FifthBit);
	printBitSet(secondBitset);

	// Fifth bit is in position four
	ASSERT_FALSE(firstBitset.test(4));

	printf("And operation with the second bitset: \n");
	firstBitset &= secondBitset;
	printBitSet(firstBitset);

	// Fifth bit is in position four
	ASSERT_FALSE(firstBitset.test(4));
}

TEST_F(BitSetTest, OperatorOrInPlace)
{
	printf("Constructing a bitset with no bit set: \n");
	nctl::BitSet<uint32_t> firstBitset;
	printBitSet(firstBitset);

	printf("Constructing a second bitset with the fifth bit set: \n");
	const nctl::BitSet<uint32_t> secondBitset(FifthBit);
	printBitSet(secondBitset);

	// Fifth bit is in position four
	ASSERT_FALSE(firstBitset.test(4));

	printf("Or operation with the second bitset: \n");
	firstBitset |= secondBitset;
	printBitSet(firstBitset);

	// Fifth bit is in position four
	ASSERT_TRUE(firstBitset.test(4));
}

TEST_F(BitSetTest, OperatorXOrInPlace)
{
	printf("Constructing a bitset with the first and the fifth bits set: \n");
	nctl::BitSet<uint32_t> firstBitset(FifthBit | 1U);
	printBitSet(firstBitset);

	printf("Constructing a second bitset with the fifth bit set: \n");
	const nctl::BitSet<uint32_t> secondBitset(FifthBit);
	printBitSet(secondBitset);

	// First bit is in position zero
	ASSERT_TRUE(firstBitset.test(0));
	// Fifth bit is in position four
	ASSERT_TRUE(firstBitset.test(4));

	printf("Xor operation with the second bitset: \n");
	firstBitset ^= secondBitset;
	printBitSet(firstBitset);

	// First bit is in position zero
	ASSERT_TRUE(firstBitset.test(0));
	// Fifth bit is in position four
	ASSERT_FALSE(firstBitset.test(4));
}

TEST_F(BitSetTest, OperatorNegate)
{
	printf("Constructing a bitset with the fifth bit set: \n");
	const nctl::BitSet<uint32_t> firstBitset(FifthBit);
	printBitSet(firstBitset);

	for (unsigned int i = 0; i < firstBitset.size(); i++)
	{
		// Fifth bit is in position four
		ASSERT_EQ(firstBitset.test(i), (i == 4) ? true : false);
	}

	printf("Constructing a second bitset as the negation of the first one: \n");
	const nctl::BitSet<uint32_t> secondBitset = ~firstBitset;
	printBitSet(secondBitset);

	for (unsigned int i = 0; i < firstBitset.size(); i++)
	{
		// Fifth bit is in position four
		ASSERT_EQ(secondBitset.test(i), (i == 4) ? false : true);
	}
}

TEST_F(BitSetTest, OperatorLeftShiftInPlace)
{
	printf("Constructing a bitset with the fifth bit set: \n");
	nctl::BitSet<uint32_t> bitset(FifthBit);
	printBitSet(bitset);

	// Fifth bit is in position four
	ASSERT_TRUE(bitset.test(4));

	printf("Shifting the bitset two positions to the left: \n");
	bitset <<= 2;
	printBitSet(bitset);

	// Fifth bit is in position four
	ASSERT_FALSE(bitset.test(4));
	// Seventh bit is in position six
	ASSERT_TRUE(bitset.test(6));
}

TEST_F(BitSetTest, OperatorRightShiftInPlace)
{
	printf("Constructing a bitset with the fifth bit set: \n");
	nctl::BitSet<uint32_t> bitset(FifthBit);
	printBitSet(bitset);

	// Fifth bit is in position four
	ASSERT_TRUE(bitset.test(4));

	printf("Shifting the bitset two positions to the right: \n");
	bitset >>= 2;
	printBitSet(bitset);

	// Fifth bit is in position four
	ASSERT_FALSE(bitset.test(4));
	// Third bit is in position two
	ASSERT_TRUE(bitset.test(2));
}

TEST_F(BitSetTest, OperatorLeftShift)
{
	printf("Constructing a bitset with the fifth bit set: \n");
	const nctl::BitSet<uint32_t> firstBitset(FifthBit);
	printBitSet(firstBitset);

	// Fifth bit is in position four
	ASSERT_TRUE(firstBitset.test(4));

	printf("Constructing a second bitset by shifting the first one by two positions to the left: \n");
	const nctl::BitSet<uint32_t> secondBitset = (firstBitset << 2);
	printBitSet(secondBitset);

	// Fifth bit is in position four
	ASSERT_FALSE(secondBitset.test(4));
	// Seventh bit is in position six
	ASSERT_TRUE(secondBitset.test(6));
}

TEST_F(BitSetTest, OperatorRightShift)
{
	printf("Constructing a bitset with the fifth bit set: \n");
	const nctl::BitSet<uint32_t> firstBitset(FifthBit);
	printBitSet(firstBitset);

	// Fifth bit is in position four
	ASSERT_TRUE(firstBitset.test(4));

	printf("Constructing a second bitset by shifting the first one by two positions to the right: \n");
	const nctl::BitSet<uint32_t> secondBitset = (firstBitset >> 2);
	printBitSet(secondBitset);

	// Fifth bit is in position four
	ASSERT_FALSE(secondBitset.test(4));
	// Third bit is in position two
	ASSERT_TRUE(secondBitset.test(2));
}

TEST_F(BitSetTest, SetAllBits)
{
	printf("Constructing a bitset with no bit set: \n");
	nctl::BitSet<uint32_t> bitset;
	printBitSet(bitset);

	ASSERT_FALSE(bitset.all());
	ASSERT_TRUE(bitset.none());

	printf("Setting all bits of the bitset: \n");
	bitset.set();
	printBitSet(bitset);

	ASSERT_TRUE(bitset.all());
	ASSERT_FALSE(bitset.none());
}

TEST_F(BitSetTest, SetBitAtPosition)
{
	printf("Constructing a bitset with no bit set: \n");
	nctl::BitSet<uint32_t> bitset;
	printBitSet(bitset);

	// Fifth bit is in position four
	ASSERT_FALSE(bitset.test(4));

	printf("Setting the fifth bit of the bitset: \n");
	bitset.set(4); // Fifth bit is in position four
	printBitSet(bitset);

	// Fifth bit is in position four
	ASSERT_TRUE(bitset.test(4));
}

TEST_F(BitSetTest, SetTrueAtPosition)
{
	printf("Constructing a bitset with no bit set: \n");
	nctl::BitSet<uint32_t> bitset;
	printBitSet(bitset);

	// Fifth bit is in position four
	ASSERT_FALSE(bitset.test(4));

	printf("Setting the fifth bit of the bitset to one: \n");
	bitset.set(4, true); // Fifth bit is in position four
	printBitSet(bitset);

	// Fifth bit is in position four
	ASSERT_TRUE(bitset.test(4));
}

TEST_F(BitSetTest, SetFalseAtPosition)
{
	printf("Constructing a bitset with the fifth bit set: \n");
	nctl::BitSet<uint32_t> bitset(FifthBit);
	printBitSet(bitset);

	// Fifth bit is in position four
	ASSERT_TRUE(bitset.test(4));

	printf("Setting the fifth bit of the bitset to zero: \n");
	bitset.set(4, false); // Fifth bit is in position four
	printBitSet(bitset);

	// Fifth bit is in position four
	ASSERT_FALSE(bitset.test(4));
}

TEST_F(BitSetTest, ResetAllBits)
{
	printf("Constructing a bitset with all bits set: \n");
	nctl::BitSet<uint32_t> bitset(AllOne);
	printBitSet(bitset);

	ASSERT_TRUE(bitset.all());
	ASSERT_FALSE(bitset.none());

	printf("Resetting all bits of the bitset: \n");
	bitset.reset();
	printBitSet(bitset);

	ASSERT_FALSE(bitset.all());
	ASSERT_TRUE(bitset.none());
}

TEST_F(BitSetTest, ResetBitAtPosition)
{
	printf("Constructing a bitset with the fifth bit set: \n");
	nctl::BitSet<uint32_t> bitset(FifthBit);
	printBitSet(bitset);

	// Fifth bit is in position four
	ASSERT_TRUE(bitset.test(4));

	printf("Resetting the fifth bit of the bitset: \n");
	bitset.reset(4); // Fifth bit is in position four
	printBitSet(bitset);

	// Fifth bit is in position four
	ASSERT_FALSE(bitset.test(4));
}

TEST_F(BitSetTest, FlipBitZeroAtPosition)
{
	printf("Constructing a bitset with no bit set: \n");
	nctl::BitSet<uint32_t> bitset;
	printBitSet(bitset);

	// Fifth bit is in position four
	ASSERT_FALSE(bitset.test(4));

	printf("Flipping the fifth bit of the bitset: \n");
	bitset.flip(4); // Fifth bit is in position four
	printBitSet(bitset);

	// Fifth bit is in position four
	ASSERT_TRUE(bitset.test(4));
}

TEST_F(BitSetTest, FlipBitOneAtPosition)
{
	printf("Constructing a bitset with the fifth bit set: \n");
	nctl::BitSet<uint32_t> bitset(FifthBit);
	printBitSet(bitset);

	// Fifth bit is in position four
	ASSERT_TRUE(bitset.test(4));

	printf("Flipping the fifth bit of the bitset: \n");
	bitset.flip(4); // Fifth bit is in position four
	printBitSet(bitset);

	// Fifth bit is in position four
	ASSERT_FALSE(bitset.test(4));
}

TEST_F(BitSetTest, OperatorAnd)
{
	printf("Constructing a bitset with the first bit set: \n");
	const nctl::BitSet<uint32_t> firstBitset(1U);
	printBitSet(firstBitset);

	printf("Constructing a second bitset with the fifth bit set: \n");
	const nctl::BitSet<uint32_t> secondBitset(FifthBit);
	printBitSet(secondBitset);

	printf("Constructing a third bitset by an and operation between the first two: \n");
	const nctl::BitSet<uint32_t> thirdBitset = (firstBitset & secondBitset);
	printBitSet(thirdBitset);

	ASSERT_TRUE(thirdBitset.none());
	// First bit is in position zero
	ASSERT_FALSE(thirdBitset.test(0));
	// Fifth bit is in position four
	ASSERT_FALSE(thirdBitset.test(4));
}

TEST_F(BitSetTest, OperatorOr)
{
	printf("Constructing a bitset with the first bit set: \n");
	const nctl::BitSet<uint32_t> firstBitset(1U);
	printBitSet(firstBitset);

	printf("Constructing a second bitset with the fifth bit set: \n");
	const nctl::BitSet<uint32_t> secondBitset(FifthBit);
	printBitSet(secondBitset);

	printf("Constructing a third bitset by an or operation between the first two: \n");
	const nctl::BitSet<uint32_t> thirdBitset = (firstBitset | secondBitset);
	printBitSet(thirdBitset);

	ASSERT_TRUE(thirdBitset.any());
	// First bit is in position zero
	ASSERT_TRUE(thirdBitset.test(0));
	// Fifth bit is in position four
	ASSERT_TRUE(thirdBitset.test(4));
}

TEST_F(BitSetTest, OperatorXOr)
{
	printf("Constructing a bitset with the first and the fifth bits set: \n");
	const nctl::BitSet<uint32_t> firstBitset(FifthBit | 1U);
	printBitSet(firstBitset);

	printf("Constructing a second bitset with the fifth bit set: \n");
	const nctl::BitSet<uint32_t> secondBitset(FifthBit);
	printBitSet(secondBitset);

	printf("Constructing a third bitset by an xor operation between the first two: \n");
	const nctl::BitSet<uint32_t> thirdBitset = (firstBitset ^ secondBitset);
	printBitSet(thirdBitset);

	ASSERT_TRUE(thirdBitset.any());
	// First bit is in position zero
	ASSERT_TRUE(thirdBitset.test(0));
	// Fifth bit is in position four
	ASSERT_FALSE(thirdBitset.test(4));
}

}
