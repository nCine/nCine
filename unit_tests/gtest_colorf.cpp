#include "gtest_color.h"

namespace {

class ColorfTest : public ::testing::Test
{
  public:
	ColorfTest() : col1_(0.25f, 0.5f, 0.25f, 0.5f), col2_(0.5f, 0.5f, 0.5f, 0.5f) { }

	nc::Colorf col1_;
	nc::Colorf col2_;
};

const float red = 0.25f;
const float green = 0.5f;
const float blue = 0.75f;
const float alpha = 0.0f;

TEST_F(ColorfTest, DefaultConstructor)
{
	const nc::Colorf color;
	printf("Constructing a new color with the default white constructor: ");
	printColor(color);

	ASSERT_FLOAT_EQ(color.data()[0], 1.0f);
	ASSERT_FLOAT_EQ(color.data()[1], 1.0f);
	ASSERT_FLOAT_EQ(color.data()[2], 1.0f);
	ASSERT_FLOAT_EQ(color.data()[3], 1.0f);
}

TEST_F(ColorfTest, ConstructFromThreeComponents)
{
	const nc::Colorf color(red, green, blue);
	printf("Constructing a new color from three components: ");
	printColor(color);

	ASSERT_FLOAT_EQ(color.data()[0], red);
	ASSERT_FLOAT_EQ(color.data()[1], green);
	ASSERT_FLOAT_EQ(color.data()[2], blue);
	ASSERT_FLOAT_EQ(color.data()[3], 1.0f);
}

TEST_F(ColorfTest, ConstructFromFourComponents)
{
	const nc::Colorf color(red, green, blue, alpha);
	printf("Constructing a new color from four components: ");
	printColor(color);

	ASSERT_FLOAT_EQ(color.data()[0], red);
	ASSERT_FLOAT_EQ(color.data()[1], green);
	ASSERT_FLOAT_EQ(color.data()[2], blue);
	ASSERT_FLOAT_EQ(color.data()[3], alpha);
}

TEST_F(ColorfTest, ConstructFromFourComponentsAndClamp)
{
	const float unclamped = 5.0f;
	const nc::Colorf color(unclamped, unclamped, unclamped, unclamped);
	printf("Constructing a new color from four unclamped components: ");
	printColor(color);

	ASSERT_FLOAT_EQ(color.data()[0], 1.0f);
	ASSERT_FLOAT_EQ(color.data()[1], 1.0f);
	ASSERT_FLOAT_EQ(color.data()[2], 1.0f);
	ASSERT_FLOAT_EQ(color.data()[3], 1.0f);
}

TEST_F(ColorfTest, ConstructFromArray)
{
	const float vec[4] = {red, green, blue, alpha};
	const nc::Colorf color(vec);
	printf("Constructing a new color from an array: ");
	printColor(color);

	ASSERT_FLOAT_EQ(color.data()[0], vec[0]);
	ASSERT_FLOAT_EQ(color.data()[1], vec[1]);
	ASSERT_FLOAT_EQ(color.data()[2], vec[2]);
	ASSERT_FLOAT_EQ(color.data()[3], vec[3]);
}

TEST_F(ColorfTest, ConstructFromNonFloatColor)
{
	nc::Colorf color(nc::Color::Black);
	printf("Constructing a new color from an unsigned char one: ");
	printColor(color);

	ASSERT_FLOAT_EQ(color.r(), 0.0f);
	ASSERT_FLOAT_EQ(color.g(), 0.0f);
	ASSERT_FLOAT_EQ(color.b(), 0.0f);
	ASSERT_FLOAT_EQ(color.a(), 1.0f);
}

TEST_F(ColorfTest, Getters)
{
	const nc::Colorf color(red, green, blue, alpha);
	printf("Constructing a new color and testing its getters: ");
	printColor(color);

	ASSERT_FLOAT_EQ(color.r(), red);
	ASSERT_FLOAT_EQ(color.g(), green);
	ASSERT_FLOAT_EQ(color.b(), blue);
	ASSERT_FLOAT_EQ(color.a(), alpha);
}

TEST_F(ColorfTest, SetThreeComponents)
{
	nc::Colorf color;
	color.set(red, green, blue);
	printf("Constructing a new color and setting three components: ");
	printColor(color);

	ASSERT_FLOAT_EQ(color.r(), red);
	ASSERT_FLOAT_EQ(color.g(), green);
	ASSERT_FLOAT_EQ(color.b(), blue);
	ASSERT_FLOAT_EQ(color.a(), 1.0f);
}

TEST_F(ColorfTest, SetFourComponents)
{
	nc::Colorf color;
	color.set(red, green, blue, alpha);
	printf("Constructing a new color and setting four components: ");
	printColor(color);

	ASSERT_FLOAT_EQ(color.r(), red);
	ASSERT_FLOAT_EQ(color.g(), green);
	ASSERT_FLOAT_EQ(color.b(), blue);
	ASSERT_FLOAT_EQ(color.a(), alpha);
}

TEST_F(ColorfTest, SetArray)
{
	const float vec[4] = {red, green, blue, alpha};
	nc::Colorf color;
	color.setVec(vec);
	printf("Constructing a new color and setting components from an array: ");
	printColor(color);

	ASSERT_FLOAT_EQ(color.data()[0], vec[0]);
	ASSERT_FLOAT_EQ(color.data()[1], vec[1]);
	ASSERT_FLOAT_EQ(color.data()[2], vec[2]);
	ASSERT_FLOAT_EQ(color.data()[3], vec[3]);
}

TEST_F(ColorfTest, SetAlpha)
{
	nc::Colorf color;
	color.setAlpha(alpha);
	printf("Constructing a new color and setting the alpha component: ");
	printColor(color);

	ASSERT_FLOAT_EQ(color.r(), 1.0f);
	ASSERT_FLOAT_EQ(color.g(), 1.0f);
	ASSERT_FLOAT_EQ(color.b(), 1.0f);
	ASSERT_FLOAT_EQ(color.a(), alpha);
}

TEST_F(ColorfTest, AssignNonFloatColor)
{
	nc::Colorf color;
	color = nc::Color::Black;
	printf("Assigning a new color from an unsigned char one: ");
	printColor(color);

	ASSERT_FLOAT_EQ(color.r(), 0.0f);
	ASSERT_FLOAT_EQ(color.g(), 0.0f);
	ASSERT_FLOAT_EQ(color.b(), 0.0f);
	ASSERT_FLOAT_EQ(color.a(), 1.0f);
}

TEST_F(ColorfTest, EqualityOperatorAfterConversion)
{
	const nc::Colorf color1 = nc::Colorf::Red;
	const nc::Colorf color2 = nc::Colorf::Red;

	ASSERT(nc::Color(color1) == nc::Color(color2));
}

TEST_F(ColorfTest, MultiplicationInPlace)
{
	const nc::Colorf oldCol1 = col1_;

	printf("color1: "); printColor(col1_);
	printf("color2: "); printColor(col2_);
	printf("Multiplying the first color by the second: ");
	printColor(col1_ *= col2_);

	ASSERT_FLOAT_EQ(col1_.r(), oldCol1.r() * col2_.r());
	ASSERT_FLOAT_EQ(col1_.g(), oldCol1.g() * col2_.g());
	ASSERT_FLOAT_EQ(col1_.b(), oldCol1.b() * col2_.b());
	ASSERT_FLOAT_EQ(col1_.a(), oldCol1.a() * col2_.a());
}

TEST_F(ColorfTest, MultiplyScalarInPlace)
{
	const float scalar = 0.5f;
	const nc::Colorf oldCol1 = col1_;

	printf("color1: "); printColor(col1_);
	printf("Multiplying the first color by the scalar %.2f: ", scalar);
	printColor(col1_ *= scalar);

	ASSERT_FLOAT_EQ(col1_.r(), oldCol1.r() * scalar);
	ASSERT_FLOAT_EQ(col1_.g(), oldCol1.g() * scalar);
	ASSERT_FLOAT_EQ(col1_.b(), oldCol1.b() * scalar);
	ASSERT_FLOAT_EQ(col1_.a(), oldCol1.a() * scalar);
}

TEST_F(ColorfTest, MultiplyScalarInPlaceAndClamp)
{
	const float scalar = 10.0f;
	const nc::Colorf oldCol1 = col1_;

	printf("color1: "); printColor(col1_);
	printf("Multiplying the first color by the scalar %.2f: ", scalar);
	printColor(col1_ *= scalar);

	ASSERT_FLOAT_EQ(col1_.r(), 1.0f);
	ASSERT_FLOAT_EQ(col1_.g(), 1.0f);
	ASSERT_FLOAT_EQ(col1_.b(), 1.0f);
	ASSERT_FLOAT_EQ(col1_.a(), 1.0f);
}

TEST_F(ColorfTest, Multiplication)
{
	printf("color1: "); printColor(col1_);
	printf("color2: "); printColor(col2_);
	const nc::Colorf mul = col1_ * col2_;
	printf("Color multiplication: ");
	printColor(mul);

	ASSERT_FLOAT_EQ(mul.r(), col1_.r() * col2_.r());
	ASSERT_FLOAT_EQ(mul.g(), col1_.g() * col2_.g());
	ASSERT_FLOAT_EQ(mul.b(), col1_.b() * col2_.b());
	ASSERT_FLOAT_EQ(mul.a(), col1_.a() * col2_.a());
}

TEST_F(ColorfTest, MultiplyScalar)
{
	const float scalar = 0.5f;
	printf("color1: "); printColor(col1_);
	const nc::Colorf mul = col1_ * scalar;
	printf("Multiplication by scalar %.2f: ", scalar);
	printColor(mul);

	ASSERT_FLOAT_EQ(mul.r(), col1_.r() * scalar);
	ASSERT_FLOAT_EQ(mul.g(), col1_.g() * scalar);
	ASSERT_FLOAT_EQ(mul.b(), col1_.b() * scalar);
	ASSERT_FLOAT_EQ(mul.a(), col1_.a() * scalar);
}

TEST_F(ColorfTest, MultiplyScalarAndClamp)
{
	const float scalar = 10.0f;
	printf("color1: "); printColor(col1_);
	const nc::Colorf mul = col1_ * scalar;
	printf("Multiplication by scalar %.2f: ", scalar);
	printColor(mul);

	ASSERT_FLOAT_EQ(mul.r(), 1.0f);
	ASSERT_FLOAT_EQ(mul.g(), 1.0f);
	ASSERT_FLOAT_EQ(mul.b(), 1.0f);
	ASSERT_FLOAT_EQ(mul.a(), 1.0f);
}

}
