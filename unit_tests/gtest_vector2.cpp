#include "gtest_vector.h"

namespace {

class Vector2Test : public ::testing::Test
{
  public:
	Vector2Test()
	    : v1_(2.5f, 2.0f), v2_(1.0f, 1.5f) {}

	nc::Vector2f v1_;
	nc::Vector2f v2_;
};

TEST_F(Vector2Test, DefaultConstructor)
{
	const nc::Vector2f newVector;
	printf("Constructing a new vector with the default constructor: ");
	printVector(newVector);

	ASSERT_FLOAT_EQ(newVector.x, 0.0f);
	ASSERT_FLOAT_EQ(newVector.y, 0.0f);
}

TEST_F(Vector2Test, ConstructWithSingleScalar)
{
	const float scalar = 0.0f;
	const nc::Vector2f newVector(scalar);
	printf("Constructing a new vector with a single scalar %.2f: ", scalar);
	printVector(newVector);

	ASSERT_FLOAT_EQ(newVector.x, scalar);
	ASSERT_FLOAT_EQ(newVector.y, scalar);
}

TEST_F(Vector2Test, ConstructWithMultipleScalars)
{
	const float x = 1.0f;
	const float y = 2.0f;
	const nc::Vector2f newVector(x, y);
	printf("Constructing a new vector with multiple scalars <%.2f, %.2f>: ", x, y);
	printVector(newVector);

	ASSERT_FLOAT_EQ(newVector.x, x);
	ASSERT_FLOAT_EQ(newVector.y, y);
}

TEST_F(Vector2Test, ConstructFromVector)
{
	const nc::Vector2f newVector(v1_);
	printf("Constructing a new vector from the first one <%.2f, %.2f>: ", v1_.x, v1_.y);
	printVector(newVector);

	ASSERT_FLOAT_EQ(newVector.x, v1_.x);
	ASSERT_FLOAT_EQ(newVector.y, v1_.y);
}

TEST_F(Vector2Test, AssignmentOperator)
{
	printf("Creating a new vector with the assignment operator\n");
	nc::Vector2f newVector;
	newVector = v1_;
	printVector(newVector);

	ASSERT_FLOAT_EQ(newVector.x, v1_.x);
	ASSERT_FLOAT_EQ(newVector.y, v1_.y);
}

TEST_F(Vector2Test, ConvertIntToFloat)
{
	const nc::Vector2i vectorInt(1, 2);
	const nc::Vector2f newVector = nc::Vector2f::convertType(vectorInt);
	printf("Creating a new float vector by converting an integer one: ");
	printVector(newVector);

	ASSERT_FLOAT_EQ(newVector.x, static_cast<float>(vectorInt.x));
	ASSERT_FLOAT_EQ(newVector.y, static_cast<float>(vectorInt.y));
}

TEST_F(Vector2Test, ConvertFloatToInt)
{
	const nc::Vector2f vectorFloat(1.0f, 2.0f);
	const nc::Vector2i newVector = nc::Vector2i::convertType(vectorFloat);
	printf("Creating a new integer vector by converting a float one: ");
	printf("<%d, %d>\n", newVector.x, newVector.y);

	ASSERT_EQ(newVector.x, static_cast<int>(vectorFloat.x));
	ASSERT_EQ(newVector.y, static_cast<int>(vectorFloat.y));
}

TEST_F(Vector2Test, SetComponents)
{
	const float scalar = 0.0f;
	printVector("v1: ", v1_);
	v1_.set(scalar, scalar);
	printf("Setting all vector components to %.2f: ", scalar);
	printVector(v1_);

	ASSERT_FLOAT_EQ(v1_.x, scalar);
	ASSERT_FLOAT_EQ(v1_.y, scalar);
}

TEST_F(Vector2Test, Addition)
{
	printVector("v1: ", v1_);
	printVector("v2: ", v2_);
	const nc::Vector2f sum = v1_ + v2_;
	printVector("Vector addition: ", sum);

	ASSERT_FLOAT_EQ(sum.x, v1_.x + v2_.x);
	ASSERT_FLOAT_EQ(sum.y, v1_.y + v2_.y);
}

TEST_F(Vector2Test, Subtraction)
{
	printVector("v1: ", v1_);
	printVector("v2: ", v2_);
	const nc::Vector2f sub = v1_ - v2_;
	printVector("Vector subtraction: ", sub);

	ASSERT_FLOAT_EQ(sub.x, v1_.x - v2_.x);
	ASSERT_FLOAT_EQ(sub.y, v1_.y - v2_.y);
}

TEST_F(Vector2Test, Multiplication)
{
	printVector("v1: ", v1_);
	printVector("v2: ", v2_);
	const nc::Vector2f mul = v1_ * v2_;
	printVector("Vector multiplication: ", mul);

	ASSERT_FLOAT_EQ(mul.x, v1_.x * v2_.x);
	ASSERT_FLOAT_EQ(mul.y, v1_.y * v2_.y);
}

TEST_F(Vector2Test, Division)
{
	printVector("v1: ", v1_);
	printVector("v2: ", v2_);
	const nc::Vector2f div = v1_ / v2_;
	printVector("Vector division: ", div);

	ASSERT_FLOAT_EQ(div.x, v1_.x / v2_.x);
	ASSERT_FLOAT_EQ(div.y, v1_.y / v2_.y);
}

TEST_F(Vector2Test, AdditionInPlace)
{
	const nc::Vector2f oldV1 = v1_;

	printVector("v1: ", v1_);
	printVector("v2: ", v2_);
	printVector("Adding the second vector to the first: ", v1_ += v2_);

	ASSERT_FLOAT_EQ(v1_.x, oldV1.x + v2_.x);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y + v2_.y);
}

