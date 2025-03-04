#include "gtest_vector.h"

namespace {

class Vector3Test : public ::testing::Test
{
  public:
	Vector3Test()
	    : v1_(2.5f, 2.0f, 0.5f), v2_(1.0f, 1.5f, 1.0f) {}

	nc::Vector3f v1_;
	nc::Vector3f v2_;
};

TEST_F(Vector3Test, DefaultConstructor)
{
	const nc::Vector3f newVector;
	printf("Constructing a new vector with the default constructor: ");
	printVector(newVector);

	ASSERT_FLOAT_EQ(newVector.x, 0.0f);
	ASSERT_FLOAT_EQ(newVector.y, 0.0f);
	ASSERT_FLOAT_EQ(newVector.z, 0.0f);
}

TEST_F(Vector3Test, ConstructWithSingleScalar)
{
	const float scalar = 0.0f;
	const nc::Vector3f newVector(scalar);
	printf("Constructing a new vector with a single scalar %.2f: ", scalar);
	printVector(newVector);

	ASSERT_FLOAT_EQ(newVector.x, scalar);
	ASSERT_FLOAT_EQ(newVector.y, scalar);
	ASSERT_FLOAT_EQ(newVector.z, scalar);
}

TEST_F(Vector3Test, ConstructWithMultipleScalars)
{
	const float x = 1.0f;
	const float y = 2.0f;
	const float z = 3.0f;
	const nc::Vector3f newVector(x, y, z);
	printf("Constructing a new vector with multiple scalars <%.2f, %.2f, %.2f>: ", x, y, z);
	printVector(newVector);

	ASSERT_FLOAT_EQ(newVector.x, x);
	ASSERT_FLOAT_EQ(newVector.y, y);
	ASSERT_FLOAT_EQ(newVector.z, z);
}

TEST_F(Vector3Test, ConstructFromVector)
{
	const nc::Vector3f newVector(v1_);
	printf("Constructing a new vector from the first one <%.2f, %.2f, %.2f>: ", v1_.x, v1_.y, v1_.z);
	printVector(newVector);

	ASSERT_FLOAT_EQ(newVector.x, v1_.x);
	ASSERT_FLOAT_EQ(newVector.y, v1_.y);
	ASSERT_FLOAT_EQ(newVector.z, v1_.z);
}

TEST_F(Vector3Test, ConstructFromVector2)
{
	const nc::Vector2f vector2(1.0f, 2.0f);
	const float zz = 3.0f;
	const nc::Vector3f newVector(vector2, zz);
	printf("Constructing a new vector from a Vector2 <%.2f, %.2f> and a scalar %.2f: ", vector2.x, vector2.y, zz);
	printVector(newVector);

	ASSERT_FLOAT_EQ(newVector.x, vector2.x);
	ASSERT_FLOAT_EQ(newVector.y, vector2.y);
	ASSERT_FLOAT_EQ(newVector.z, zz);
}

TEST_F(Vector3Test, AssignmentOperator)
{
	printf("Creating a new vector with the assignment operator\n");
	nc::Vector3f newVector;
	newVector = v1_;
	printVector(newVector);

	ASSERT_FLOAT_EQ(newVector.x, v1_.x);
	ASSERT_FLOAT_EQ(newVector.y, v1_.y);
	ASSERT_FLOAT_EQ(newVector.z, v1_.z);
}

TEST_F(Vector3Test, ConvertIntToFloat)
{
	const nc::Vector3i vectorInt(1, 2, 3);
	const nc::Vector3f newVector = nc::Vector3f::convertType(vectorInt);
	printf("Creating a new float vector by converting an integer one: ");
	printVector(newVector);

	ASSERT_FLOAT_EQ(newVector.x, static_cast<float>(vectorInt.x));
	ASSERT_FLOAT_EQ(newVector.y, static_cast<float>(vectorInt.y));
	ASSERT_FLOAT_EQ(newVector.z, static_cast<float>(vectorInt.z));
}

TEST_F(Vector3Test, ConvertFloatToInt)
{
	const nc::Vector3f vectorFloat(1.0f, 2.0f, 3.0f);
	const nc::Vector3i newVector = nc::Vector3i::convertType(vectorFloat);
	printf("Creating a new integer vector by converting a float one: ");
	printf("<%d, %d, %d>\n", newVector.x, newVector.y, newVector.z);

	ASSERT_EQ(newVector.x, static_cast<int>(vectorFloat.x));
	ASSERT_EQ(newVector.y, static_cast<int>(vectorFloat.y));
	ASSERT_EQ(newVector.z, static_cast<int>(vectorFloat.z));
}

TEST_F(Vector3Test, SetComponents)
{
	const float scalar = 0.0f;
	printVector("v1: ", v1_);
	v1_.set(scalar, scalar, scalar);
	printf("Setting all vector components to %.2f: ", scalar);
	printVector(v1_);

	ASSERT_FLOAT_EQ(v1_.x, scalar);
	ASSERT_FLOAT_EQ(v1_.y, scalar);
	ASSERT_FLOAT_EQ(v1_.z, scalar);
}

TEST_F(Vector3Test, Addition)
{
	printVector("v1: ", v1_);
	printVector("v2: ", v2_);
	const nc::Vector3f sum = v1_ + v2_;
	printVector("Vector addition: ", sum);

	ASSERT_FLOAT_EQ(sum.x, v1_.x + v2_.x);
	ASSERT_FLOAT_EQ(sum.y, v1_.y + v2_.y);
	ASSERT_FLOAT_EQ(sum.z, v1_.z + v2_.z);
}

TEST_F(Vector3Test, Subtraction)
{
	printVector("v1: ", v1_);
	printVector("v2: ", v2_);
	const nc::Vector3f sub = v1_ - v2_;
	printVector("Vector subtraction: ", sub);

	ASSERT_FLOAT_EQ(sub.x, v1_.x - v2_.x);
	ASSERT_FLOAT_EQ(sub.y, v1_.y - v2_.y);
	ASSERT_FLOAT_EQ(sub.z, v1_.z - v2_.z);
}

TEST_F(Vector3Test, Multiplication)
{
	printVector("v1: ", v1_);
	printVector("v2: ", v2_);
	const nc::Vector3f mul = v1_ * v2_;
	printVector("Vector multiplication: ", mul);

	ASSERT_FLOAT_EQ(mul.x, v1_.x * v2_.x);
	ASSERT_FLOAT_EQ(mul.y, v1_.y * v2_.y);
	ASSERT_FLOAT_EQ(mul.z, v1_.z * v2_.z);
}

TEST_F(Vector3Test, Division)
{
	printVector("v1: ", v1_);
	printVector("v2: ", v2_);
	const nc::Vector3f div = v1_ / v2_;
	printVector("Vector division: ", div);

	ASSERT_FLOAT_EQ(div.x, v1_.x / v2_.x);
	ASSERT_FLOAT_EQ(div.y, v1_.y / v2_.y);
	ASSERT_FLOAT_EQ(div.z, v1_.z / v2_.z);
}

TEST_F(Vector3Test, AdditionInPlace)
{
	const nc::Vector3f oldV1 = v1_;

	printVector("v1: ", v1_);
	printVector("v2: ", v2_);
	printVector("Adding the second vector to the first: ", v1_ += v2_);

	ASSERT_FLOAT_EQ(v1_.x, oldV1.x + v2_.x);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y + v2_.y);
	ASSERT_FLOAT_EQ(v1_.z, oldV1.z + v2_.z);
}

