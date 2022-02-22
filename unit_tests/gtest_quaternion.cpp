#include "gtest_quaternion.h"

namespace {

class QuaternionTest : public ::testing::Test
{
  public:
	QuaternionTest()
	    : q1_(2.5f, 2.0f, 0.5f, 1.0), q2_(1.0f, 1.5f, 1.0f, 2.0) {}

	nc::Quaternionf q1_;
	nc::Quaternionf q2_;
};

TEST_F(QuaternionTest, DefaultConstructor)
{
	const nc::Quaternionf newQuat;
	printf("Constructing a new quaternion with the default constructor: ");
	printQuaternion(newQuat);

	ASSERT_FLOAT_EQ(newQuat.x, 0.0f);
	ASSERT_FLOAT_EQ(newQuat.y, 0.0f);
	ASSERT_FLOAT_EQ(newQuat.z, 0.0f);
	ASSERT_FLOAT_EQ(newQuat.w, 1.0f);
}

TEST_F(QuaternionTest, ConstructWithVector4)
{
	const nc::Vector4f vec(1.0f, 0.25f, 2.0f, 0.75f);
	const nc::Quaternionf newQuat(vec);
	printf("Constructing a new quaternion from vector <%.2f, %.2f, %.2f, %.2f>: ", vec.x, vec.y, vec.z, vec.w);
	printQuaternion(newQuat);

	ASSERT_FLOAT_EQ(newQuat.x, vec.x);
	ASSERT_FLOAT_EQ(newQuat.y, vec.y);
	ASSERT_FLOAT_EQ(newQuat.z, vec.z);
	ASSERT_FLOAT_EQ(newQuat.w, vec.w);
}

TEST_F(QuaternionTest, SetComponents)
{
	printQuaternion("q1: ", q1_);
	q1_.set(0.0f, 0.0f, 0.0f, 0.0f);
	printQuaternion("Setting quaternion components: ", q1_);

	ASSERT_FLOAT_EQ(q1_.x, 0.0f);
	ASSERT_FLOAT_EQ(q1_.y, 0.0f);
	ASSERT_FLOAT_EQ(q1_.z, 0.0f);
	ASSERT_FLOAT_EQ(q1_.w, 0.0f);
}

TEST_F(QuaternionTest, Addition)
{
	printQuaternion("q1: ", q1_);
	printQuaternion("q2: ", q2_);
	const nc::Quaternionf sum = q1_ + q2_;
	printQuaternion("Quaternion addition: ", sum);

	ASSERT_FLOAT_EQ(sum.x, q1_.x + q2_.x);
	ASSERT_FLOAT_EQ(sum.y, q1_.y + q2_.y);
	ASSERT_FLOAT_EQ(sum.z, q1_.z + q2_.z);
	ASSERT_FLOAT_EQ(sum.w, q1_.w + q2_.w);
}

TEST_F(QuaternionTest, Subtraction)
{
	printQuaternion("q1: ", q1_);
	printQuaternion("q2: ", q2_);
	const nc::Quaternionf sub = q1_ - q2_;
	printQuaternion("Quaternion subtraction: ", sub);

	ASSERT_FLOAT_EQ(sub.x, q1_.x - q2_.x);
	ASSERT_FLOAT_EQ(sub.y, q1_.y - q2_.y);
	ASSERT_FLOAT_EQ(sub.z, q1_.z - q2_.z);
	ASSERT_FLOAT_EQ(sub.w, q1_.w - q2_.w);
}

TEST_F(QuaternionTest, Multiplication)
{
	printQuaternion("q1: ", q1_);
	printQuaternion("q2: ", q2_);
	const nc::Quaternionf mul = q1_ * q2_;
	printQuaternion("Quaternion multiplication: ", mul);

	ASSERT_FLOAT_EQ(mul.x, q1_.w * q2_.x + q1_.x * q2_.w + q1_.y * q2_.z - q1_.z * q2_.y);
	ASSERT_FLOAT_EQ(mul.y, q1_.w * q2_.y + q1_.y * q2_.w + q1_.z * q2_.x - q1_.x * q2_.z);
	ASSERT_FLOAT_EQ(mul.z, q1_.w * q2_.z + q1_.z * q2_.w + q1_.x * q2_.y - q1_.y * q2_.x);
	ASSERT_FLOAT_EQ(mul.w, q1_.w * q2_.w - q1_.x * q2_.x - q1_.y * q2_.y - q1_.z * q2_.z);
}

TEST_F(QuaternionTest, AdditionInPlace)
{
	const nc::Quaternionf oldQ1 = q1_;

	printQuaternion("q1: ", q1_);
	printQuaternion("q2: ", q2_);
	printQuaternion("Adding the second quaternion to the first: ", q1_ += q2_);

	ASSERT_FLOAT_EQ(q1_.x, oldQ1.x + q2_.x);
	ASSERT_FLOAT_EQ(q1_.y, oldQ1.y + q2_.y);
	ASSERT_FLOAT_EQ(q1_.z, oldQ1.z + q2_.z);
	ASSERT_FLOAT_EQ(q1_.w, oldQ1.w + q2_.w);
}

TEST_F(QuaternionTest, SubtractionInPlace)
{
	const nc::Quaternionf oldQ1 = q1_;

	printQuaternion("q1: ", q1_);
	printQuaternion("q2: ", q2_);
	printQuaternion("Subtracting the second quaternion from the first: ", q1_ -= q2_);

	ASSERT_FLOAT_EQ(q1_.x, oldQ1.x - q2_.x);
	ASSERT_FLOAT_EQ(q1_.y, oldQ1.y - q2_.y);
	ASSERT_FLOAT_EQ(q1_.z, oldQ1.z - q2_.z);
	ASSERT_FLOAT_EQ(q1_.w, oldQ1.w - q2_.w);
}

TEST_F(QuaternionTest, MultiplicationInPlace)
{
	const nc::Quaternionf oldQ1 = q1_;

	printQuaternion("q1: ", q1_);
	printQuaternion("q2: ", q2_);
	printQuaternion("Multiplying the first quaternion by the second: ", q1_ *= q2_);

	ASSERT_FLOAT_EQ(q1_.x, oldQ1.w * q2_.x + oldQ1.x * q2_.w + oldQ1.y * q2_.z - oldQ1.z * q2_.y);
	ASSERT_FLOAT_EQ(q1_.y, oldQ1.w * q2_.y + oldQ1.y * q2_.w + oldQ1.z * q2_.x - oldQ1.x * q2_.z);
	ASSERT_FLOAT_EQ(q1_.z, oldQ1.w * q2_.z + oldQ1.z * q2_.w + oldQ1.x * q2_.y - oldQ1.y * q2_.x);
	ASSERT_FLOAT_EQ(q1_.w, oldQ1.w * q2_.w - oldQ1.x * q2_.x - oldQ1.y * q2_.y - oldQ1.z * q2_.z);
}

TEST_F(QuaternionTest, MultiplyScalar)
{
	const float scalar = 2.0f;
	printQuaternion("q1: ", q1_);
	const nc::Quaternionf mul = q1_ * scalar;
	printf("Multiplication by scalar %.2f: ", scalar);
	printQuaternion(mul);

	ASSERT_FLOAT_EQ(mul.x, q1_.x * scalar);
	ASSERT_FLOAT_EQ(mul.y, q1_.y * scalar);
	ASSERT_FLOAT_EQ(mul.z, q1_.z * scalar);
	ASSERT_FLOAT_EQ(mul.w, q1_.w * scalar);
}

TEST_F(QuaternionTest, DivideScalar)
{
	const float scalar = 2.0f;
	printQuaternion("q1: ", q1_);
	const nc::Quaternionf div = q1_ / scalar;
	printf("Division by scalar %.2f: ", scalar);
	printQuaternion(div);

	ASSERT_FLOAT_EQ(div.x, q1_.x / scalar);
	ASSERT_FLOAT_EQ(div.y, q1_.y / scalar);
	ASSERT_FLOAT_EQ(div.z, q1_.z / scalar);
	ASSERT_FLOAT_EQ(div.w, q1_.w / scalar);
}

TEST_F(QuaternionTest, MultiplyScalarInPlace)
{
	const float scalar = 2.0f;
	const nc::Quaternionf oldQ1 = q1_;

	printQuaternion("q1: ", q1_);
	printf("Multiplying the first quaternion by the scalar %.2f: ", scalar);
	printQuaternion(q1_ *= scalar);

	ASSERT_FLOAT_EQ(q1_.x, oldQ1.x * scalar);
	ASSERT_FLOAT_EQ(q1_.y, oldQ1.y * scalar);
	ASSERT_FLOAT_EQ(q1_.z, oldQ1.z * scalar);
	ASSERT_FLOAT_EQ(q1_.w, oldQ1.w * scalar);
}

TEST_F(QuaternionTest, DivideScalarInPlace)
{
	const float scalar = 2.0f;
	const nc::Quaternionf oldQ1 = q1_;

	printQuaternion("q1: ", q1_);
	printf("Dividing the first quaternion by the scalar %.2f: ", scalar);
	printQuaternion(q1_ /= scalar);

	ASSERT_FLOAT_EQ(q1_.x, oldQ1.x / scalar);
	ASSERT_FLOAT_EQ(q1_.y, oldQ1.y / scalar);
	ASSERT_FLOAT_EQ(q1_.z, oldQ1.z / scalar);
	ASSERT_FLOAT_EQ(q1_.w, oldQ1.w / scalar);
}

TEST_F(QuaternionTest, Normalize)
{
	const nc::Quaternionf oldQ1 = q1_;

	printQuaternion("q1: ", q1_);
	printf("Normalizing the first quaternion: ");
	printQuaternion(q1_.normalize());

	const float magnitude = sqrt(oldQ1.x * oldQ1.x + oldQ1.y * oldQ1.y + oldQ1.z * oldQ1.z + oldQ1.w * oldQ1.w);
	ASSERT_FLOAT_EQ(q1_.x, oldQ1.x / magnitude);
	ASSERT_FLOAT_EQ(q1_.y, oldQ1.y / magnitude);
	ASSERT_FLOAT_EQ(q1_.z, oldQ1.z / magnitude);
	ASSERT_FLOAT_EQ(q1_.w, oldQ1.w / magnitude);
}

TEST_F(QuaternionTest, Normalized)
{
	printQuaternion("q1: ", q1_);
	const nc::Quaternionf newQuat = q1_.normalized();
	printf("Creating a new quaternion as the normalized version of the first one: ");
	printQuaternion(q1_.normalize());

	const float magnitude = sqrt(q1_.x * q1_.x + q1_.y * q1_.y + q1_.z * q1_.z + q1_.w * q1_.w);
	ASSERT_FLOAT_EQ(newQuat.x, q1_.x / magnitude);
	ASSERT_FLOAT_EQ(newQuat.y, q1_.y / magnitude);
	ASSERT_FLOAT_EQ(newQuat.z, q1_.z / magnitude);
	ASSERT_FLOAT_EQ(newQuat.w, q1_.w / magnitude);
}

TEST_F(QuaternionTest, Magnitude)
{
	printQuaternion("q1: ", q1_);
	printf("The magnitude of the first quaternion: %.2f\n", q1_.magnitude());

	const float magnitude = sqrt(q1_.x * q1_.x + q1_.y * q1_.y + q1_.z * q1_.z + q1_.w * q1_.w);
	ASSERT_FLOAT_EQ(q1_.magnitude(), magnitude);
}

TEST_F(QuaternionTest, SqrMagnitude)
{
	printQuaternion("q1: ", q1_);
	printf("The squared magnitude of the first quaternion: %.2f\n", q1_.sqrMagnitude());

	const float sqrMagnitude = q1_.x * q1_.x + q1_.y * q1_.y + q1_.z * q1_.z + q1_.w * q1_.w;
	ASSERT_FLOAT_EQ(q1_.sqrMagnitude(), sqrMagnitude);
}

TEST_F(QuaternionTest, AddConstantQuaternions)
{
	printf("Adding all constant quaternions together: ");
	const nc::Quaternionf constants = nc::Quaternionf::Zero + nc::Quaternionf::Identity;
	printQuaternion(constants);

	ASSERT_FLOAT_EQ(constants.x, 0.0f);
	ASSERT_FLOAT_EQ(constants.y, 0.0f);
	ASSERT_FLOAT_EQ(constants.z, 0.0f);
	ASSERT_FLOAT_EQ(constants.w, 1.0f);
}

TEST_F(QuaternionTest, EqualityOperator)
{
	printQuaternion("q1: ", q1_);
	const nc::Quaternionf newQuat = q1_;
	printQuaternion("Creating a new quaternion as a copy of the first one: ", newQuat);

	printf("The first quaternion components are equal to the new one: %d", q1_ == newQuat);

	ASSERT_TRUE(q1_ == newQuat);
	ASSERT_FLOAT_EQ(q1_.x, newQuat.x);
	ASSERT_FLOAT_EQ(q1_.y, newQuat.y);
	ASSERT_FLOAT_EQ(q1_.z, newQuat.z);
	ASSERT_FLOAT_EQ(q1_.w, newQuat.w);
}

TEST_F(QuaternionTest, Negation)
{
	printQuaternion("q1: ", q1_);
	const nc::Quaternionf newQuat = -q1_;
	printQuaternion("Creating a new quaternion as the negation of the first one: ", newQuat);

	ASSERT_FLOAT_EQ(q1_.x, -newQuat.x);
	ASSERT_FLOAT_EQ(q1_.y, -newQuat.y);
	ASSERT_FLOAT_EQ(q1_.z, -newQuat.z);
	ASSERT_FLOAT_EQ(q1_.w, newQuat.w);
}

TEST_F(QuaternionTest, Conjugated)
{
	printQuaternion("q1: ", q1_);
	const nc::Quaternionf newQuat = q1_.conjugated();
	printQuaternion("Creating a new quaternion as the conjugate of the first one: ", newQuat);

	ASSERT_FLOAT_EQ(q1_.x, -newQuat.x);
	ASSERT_FLOAT_EQ(q1_.y, -newQuat.y);
	ASSERT_FLOAT_EQ(q1_.z, -newQuat.z);
	ASSERT_FLOAT_EQ(q1_.w, newQuat.w);
}

TEST_F(QuaternionTest, Conjugate)
{
	const nc::Quaternionf oldQ1 = q1_;

	printQuaternion("q1: ", q1_);
	q1_.conjugate();
	printQuaternion("Conjugating the first quaternion: ", q1_);

	ASSERT_FLOAT_EQ(q1_.x, -oldQ1.x);
	ASSERT_FLOAT_EQ(q1_.y, -oldQ1.y);
	ASSERT_FLOAT_EQ(q1_.z, -oldQ1.z);
	ASSERT_FLOAT_EQ(q1_.w, oldQ1.w);
}

TEST_F(QuaternionTest, PointerAccess)
{
	printQuaternion("q1: ", q1_);
	const float *ptr = q1_.data();
	printf("Components: ");
	for (unsigned int i = 0; i < 4; i++)
		printf("[%u]:%.2f ", i, ptr[i]);
	printf("\n");

	ASSERT_FLOAT_EQ(q1_.x, ptr[0]);
	ASSERT_FLOAT_EQ(q1_.y, ptr[1]);
	ASSERT_FLOAT_EQ(q1_.z, ptr[2]);
	ASSERT_FLOAT_EQ(q1_.w, ptr[3]);
}

TEST_F(QuaternionTest, SubscriptOperator)
{
	printQuaternion("q1: ", q1_);
	printf("Components: ");
	for (unsigned int i = 0; i < 4; i++)
		printf("[%u]:%.2f ", i, q1_[i]);
	printf("\n");

	ASSERT_FLOAT_EQ(q1_.x, q1_[0]);
	ASSERT_FLOAT_EQ(q1_.y, q1_[1]);
	ASSERT_FLOAT_EQ(q1_.z, q1_[2]);
	ASSERT_FLOAT_EQ(q1_.w, q1_[3]);
}

}
