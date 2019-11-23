#include "gtest_color.h"

namespace {

class ColorHdrTest : public ::testing::Test
{
  public:
	ColorHdrTest()
	    : col1_(5.0f, 5.0f, 5.0f), col2_(2.5f, 3.3f, 2.5f) {}

	nc::ColorHdr col1_;
	nc::ColorHdr col2_;
};

const float red = 0.25f;
const float green = 0.5f;
const float blue = 0.75f;

TEST_F(ColorHdrTest, DefaultConstructor)
{
	const nc::ColorHdr color;
	printColor("Constructing a new color with the default white constructor: ", color);

	ASSERT_FLOAT_EQ(color.data()[0], 1.0f);
	ASSERT_FLOAT_EQ(color.data()[1], 1.0f);
	ASSERT_FLOAT_EQ(color.data()[2], 1.0f);
}

TEST_F(ColorHdrTest, ConstructFromThreeComponents)
{
	const nc::ColorHdr color(red, green, blue);
	printColor("Constructing a new color from three components: ", color);

	ASSERT_FLOAT_EQ(color.data()[0], red);
	ASSERT_FLOAT_EQ(color.data()[1], green);
	ASSERT_FLOAT_EQ(color.data()[2], blue);
}

TEST_F(ColorHdrTest, ConstructFromArray)
{
	const float vec[nc::ColorHdr::NumChannels] = { red, green, blue };
	const nc::ColorHdr color(vec);
	printColor("Constructing a new color from an array: ", color);

	ASSERT_FLOAT_EQ(color.data()[0], vec[0]);
	ASSERT_FLOAT_EQ(color.data()[1], vec[1]);
	ASSERT_FLOAT_EQ(color.data()[2], vec[2]);
}

TEST_F(ColorHdrTest, ConstructFromClampedFloatColor)
{
	nc::ColorHdr color(nc::Colorf::Black);
	printColor("Constructing a new color from a float one: ", color);

	ASSERT_FLOAT_EQ(color.r(), 0.0f);
	ASSERT_FLOAT_EQ(color.g(), 0.0f);
	ASSERT_FLOAT_EQ(color.b(), 0.0f);
}

TEST_F(ColorHdrTest, Getters)
{
	const nc::ColorHdr color(red, green, blue);
	printColor("Constructing a new color and testing its getters: ", color);

	ASSERT_FLOAT_EQ(color.r(), red);
	ASSERT_FLOAT_EQ(color.g(), green);
	ASSERT_FLOAT_EQ(color.b(), blue);
}

TEST_F(ColorHdrTest, SetThreeComponents)
{
	nc::ColorHdr color;
	color.set(red, green, blue);
	printColor("Constructing a new color and setting three components: ", color);

	ASSERT_FLOAT_EQ(color.r(), red);
	ASSERT_FLOAT_EQ(color.g(), green);
	ASSERT_FLOAT_EQ(color.b(), blue);
}

TEST_F(ColorHdrTest, SetArray)
{
	const float vec[nc::ColorHdr::NumChannels] = { red, green, blue };
	nc::ColorHdr color;
	color.setVec(vec);
	printColor("Constructing a new color and setting components from an array: ", color);

	ASSERT_FLOAT_EQ(color.data()[0], vec[0]);
	ASSERT_FLOAT_EQ(color.data()[1], vec[1]);
	ASSERT_FLOAT_EQ(color.data()[2], vec[2]);
}

TEST_F(ColorHdrTest, Clamp)
{
	nc::ColorHdr color(-1.0f, -1.0f, -1.0f);
	color.clamp();
	printColor("Constructing a new color and clamping its channels: ", color);

	ASSERT_FLOAT_EQ(color.r(), 0.0f);
	ASSERT_FLOAT_EQ(color.g(), 0.0f);
	ASSERT_FLOAT_EQ(color.b(), 0.0f);
}

TEST_F(ColorHdrTest, Clamped)
{
	nc::ColorHdr color(-1.0f, -1.0f, -1.0f);
	const nc::ColorHdr newColor = color.clamped();
	printColor("Constructing a new color by clamping another one: ", newColor);

	ASSERT_FLOAT_EQ(newColor.r(), 0.0f);
	ASSERT_FLOAT_EQ(newColor.g(), 0.0f);
	ASSERT_FLOAT_EQ(newColor.b(), 0.0f);
}

TEST_F(ColorHdrTest, AssignNonFloatColor)
{
	nc::ColorHdr color;
	color = nc::Colorf::Black;
	printColor("Assigning a new color from a float one: ", color);

	ASSERT_FLOAT_EQ(color.r(), 0.0f);
	ASSERT_FLOAT_EQ(color.g(), 0.0f);
	ASSERT_FLOAT_EQ(color.b(), 0.0f);
}

TEST_F(ColorHdrTest, EqualityOperatorAfterConversion)
{
	const nc::Color color1 = nc::Color(nc::Colorf(nc::ColorHdr::Red));
	const nc::Color color2 = nc::Color(nc::Colorf(nc::ColorHdr::Red));

	ASSERT(color1 == color2);
}

TEST_F(ColorHdrTest, AdditionInPlace)
{
	const nc::ColorHdr oldCol1 = col1_;

	printColor("color1: ", col1_);
	printColor("color2: ", col2_);
	printColor("Adding the second color to the first: ", col1_ += col2_);

	ASSERT_EQ(col1_.r(), oldCol1.r() + col2_.r());
	ASSERT_EQ(col1_.g(), oldCol1.g() + col2_.g());
	ASSERT_EQ(col1_.b(), oldCol1.b() + col2_.b());
}

TEST_F(ColorHdrTest, SubtractionInPlace)
{
	const nc::ColorHdr oldCol1 = col1_;

	printColor("color1: ", col1_);
	printColor("color2: ", col2_);
	printColor("Subtracting the second color from the first: ", col1_ -= col2_);

	ASSERT_EQ(col1_.r(), oldCol1.r() - col2_.r());
	ASSERT_EQ(col1_.g(), oldCol1.g() - col2_.g());
	ASSERT_EQ(col1_.b(), oldCol1.b() - col2_.b());
}

TEST_F(ColorHdrTest, MultiplicationInPlace)
{
	const nc::ColorHdr oldCol1 = col1_;

	printColor("color1: ", col1_);
	printColor("color2: ", col2_);
	printColor("Multiplying the first color by the second: ", col1_ *= col2_);

	ASSERT_FLOAT_EQ(col1_.r(), oldCol1.r() * col2_.r());
	ASSERT_FLOAT_EQ(col1_.g(), oldCol1.g() * col2_.g());
	ASSERT_FLOAT_EQ(col1_.b(), oldCol1.b() * col2_.b());
}

TEST_F(ColorHdrTest, MultiplyScalarInPlace)
{
	const float scalar = 0.5f;
	const nc::ColorHdr oldCol1 = col1_;

	printColor("color1: ", col1_);
	printf("Multiplying the first color by the scalar %.2f: ", scalar);
	printColor(col1_ *= scalar);

	ASSERT_FLOAT_EQ(col1_.r(), oldCol1.r() * scalar);
	ASSERT_FLOAT_EQ(col1_.g(), oldCol1.g() * scalar);
	ASSERT_FLOAT_EQ(col1_.b(), oldCol1.b() * scalar);
}

TEST_F(ColorHdrTest, Addition)
{
	printColor("color1: ", col1_);
	printColor("color2: ", col2_);
	const nc::ColorHdr add = col1_ + col2_;
	printColor("Color addition: ", add);

	ASSERT_EQ(add.r(), col1_.r() + col2_.r());
	ASSERT_EQ(add.g(), col1_.g() + col2_.g());
	ASSERT_EQ(add.b(), col1_.b() + col2_.b());
}

TEST_F(ColorHdrTest, Subtraction)
{
	printColor("color1: ", col1_);
	printColor("color2: ", col2_);
	const nc::ColorHdr sub = col1_ - col2_;
	printColor("Color subtraction: ", sub);

	ASSERT_EQ(sub.r(), col1_.r() - col2_.r());
	ASSERT_EQ(sub.g(), col1_.g() - col2_.g());
	ASSERT_EQ(sub.b(), col1_.b() - col2_.b());
}

TEST_F(ColorHdrTest, Multiplication)
{
	printColor("color1: ", col1_);
	printColor("color2: ", col2_);
	const nc::ColorHdr mul = col1_ * col2_;
	printColor("Color multiplication: ", mul);

	ASSERT_FLOAT_EQ(mul.r(), col1_.r() * col2_.r());
	ASSERT_FLOAT_EQ(mul.g(), col1_.g() * col2_.g());
	ASSERT_FLOAT_EQ(mul.b(), col1_.b() * col2_.b());
}

TEST_F(ColorHdrTest, MultiplyScalar)
{
	const float scalar = 0.5f;
	printColor("color1: ", col1_);
	const nc::ColorHdr mul = col1_ * scalar;
	printf("Multiplication by scalar %.2f: ", scalar);
	printColor(mul);

	ASSERT_FLOAT_EQ(mul.r(), col1_.r() * scalar);
	ASSERT_FLOAT_EQ(mul.g(), col1_.g() * scalar);
	ASSERT_FLOAT_EQ(mul.b(), col1_.b() * scalar);
}

}
