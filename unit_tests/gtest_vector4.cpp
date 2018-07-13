#include "gtest_vector.h"

namespace {

class Vector4Test : public ::testing::Test
{
  public:
	Vector4Test() : v1_(2.5f, 2.0f, 0.5f, 1.0f), v2_(1.0f, 1.5f, 1.0f, 2.0f) { }

	nc::Vector4f v1_;
	nc::Vector4f v2_;
};

TEST_F(Vector4Test, ConstructWithSingleScalar)
{
	const float scalar = 0.0f;
	const nc::Vector4f newVector(scalar);
	printf("Constructing a new vector with a single scalar %.2f: ", scalar);
	printVector(newVector);

	ASSERT_FLOAT_EQ(newVector.x, scalar);
	ASSERT_FLOAT_EQ(newVector.y, scalar);
	ASSERT_FLOAT_EQ(newVector.z, scalar);
	ASSERT_FLOAT_EQ(newVector.w, scalar);
}

TEST_F(Vector4Test, SetComponents)
{
	const float scalar = 0.0f;
	printf("v1: "); printVector(v1_);
	v1_.set(scalar, scalar, scalar, scalar);
	printf("Setting all vector components to %.2f: ", scalar);
	printVector(v1_);

	ASSERT_FLOAT_EQ(v1_.x, scalar);
	ASSERT_FLOAT_EQ(v1_.y, scalar);
	ASSERT_FLOAT_EQ(v1_.z, scalar);
	ASSERT_FLOAT_EQ(v1_.w, scalar);
}

TEST_F(Vector4Test, Addition)
{
	printf("v1: "); printVector(v1_);
	printf("v2: "); printVector(v2_);
	const nc::Vector4f sum = v1_ + v2_;
	printf("Vector addition: ");
	printVector(sum);

	ASSERT_FLOAT_EQ(sum.x, v1_.x + v2_.x);
	ASSERT_FLOAT_EQ(sum.y, v1_.y + v2_.y);
	ASSERT_FLOAT_EQ(sum.z, v1_.z + v2_.z);
	ASSERT_FLOAT_EQ(sum.w, v1_.w + v2_.w);
}

TEST_F(Vector4Test, Subtraction)
{
	printf("v1: "); printVector(v1_);
	printf("v2: "); printVector(v2_);
	const nc::Vector4f sub = v1_ - v2_;
	printf("Vector subtraction: ");
	printVector(sub);

	ASSERT_FLOAT_EQ(sub.x, v1_.x - v2_.x);
	ASSERT_FLOAT_EQ(sub.y, v1_.y - v2_.y);
	ASSERT_FLOAT_EQ(sub.z, v1_.z - v2_.z);
	ASSERT_FLOAT_EQ(sub.w, v1_.w - v2_.w);
}

TEST_F(Vector4Test, Multiplication)
{
	printf("v1: "); printVector(v1_);
	printf("v2: "); printVector(v2_);
	const nc::Vector4f mul = v1_ * v2_;
	printf("Vector multiplication: ");
	printVector(mul);

	ASSERT_FLOAT_EQ(mul.x, v1_.x * v2_.x);
	ASSERT_FLOAT_EQ(mul.y, v1_.y * v2_.y);
	ASSERT_FLOAT_EQ(mul.z, v1_.z * v2_.z);
	ASSERT_FLOAT_EQ(mul.w, v1_.w * v2_.w);
}

TEST_F(Vector4Test, Division)
{
	printf("v1: "); printVector(v1_);
	printf("v2: "); printVector(v2_);
	const nc::Vector4f div = v1_ / v2_;
	printf("Vector division: ");
	printVector(div);

	ASSERT_FLOAT_EQ(div.x, v1_.x / v2_.x);
	ASSERT_FLOAT_EQ(div.y, v1_.y / v2_.y);
	ASSERT_FLOAT_EQ(div.z, v1_.z / v2_.z);
	ASSERT_FLOAT_EQ(div.w, v1_.w / v2_.w);
}

TEST_F(Vector4Test, AdditionInPlace)
{
	const nc::Vector4f oldV1 = v1_;

	printf("v1: "); printVector(v1_);
	printf("v2: "); printVector(v2_);
	printf("Adding the second vector to the first: ");
	printVector(v1_ += v2_);

	ASSERT_FLOAT_EQ(v1_.x, oldV1.x + v2_.x);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y + v2_.y);
	ASSERT_FLOAT_EQ(v1_.z, oldV1.z + v2_.z);
	ASSERT_FLOAT_EQ(v1_.w, oldV1.w + v2_.w);
}