TEST_F(Vector3Test, SubtractionInPlace)
{
	const nc::Vector3f oldV1 = v1_;

	printVector("v1: ", v1_);
	printVector("v2: ", v2_);
	printVector("Subtracting the second vector from the first: ", v1_ -= v2_);

	ASSERT_FLOAT_EQ(v1_.x, oldV1.x - v2_.x);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y - v2_.y);
	ASSERT_FLOAT_EQ(v1_.z, oldV1.z - v2_.z);
}

TEST_F(Vector3Test, MultiplicationInPlace)
{
	const nc::Vector3f oldV1 = v1_;

	printVector("v1: ", v1_);
	printVector("v2: ", v2_);
	printVector("Multiplying the first vector by the second: ", v1_ *= v2_);

	ASSERT_FLOAT_EQ(v1_.x, oldV1.x * v2_.x);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y * v2_.y);
	ASSERT_FLOAT_EQ(v1_.z, oldV1.z * v2_.z);
}

TEST_F(Vector3Test, DivisionInPlace)
{
	const nc::Vector3f oldV1 = v1_;

	printVector("v1: ", v1_);
	printVector("v2: ", v2_);
	printVector("Dividing the first vector by the second: ", v1_ /= v2_);

	ASSERT_FLOAT_EQ(v1_.x, oldV1.x / v2_.x);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y / v2_.y);
	ASSERT_FLOAT_EQ(v1_.z, oldV1.z / v2_.z);
}