TEST_F(Vector2Test, SubtractionInPlace)
{
	const nc::Vector2f oldV1 = v1_;

	printVector("v1: ", v1_);
	printVector("v2: ", v2_);
	printVector("Subtracting the second vector from the first: ", v1_ -= v2_);

	ASSERT_FLOAT_EQ(v1_.x, oldV1.x - v2_.x);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y - v2_.y);
}

TEST_F(Vector2Test, MultiplicationInPlace)
{
	const nc::Vector2f oldV1 = v1_;

	printVector("v1: ", v1_);
	printVector("v2: ", v2_);
	printVector("Multiplying the first vector by the second: ", v1_ *= v2_);

	ASSERT_FLOAT_EQ(v1_.x, oldV1.x * v2_.x);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y * v2_.y);
}

TEST_F(Vector2Test, DivisionInPlace)
{
	const nc::Vector2f oldV1 = v1_;

	printVector("v1: ", v1_);
	printVector("v2: ", v2_);
	printVector("Dividing the first vector by the second: ", v1_ /= v2_);

	ASSERT_FLOAT_EQ(v1_.x, oldV1.x / v2_.x);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y / v2_.y);
}

TEST_F(Vector2Test, AddScalar)
{
	const float scalar = 1.0f;
	printVector("v1: ", v1_);
	const nc::Vector2f add = v1_ + scalar;
	printf("Addition of scalar %.2f: ", scalar);
	printVector(add);

	ASSERT_FLOAT_EQ(add.x, v1_.x + scalar);
	ASSERT_FLOAT_EQ(add.y, v1_.y + scalar);
}

TEST_F(Vector2Test, SubtractScalar)
{
	const float scalar = 1.0f;
	printVector("v1: ", v1_);
	const nc::Vector2f sub = v1_ - scalar;
	printf("Subtraction of scalar %.2f: ", scalar);
	printVector(sub);

	ASSERT_FLOAT_EQ(sub.x, v1_.x - scalar);
	ASSERT_FLOAT_EQ(sub.y, v1_.y - scalar);
}