TEST_F(Vector4Test, SubtractionInPlace)
{
	const nc::Vector4f oldV1 = v1_;

	printf("v1: "); printVector(v1_);
	printf("v2: "); printVector(v2_);
	printf("Subtracting the second vector from the first: ");
	printVector(v1_ -= v2_);

	ASSERT_FLOAT_EQ(v1_.x, oldV1.x - v2_.x);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y - v2_.y);
	ASSERT_FLOAT_EQ(v1_.z, oldV1.z - v2_.z);
	ASSERT_FLOAT_EQ(v1_.w, oldV1.w - v2_.w);
}

TEST_F(Vector4Test, MultiplicationInPlace)
{
	const nc::Vector4f oldV1 = v1_;

	printf("v1: "); printVector(v1_);
	printf("v2: "); printVector(v2_);
	printf("Multiplying the first vector by the second: ");
	printVector(v1_ *= v2_);

	ASSERT_FLOAT_EQ(v1_.x, oldV1.x * v2_.x);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y * v2_.y);
	ASSERT_FLOAT_EQ(v1_.z, oldV1.z * v2_.z);
	ASSERT_FLOAT_EQ(v1_.w, oldV1.w * v2_.w);
}

TEST_F(Vector4Test, DivisionInPlace)
{
	const nc::Vector4f oldV1 = v1_;

	printf("v1: "); printVector(v1_);
	printf("v2: "); printVector(v2_);
	printf("Dividing the first vector by the second: ");
	printVector(v1_ /= v2_);

	ASSERT_FLOAT_EQ(v1_.x, oldV1.x / v2_.x);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y / v2_.y);
	ASSERT_FLOAT_EQ(v1_.z, oldV1.z / v2_.z);
	ASSERT_FLOAT_EQ(v1_.w, oldV1.w / v2_.w);
}

TEST_F(Vector4Test, AddScalar)
{
	const float scalar = 1.0f;
	printf("v1: "); printVector(v1_);
	const nc::Vector4f add = v1_ + scalar;
	printf("Addition of scalar %.2f: ", scalar);
	printVector(add);

	ASSERT_FLOAT_EQ(add.x, v1_.x + scalar);
	ASSERT_FLOAT_EQ(add.y, v1_.y + scalar);
	ASSERT_FLOAT_EQ(add.z, v1_.z + scalar);
	ASSERT_FLOAT_EQ(add.w, v1_.w + scalar);
}

TEST_F(Vector4Test, SubtractScalar)
{
	const float scalar = 1.0f;
	printf("v1: "); printVector(v1_);
	const nc::Vector4f sub = v1_ - scalar;
	printf("Subtraction of scalar %.2f: ", scalar);
	printVector(sub);

	ASSERT_FLOAT_EQ(sub.x, v1_.x - scalar);
	ASSERT_FLOAT_EQ(sub.y, v1_.y - scalar);
	ASSERT_FLOAT_EQ(sub.z, v1_.z - scalar);
	ASSERT_FLOAT_EQ(sub.w, v1_.w - scalar);
}

TEST_F(Vector4Test, MultiplyScalar)
{
	const float scalar = 2.0f;
	printf("v1: "); printVector(v1_);
	const nc::Vector4f mul = v1_ * scalar;
	printf("Multiplication by scalar %.2f: ", scalar);
	printVector(mul);

	ASSERT_FLOAT_EQ(mul.x, v1_.x * scalar);
	ASSERT_FLOAT_EQ(mul.y, v1_.y * scalar);
	ASSERT_FLOAT_EQ(mul.z, v1_.z * scalar);
	ASSERT_FLOAT_EQ(mul.w, v1_.w * scalar);
}

TEST_F(Vector4Test, DivideScalar)
{
	const float scalar = 2.0f;
	printf("v1: "); printVector(v1_);
	const nc::Vector4f div = v1_ / scalar;
	printf("Division by scalar %.2f: ", scalar);
	printVector(div);

	ASSERT_FLOAT_EQ(div.x, v1_.x / scalar);
	ASSERT_FLOAT_EQ(div.y, v1_.y / scalar);
	ASSERT_FLOAT_EQ(div.z, v1_.z / scalar);
	ASSERT_FLOAT_EQ(div.w, v1_.w / scalar);
}

TEST_F(Vector4Test, AddScalarInPlace)
{
	const float scalar = 1.0f;
	const nc::Vector4f oldV1 = v1_;

	printf("v1: "); printVector(v1_);
	printf("Adding the scalar %.2f to the first vector: ", scalar);
	printVector(v1_ += scalar);

	ASSERT_FLOAT_EQ(v1_.x, oldV1.x + scalar);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y + scalar);
	ASSERT_FLOAT_EQ(v1_.z, oldV1.z + scalar);
	ASSERT_FLOAT_EQ(v1_.w, oldV1.w + scalar);
}

