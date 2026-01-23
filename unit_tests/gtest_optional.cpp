#include "gtest_optional.h"

namespace {

class OptionalTest : public ::testing::Test
{
  public:
	OptionalTest()
	    : optional(DefaultValue) {}

  protected:
	nctl::Optional<uint32_t> optional;
};

#ifndef __EMSCRIPTEN__
TEST(OptionalDeathTest, AccessNoValue)
{
	printf("Trying to access a value from a disengaged optional\n");
	nctl::Optional<uint32_t> newOptional;

	ASSERT_DEATH(newOptional.value(), "");
}

TEST(OptionalDeathTest, ConstAccessNoValue)
{
	printf("Trying to access a value from a constant disengaged optional\n");
	const nctl::Optional<uint32_t> newOptional;

	ASSERT_DEATH(newOptional.value(), "");
}

TEST(OptionalDeathTest, DereferenceOperatorNoValue)
{
	printf("Trying to dereference a value from a disengaged optional\n");
	nctl::Optional<uint32_t> newOptional;

	ASSERT_DEATH(*newOptional, "");
}

TEST(OptionalDeathTest, ConstDereferenceOperatorNoValue)
{
	printf("Trying to dereference a value from a constant disengaged optional\n");
	const nctl::Optional<uint32_t> newOptional;

	ASSERT_DEATH(*newOptional, "");
}

TEST(OptionalDeathTest, MoveDereferenceOperatorNoValue)
{
	printf("Trying to move dereference a value from a disengaged optional\n");
	nctl::Optional<uint32_t> newOptional;

	ASSERT_DEATH(*nctl::move(newOptional), "");
}

TEST(OptionalDeathTest, ConstMoveDereferenceOperatorNoValue)
{
	printf("Trying to move dereference a value from a constant disengaged optional\n");
	const nctl::Optional<uint32_t> newOptional;

	ASSERT_DEATH(*nctl::move(newOptional), "");
}
#endif

TEST_F(OptionalTest, Constructor)
{
	printf("Constructing a new optional with the default constructor: ");
	nctl::Optional<uint32_t> newOptional;
	printOptional(newOptional);

	ASSERT_FALSE(newOptional.hasValue());
}

TEST_F(OptionalTest, ConstructorValue)
{
	printf("Constructing a new optional with a specified initial value: ");
	nctl::Optional<uint32_t> newOptional(DefaultValue);
	printOptional(newOptional);

	ASSERT_TRUE(newOptional.hasValue());
	ASSERT_EQ(newOptional.value(), DefaultValue);
}

TEST_F(OptionalTest, NullOptConstructor)
{
	printf("Constructing a new optional with nullopt: ");
	nctl::Optional<uint32_t> newOptional = nctl::nullopt;
	printOptional(newOptional);

	ASSERT_FALSE(newOptional.hasValue());
}

TEST_F(OptionalTest, CopyConstructor)
{
	printf("Constructing a second optional as a copy of the first: ");
	nctl::Optional<uint32_t> newOptional(optional);
	printOptional(newOptional);

	ASSERT_TRUE(optional.hasValue());
	ASSERT_EQ(optional.value(), DefaultValue);
	ASSERT_TRUE(newOptional.hasValue());
	ASSERT_EQ(newOptional.value(), DefaultValue);
}

TEST_F(OptionalTest, MoveConstructor)
{
	printf("Constructing a second optional moving from the first: ");
	nctl::Optional<uint32_t> newOptional(nctl::move(optional));
	printOptional(newOptional);

	ASSERT_TRUE(optional.hasValue());
	ASSERT_TRUE(newOptional.hasValue());
	ASSERT_EQ(newOptional.value(), DefaultValue);
}

TEST_F(OptionalTest, CopyAssignmentOperator)
{
	printf("Copy-assigning the first optional to the second one: ");
	nctl::Optional<uint32_t> newOptional;
	newOptional = optional;
	printOptional(newOptional);

	ASSERT_TRUE(optional.hasValue());
	ASSERT_EQ(optional.value(), DefaultValue);
	ASSERT_TRUE(newOptional.hasValue());
	ASSERT_EQ(newOptional.value(), DefaultValue);
}

TEST_F(OptionalTest, MoveAssignmentOperator)
{
	printf("Move-assigning the first optional to the second one: ");
	nctl::Optional<uint32_t> newOptional;
	newOptional = nctl::move(optional);
	printOptional(newOptional);

	ASSERT_TRUE(optional.hasValue());
	ASSERT_TRUE(newOptional.hasValue());
	ASSERT_EQ(newOptional.value(), DefaultValue);
}

TEST_F(OptionalTest, NullOptAssignment)
{
	printf("Constructing a new optional with a specified initial value: ");
	nctl::Optional<uint32_t> newOptional(DefaultValue);
	printOptional(newOptional);
	ASSERT_TRUE(newOptional.hasValue());

	printf("Assigning nullopt: ");
	newOptional = nctl::nullopt;
	printOptional(newOptional);
	ASSERT_FALSE(newOptional.hasValue());
}

TEST_F(OptionalTest, EmplaceValue)
{
	printf("Emplacing a value into a new optional: ");
	nctl::Optional<uint32_t> newOptional;
	newOptional.emplace(DefaultValue);
	printOptional(newOptional);

	ASSERT_TRUE(newOptional.hasValue());
	ASSERT_EQ(newOptional.value(), DefaultValue);
}

TEST_F(OptionalTest, MakeOptional)
{
	printf("Creating a new optional with makeOptional: ");
	nctl::Optional<uint32_t> newOptional = nctl::makeOptional<uint32_t>(DefaultValue);
	printOptional(newOptional);

	ASSERT_TRUE(newOptional.hasValue());
	ASSERT_EQ(newOptional.value(), DefaultValue);
}

TEST_F(OptionalTest, Reset)
{
	printf("Constructing a new optional with a specified initial value: ");
	nctl::Optional<uint32_t> newOptional(DefaultValue);
	printOptional(newOptional);
	ASSERT_TRUE(newOptional.hasValue());
	ASSERT_EQ(newOptional.value(), DefaultValue);

	printf("Resetting the optional: ");
	newOptional.reset();
	printOptional(newOptional);
	ASSERT_FALSE(newOptional.hasValue());
	ASSERT_FALSE(bool(newOptional)); // Also checking the bool operator on a resetted otpional
}

TEST_F(OptionalTest, ValueOrDisengaged)
{
	printf("Constructing a new optional with the default constructor: ");
	nctl::Optional<uint32_t> newOptional;
	printOptional(newOptional);
	ASSERT_FALSE(newOptional.hasValue());

	const uint32_t newValue = newOptional.valueOr(DefaultValue + 1);
	printf("Copied value: %u\n", newValue);
	ASSERT_EQ(newValue, DefaultValue + 1);
}

TEST_F(OptionalTest, ValueOrEngaged)
{
	printf("Constructing a new optional with a specified initial value: ");
	nctl::Optional<uint32_t> newOptional(DefaultValue);
	printOptional(newOptional);
	ASSERT_TRUE(newOptional.hasValue());

	const uint32_t newValue = newOptional.valueOr(DefaultValue + 1);
	printf("Copied value: %u\n", newValue);
	ASSERT_EQ(newValue, DefaultValue);
}

TEST_F(OptionalTest, BoolOperator)
{
	printf("Checking that the optional has a value using the bool operator: %d\n", bool(optional));
	ASSERT_TRUE(optional.hasValue());
	ASSERT_TRUE(bool(optional));
}

TEST_F(OptionalTest, DereferenceOperator)
{
	uint32_t newValue = *optional;
	printf("Copied value: %u\n", newValue);

	ASSERT_TRUE(optional.hasValue());
	ASSERT_EQ(optional.value(), DefaultValue);
	ASSERT_EQ(newValue, DefaultValue);
}

TEST_F(OptionalTest, ConstDereferenceOperator)
{
	const uint32_t newValue = *optional;
	printf("Copied value: %u\n", newValue);

	ASSERT_TRUE(optional.hasValue());
	ASSERT_EQ(optional.value(), DefaultValue);
	ASSERT_EQ(newValue, DefaultValue);
}

TEST_F(OptionalTest, MoveDereferenceOperator)
{
	printf("Constructing a new optional with a specified initial value: ");
	nctl::Optional<uint32_t> newOptional(DefaultValue);
	printOptional(newOptional);

	uint32_t newValue = *nctl::move(newOptional);
	printf("Moved value: %u\n", newValue);

	ASSERT_TRUE(newOptional.hasValue());
	ASSERT_EQ(newOptional.value(), DefaultValue);
	ASSERT_EQ(newValue, DefaultValue);
}

TEST_F(OptionalTest, ConstMoveDereferenceOperator)
{
	printf("Constructing a new optional with a specified initial value: ");
	nctl::Optional<uint32_t> newOptional(DefaultValue);
	printOptional(newOptional);

	const uint32_t newValue = *nctl::move(newOptional);
	printf("Moved value: %u\n", newValue);

	ASSERT_TRUE(newOptional.hasValue());
	ASSERT_EQ(newOptional.value(), DefaultValue);
	ASSERT_EQ(newValue, DefaultValue);
}

TEST_F(OptionalTest, EqualityOperator)
{
	printf("Constructing a new optional with a specified initial value: ");
	nctl::Optional<uint32_t> newOptional(DefaultValue);
	printOptional(newOptional);

	printf("Checking equality with the first optional: %d\n", newOptional == optional);
	ASSERT_TRUE(newOptional == optional);
	ASSERT_EQ(newOptional, optional);
}

TEST_F(OptionalTest, NullOptEqualityOperator)
{
	printf("Constructing a new optional with the default constructor: ");
	nctl::Optional<uint32_t> newOptional;

	printf("Checking equality with nullopt: %d\n", newOptional == nctl::nullopt);
	ASSERT_TRUE(newOptional == nctl::nullopt);
	ASSERT_FALSE(newOptional != nctl::nullopt);
}

TEST_F(OptionalTest, InqualityOperator)
{
	printf("Constructing a new optional with a specified initial value: ");
	nctl::Optional<uint32_t> newOptional(DefaultValue + 1);
	printOptional(newOptional);

	printf("Checking inequality with the first optional: %d\n", newOptional != optional);
	ASSERT_TRUE(newOptional != optional);
	ASSERT_NE(newOptional, optional);
}

TEST_F(OptionalTest, LessThanOperator)
{
	printf("Constructing a new optional with a specified initial value: ");
	nctl::Optional<uint32_t> newOptional(DefaultValue - 1);
	printOptional(newOptional);

	printf("Checking less-than relationship with the first optional: %d\n", newOptional < optional);
	ASSERT_TRUE(newOptional < optional);
	ASSERT_LT(newOptional, optional);

	printf("Checking less-than or equal relationship with the first optional: %d\n", newOptional <= optional);
	ASSERT_TRUE(newOptional <= optional);
	ASSERT_LE(newOptional, optional);
}

TEST_F(OptionalTest, GreaterThanOperator)
{
	printf("Constructing a new optional with a specified initial value: ");
	nctl::Optional<uint32_t> newOptional(DefaultValue + 1);
	printOptional(newOptional);

	printf("Checking greater-than relationship with the first optional: %d\n", newOptional > optional);
	ASSERT_TRUE(newOptional > optional);
	ASSERT_GT(newOptional, optional);

	printf("Checking greater-than or equal relationship with the first optional: %d\n", newOptional >= optional);
	ASSERT_TRUE(newOptional >= optional);
	ASSERT_GE(newOptional, optional);
}

TEST_F(OptionalTest, ValueEqualityOperator)
{
	const uint32_t newValue = DefaultValue;
	printf("Creating a new value: %u\n", newValue);

	printf("Checking equality with the first optional: %d, %d\n",
	       newValue == optional, optional == newValue);
	ASSERT_TRUE(newValue == optional);
	ASSERT_EQ(newValue, optional);

	ASSERT_TRUE(optional == newValue);
	ASSERT_EQ(optional, newValue);
}

TEST_F(OptionalTest, ValueInqualityOperator)
{
	const uint32_t newValue = DefaultValue + 1;
	printf("Creating a new value: %u\n", newValue);

	printf("Checking inequality with the first optional: %d, %d\n",
	       newValue != optional, optional != newValue);
	ASSERT_TRUE(newValue != optional);
	ASSERT_NE(newValue, optional);

	ASSERT_TRUE(optional != newValue);
	ASSERT_NE(optional, newValue);
}

TEST_F(OptionalTest, ValueLessThanOperator)
{
	const uint32_t newValue = DefaultValue - 1;
	printf("Creating a new value: %u\n", newValue);

	printf("Checking less-than relationship with the first optional: %d, %d\n",
	       newValue < optional, optional > newValue);
	ASSERT_TRUE(newValue < optional);
	ASSERT_LT(newValue, optional);

	ASSERT_TRUE(optional > newValue);
	ASSERT_GT(optional, newValue);

	printf("Checking less-than or equal relationship with the first optional: %d, %d\n",
	       newValue <= optional, optional >= newValue);
	ASSERT_TRUE(newValue <= optional);
	ASSERT_LE(newValue, optional);

	ASSERT_TRUE(optional >= newValue);
	ASSERT_GE(optional, newValue);
}

TEST_F(OptionalTest, ValueGreaterThanOperator)
{
	const uint32_t newValue = DefaultValue + 1;
	printf("Creating a new value: %u\n", newValue);

	printf("Checking greater-than relationship with the first optional: %d, %d\n",
	       newValue > optional, optional < newValue);
	ASSERT_TRUE(newValue > optional);
	ASSERT_GT(newValue, optional);

	ASSERT_TRUE(optional < newValue);
	ASSERT_LT(optional, newValue);

	printf("Checking greater-than or equal relationship with the first optional: %d, %d\n",
	       newValue >= optional, optional <= newValue);
	ASSERT_TRUE(newValue >= optional);
	ASSERT_GE(newValue, optional);

	ASSERT_TRUE(optional <= newValue);
	ASSERT_LE(optional, newValue);
}

}
