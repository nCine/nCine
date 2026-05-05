#include "gtest_pointermath.h"

namespace {

TEST(PointerMathTest, AddToNumber)
{
	const uintptr_t ptr = Address;

	const uintptr_t newPtr = nctl::PointerMath::add(ptr, Value);
	printf("Add together address 0x%llu and value 0x%llu: 0x%llu\n", ptr, Value, newPtr);
	ASSERT_EQ(newPtr, ptr + Value);
}

TEST(PointerMathTest, SubtractFromNumber)
{
	const uintptr_t ptr = Address;

	const uintptr_t newPtr = nctl::PointerMath::subtract(ptr, Value);
	printf("Subtract 0x%llu from address 0x%llu: 0x%llu\n", Value, ptr, newPtr);
	ASSERT_EQ(newPtr, ptr - Value);
}

TEST(PointerMathTest, DistanceNumbers)
{
	const uintptr_t firstPtr = Address + Value;
	const uintptr_t secondPtr = Address;

	const ptrdiff_t distance = nctl::PointerMath::distance(firstPtr, secondPtr);
	printf("Distance between address 0x%llu and address 0x%llu: 0x%llu\n", firstPtr, secondPtr, Value);
	ASSERT_EQ(distance, Value);
	ASSERT_EQ(distance, firstPtr - secondPtr);
}

TEST(PointerMathTest, AlignNumber)
{
	const uintptr_t misaligned = Address - Disalign;

	const uintptr_t aligned = nctl::PointerMath::align(misaligned, Alignment);
	const size_t adjustment = nctl::PointerMath::alignAdjustment(misaligned, Alignment);
	printf("Align address 0x%llu to %llu bytes alignment: 0x%llu (+%llu)\n", misaligned, Alignment, aligned, adjustment);

	ASSERT_EQ(Disalign, adjustment);
	ASSERT_EQ(misaligned + adjustment, aligned);
}

TEST(PointerMathTest, AlignNumberWithHeader)
{
	const size_t misaligned = Address - Disalign;

	const size_t adjustment = nctl::PointerMath::alignWithHeader(misaligned, Alignment, HeaderSize);
	printf("Adjustment for address 0x%llu with %llu bytes alignment and %llu bytes header: %llu bytes\n", misaligned, Alignment, HeaderSize, adjustment);

	ASSERT_EQ(Disalign + Alignment, adjustment);
}

TEST(PointerMathTest, AlignNumberWithSmallHeader)
{
	const size_t misaligned = Address - Disalign;

	const size_t adjustment = nctl::PointerMath::alignWithHeader(misaligned, Alignment, SmallHeaderSize);
	printf("Adjustment for address 0x%llu with %llu bytes alignment and %llu bytes header: %llu bytes\n", misaligned, Alignment, SmallHeaderSize, adjustment);

	ASSERT_EQ(Disalign, adjustment);
}

TEST(PointerMathTest, AddToAddress)
{
	void *ptr = reinterpret_cast<void *>(Address);
	ASSERT_EQ(uintptr_t(ptr), Address);

	const void *newPtr = nctl::PointerMath::add(ptr, Value);
	printf("Add together address 0x%llu and value 0x%llu: 0x%llu\n", uintptr_t(ptr), Value, uintptr_t(newPtr));
	ASSERT_EQ(uintptr_t(newPtr), uintptr_t(ptr) + Value);
}

TEST(PointerMathTest, SubtractFromAddress)
{
	void *ptr = reinterpret_cast<void *>(Address);
	ASSERT_EQ(uintptr_t(ptr), Address);

	const void *newPtr = nctl::PointerMath::subtract(ptr, Value);
	printf("Subtract 0x%llu from address 0x%llu: 0x%llu\n", Value, uintptr_t(ptr), uintptr_t(newPtr));
	ASSERT_EQ(uintptr_t(newPtr), uintptr_t(ptr) - Value);
}

TEST(PointerMathTest, DistanceAddresses)
{
	void *firstPtr = reinterpret_cast<void *>(Address + Value);
	ASSERT_EQ(uintptr_t(firstPtr), Address + Value);
	void *secondPtr = reinterpret_cast<void *>(Address);
	ASSERT_EQ(uintptr_t(secondPtr), Address);

	const ptrdiff_t distance = nctl::PointerMath::distance(firstPtr, secondPtr);
	printf("Distance between address 0x%llu and address 0x%llu: 0x%llu\n", uintptr_t(firstPtr), uintptr_t(secondPtr), distance);
	ASSERT_EQ(distance, Value);
	ASSERT_EQ(distance, uintptr_t(firstPtr) - uintptr_t(secondPtr));
}

TEST(PointerMathTest, AlignAddress)
{
	const size_t misaligned = Address - Disalign;
	void *ptr = reinterpret_cast<void *>(misaligned);

	void *newPtr = nctl::PointerMath::align(ptr, Alignment);
	const size_t adjustment = nctl::PointerMath::alignAdjustment(ptr, Alignment);
	printf("Align address 0x%llu to %llu bytes alignment: 0x%llu (+%llu)\n", uintptr_t(ptr), Alignment, uintptr_t(newPtr), adjustment);

	ASSERT_EQ(Disalign, adjustment);
	ASSERT_EQ(uintptr_t(ptr) + adjustment, uintptr_t(newPtr));
}

TEST(PointerMathTest, AlignAddressWithHeader)
{
	const size_t misaligned = Address - Disalign;
	void *ptr = reinterpret_cast<void *>(misaligned);

	const size_t adjustment = nctl::PointerMath::alignWithHeader(ptr, Alignment, HeaderSize);
	printf("Adjustment for address 0x%llu with %llu bytes alignment and %llu bytes header: %llu bytes\n", uintptr_t(ptr), Alignment, HeaderSize, adjustment);

	ASSERT_EQ(Disalign + Alignment, adjustment);
}

TEST(PointerMathTest, AlignAddressWithSmallHeader)
{
	const size_t misaligned = Address - Disalign;
	void *ptr = reinterpret_cast<void *>(misaligned);

	const size_t adjustment = nctl::PointerMath::alignWithHeader(ptr, Alignment, SmallHeaderSize);
	printf("Adjustment for address 0x%llu with %llu bytes alignment and %llu bytes header: %llu bytes\n", uintptr_t(ptr), Alignment, SmallHeaderSize, adjustment);

	ASSERT_EQ(Disalign, adjustment);
}

}