TEST_F(Vector2Test, MultiplyScalar)
{
	const float scalar = 2.0f;
	printVector("v1: ", v1_);
	const nc::Vector2f mul = v1_ * scalar;
	printf("Multiplication by scalar %.2f: ", scalar);
	printVector(mul);

	ASSERT_FLOAT_EQ(mul.x, v1_.x * scalar);
	ASSERT_FLOAT_EQ(mul.y, v1_.y * scalar);
}

TEST_F(Vector2Test, DivideScalar)
{
	const float scalar = 2.0f;
	printVector("v1: ", v1_);
	const nc::Vector2f div = v1_ / scalar;
	printf("Division by scalar %.2f: ", scalar);
	printVector(div);

	ASSERT_FLOAT_EQ(div.x, v1_.x / scalar);
	ASSERT_FLOAT_EQ(div.y, v1_.y / scalar);
}

TEST_F(Vector2Test, AddScalarInPlace)
{
	const float scalar = 1.0f;
	const nc::Vector2f oldV1 = v1_;

	printVector("v1: ", v1_);
	printf("Adding the scalar %.2f to the first vector: ", scalar);
	printVector(v1_ += scalar);

	ASSERT_FLOAT_EQ(v1_.x, oldV1.x + scalar);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y + scalar);
}

TEST_F(Vector2Test, SubtractScalarInPlace)
{
	const float scalar = 1.0f;
	const nc::Vector2f oldV1 = v1_;

	printVector("v1: ", v1_);
	printf("Subtracting the scalar %.2f to the first vector: ", scalar);
	printVector(v1_ -= scalar);

	ASSERT_FLOAT_EQ(v1_.x, oldV1.x - scalar);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y - scalar);
}

TEST_F(Vector2Test, MultiplyScalarInPlace)
{
	const float scalar = 2.0f;
	const nc::Vector2f oldV1 = v1_;

	printVector("v1: ", v1_);
	printf("Multiplying the first vector by the scalar %.2f: ", scalar);
	printVector(v1_ *= scalar);

	ASSERT_FLOAT_EQ(v1_.x, oldV1.x * scalar);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y * scalar);
}

TEST_F(Vector2Test, DivideScalarInPlace)
{
	const float scalar = 2.0f;
	const nc::Vector2f oldV1 = v1_;

	printVector("v1: ", v1_);
	printf("Dividing the first vector by the scalar %.2f: ", scalar);
	printVector(v1_ /= scalar);

	ASSERT_FLOAT_EQ(v1_.x, oldV1.x / scalar);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y / scalar);
}

TEST_F(Vector2Test, MultiplyScalarAsFirst)
{
	const float scalar = 2.0f;
	printVector("v1: ", v1_);
	const nc::Vector2f mul = scalar * v1_;
	printf("Multiplication by scalar %.2f: ", scalar);
	printVector(mul);

	ASSERT_FLOAT_EQ(mul.x, scalar * v1_.x);
	ASSERT_FLOAT_EQ(mul.y, scalar * v1_.y);
}

TEST_F(Vector2Test, DotProduct)
{
	printVector("v1: ", v1_);
	printVector("v2: ", v2_);
	const float result = nc::dot(v1_, v2_);
	printf("Dot product: %.2f\n", result);

	ASSERT_FLOAT_EQ(result, v1_.x * v2_.x + v1_.y * v2_.y);
}

TEST_F(Vector2Test, Normalized)
{
	printVector("v1: ", v1_);
	const nc::Vector2f newVector = v1_.normalized();
	printVector("Creating a new vector as the normalized version of the first one: ", newVector);

	const float length = sqrt(v1_.x * v1_.x + v1_.y * v1_.y);
	ASSERT_FLOAT_EQ(newVector.x, v1_.x / length);
	ASSERT_FLOAT_EQ(newVector.y, v1_.y / length);
}

TEST_F(Vector2Test, NormalizedZero)
{
	nc::Vector2f shortVector(FLT_MIN, 0.0f);
	printVector("Short vector: ", shortVector);
	const nc::Vector2f newVector = shortVector.normalized();
	printVector("Creating a new vector as the normalized version of the first one: ", newVector);

	const float length = sqrt(newVector.x * newVector.x + newVector.y * newVector.y);
	ASSERT_EQ(length, 0.0f);
	ASSERT_EQ(newVector.x, 0.0f);
	ASSERT_EQ(newVector.y, 0.0f);
}