TEST_F(Vector3Test, AddScalar)
{
	const float scalar = 1.0f;
	printVector("v1: ", v1_);
	const nc::Vector3f add = v1_ + scalar;
	printf("Addition of scalar %.2f: ", scalar);
	printVector(add);

	ASSERT_FLOAT_EQ(add.x, v1_.x + scalar);
	ASSERT_FLOAT_EQ(add.y, v1_.y + scalar);
	ASSERT_FLOAT_EQ(add.z, v1_.z + scalar);
}

TEST_F(Vector3Test, SubtractScalar)
{
	const float scalar = 1.0f;
	printVector("v1: ", v1_);
	const nc::Vector3f sub = v1_ - scalar;
	printf("Subtraction of scalar %.2f: ", scalar);
	printVector(sub);

	ASSERT_FLOAT_EQ(sub.x, v1_.x - scalar);
	ASSERT_FLOAT_EQ(sub.y, v1_.y - scalar);
	ASSERT_FLOAT_EQ(sub.z, v1_.z - scalar);
}

TEST_F(Vector3Test, MultiplyScalar)
{
	const float scalar = 2.0f;
	printVector("v1: ", v1_);
	const nc::Vector3f mul = v1_ * scalar;
	printf("Multiplication by scalar %.2f: ", scalar);
	printVector(mul);

	ASSERT_FLOAT_EQ(mul.x, v1_.x * scalar);
	ASSERT_FLOAT_EQ(mul.y, v1_.y * scalar);
	ASSERT_FLOAT_EQ(mul.z, v1_.z * scalar);
}

TEST_F(Vector3Test, DivideScalar)
{
	const float scalar = 2.0f;
	printVector("v1: ", v1_);
	const nc::Vector3f div = v1_ / scalar;
	printf("Division by scalar %.2f: ", scalar);
	printVector(div);

	ASSERT_FLOAT_EQ(div.x, v1_.x / scalar);
	ASSERT_FLOAT_EQ(div.y, v1_.y / scalar);
	ASSERT_FLOAT_EQ(div.z, v1_.z / scalar);
}

TEST_F(Vector3Test, AddScalarInPlace)
{
	const float scalar = 1.0f;
	const nc::Vector3f oldV1 = v1_;

	printVector("v1: ", v1_);
	printf("Adding the scalar %.2f to the first vector: ", scalar);
	printVector(v1_ += scalar);

	ASSERT_FLOAT_EQ(v1_.x, oldV1.x + scalar);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y + scalar);
	ASSERT_FLOAT_EQ(v1_.z, oldV1.z + scalar);
}

TEST_F(Vector3Test, SubtractScalarInPlace)
{
	const float scalar = 1.0f;
	const nc::Vector3f oldV1 = v1_;

	printVector("v1: ", v1_);
	printf("Subtracting the scalar %.2f to the first vector: ", scalar);
	printVector(v1_ -= scalar);

	ASSERT_FLOAT_EQ(v1_.x, oldV1.x - scalar);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y - scalar);
	ASSERT_FLOAT_EQ(v1_.z, oldV1.z - scalar);
}

TEST_F(Vector3Test, MultiplyScalarInPlace)
{
	const float scalar = 2.0f;
	const nc::Vector3f oldV1 = v1_;

	printVector("v1: ", v1_);
	printf("Multiplying the first vector by the scalar %.2f: ", scalar);
	printVector(v1_ *= scalar);

	ASSERT_FLOAT_EQ(v1_.x, oldV1.x * scalar);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y * scalar);
	ASSERT_FLOAT_EQ(v1_.z, oldV1.z * scalar);
}

