#include "gtest_matrix4x4.h"

namespace {

class Matrix4x4Test : public ::testing::Test
{
  public:
	Matrix4x4Test()
	    : m1_(nc::Vector4f(2.5f, 2.0f, 0.5f, 1.0),
	          nc::Vector4f(2.5f, 2.0f, 0.5f, 1.0),
	          nc::Vector4f(2.5f, 2.0f, 0.5f, 1.0),
	          nc::Vector4f(2.5f, 2.0f, 0.5f, 1.0)),

	      m2_(nc::Vector4f(1.0f, 1.5f, 1.0f, 2.0),
	          nc::Vector4f(1.0f, 1.5f, 1.0f, 2.0),
	          nc::Vector4f(1.0f, 1.5f, 1.0f, 2.0),
	          nc::Vector4f(1.0f, 1.5f, 1.0f, 2.0))
	{}

	nc::Matrix4x4f m1_;
	nc::Matrix4x4f m2_;
};

TEST_F(Matrix4x4Test, SetVectors)
{
	printMatrix("m1:\n", m1_);

	const nc::Vector4f v1(0.0f, 0.0f, 0.0f, 0.0f);
	const nc::Vector4f v2(0.0f, 0.0f, 0.0f, 0.0f);
	const nc::Vector4f v3(0.0f, 0.0f, 0.0f, 0.0f);
	const nc::Vector4f v4(0.0f, 0.0f, 0.0f, 0.0f);

	m1_.set(v1, v2, v3, v4);
	printMatrix("Setting matrix vectors:\n", m1_);

	assertVectorsAreEqual(m1_[0], v1);
	assertVectorsAreEqual(m1_[1], v2);
	assertVectorsAreEqual(m1_[2], v3);
	assertVectorsAreEqual(m1_[3], v4);
}

TEST_F(Matrix4x4Test, Addition)
{
	printMatrix("m1:\n", m1_);
	printMatrix("m2:\n", m2_);
	const nc::Matrix4x4f sum = m1_ + m2_;
	printMatrix("Matrix addition:\n", sum);

	assertVectorsAreEqual(sum[0], m1_[0] + m2_[0]);
	assertVectorsAreEqual(sum[1], m1_[1] + m2_[1]);
	assertVectorsAreEqual(sum[2], m1_[2] + m2_[2]);
	assertVectorsAreEqual(sum[3], m1_[3] + m2_[3]);
}

TEST_F(Matrix4x4Test, Subtraction)
{
	printMatrix("m1:\n", m1_);
	printMatrix("m2:\n", m2_);
	const nc::Matrix4x4f sub = m1_ - m2_;
	printMatrix("Matrix subtraction:\n", sub);

	assertVectorsAreEqual(sub[0], m1_[0] - m2_[0]);
	assertVectorsAreEqual(sub[1], m1_[1] - m2_[1]);
	assertVectorsAreEqual(sub[2], m1_[2] - m2_[2]);
	assertVectorsAreEqual(sub[3], m1_[3] - m2_[3]);
}

TEST_F(Matrix4x4Test, Multiplication)
{
	printMatrix("m1:\n", m1_);
	printMatrix("m2:\n", m2_);
	const nc::Matrix4x4f mul = m1_ * m2_;
	printMatrix("Matrix multiplication:\n", mul);

	assertVectorsAreEqual(mul[0], m1_[0] * m2_[0][0] + m1_[1] * m2_[0][1] + m1_[2] * m2_[0][2] + m1_[3] * m2_[0][3]);
	assertVectorsAreEqual(mul[1], m1_[0] * m2_[1][0] + m1_[1] * m2_[1][1] + m1_[2] * m2_[1][2] + m1_[3] * m2_[1][3]);
	assertVectorsAreEqual(mul[2], m1_[0] * m2_[2][0] + m1_[1] * m2_[2][1] + m1_[2] * m2_[2][2] + m1_[3] * m2_[2][3]);
	assertVectorsAreEqual(mul[3], m1_[0] * m2_[3][0] + m1_[1] * m2_[3][1] + m1_[2] * m2_[3][2] + m1_[3] * m2_[3][3]);
}

TEST_F(Matrix4x4Test, Division)
{
	printMatrix("m1:\n", m1_);
	printMatrix("m2:\n", m2_);
	const nc::Matrix4x4f div = m1_ / m2_;
	printMatrix("Matrix division:\n", div);

	assertVectorsAreEqual(div[0], m1_[0] / m2_[0]);
	assertVectorsAreEqual(div[1], m1_[1] / m2_[1]);
	assertVectorsAreEqual(div[2], m1_[2] / m2_[2]);
	assertVectorsAreEqual(div[3], m1_[3] / m2_[3]);
}

TEST_F(Matrix4x4Test, AdditionInPlace)
{
	const nc::Matrix4x4f oldM1 = m1_;

	printMatrix("m1:\n", m1_);
	printMatrix("m2:\n", m2_);
	printMatrix("Adding the second matrix to the first:\n", m1_ += m2_);

	assertVectorsAreEqual(m1_[0], oldM1[0] + m2_[0]);
	assertVectorsAreEqual(m1_[1], oldM1[1] + m2_[1]);
	assertVectorsAreEqual(m1_[2], oldM1[2] + m2_[2]);
	assertVectorsAreEqual(m1_[3], oldM1[3] + m2_[3]);
}

TEST_F(Matrix4x4Test, SubtractionInPlace)
{
	const nc::Matrix4x4f oldM1 = m1_;

	printMatrix("m1:\n", m1_);
	printMatrix("m2:\n", m2_);
	printMatrix("Subtracting the second matrix from the first:\n", m1_ -= m2_);

	assertVectorsAreEqual(m1_[0], oldM1[0] - m2_[0]);
	assertVectorsAreEqual(m1_[1], oldM1[1] - m2_[1]);
	assertVectorsAreEqual(m1_[2], oldM1[2] - m2_[2]);
	assertVectorsAreEqual(m1_[3], oldM1[3] - m2_[3]);
}

TEST_F(Matrix4x4Test, MultiplicationInPlace)
{
	const nc::Matrix4x4f oldM1 = m1_;

	printMatrix("m1:\n", m1_);
	printMatrix("m2:\n", m2_);
	printMatrix("Multiplying the first matrix by the second:\n", m1_ *= m2_);

	assertVectorsAreEqual(m1_[0], oldM1[0] * m2_[0][0] + oldM1[1] * m2_[0][1] + oldM1[2] * m2_[0][2] + oldM1[3] * m2_[0][3]);
	assertVectorsAreEqual(m1_[1], oldM1[0] * m2_[1][0] + oldM1[1] * m2_[1][1] + oldM1[2] * m2_[1][2] + oldM1[3] * m2_[1][3]);
	assertVectorsAreEqual(m1_[2], oldM1[0] * m2_[2][0] + oldM1[1] * m2_[2][1] + oldM1[2] * m2_[2][2] + oldM1[3] * m2_[2][3]);
	assertVectorsAreEqual(m1_[3], oldM1[0] * m2_[3][0] + oldM1[1] * m2_[3][1] + oldM1[2] * m2_[3][2] + oldM1[3] * m2_[3][3]);
}

TEST_F(Matrix4x4Test, DivisionInPlace)
{
	const nc::Matrix4x4f oldM1 = m1_;

	printMatrix("m1:\n", m1_);
	printMatrix("m2:\n", m2_);
	printMatrix("Dividing the first matrix by the second:\n", m1_ /= m2_);

	assertVectorsAreEqual(m1_[0], oldM1[0] / m2_[0]);
	assertVectorsAreEqual(m1_[1], oldM1[1] / m2_[1]);
	assertVectorsAreEqual(m1_[2], oldM1[2] / m2_[2]);
	assertVectorsAreEqual(m1_[3], oldM1[3] / m2_[3]);
}

TEST_F(Matrix4x4Test, AddScalar)
{
	const float scalar = 1.0f;
	printMatrix("m1:\n", m1_);
	const nc::Matrix4x4f add = m1_ + scalar;
	printf("Addition of scalar %.2f:\n", scalar);
	printMatrix(add);

	assertVectorsAreEqual(add[0], m1_[0] + scalar);
	assertVectorsAreEqual(add[1], m1_[1] + scalar);
	assertVectorsAreEqual(add[2], m1_[2] + scalar);
	assertVectorsAreEqual(add[3], m1_[3] + scalar);
}

TEST_F(Matrix4x4Test, SubtractScalar)
{
	const float scalar = 1.0f;
	printMatrix("m1:\n", m1_);
	const nc::Matrix4x4f sub = m1_ - scalar;
	printf("Subtraction of scalar %.2f:\n", scalar);
	printMatrix(sub);

	assertVectorsAreEqual(sub[0], m1_[0] - scalar);
	assertVectorsAreEqual(sub[1], m1_[1] - scalar);
	assertVectorsAreEqual(sub[2], m1_[2] - scalar);
	assertVectorsAreEqual(sub[3], m1_[3] - scalar);
}

TEST_F(Matrix4x4Test, MultiplyScalar)
{
	const float scalar = 2.0f;
	printMatrix("m1:\n", m1_);
	const nc::Matrix4x4f mul = m1_ * scalar;
	printf("Multiplication by scalar %.2f:\n", scalar);
	printMatrix(mul);

	assertVectorsAreEqual(mul[0], m1_[0] * scalar);
	assertVectorsAreEqual(mul[1], m1_[1] * scalar);
	assertVectorsAreEqual(mul[2], m1_[2] * scalar);
	assertVectorsAreEqual(mul[3], m1_[3] * scalar);
}

TEST_F(Matrix4x4Test, DivideScalar)
{
	const float scalar = 2.0f;
	printMatrix("m1:\n", m1_);
	const nc::Matrix4x4f div = m1_ / scalar;
	printf("Division by scalar %.2f:\n", scalar);
	printMatrix(div);

	assertVectorsAreEqual(div[0], m1_[0] / scalar);
	assertVectorsAreEqual(div[1], m1_[1] / scalar);
	assertVectorsAreEqual(div[2], m1_[2] / scalar);
	assertVectorsAreEqual(div[3], m1_[3] / scalar);
}

TEST_F(Matrix4x4Test, AddScalarInPlace)
{
	const float scalar = 1.0f;
	const nc::Matrix4x4f oldM1 = m1_;

	printMatrix("m1:\n", m1_);
	printf("Adding the scalar %.2f to the first matrix:\n", scalar);
	printMatrix(m1_ += scalar);

	assertVectorsAreEqual(m1_[0], oldM1[0] + scalar);
	assertVectorsAreEqual(m1_[1], oldM1[1] + scalar);
	assertVectorsAreEqual(m1_[2], oldM1[2] + scalar);
	assertVectorsAreEqual(m1_[3], oldM1[3] + scalar);
}

TEST_F(Matrix4x4Test, SubtractScalarInPlace)
{
	const float scalar = 1.0f;
	const nc::Matrix4x4f oldM1 = m1_;

	printMatrix("m1:\n", m1_);
	printf("Subtracting the scalar %.2f to the first matrix:\n", scalar);
	printMatrix(m1_ -= scalar);

	assertVectorsAreEqual(m1_[0], oldM1[0] - scalar);
	assertVectorsAreEqual(m1_[1], oldM1[1] - scalar);
	assertVectorsAreEqual(m1_[2], oldM1[2] - scalar);
	assertVectorsAreEqual(m1_[3], oldM1[3] - scalar);
}

TEST_F(Matrix4x4Test, MultiplyScalarInPlace)
{
	const float scalar = 2.0f;
	const nc::Matrix4x4f oldM1 = m1_;

	printMatrix("m1:\n", m1_);
	printf("Multiplying the first matrix by the scalar %.2f:\n", scalar);
	printMatrix(m1_ *= scalar);

	assertVectorsAreEqual(m1_[0], oldM1[0] * scalar);
	assertVectorsAreEqual(m1_[1], oldM1[1] * scalar);
	assertVectorsAreEqual(m1_[2], oldM1[2] * scalar);
	assertVectorsAreEqual(m1_[3], oldM1[3] * scalar);
}

TEST_F(Matrix4x4Test, DivideScalarInPlace)
{
	const float scalar = 2.0f;
	const nc::Matrix4x4f oldM1 = m1_;

	printMatrix("m1:\n", m1_);
	printf("Dividing the first matrix by the scalar %.2f:\n", scalar);
	printMatrix(m1_ /= scalar);

	assertVectorsAreEqual(m1_[0], oldM1[0] / scalar);
	assertVectorsAreEqual(m1_[1], oldM1[1] / scalar);
	assertVectorsAreEqual(m1_[2], oldM1[2] / scalar);
	assertVectorsAreEqual(m1_[3], oldM1[3] / scalar);
}

TEST_F(Matrix4x4Test, MultiplyScalarAsFirst)
{
	const float scalar = 2.0f;
	printMatrix("m1:\n", m1_);
	const nc::Matrix4x4f mul = scalar * m1_;
	printf("Multiplication by scalar %.2f:\n", scalar);
	printMatrix(mul);

	assertVectorsAreEqual(mul[0], scalar * m1_[0]);
	assertVectorsAreEqual(mul[1], scalar * m1_[1]);
	assertVectorsAreEqual(mul[2], scalar * m1_[2]);
	assertVectorsAreEqual(mul[3], scalar * m1_[3]);
}

TEST_F(Matrix4x4Test, AddConstantVectors)
{
	printf("Adding all constant matrices together:\n");
	const nc::Matrix4x4f constants = nc::Matrix4x4f::Zero + nc::Matrix4x4f::Identity;
	printMatrix(constants);

	assertVectorsAreEqual(constants[0], nc::Vector4f::XAxis);
	assertVectorsAreEqual(constants[1], nc::Vector4f::YAxis);
	assertVectorsAreEqual(constants[2], nc::Vector4f::ZAxis);
	assertVectorsAreEqual(constants[3], nc::Vector4f::WAxis);
}

TEST_F(Matrix4x4Test, EqualityOperator)
{
	printMatrix("m1:\n", m1_);
	const nc::Matrix4x4f newMatrix = m1_;
	printMatrix("Creating a new matrix as a copy of the first one:\n", newMatrix);

	printf("The first matrix vectors are equal to the new one: %d", m1_ == newMatrix);

	ASSERT_TRUE(m1_ == newMatrix);
	assertVectorsAreEqual(m1_[0], newMatrix[0]);
	assertVectorsAreEqual(m1_[1], newMatrix[1]);
	assertVectorsAreEqual(m1_[2], newMatrix[2]);
	assertVectorsAreEqual(m1_[3], newMatrix[3]);
}

TEST_F(Matrix4x4Test, Negation)
{
	printMatrix("m1:\n", m1_);
	const nc::Matrix4x4f newMatrix = -m1_;
	printMatrix("Creating a new matrix as the negation of the first one:\n", newMatrix);

	assertVectorsAreEqual(m1_[0], -newMatrix[0]);
	assertVectorsAreEqual(m1_[1], -newMatrix[1]);
	assertVectorsAreEqual(m1_[2], -newMatrix[2]);
	assertVectorsAreEqual(m1_[3], -newMatrix[3]);
}

TEST_F(Matrix4x4Test, PointerAccess)
{
	printMatrix("m1:\n", m1_);
	const float *ptr = m1_.data();
	printf("Vectors:\n");
	for (unsigned int i = 0; i < 16; i += 4)
		printf("[%u] : <%.2f, %.2f, %.2f, %.2f> \n", i / 4, ptr[i], ptr[i + 1], ptr[i + 2], ptr[i + 3]);
	printf("\n");

	assertVectorsAreEqual(m1_[0], ptr[0], ptr[1], ptr[2], ptr[3]);
	assertVectorsAreEqual(m1_[1], ptr[4], ptr[5], ptr[6], ptr[7]);
	assertVectorsAreEqual(m1_[2], ptr[8], ptr[9], ptr[10], ptr[11]);
	assertVectorsAreEqual(m1_[3], ptr[12], ptr[13], ptr[14], ptr[15]);
}

}
