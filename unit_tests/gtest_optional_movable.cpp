#include "gtest_optional.h"
#include "test_movable.h"

namespace {

void printOptional(const nctl::Optional<Movable> &opt)
{
	if (opt.hasValue())
		printf("(movable has size %u)\n", opt.value().size());
	else
		printf("(no movable)\n");
}

class OptionalMovableTest : public ::testing::Test
{
  public:
	OptionalMovableTest()
	    : optional(Movable(Movable::Construction::INITIALIZED)) {}

  protected:
	nctl::Optional<Movable> optional;
};

#ifndef __EMSCRIPTEN__
TEST(OptionalMovableDeathTest, AccessNoValue)
{
	printf("Trying to access a value from a disengaged optional\n");
	nctl::Optional<Movable> newOptional;

	ASSERT_DEATH(newOptional.value(), "");
}

TEST(OptionalMovableDeathTest, ConstAccessNoValue)
{
	printf("Trying to access a value from a constant disengaged optional\n");
	const nctl::Optional<Movable> newOptional;

	ASSERT_DEATH(newOptional.value(), "");
}

TEST(OptionalMovableDeathTest, DereferenceOperatorNoValue)
{
	printf("Trying to dereference a value from a disengaged optional\n");
	nctl::Optional<Movable> newOptional;

	ASSERT_DEATH(*newOptional, "");
}

TEST(OptionalMovableDeathTest, ConstDereferenceOperatorNoValue)
{
	printf("Trying to dereference a value from a constant disengaged optional\n");
	const nctl::Optional<Movable> newOptional;

	ASSERT_DEATH(*newOptional, "");
}

TEST(OptionalMovableDeathTest, MoveDereferenceOperatorNoValue)
{
	printf("Trying to move dereference a value from a disengaged optional\n");
	nctl::Optional<Movable> newOptional;

	ASSERT_DEATH(*nctl::move(newOptional), "");
}

TEST(OptionalMovableDeathTest, ConstMoveDereferenceOperatorNoValue)
{
	printf("Trying to move dereference a value from a constant disengaged optional\n");
	const nctl::Optional<Movable> newOptional;

	ASSERT_DEATH(*nctl::move(newOptional), "");
}

TEST(OptionalMovableDeathTest, ArrowDereferenceOperator)
{
	printf("Trying to arrow dereference a value from a disengaged optional\n");
	nctl::Optional<Movable> newOptional;

	ASSERT_DEATH(newOptional->size(), "");
}

TEST(OptionalMovableDeathTest, ConstArrowDereferenceOperator)
{
	printf("Trying to arrow dereference a value from a constant disengaged optional\n");
	const nctl::Optional<Movable> newOptional;

	ASSERT_DEATH(newOptional->size(), "");
}
#endif

TEST_F(OptionalMovableTest, Constructor)
{
	printf("Constructing a new optional with the default constructor: ");
	nctl::Optional<Movable> newOptional;
	printOptional(newOptional);

	ASSERT_FALSE(newOptional.hasValue());
}

TEST_F(OptionalMovableTest, ConstructorValue)
{
	printf("Constructing a new optional with a specified initial value: ");
	nctl::Optional<Movable> newOptional(Movable{Movable::Construction::INITIALIZED});
	printOptional(newOptional);

	ASSERT_TRUE(newOptional.hasValue());
	ASSERT_EQ(newOptional.value().size(), Movable::Size);
}

TEST_F(OptionalMovableTest, CopyConstructor)
{
	printf("Constructing a second optional as a copy of the first: ");
	nctl::Optional<Movable> newOptional(optional);
	printOptional(newOptional);

	ASSERT_TRUE(optional.hasValue());
	ASSERT_EQ(optional.value().size(), Movable::Size);
	ASSERT_TRUE(newOptional.hasValue());
	ASSERT_EQ(newOptional.value().size(), Movable::Size);
}

TEST_F(OptionalMovableTest, MoveConstructor)
{
	printf("Constructing a second optional moving from the first: ");
	nctl::Optional<Movable> newOptional(nctl::move(optional));
	printOptional(newOptional);

	ASSERT_TRUE(optional.hasValue());
	ASSERT_TRUE(newOptional.hasValue());
	ASSERT_EQ(newOptional.value().size(), Movable::Size);
}

TEST_F(OptionalMovableTest, CopyAssignmentOperator)
{
	printf("Copy-assigning the first optional to the second one: ");
	nctl::Optional<Movable> newOptional;
	newOptional = optional;
	printOptional(newOptional);

	ASSERT_TRUE(optional.hasValue());
	ASSERT_EQ(optional.value().size(), Movable::Size);
	ASSERT_TRUE(newOptional.hasValue());
	ASSERT_EQ(newOptional.value().size(), Movable::Size);
}

TEST_F(OptionalMovableTest, MoveAssignmentOperator)
{
	printf("Move-assigning the first optional to the second one: ");
	nctl::Optional<Movable> newOptional;
	newOptional = nctl::move(optional);
	printOptional(newOptional);

	ASSERT_TRUE(optional.hasValue());
	ASSERT_TRUE(newOptional.hasValue());
	ASSERT_EQ(newOptional.value().size(), Movable::Size);
}

TEST_F(OptionalMovableTest, EmplaceValue)
{
	printf("Emplacing a value into a new optional: ");
	nctl::Optional<Movable> newOptional;
	newOptional.emplace(Movable(Movable::Construction::INITIALIZED));
	printOptional(newOptional);

	ASSERT_TRUE(newOptional.hasValue());
	ASSERT_EQ(newOptional.value().size(), Movable::Size);
}

TEST(OptionalTest, MakeOptional)
{
	printf("Creating a new optional with makeOptional: ");
	nctl::Optional<Movable> newOptional =
	        nctl::makeOptional<Movable>(Movable(Movable::Construction::INITIALIZED));
	printOptional(newOptional);

	ASSERT_TRUE(newOptional.hasValue());
	ASSERT_EQ(newOptional.value().size(), Movable::Size);
}

TEST_F(OptionalMovableTest, Reset)
{
	printf("Constructing a new optional with a specified initial value: ");
	nctl::Optional<Movable> newOptional(Movable{Movable::Construction::INITIALIZED});
	printOptional(newOptional);
	ASSERT_TRUE(newOptional.hasValue());
	ASSERT_EQ(newOptional.value().size(), Movable::Size);

	printf("Resetting the optional: ");
	newOptional.reset();
	printOptional(newOptional);
	ASSERT_FALSE(newOptional.hasValue());
	ASSERT_FALSE(bool(newOptional)); // Also checking the bool operator on a resetted otpional
}

TEST_F(OptionalMovableTest, ValueOrDisengaged)
{
	printf("Constructing a new optional with the default constructor: ");
	nctl::Optional<Movable> newOptional;
	printOptional(newOptional);
	ASSERT_FALSE(newOptional.hasValue());

	const Movable newValue = newOptional.valueOr(Movable(Movable::Construction::INITIALIZED));
	printf("Copied value size: %u\n", newValue.size());
	// Optional is disengaged and the value will get the initialized movable array
	ASSERT_EQ(newValue.size(), Movable::Size);
	for (unsigned int i = 0; i < Movable::Size; i++)
		ASSERT_EQ(newValue[i], i);
}

TEST_F(OptionalMovableTest, ValueOrEngaged)
{
	printf("Constructing a new optional with a specified initial value: ");
	nctl::Optional<Movable> newOptional(Movable{});
	printOptional(newOptional);
	ASSERT_TRUE(newOptional.hasValue());

	const Movable newValue = newOptional.valueOr(Movable(Movable::Construction::INITIALIZED));
	printf("Copied value size: %u\n", newValue.size());
	// Optional is engaged and the value will retain the empty movable array
	ASSERT_EQ(newValue.size(), 0);
}

TEST_F(OptionalMovableTest, BoolOperator)
{
	printf("Checking that the optional has a value using the bool operator: %d\n", bool(optional));
	ASSERT_TRUE(optional.hasValue());
	ASSERT_TRUE(bool(optional));
}

TEST_F(OptionalMovableTest, DereferenceOperator)
{
	Movable newValue = *optional;
	printf("Copied value size: %u\n", newValue.size());

	ASSERT_TRUE(optional.hasValue());
	ASSERT_EQ(optional.value().size(), Movable::Size);
	ASSERT_EQ(newValue.size(), Movable::Size);
}

TEST_F(OptionalMovableTest, ConstDereferenceOperator)
{
	const Movable newValue = *optional;
	printf("Copied value size: %u\n", newValue.size());

	ASSERT_TRUE(optional.hasValue());
	ASSERT_EQ(optional.value().size(), Movable::Size);
	ASSERT_EQ(newValue.size(), Movable::Size);
}

TEST_F(OptionalMovableTest, MoveDereferenceOperator)
{
	printf("Constructing a new optional with a specified initial value: ");
	nctl::Optional<Movable> newOptional(Movable{Movable::Construction::INITIALIZED});
	printOptional(newOptional);

	Movable newValue = *nctl::move(newOptional);
	printf("Moved value size: %u\n", newValue.size());

	ASSERT_TRUE(newOptional.hasValue());
	ASSERT_EQ(newOptional.value().size(), 0);
	ASSERT_EQ(newValue.size(), Movable::Size);
}

TEST_F(OptionalMovableTest, ConstMoveDereferenceOperator)
{
	printf("Constructing a new optional with a specified initial value: ");
	nctl::Optional<Movable> newOptional(Movable{Movable::Construction::INITIALIZED});
	printOptional(newOptional);

	const Movable newValue = *nctl::move(newOptional);
	printf("Moved value size: %u\n", newValue.size());

	ASSERT_TRUE(newOptional.hasValue());
	ASSERT_EQ(newOptional.value().size(), 0);
	ASSERT_EQ(newValue.size(), Movable::Size);
}

TEST_F(OptionalMovableTest, ArrowDereferenceOperator)
{
	printf("Constructing a new optional with a specified initial value: ");
	nctl::Optional<Movable> newOptional(Movable{Movable::Construction::INITIALIZED});
	printOptional(newOptional);

	uint32_t size = newOptional->size();
	printf("Moved value size: %u\n", size);

	ASSERT_TRUE(newOptional.hasValue());
	ASSERT_EQ(newOptional->size(), Movable::Size);
	ASSERT_EQ(size, Movable::Size);
}

TEST_F(OptionalMovableTest, ConstArrowDereferenceOperator)
{
	printf("Constructing a new optional with a specified initial value: ");
	nctl::Optional<Movable> newOptional(Movable{Movable::Construction::INITIALIZED});
	printOptional(newOptional);

	const uint32_t size = newOptional->size();
	printf("Moved value size: %u\n", size);

	ASSERT_TRUE(newOptional.hasValue());
	ASSERT_EQ(newOptional->size(), Movable::Size);
	ASSERT_EQ(size, Movable::Size);
}

TEST_F(OptionalMovableTest, EqualityOperator)
{
	printf("Constructing a new optional with a specified initial value: ");
	nctl::Optional<Movable> newOptional(Movable{Movable::Construction::INITIALIZED});
	printOptional(newOptional);

	printf("Checking equality with the first optional: %d\n", newOptional == optional);
	ASSERT_TRUE(newOptional == optional);
	ASSERT_EQ(newOptional, optional);
}

TEST_F(OptionalMovableTest, InqualityOperator)
{
	printf("Constructing a new optional with a specified initial value: ");
	nctl::Optional<Movable> newOptional(Movable{});
	printOptional(newOptional);

	printf("Checking inequality with the first optional: %d\n", newOptional != optional);
	ASSERT_TRUE(newOptional != optional);
	ASSERT_NE(newOptional, optional);
}

TEST_F(OptionalMovableTest, ValueEqualityOperator)
{
	const Movable newValue(Movable::Construction::INITIALIZED);
	printf("Creating a new value with size: %u\n", newValue.size());

	printf("Checking equality with the first optional: %d, %d\n",
	       newValue == optional, optional == newValue);
	ASSERT_TRUE(newValue == optional);
	ASSERT_EQ(newValue, optional);

	ASSERT_TRUE(optional == newValue);
	ASSERT_EQ(optional, newValue);
}

TEST_F(OptionalMovableTest, ValueInqualityOperator)
{
	const Movable newValue;
	printf("Creating a new value with size: %u\n", newValue.size());

	printf("Checking inequality with the first optional: %d, %d\n",
	       newValue != optional, optional != newValue);
	ASSERT_TRUE(newValue != optional);
	ASSERT_NE(newValue, optional);

	ASSERT_TRUE(optional != newValue);
	ASSERT_NE(optional, newValue);
}

}
