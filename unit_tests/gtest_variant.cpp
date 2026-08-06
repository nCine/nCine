#include "gtest_variant.h"

namespace {

struct VariantTypeName
{
	const char *operator()(int) const { return "int"; }
	const char *operator()(float) const { return "float"; }
};

class VariantTest : public ::testing::Test
{
  public:
	VariantTest()
	    : variant(DefaultValue) {}

  protected:
	IntFloatVariant variant;
};

TEST_F(VariantTest, DefaultConstructor)
{
	printf("Default-constructing a variant: ");
	IntFloatVariant newVariant;
	printVariant(newVariant);

	ASSERT_TRUE(newVariant.holds<int>());
	ASSERT_EQ(newVariant.get<int>(), 0);
}

#ifndef __EMSCRIPTEN__
TEST(VariantDeathTest, GetWrongType)
{
	printf("Trying to access a variant holding an int as a float\n");
	IntFloatVariant newVariant(DefaultValue);

	ASSERT_DEATH(newVariant.get<float>(), "");
}

TEST(VariantDeathTest, ConstGetWrongType)
{
	printf("Trying to access a constant variant holding an int as a float\n");
	const IntFloatVariant newVariant(DefaultValue);

	ASSERT_DEATH(newVariant.get<float>(), "");
}
#endif

TEST_F(VariantTest, ConstructorInt)
{
	printf("Constructing a new variant with an int value: ");
	IntFloatVariant newVariant(DefaultValue);
	printVariant(newVariant);

	ASSERT_TRUE(newVariant.holds<int>());
	ASSERT_FALSE(newVariant.holds<float>());
	ASSERT_EQ(newVariant.get<int>(), DefaultValue);
}

TEST_F(VariantTest, ConstructorFloat)
{
	printf("Constructing a new variant with a float value: ");
	IntFloatVariant newVariant(DefaultFloatValue);
	printVariant(newVariant);

	ASSERT_TRUE(newVariant.holds<float>());
	ASSERT_FALSE(newVariant.holds<int>());
	ASSERT_FLOAT_EQ(newVariant.get<float>(), DefaultFloatValue);
}

TEST_F(VariantTest, CopyConstructor)
{
	printf("Constructing a second variant as a copy of the first: ");
	IntFloatVariant newVariant(variant);
	printVariant(newVariant);

	ASSERT_TRUE(variant.holds<int>());
	ASSERT_EQ(variant.get<int>(), DefaultValue);
	ASSERT_TRUE(newVariant.holds<int>());
	ASSERT_EQ(newVariant.get<int>(), DefaultValue);
}

TEST_F(VariantTest, MoveConstructor)
{
	printf("Constructing a second variant moving from the first: ");
	IntFloatVariant newVariant(nctl::move(variant));
	printVariant(newVariant);

	ASSERT_TRUE(variant.holds<int>());
	ASSERT_TRUE(newVariant.holds<int>());
	ASSERT_EQ(newVariant.get<int>(), DefaultValue);
}

TEST_F(VariantTest, EmplaceDifferentType)
{
	printf("Emplacing a float value over an int value: ");
	variant.emplace<float>(DefaultFloatValue);
	printVariant(variant);

	ASSERT_TRUE(variant.holds<float>());
	ASSERT_FALSE(variant.holds<int>());
	ASSERT_FLOAT_EQ(variant.get<float>(), DefaultFloatValue);
}

TEST_F(VariantTest, EmplaceSameType)
{
	printf("Emplacing a new int value over the previous one: ");
	variant.emplace<int>(DefaultValue + 1);
	printVariant(variant);

	ASSERT_TRUE(variant.holds<int>());
	ASSERT_EQ(variant.get<int>(), DefaultValue + 1);
}

TEST_F(VariantTest, CopyAssignmentSameType)
{
	printf("Copy-assigning a variant holding an int to another one holding an int: ");
	IntFloatVariant newVariant(DefaultValue + 1);
	newVariant = variant;
	printVariant(newVariant);

	ASSERT_TRUE(variant.holds<int>());
	ASSERT_TRUE(newVariant.holds<int>());
	ASSERT_EQ(newVariant.get<int>(), DefaultValue);
}

TEST_F(VariantTest, CopyAssignmentDifferentType)
{
	printf("Copy-assigning a variant holding an int to one holding a float: ");
	IntFloatVariant newVariant(DefaultFloatValue);
	newVariant = variant;
	printVariant(newVariant);

	ASSERT_TRUE(variant.holds<int>());
	ASSERT_TRUE(newVariant.holds<int>());
	ASSERT_FALSE(newVariant.holds<float>());
	ASSERT_EQ(newVariant.get<int>(), DefaultValue);
}

TEST_F(VariantTest, MoveAssignmentSameType)
{
	printf("Move-assigning a variant holding an int to another one holding an int: ");
	IntFloatVariant newVariant(DefaultValue + 1);
	newVariant = nctl::move(variant);
	printVariant(newVariant);

	ASSERT_TRUE(variant.holds<int>());
	ASSERT_TRUE(newVariant.holds<int>());
	ASSERT_EQ(newVariant.get<int>(), DefaultValue);
}

TEST_F(VariantTest, MoveAssignmentDifferentType)
{
	printf("Move-assigning a variant holding an int to one holding a float: ");
	IntFloatVariant newVariant(DefaultFloatValue);
	newVariant = nctl::move(variant);
	printVariant(newVariant);

	ASSERT_TRUE(variant.holds<int>());
	ASSERT_TRUE(newVariant.holds<int>());
	ASSERT_FALSE(newVariant.holds<float>());
	ASSERT_EQ(newVariant.get<int>(), DefaultValue);
}

TEST_F(VariantTest, SelfCopyAssignment)
{
	printf("Copy-assigning a variant to itself: ");
	variant = variant;
	printVariant(variant);

	ASSERT_TRUE(variant.holds<int>());
	ASSERT_EQ(variant.get<int>(), DefaultValue);
}

TEST_F(VariantTest, SelfMoveAssignment)
{
	printf("Move-assigning a variant to itself: ");
	variant = nctl::move(variant);
	printVariant(variant);

	ASSERT_TRUE(variant.holds<int>());
	ASSERT_EQ(variant.get<int>(), DefaultValue);
}

TEST_F(VariantTest, ConstGet)
{
	const IntFloatVariant &constVariant = variant;
	printf("Accessing a constant variant holding an int: ");
	printVariant(constVariant);

	ASSERT_TRUE(constVariant.holds<int>());
	ASSERT_EQ(constVariant.get<int>(), DefaultValue);
}

TEST_F(VariantTest, GetIf)
{
	printf("Retrieving a pointer to the held int value: ");
	printVariant(variant);

	ASSERT_NE(variant.getIf<int>(), nullptr);
	ASSERT_EQ(*variant.getIf<int>(), DefaultValue);
	ASSERT_EQ(variant.getIf<float>(), nullptr);
}

TEST_F(VariantTest, ConstGetIf)
{
	const IntFloatVariant &constVariant = variant;
	printf("Retrieving a constant pointer to the held int value: ");
	printVariant(constVariant);

	ASSERT_NE(constVariant.getIf<int>(), nullptr);
	ASSERT_EQ(*constVariant.getIf<int>(), DefaultValue);
	ASSERT_EQ(constVariant.getIf<float>(), nullptr);
}

TEST_F(VariantTest, Visit)
{
	printf("Visiting a variant holding an int: ");
	printVariant(variant);

	VariantTypeName visitor;
	ASSERT_STREQ(variant.visit(visitor), "int");
}

TEST_F(VariantTest, ConstVisit)
{
	const IntFloatVariant &constVariant = variant;
	printf("Visiting a constant variant holding an int: ");
	printVariant(constVariant);

	VariantTypeName visitor;
	ASSERT_STREQ(constVariant.visit(visitor), "int");
}

}