TEST_F(Vector3Test, DivideScalarInPlace)
{
	const float scalar = 2.0f;
	const nc::Vector3f oldV1 = v1_;

	printVector("v1: ", v1_);
	printf("Dividing the first vector by the scalar %.2f: ", scalar);
	printVector(v1_ /= scalar);

	ASSERT_FLOAT_EQ(v1_.x, oldV1.x / scalar);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y / scalar);
	ASSERT_FLOAT_EQ(v1_.z, oldV1.z / scalar);
}

TEST_F(Vector3Test, MultiplyScalarAsFirst)
{
	const float scalar = 2.0f;
	printVector("v1: ", v1_);
	const nc::Vector3f mul = scalar * v1_;
	printf("Multiplication by scalar %.2f: ", scalar);
	printVector(mul);

	ASSERT_FLOAT_EQ(mul.x, scalar * v1_.x);
	ASSERT_FLOAT_EQ(mul.y, scalar * v1_.y);
	ASSERT_FLOAT_EQ(mul.z, scalar * v1_.z);
}

TEST_F(Vector3Test, DotProduct)
{
	printVector("v1: ", v1_);
	printVector("v2: ", v2_);
	const float result = nc::dot(v1_, v2_);
	printf("Dot product: %.2f\n", result);

	ASSERT_FLOAT_EQ(result, v1_.x * v2_.x + v1_.y * v2_.y + v1_.z * v2_.z);
}

TEST_F(Vector3Test, CrossProduct)
{
	printVector("v1: ", v1_);
	printVector("v2: ", v2_);
	const nc::Vector3f crossVector = nc::cross(v1_, v2_);
	printf("Cross product: ");
	printVector(crossVector);

	ASSERT_FLOAT_EQ(crossVector.x, v1_.y * v2_.z - v1_.z * v2_.y);
	ASSERT_FLOAT_EQ(crossVector.y, v1_.z * v2_.x - v1_.x * v2_.z);
	ASSERT_FLOAT_EQ(crossVector.z, v1_.x * v2_.y - v1_.y * v2_.x);
}

TEST_F(Vector3Test, Normalized)
{
	printVector("v1: ", v1_);
	const nc::Vector3f newVector = v1_.normalized();
	printVector("Creating a new vector as the normalized version of the first one: ", newVector);

	const float length = sqrt(v1_.x * v1_.x + v1_.y * v1_.y + v1_.z * v1_.z);
	ASSERT_FLOAT_EQ(newVector.x, v1_.x / length);
	ASSERT_FLOAT_EQ(newVector.y, v1_.y / length);
	ASSERT_FLOAT_EQ(newVector.z, v1_.z / length);
}

TEST_F(Vector3Test, NormalizedZero)
{
	nc::Vector3f shortVector(FLT_MIN, 0.0f, 0.0f);
	printVector("Short vector: ", shortVector);
	const nc::Vector3f newVector = shortVector.normalized();
	printVector("Creating a new vector as the normalized version of the first one: ", newVector);

	const float length = sqrt(newVector.x * newVector.x +
	                          newVector.y * newVector.y +
	                          newVector.z * newVector.z);
	ASSERT_EQ(length, 0.0f);
	ASSERT_EQ(newVector.x, 0.0f);
	ASSERT_EQ(newVector.y, 0.0f);
	ASSERT_EQ(newVector.z, 0.0f);
}

TEST_F(Vector3Test, Normalize)
{
	const nc::Vector3f oldV1 = v1_;

	printVector("v1: ", v1_);
	printVector("Normalizing the first vector: ", v1_.normalize());

	const float length = sqrt(oldV1.x * oldV1.x + oldV1.y * oldV1.y + oldV1.z * oldV1.z);
	ASSERT_FLOAT_EQ(v1_.x, oldV1.x / length);
	ASSERT_FLOAT_EQ(v1_.y, oldV1.y / length);
	ASSERT_FLOAT_EQ(v1_.z, oldV1.z / length);
}