TEST_F(Vector2Test, Normalize)
{
	const nc::Vector2f oldV1 = v1_;

	printVector("v1: ", v1_);
	printVector("Normalizing the first vector: ", v1_.normalize());

	const float length = sqrt(oldV1.x * oldV1.x + oldV1.y * oldV1.y);
	ASSERT_FLOAT_EQ(v1_.x, oldV1.x / length);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y / length);
}

TEST_F(Vector2Test, NormalizeZero)
{
	nc::Vector2f shortVector(FLT_MIN, 0.0f);

	printVector("Short vector: ", shortVector);
	printVector("Normalizing the short vector: ", shortVector.normalize());

	const float length = sqrt(shortVector.x * shortVector.x +
	                          shortVector.y * shortVector.y);
	ASSERT_EQ(length, 0.0f);
	ASSERT_EQ(shortVector.x, 0.0f);
	ASSERT_EQ(shortVector.y, 0.0f);
}

TEST_F(Vector2Test, Length)
{
	printVector("v1: ", v1_);
	printf("Length of the first vector: %.2f\n", v1_.length());

	const float length = sqrt(v1_.x * v1_.x + v1_.y * v1_.y);
	ASSERT_FLOAT_EQ(v1_.length(), length);
}

TEST_F(Vector2Test, SqrLength)
{
	printVector("v1: ", v1_);
	printf("Squared length of the first vector: %.2f\n", v1_.sqrLength());

	const float sqrLength = v1_.x * v1_.x + v1_.y * v1_.y;
	ASSERT_FLOAT_EQ(v1_.sqrLength(), sqrLength);
}

TEST_F(Vector2Test, AddConstantVectors)
{
	printf("Adding all constant vectors together: ");
	const nc::Vector2f constants = nc::Vector2f::Zero +
	                               nc::Vector2f::XAxis +
	                               nc::Vector2f::YAxis;
	printVector(constants);

	ASSERT_FLOAT_EQ(constants.x, 1.0f);
	ASSERT_FLOAT_EQ(constants.y, 1.0f);
}

TEST_F(Vector2Test, EqualityOperator)
{
	printVector("v1: ", v1_);
	const nc::Vector2f newVector = v1_;
	printVector("Creating a new vector as a copy of the first one: ", newVector);

	printf("The first vector components are equal to the new one: %d\n", v1_ == newVector);

	ASSERT_TRUE(v1_ == newVector);
	ASSERT_FLOAT_EQ(v1_.x, newVector.x);
	ASSERT_FLOAT_EQ(v1_.y, newVector.y);
}

TEST_F(Vector2Test, Negation)
{
	printVector("v1: ", v1_);
	const nc::Vector2f newVector = -v1_;
	printVector("Creating a new vector as the negation of the first one: ", newVector);

	ASSERT_FLOAT_EQ(v1_.x, -newVector.x);
	ASSERT_FLOAT_EQ(v1_.y, -newVector.y);
}

TEST_F(Vector2Test, PointerAccess)
{
	printVector("v1: ", v1_);
	const float *ptr = v1_.data();
	printf("Components: ");
	for (unsigned int i = 0; i < 2; i++)
		printf("[%u]:%.2f ", i, ptr[i]);
	printf("\n");

	ASSERT_FLOAT_EQ(v1_.x, ptr[0]);
	ASSERT_FLOAT_EQ(v1_.y, ptr[1]);
}

TEST_F(Vector2Test, SubscriptOperator)
{
	printVector("v1: ", v1_);
	printf("Components: ");
	for (unsigned int i = 0; i < 2; i++)
		printf("[%u]:%.2f ", i, v1_[i]);
	printf("\n");

	ASSERT_FLOAT_EQ(v1_.x, v1_[0]);
	ASSERT_FLOAT_EQ(v1_.y, v1_[1]);
}

}