TEST_F(Vector4Test, SubtractScalarInPlace)
{
	const float scalar = 1.0f;
	const nc::Vector4f oldV1 = v1_;

	printf("v1: "); printVector(v1_);
	printf("Subtracting the scalar %.2f to the first vector: ", scalar);
	printVector(v1_ -= scalar);

	ASSERT_FLOAT_EQ(v1_.x, oldV1.x - scalar);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y - scalar);
	ASSERT_FLOAT_EQ(v1_.z, oldV1.z - scalar);
	ASSERT_FLOAT_EQ(v1_.w, oldV1.w - scalar);
}

TEST_F(Vector4Test, MultiplyScalarInPlace)
{
	const float scalar = 2.0f;
	const nc::Vector4f oldV1 = v1_;

	printf("v1: "); printVector(v1_);
	printf("Multiplying the first vector by the scalar %.2f: ", scalar);
	printVector(v1_ *= scalar);

	ASSERT_FLOAT_EQ(v1_.x, oldV1.x * scalar);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y * scalar);
	ASSERT_FLOAT_EQ(v1_.z, oldV1.z * scalar);
	ASSERT_FLOAT_EQ(v1_.w, oldV1.w * scalar);
}

TEST_F(Vector4Test, DivideScalarInPlace)
{
	const float scalar = 2.0f;
	const nc::Vector4f oldV1 = v1_;

	printf("v1: "); printVector(v1_);
	printf("Dividing the first vector by the scalar %.2f: ", scalar);
	printVector(v1_ /= scalar);

	ASSERT_FLOAT_EQ(v1_.x, oldV1.x / scalar);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y / scalar);
	ASSERT_FLOAT_EQ(v1_.z, oldV1.z / scalar);
	ASSERT_FLOAT_EQ(v1_.w, oldV1.w / scalar);
}

TEST_F(Vector4Test, MultiplyScalarAsFirst)
{
	const float scalar = 2.0f;
	printf("v1: "); printVector(v1_);
	const nc::Vector4f mul = scalar * v1_;
	printf("Multiplication by scalar %.2f: ", scalar);
	printVector(mul);

	ASSERT_FLOAT_EQ(mul.x, scalar * v1_.x);
	ASSERT_FLOAT_EQ(mul.y, scalar * v1_.y);
	ASSERT_FLOAT_EQ(mul.z, scalar * v1_.z);
	ASSERT_FLOAT_EQ(mul.w, scalar * v1_.w);
}

TEST_F(Vector4Test, DotProduct)
{
	printf("v1: "); printVector(v1_);
	printf("v2: "); printVector(v2_);
	const float result = nc::dot(v1_, v2_);
	printf("Dot product: %.2f\n", result);

	ASSERT_FLOAT_EQ(result, v1_.x * v2_.x + v1_.y * v2_.y + v1_.z * v2_.z + v1_.w * v2_.w);
}

TEST_F(Vector4Test, Normalize)
{
	const nc::Vector4f oldV1 = v1_;

	printf("v1: "); printVector(v1_);
	printf("Normalizing the first vector: ");
	printVector(v1_.normalize());

	const float length = sqrt(oldV1.x * oldV1.x + oldV1.y * oldV1.y + oldV1.z * oldV1.z + oldV1.w * oldV1.w);
	ASSERT_FLOAT_EQ(v1_.x, oldV1.x / length);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y / length);
	ASSERT_FLOAT_EQ(v1_.z, oldV1.z / length);
	ASSERT_FLOAT_EQ(v1_.w, oldV1.w / length);
}

TEST_F(Vector4Test, Normalized)
{
	printf("v1: "); printVector(v1_);
	const nc::Vector4f newVector = v1_.normalized();
	printf("Creating a new vector as the normalized version of the first one: ");
	printVector(v1_.normalize());

	const float length = sqrt(v1_.x * v1_.x + v1_.y * v1_.y + v1_.z * v1_.z + v1_.w * v1_.w);
	ASSERT_FLOAT_EQ(newVector.x, v1_.x / length);
	ASSERT_FLOAT_EQ(newVector.y, v1_.y / length);
	ASSERT_FLOAT_EQ(newVector.z, v1_.z / length);
	ASSERT_FLOAT_EQ(newVector.w, v1_.w / length);
}