TEST_F(Vector3Test, NormalizeZero)
{
	nc::Vector3f shortVector(FLT_MIN, 0.0f, 0.0f);

	printVector("Short vector: ", shortVector);
	printVector("Normalizing the short vector: ", shortVector.normalize());

	const float length = sqrt(shortVector.x * shortVector.x +
	                          shortVector.y * shortVector.y +
	                          shortVector.z * shortVector.z);
	ASSERT_EQ(length, 0.0f);
	ASSERT_EQ(shortVector.x, 0.0f);
	ASSERT_EQ(shortVector.y, 0.0f);
	ASSERT_EQ(shortVector.z, 0.0f);
}

TEST_F(Vector3Test, Length)
{
	printVector("v1: ", v1_);
	printf("Length of the first vector: %.2f\n", v1_.length());

	const float length = sqrt(v1_.x * v1_.x + v1_.y * v1_.y + v1_.z * v1_.z);
	ASSERT_FLOAT_EQ(v1_.length(), length);
}

TEST_F(Vector3Test, SqrLength)
{
	printVector("v1: ", v1_);
	printf("Squared length of the first vector: %.2f\n", v1_.sqrLength());

	const float sqrLength = v1_.x * v1_.x + v1_.y * v1_.y + v1_.z * v1_.z;
	ASSERT_FLOAT_EQ(v1_.sqrLength(), sqrLength);
}

TEST_F(Vector3Test, ToVector2)
{
	printVector("v1: ", v1_);
	const nc::Vector2f newVector = v1_.toVector2();
	printVector("First vector converted to a 2 components one: ", newVector);

	ASSERT_FLOAT_EQ(v1_.x, newVector.x);
	ASSERT_FLOAT_EQ(v1_.y, newVector.y);
}

TEST_F(Vector3Test, AddConstantVectors)
{
	printf("Adding all constant vectors together: ");
	const nc::Vector3f constants = nc::Vector3f::Zero +
	                               nc::Vector3f::XAxis +
	                               nc::Vector3f::YAxis +
	                               nc::Vector3f::ZAxis;
	printVector(constants);

	ASSERT_FLOAT_EQ(constants.x, 1.0f);
	ASSERT_FLOAT_EQ(constants.y, 1.0f);
	ASSERT_FLOAT_EQ(constants.z, 1.0f);
}

TEST_F(Vector3Test, EqualityOperator)
{
	printVector("v1: ", v1_);
	const nc::Vector3f newVector = v1_;
	printVector("Creating a new vector as a copy of the first one: ", newVector);

	printf("The first vector components are equal to the new one: %d\n", v1_ == newVector);

	ASSERT_TRUE(v1_ == newVector);
	ASSERT_FLOAT_EQ(v1_.x, newVector.x);
	ASSERT_FLOAT_EQ(v1_.y, newVector.y);
	ASSERT_FLOAT_EQ(v1_.z, newVector.z);
}

TEST_F(Vector3Test, Negation)
{
	printVector("v1: ", v1_);
	const nc::Vector3f newVector = -v1_;
	printVector("Creating a new vector as the negation of the first one: ", newVector);

	ASSERT_FLOAT_EQ(v1_.x, -newVector.x);
	ASSERT_FLOAT_EQ(v1_.y, -newVector.y);
	ASSERT_FLOAT_EQ(v1_.z, -newVector.z);
}

TEST_F(Vector3Test, PointerAccess)
{
	printVector("v1: ", v1_);
	const float *ptr = v1_.data();
	printf("Components: ");
	for (unsigned int i = 0; i < 3; i++)
		printf("[%u]:%.2f ", i, ptr[i]);
	printf("\n");

	ASSERT_FLOAT_EQ(v1_.x, ptr[0]);
	ASSERT_FLOAT_EQ(v1_.y, ptr[1]);
	ASSERT_FLOAT_EQ(v1_.z, ptr[2]);
}

TEST_F(Vector3Test, SubscriptOperator)
{
	printVector("v1: ", v1_);
	printf("Components: ");
	for (unsigned int i = 0; i < 3; i++)
		printf("[%u]:%.2f ", i, v1_[i]);
	printf("\n");

	ASSERT_FLOAT_EQ(v1_.x, v1_[0]);
	ASSERT_FLOAT_EQ(v1_.y, v1_[1]);
	ASSERT_FLOAT_EQ(v1_.z, v1_[2]);
}

}
