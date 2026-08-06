#include "gtest_variant.h"
#include "test_movable.h"

namespace {

using IntMovableVariant = nctl::Variant<int, Movable>;

struct MovableSizeVisitor
{
	unsigned int operator()(int) const { return 0; }
	unsigned int operator()(const Movable &movable) const { return movable.size(); }
};

void printVariant(const IntMovableVariant &variant)
{
	if (variant.holds<int>())
		printf("(holds int: %d)\n", variant.get<int>());
	else if (variant.holds<Movable>())
		printf("(holds movable with size %u)\n", variant.get<Movable>().size());
}

class VariantMovableTest : public ::testing::Test
{
  public:
	VariantMovableTest()
	    : variant(Movable(Movable::Construction::INITIALIZED)) {}

  protected:
	IntMovableVariant variant;
};

#ifndef __EMSCRIPTEN__
TEST(VariantMovableDeathTest, GetWrongType)
{
	printf("Trying to access a variant holding an int as a movable\n");
	IntMovableVariant newVariant(DefaultValue);

	ASSERT_DEATH(newVariant.get<Movable>(), "");
}

TEST(VariantMovableDeathTest, ConstGetWrongType)
{
	printf("Trying to access a constant variant holding an int as a movable\n");
	const IntMovableVariant newVariant(DefaultValue);

	ASSERT_DEATH(newVariant.get<Movable>(), "");
}
#endif

TEST_F(VariantMovableTest, DefaultConstructor)
{
	printf("Default-constructing a variant: ");
	IntMovableVariant newVariant;
	printVariant(newVariant);

	ASSERT_TRUE(newVariant.holds<int>());
	ASSERT_EQ(newVariant.get<int>(), 0);
}

TEST_F(VariantMovableTest, ConstructorInt)
{
	printf("Constructing a new variant with an int value: ");
	IntMovableVariant newVariant(DefaultValue);
	printVariant(newVariant);

	ASSERT_TRUE(newVariant.holds<int>());
	ASSERT_EQ(newVariant.get<int>(), DefaultValue);
}

TEST_F(VariantMovableTest, ConstructorMovable)
{
	printf("Constructing a new variant with a movable value: ");
	IntMovableVariant newVariant(Movable{Movable::Construction::INITIALIZED});
	printVariant(newVariant);

	ASSERT_TRUE(newVariant.holds<Movable>());
	ASSERT_EQ(newVariant.get<Movable>().size(), Movable::Size);
}

#if !TEST_MOVABLE_ONLY
TEST_F(VariantMovableTest, CopyConstructor)
{
	printf("Constructing a second variant as a copy of the first: ");
	IntMovableVariant newVariant(variant);
	printVariant(newVariant);

	ASSERT_TRUE(variant.holds<Movable>());
	ASSERT_EQ(variant.get<Movable>().size(), Movable::Size);
	ASSERT_TRUE(newVariant.holds<Movable>());
	ASSERT_EQ(newVariant.get<Movable>().size(), Movable::Size);
	ASSERT_TRUE(newVariant.get<Movable>() == variant.get<Movable>());
}
#endif

TEST_F(VariantMovableTest, MoveConstructor)
{
	printf("Constructing a second variant moving from the first: ");
	IntMovableVariant newVariant(nctl::move(variant));
	printVariant(newVariant);

	// The source variant still holds a `Movable`, but it has been moved from
	ASSERT_TRUE(variant.holds<Movable>());
	ASSERT_EQ(variant.get<Movable>().size(), 0);

	ASSERT_TRUE(newVariant.holds<Movable>());
	ASSERT_EQ(newVariant.get<Movable>().size(), Movable::Size);
}

TEST_F(VariantMovableTest, EmplaceMovableOverInt)
{
	printf("Emplacing a movable value over an int value: ");
	IntMovableVariant newVariant(DefaultValue);
	newVariant.emplace<Movable>(Movable::Construction::INITIALIZED);
	printVariant(newVariant);

	ASSERT_TRUE(newVariant.holds<Movable>());
	ASSERT_EQ(newVariant.get<Movable>().size(), Movable::Size);
}

TEST_F(VariantMovableTest, EmplaceIntOverMovable)
{
	printf("Emplacing an int value over a movable value: ");
	variant.emplace<int>(DefaultValue);
	printVariant(variant);

	ASSERT_TRUE(variant.holds<int>());
	ASSERT_EQ(variant.get<int>(), DefaultValue);
}

#if !TEST_MOVABLE_ONLY
TEST_F(VariantMovableTest, CopyAssignmentSameType)
{
	printf("Copy-assigning a variant holding a movable to another one holding a movable: ");
	IntMovableVariant newVariant(Movable{Movable::Construction::INITIALIZED});
	newVariant = variant;
	printVariant(newVariant);

	ASSERT_TRUE(variant.holds<Movable>());
	ASSERT_TRUE(newVariant.holds<Movable>());
	ASSERT_EQ(newVariant.get<Movable>().size(), Movable::Size);
}

TEST_F(VariantMovableTest, CopyAssignmentDifferentType)
{
	printf("Copy-assigning a variant holding a movable to one holding an int: ");
	IntMovableVariant newVariant(DefaultValue);
	newVariant = variant;
	printVariant(newVariant);

	ASSERT_TRUE(variant.holds<Movable>());
	ASSERT_TRUE(newVariant.holds<Movable>());
	ASSERT_FALSE(newVariant.holds<int>());
	ASSERT_EQ(newVariant.get<Movable>().size(), Movable::Size);
}
#endif

TEST_F(VariantMovableTest, MoveAssignmentSameType)
{
	printf("Move-assigning a variant holding a movable to another one holding a movable: ");
	IntMovableVariant newVariant(Movable{Movable::Construction::INITIALIZED});
	newVariant = nctl::move(variant);
	printVariant(newVariant);

	ASSERT_TRUE(newVariant.holds<Movable>());
	ASSERT_EQ(newVariant.get<Movable>().size(), Movable::Size);
}

TEST_F(VariantMovableTest, MoveAssignmentDifferentType)
{
	printf("Move-assigning a variant holding a movable to one holding an int: ");
	IntMovableVariant newVariant(DefaultValue);
	newVariant = nctl::move(variant);
	printVariant(newVariant);

	ASSERT_TRUE(newVariant.holds<Movable>());
	ASSERT_FALSE(newVariant.holds<int>());
	ASSERT_EQ(newVariant.get<Movable>().size(), Movable::Size);
}

TEST_F(VariantMovableTest, ContentIntegrity)
{
	printf("Checking the content of the movable value: ");
	variant.get<Movable>().printAndAssert();

	ASSERT_TRUE(variant.holds<Movable>());
	ASSERT_EQ(variant.get<Movable>().size(), Movable::Size);
}

TEST_F(VariantMovableTest, GetIf)
{
	printf("Retrieving a pointer to the held movable value: ");
	printVariant(variant);

	Movable *ptr = variant.getIf<Movable>();
	ASSERT_NE(ptr, nullptr);
	ASSERT_EQ(ptr->size(), Movable::Size);
	ASSERT_EQ(variant.getIf<int>(), nullptr);
}

TEST_F(VariantMovableTest, ConstGetIf)
{
	const IntMovableVariant &constVariant = variant;
	printf("Retrieving a constant pointer to the held movable value: ");
	printVariant(constVariant);

	const Movable *ptr = constVariant.getIf<Movable>();
	ASSERT_NE(ptr, nullptr);
	ASSERT_EQ(ptr->size(), Movable::Size);
	ASSERT_EQ(constVariant.getIf<int>(), nullptr);
}

TEST_F(VariantMovableTest, Visit)
{
	printf("Visiting a variant holding a movable value: ");
	printVariant(variant);

	MovableSizeVisitor visitor;
	ASSERT_EQ(variant.visit(visitor), Movable::Size);
}

TEST_F(VariantMovableTest, ConstVisit)
{
	const IntMovableVariant &constVariant = variant;
	printf("Visiting a constant variant holding a movable value: ");
	printVariant(constVariant);

	MovableSizeVisitor visitor;
	ASSERT_EQ(constVariant.visit(visitor), Movable::Size);
}

}