TEST_F(Vector4Test, Length)
{
	printf("v1: "); printVector(v1_);
	printf("Length of the first vector: %.2f\n", v1_.length());

	const float length = sqrt(v1_.x * v1_.x + v1_.y * v1_.y + v1_.z * v1_.z + v1_.w * v1_.w);
	ASSERT_FLOAT_EQ(v1_.length(), length);
}

TEST_F(Vector4Test, SqrLength)
{
	printf("v1: "); printVector(v1_);
	printf("Squared length of the first vector: %.2f\n", v1_.sqrLength());

	const float sqrLength = v1_.x * v1_.x + v1_.y * v1_.y + v1_.z * v1_.z + v1_.w * v1_.w;
	ASSERT_FLOAT_EQ(v1_.sqrLength(), sqrLength);
}

TEST_F(Vector4Test, ToVector2)
{
	printf("v1: "); printVector(v1_);
	const nc::Vector2f newVector = v1_.toVector2();
	printf("First vector converted to a 2 components one: "); printVector(newVector);

	ASSERT_FLOAT_EQ(v1_.x, newVector.x);
	ASSERT_FLOAT_EQ(v1_.y, newVector.y);
}

TEST_F(Vector4Test, ToVector3)
{
	printf("v1: "); printVector(v1_);
	const nc::Vector3f newVector = v1_.toVector3();
	printf("First vector converted to a 3 components one: "); printVector(newVector);

	ASSERT_FLOAT_EQ(v1_.x, newVector.x);
	ASSERT_FLOAT_EQ(v1_.y, newVector.y);
	ASSERT_FLOAT_EQ(v1_.z, newVector.z);
}

TEST_F(Vector4Test, AddConstantVectors)
{
	printf("Adding all constant vectors together: ");
	const nc::Vector4f constants = nc::Vector4f::Zero +
	                               nc::Vector4f::XAxis +
	                               nc::Vector4f::YAxis +
	                               nc::Vector4f::ZAxis +
	                               nc::Vector4f::WAxis;
	printVector(constants);

	ASSERT_FLOAT_EQ(constants.x, 1.0f);
	ASSERT_FLOAT_EQ(constants.y, 1.0f);
	ASSERT_FLOAT_EQ(constants.z, 1.0f);
	ASSERT_FLOAT_EQ(constants.w, 1.0f);
}

TEST_F(Vector4Test, EqualityOperator)
{
	printf("v1: "); printVector(v1_);
	const nc::Vector4f newVector = v1_;
	printf("Creating a new vector as a copy of the first one: "); printVector(newVector);

	printf("The first vector components are equal to the new one: %d", v1_ == newVector);

	ASSERT_TRUE(v1_ == newVector);
	ASSERT_FLOAT_EQ(v1_.x, newVector.x);
	ASSERT_FLOAT_EQ(v1_.y, newVector.y);
	ASSERT_FLOAT_EQ(v1_.z, newVector.z);
	ASSERT_FLOAT_EQ(v1_.w, newVector.w);
}

TEST_F(Vector4Test, Negation)
{
	printf("v1: "); printVector(v1_);
	const nc::Vector4f newVector = -v1_;
	printf("Creating a new vector as the negation of the first one: "); printVector(newVector);

	ASSERT_FLOAT_EQ(v1_.x, -newVector.x);
	ASSERT_FLOAT_EQ(v1_.y, -newVector.y);
	ASSERT_FLOAT_EQ(v1_.z, -newVector.z);
	ASSERT_FLOAT_EQ(v1_.w, -newVector.w);
}

TEST_F(Vector4Test, PointerAccess)
{
	printf("v1: "); printVector(v1_);
	const float *ptr = v1_.data();
	printf("Components: ");
	for(unsigned int i = 0; i < 4; i++)
		printf("[%u]:%.2f ", i, ptr[i]);
	printf("\n");

	ASSERT_FLOAT_EQ(v1_.x, ptr[0]);
	ASSERT_FLOAT_EQ(v1_.y, ptr[1]);
	ASSERT_FLOAT_EQ(v1_.z, ptr[2]);
	ASSERT_FLOAT_EQ(v1_.w, ptr[3]);
}

TEST_F(Vector4Test, SubscriptOperator)
{
	printf("v1: "); printVector(v1_);
	printf("Components: ");
	for(unsigned int i = 0; i < 4; i++)
		printf("[%u]:%.2f ", i, v1_[i]);
	printf("\n");

	ASSERT_FLOAT_EQ(v1_.x, v1_[0]);
	ASSERT_FLOAT_EQ(v1_.y, v1_[1]);
	ASSERT_FLOAT_EQ(v1_.z, v1_[2]);
	ASSERT_FLOAT_EQ(v1_.w, v1_[3]);
}

}
