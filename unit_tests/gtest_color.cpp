#include "gtest_color.h"

namespace {

class ColorTest : public ::testing::Test
{
  public:
	ColorTest()
	    : col1_(128, 128, 128, 128), col2_(64, 85, 64, 127) {}

	nc::Color col1_;
	nc::Color col2_;
};

const unsigned char red = 64;
const unsigned char green = 64 * 2;
const unsigned char blue = 64 * 3;
const unsigned char alpha = 0;

TEST_F(ColorTest, DefaultConstructor)
{
	const nc::Color color;
	printColor("Constructing a new color with the default white constructor: ", color);

	ASSERT_EQ(color.data()[0], 255);
	ASSERT_EQ(color.data()[1], 255);
	ASSERT_EQ(color.data()[2], 255);
	ASSERT_EQ(color.data()[3], 255);
}

TEST_F(ColorTest, ConstructFromThreeComponents)
{
	const nc::Color color(red, green, blue);
	printColor("Constructing a new color from three components: ", color);

	ASSERT_EQ(color.data()[0], red);
	ASSERT_EQ(color.data()[1], green);
	ASSERT_EQ(color.data()[2], blue);
	ASSERT_EQ(color.data()[3], 255);
}

TEST_F(ColorTest, ConstructFromFourComponents)
{
	const nc::Color color(red, green, blue, alpha);
	printColor("Constructing a new color from four components: ", color);

	ASSERT_EQ(color.data()[0], red);
	ASSERT_EQ(color.data()[1], green);
	ASSERT_EQ(color.data()[2], blue);
	ASSERT_EQ(color.data()[3], alpha);
}

TEST_F(ColorTest, ConstructFromFourComponentsAndClamp)
{
	const unsigned int unclamped = static_cast<unsigned int>(-1);
	const nc::Color color(unclamped, unclamped, unclamped, unclamped);
	printColor("Constructing a new color from four unclamped components: ", color);

	ASSERT_EQ(color.data()[0], 255);
	ASSERT_EQ(color.data()[1], 255);
	ASSERT_EQ(color.data()[2], 255);
	ASSERT_EQ(color.data()[3], 255);
}

TEST_F(ColorTest, ConstructFromArray)
{
	const unsigned int vec[nc::Color::NumChannels] = { red, green, blue, alpha };
	const nc::Color color(vec);
	printColor("Constructing a new color from an array: ", color);

	ASSERT_EQ(color.data()[0], vec[0]);
	ASSERT_EQ(color.data()[1], vec[1]);
	ASSERT_EQ(color.data()[2], vec[2]);
	ASSERT_EQ(color.data()[3], vec[3]);
}

TEST_F(ColorTest, ConstructFromFloatColor)
{
	nc::Color color(nc::Colorf::Black);
	printColor("Constructing a new color from a normalized float one: ", color);

	ASSERT_EQ(color.r(), 0);
	ASSERT_EQ(color.g(), 0);
	ASSERT_EQ(color.b(), 0);
	ASSERT_EQ(color.a(), 255);
}

TEST_F(ColorTest, Getters)
{
	const nc::Color color(red, green, blue, alpha);
	printColor("Constructing a new color and testing its getters: ", color);

	ASSERT_EQ(color.r(), red);
	ASSERT_EQ(color.g(), green);
	ASSERT_EQ(color.b(), blue);
	ASSERT_EQ(color.a(), alpha);
}

TEST_F(ColorTest, SetThreeComponents)
{
	nc::Color color;
	color.set(red, green, blue);
	printColor("Constructing a new color and setting three components: ", color);

	ASSERT_EQ(color.r(), red);
	ASSERT_EQ(color.g(), green);
	ASSERT_EQ(color.b(), blue);
	ASSERT_EQ(color.a(), 255);
}

TEST_F(ColorTest, SetFourComponents)
{
	nc::Color color;
	color.set(red, green, blue, alpha);
	printColor("Constructing a new color and setting four components: ", color);

	ASSERT_EQ(color.r(), red);
	ASSERT_EQ(color.g(), green);
	ASSERT_EQ(color.b(), blue);
	ASSERT_EQ(color.a(), alpha);
}

TEST_F(ColorTest, SetArray)
{
	const unsigned int vec[nc::Color::NumChannels] = { red, green, blue, alpha };
	nc::Color color;
	color.setVec(vec);
	printColor("Constructing a new color and setting components from an array: ", color);

	ASSERT_EQ(color.data()[0], vec[0]);
	ASSERT_EQ(color.data()[1], vec[1]);
	ASSERT_EQ(color.data()[2], vec[2]);
	ASSERT_EQ(color.data()[3], vec[3]);
}

TEST_F(ColorTest, SetAlpha)
{
	nc::Color color;
	color.setAlpha(alpha);
	printColor("Constructing a new color and setting the alpha component: ", color);

	ASSERT_EQ(color.r(), 255);
	ASSERT_EQ(color.g(), 255);
	ASSERT_EQ(color.b(), 255);
	ASSERT_EQ(color.a(), alpha);
}

TEST_F(ColorTest, AssignFloatColor)
{
	nc::Color color;
	color = nc::Colorf::Black;
	printColor("Assigning a new color from a normalized float one: ", color);

	ASSERT_EQ(color.r(), 0);
	ASSERT_EQ(color.g(), 0);
	ASSERT_EQ(color.b(), 0);
	ASSERT_EQ(color.a(), 255);
}

TEST_F(ColorTest, EqualityOperator)
{
	const nc::Color color1 = nc::Color::Red;
	const nc::Color color2 = nc::Color::Red;

	ASSERT(color1 == color2);
}

TEST_F(ColorTest, AdditionInPlace)
{
	const nc::Color oldCol1 = col1_;

	printColor("color1: ", col1_);
	printColor("color2: ", col2_);
	printColor("Adding the second color to the first: ", col1_ += col2_);

	ASSERT_EQ(col1_.r(), static_cast<unsigned char>(oldCol1.r() + col2_.r()));
	ASSERT_EQ(col1_.g(), static_cast<unsigned char>(oldCol1.g() + col2_.g()));
	ASSERT_EQ(col1_.b(), static_cast<unsigned char>(oldCol1.b() + col2_.b()));
	ASSERT_EQ(col1_.a(), static_cast<unsigned char>(oldCol1.a() + col2_.a()));
}

TEST_F(ColorTest, SubtractionInPlace)
{
	const nc::Color oldCol1 = col1_;

	printColor("color1: ", col1_);
	printColor("color2: ", col2_);
	printColor("Subtracting the second color from the first: ", col1_ -= col2_);

	ASSERT_EQ(col1_.r(), static_cast<unsigned char>(oldCol1.r() - col2_.r()));
	ASSERT_EQ(col1_.g(), static_cast<unsigned char>(oldCol1.g() - col2_.g()));
	ASSERT_EQ(col1_.b(), static_cast<unsigned char>(oldCol1.b() - col2_.b()));
	ASSERT_EQ(col1_.a(), static_cast<unsigned char>(oldCol1.a() - col2_.a()));
}

TEST_F(ColorTest, MultiplicationInPlace)
{
	const nc::Color oldCol1 = col1_;

	printColor("color1: ", col1_);
	printColor("color2: ", col2_);
	printColor("Multiplying the first color by the second: ", col1_ *= col2_);

	ASSERT_EQ(col1_.r(), static_cast<unsigned char>(oldCol1.r() * (col2_.r() / 255.0f)));
	ASSERT_EQ(col1_.g(), static_cast<unsigned char>(oldCol1.g() * (col2_.g() / 255.0f)));
	ASSERT_EQ(col1_.b(), static_cast<unsigned char>(oldCol1.b() * (col2_.b() / 255.0f)));
	ASSERT_EQ(col1_.a(), static_cast<unsigned char>(oldCol1.a() * (col2_.a() / 255.0f)));
}

TEST_F(ColorTest, MultiplyScalarInPlace)
{
	const float scalar = 0.5f;
	const nc::Color oldCol1 = col1_;

	printColor("color1: ", col1_);
	printf("Multiplying the first color by the scalar %.2f: ", scalar);
	printColor(col1_ *= scalar);

	ASSERT_EQ(col1_.r(), oldCol1.r() * scalar);
	ASSERT_EQ(col1_.g(), oldCol1.g() * scalar);
	ASSERT_EQ(col1_.b(), oldCol1.b() * scalar);
	ASSERT_EQ(col1_.a(), oldCol1.a() * scalar);
}

TEST_F(ColorTest, MultiplyScalarInPlaceAndClamp)
{
	const float scalar = 10.0f;

	printColor("color1: ", col1_);
	printf("Multiplying the first color by the scalar %.2f: ", scalar);
	printColor(col1_ *= scalar);

	ASSERT_EQ(col1_.r(), 255);
	ASSERT_EQ(col1_.g(), 255);
	ASSERT_EQ(col1_.b(), 255);
	ASSERT_EQ(col1_.a(), 255);
}

TEST_F(ColorTest, Addition)
{
	printColor("color1: ", col1_);
	printColor("color2: ", col2_);
	const nc::Color add = col1_ + col2_;
	printColor("Color addition: ", add);

	ASSERT_EQ(add.r(), static_cast<unsigned char>(col1_.r() + col2_.r()));
	ASSERT_EQ(add.g(), static_cast<unsigned char>(col1_.g() + col2_.g()));
	ASSERT_EQ(add.b(), static_cast<unsigned char>(col1_.b() + col2_.b()));
	ASSERT_EQ(add.a(), static_cast<unsigned char>(col1_.a() + col2_.a()));
}

TEST_F(ColorTest, Subtraction)
{
	printColor("color1: ", col1_);
	printColor("color2: ", col2_);
	const nc::Color sub = col1_ - col2_;
	printColor("Color subtraction: ", sub);

	ASSERT_EQ(sub.r(), static_cast<unsigned char>(col1_.r() - col2_.r()));
	ASSERT_EQ(sub.g(), static_cast<unsigned char>(col1_.g() - col2_.g()));
	ASSERT_EQ(sub.b(), static_cast<unsigned char>(col1_.b() - col2_.b()));
	ASSERT_EQ(sub.a(), static_cast<unsigned char>(col1_.a() - col2_.a()));
}

TEST_F(ColorTest, Multiplication)
{
	printColor("color1: ", col1_);
	printColor("color2: ", col2_);
	const nc::Color mul = col1_ * col2_;
	printColor("Color multiplication: ", mul);

	ASSERT_EQ(mul.r(), static_cast<unsigned char>(col1_.r() * (col2_.r() / 255.0f)));
	ASSERT_EQ(mul.g(), static_cast<unsigned char>(col1_.g() * (col2_.g() / 255.0f)));
	ASSERT_EQ(mul.b(), static_cast<unsigned char>(col1_.b() * (col2_.b() / 255.0f)));
	ASSERT_EQ(mul.a(), static_cast<unsigned char>(col1_.a() * (col2_.a() / 255.0f)));
}

TEST_F(ColorTest, MultiplyScalar)
{
	const float scalar = 0.5f;
	printColor("color1: ", col1_);
	const nc::Color mul = col1_ * scalar;
	printf("Multiplication by scalar %.2f: ", scalar);
	printColor(mul);

	ASSERT_EQ(mul.r(), col1_.r() * scalar);
	ASSERT_EQ(mul.g(), col1_.g() * scalar);
	ASSERT_EQ(mul.b(), col1_.b() * scalar);
	ASSERT_EQ(mul.a(), col1_.a() * scalar);
}

TEST_F(ColorTest, MultiplyScalarAndClamp)
{
	const float scalar = 10.0f;
	printColor("color1: ", col1_);
	const nc::Color mul = col1_ * scalar;
	printf("Multiplication by scalar %.2f: ", scalar);
	printColor(mul);

	ASSERT_EQ(mul.r(), 255);
	ASSERT_EQ(mul.g(), 255);
	ASSERT_EQ(mul.b(), 255);
	ASSERT_EQ(mul.a(), 255);
}

}
