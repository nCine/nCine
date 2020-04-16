#include "gtest_pointermath.h"

namespace {

TEST(PointerMathTest, AddNumber)
{
	void *ptr = reinterpret_cast<void *>(Address);
	ASSERT_EQ(uintptr_t(ptr), Address);

	void *newPtr = nctl::PointerMath::add(ptr, Value);
	printf("Add 0x%lx to address 0x%lx: 0x%lx\n", Value, uintptr_t(ptr), uintptr_t(newPtr));
	ASSERT_EQ(uintptr_t(newPtr), uintptr_t(ptr) + Value);
}

TEST(PointerMathTest, SubtractNumber)
{
	void *ptr = reinterpret_cast<void *>(Address);
	ASSERT_EQ(uintptr_t(ptr), Address);

	void *newPtr = nctl::PointerMath::subtract(ptr, Value);
	printf("Subtract 0x%lx from address 0x%lx: 0x%lx\n", Value, uintptr_t(ptr), uintptr_t(newPtr));
	ASSERT_EQ(uintptr_t(newPtr), uintptr_t(ptr) - Value);
}

TEST(PointerMathTest, AddAddress)
{
	void *firstPtr = reinterpret_cast<void *>(Address);
	ASSERT_EQ(uintptr_t(firstPtr), Address);
	void *secondPtr = reinterpret_cast<void *>(Value);
	ASSERT_EQ(uintptr_t(secondPtr), Value);

	const uintptr_t newPtr = nctl::PointerMath::add(firstPtr, secondPtr);
	printf("Add together address 0x%lx and address 0x%lx: 0x%lx\n", uintptr_t(firstPtr), uintptr_t(secondPtr), newPtr);
	ASSERT_EQ(uintptr_t(newPtr), uintptr_t(firstPtr) + uintptr_t(secondPtr));
}

TEST(PointerMathTest, SubtractAddress)
{
	void *firstPtr = reinterpret_cast<void *>(Address);
	ASSERT_EQ(uintptr_t(firstPtr), Address);
	void *secondPtr = reinterpret_cast<void *>(Value);
	ASSERT_EQ(uintptr_t(secondPtr), Value);

	const uintptr_t newPtr = nctl::PointerMath::subtract(firstPtr, secondPtr);
	printf("Subtract address 0x%lx from address 0x%lx: 0x%lx\n", uintptr_t(secondPtr), uintptr_t(firstPtr), newPtr);
	ASSERT_EQ(uintptr_t(newPtr), uintptr_t(firstPtr) - uintptr_t(secondPtr));
}

TEST(PointerMathTest, AlignAddress)
{
	const size_t Misaligned = Address - Disalign;
	void *ptr = reinterpret_cast<void *>(Misaligned);

	void *newPtr = nctl::PointerMath::align(ptr, Alignment);
	const uint8_t adjustment = nctl::PointerMath::alignAdjustment(ptr, Alignment);
	printf("Align address 0x%lx to %u bytes alignment: 0x%lx (+%u)\n", uintptr_t(ptr), Alignment, uintptr_t(newPtr), adjustment);

	ASSERT_EQ(Disalign, adjustment);
	ASSERT_EQ(uintptr_t(ptr) + adjustment, uintptr_t(newPtr));
}

TEST(PointerMathTest, AlignAddressWithHeader)
{
	const size_t Misaligned = Address - Disalign;
	void *ptr = reinterpret_cast<void *>(Misaligned);

	const uint8_t adjustment = nctl::PointerMath::alignWithHeader(ptr, Alignment, HeaderSize);
	printf("Adjustment for address 0x%lx with %u bytes alignment and %u bytes header: %u bytes\n", uintptr_t(ptr), Alignment, HeaderSize, adjustment);

	ASSERT_EQ(Disalign + Alignment, adjustment);
}

TEST(PointerMathTest, AlignAddressWithSmallHeader)
{
	const size_t Misaligned = Address - Disalign;
	void *ptr = reinterpret_cast<void *>(Misaligned);

	const uint8_t adjustment = nctl::PointerMath::alignWithHeader(ptr, Alignment, SmallHeaderSize);
	printf("Adjustment for address 0x%lx with %u bytes alignment and %u bytes header: %u bytes\n", uintptr_t(ptr), Alignment, SmallHeaderSize, adjustment);

	ASSERT_EQ(Disalign, adjustment);
}

}
