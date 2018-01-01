#include "gtest_matrix4x4.h"

namespace {

class Matrix4x4OperationsTest : public ::testing::Test
{
  public:
	Matrix4x4OperationsTest()
		: m1_(nc::Matrix4x4f::rotationX(60))
	{ }

	nc::Matrix4x4f m1_;
};

TEST_F(Matrix4x4OperationsTest, Transposed)
{
	printf("m1:\n"); printMatrix(m1_);
	const nc::Matrix4x4f tr = m1_.transposed();
	printf("Matrix transposition:\n");
	printMatrix(tr);

	assertVectorsAreEqual(tr[0], m1_[0].x, m1_[1].x, m1_[2].x, m1_[3].x);
	assertVectorsAreEqual(tr[1], m1_[0].y, m1_[1].y, m1_[2].y, m1_[3].y);
	assertVectorsAreEqual(tr[2], m1_[0].z, m1_[1].z, m1_[2].z, m1_[3].z);
	assertVectorsAreEqual(tr[3], m1_[0].w, m1_[1].w, m1_[2].w, m1_[3].w);
}

TEST_F(Matrix4x4OperationsTest, TransposeInPlace)
{
	const nc::Matrix4x4f oldM1 = m1_;

	printf("m1:\n"); printMatrix(m1_);
	m1_.transpose();
	printf("Transposing the matrix:\n");
	printMatrix(m1_);

	assertVectorsAreEqual(m1_[0], oldM1[0].x, oldM1[1].x, oldM1[2].x, oldM1[3].x);
	assertVectorsAreEqual(m1_[1], oldM1[0].y, oldM1[1].y, oldM1[2].y, oldM1[3].y);
	assertVectorsAreEqual(m1_[2], oldM1[0].z, oldM1[1].z, oldM1[2].z, oldM1[3].z);
	assertVectorsAreEqual(m1_[3], oldM1[0].w, oldM1[1].w, oldM1[2].w, oldM1[3].w);
}

TEST_F(Matrix4x4OperationsTest, UniformScaleIdentity)
{
	const float sc = 0.5f;
	printf("The identity matrix:\n"); printMatrix(nc::Matrix4x4f::Identity);
	const nc::Matrix4x4f newMatrix = nc::Matrix4x4f::Identity.scale(sc);
	printf("Scaling the identity matrix by the scalar %.2f:\n", sc);
	printMatrix(newMatrix);

	assertVectorsAreEqual(newMatrix[0], nc::Matrix4x4f::Identity[0] * sc);
	assertVectorsAreEqual(newMatrix[1], nc::Matrix4x4f::Identity[1] * sc);
	assertVectorsAreEqual(newMatrix[2], nc::Matrix4x4f::Identity[2] * sc);
	assertVectorsAreEqual(newMatrix[3], nc::Matrix4x4f::Identity[3]);
}

TEST_F(Matrix4x4OperationsTest, ScaleIdentity)
{
	const nc::Vector3f sc(2.0f, 0.5, 0.75f);
	printf("The identity matrix:\n"); printMatrix(nc::Matrix4x4f::Identity);
	const nc::Matrix4x4f newMatrix = nc::Matrix4x4f::Identity.scale(sc);
	printf("Scaling the identity matrix by <%.2f, %.2f, %.2f>:\n", sc.x, sc.y, sc.z);
	printMatrix(newMatrix);

	assertVectorsAreEqual(newMatrix[0], nc::Matrix4x4f::Identity[0] * sc.x);
	assertVectorsAreEqual(newMatrix[1], nc::Matrix4x4f::Identity[1] * sc.y);
	assertVectorsAreEqual(newMatrix[2], nc::Matrix4x4f::Identity[2] * sc.z);
	assertVectorsAreEqual(newMatrix[3], nc::Matrix4x4f::Identity[3]);
}

TEST_F(Matrix4x4OperationsTest, TranslateIdentity)
{
	const nc::Vector3f tr(1.0f, 0.5, 0.75f);
	printf("The identity matrix:\n"); printMatrix(nc::Matrix4x4f::Identity);
	const nc::Matrix4x4f newMatrix = nc::Matrix4x4f::Identity.translation(tr);
	printf("Translating the identity matrix by <%.2f, %.2f, %.2f>:\n", tr.x, tr.y, tr.z);
	printMatrix(newMatrix);

	assertVectorsAreEqual(newMatrix[0], nc::Matrix4x4f::Identity[0]);
	assertVectorsAreEqual(newMatrix[1], nc::Matrix4x4f::Identity[1]);
	assertVectorsAreEqual(newMatrix[2], nc::Matrix4x4f::Identity[2]);
	assertVectorsAreEqual(newMatrix[3], tr.x, tr.y, tr.z, 1.0f);
}

TEST_F(Matrix4x4OperationsTest, RotateXIdentity)
{
	const float deg = 90.0f;
	printf("The identity matrix:\n"); printMatrix(nc::Matrix4x4f::Identity);
	const nc::Matrix4x4f newMatrix = nc::Matrix4x4f::Identity.rotationX(deg);
	printf("Rotating the identity matrix by %.2f degrees around X:\n", deg);
	printMatrix(newMatrix);

	assertVectorsAreNear(newMatrix[0], nc::Matrix4x4f::Identity[0], 0.0001f);
	assertVectorsAreNear(newMatrix[1], nc::Matrix4x4f::Identity[2], 0.0001f);
	assertVectorsAreNear(newMatrix[2], -nc::Matrix4x4f::Identity[1], 0.0001f);
	assertVectorsAreNear(newMatrix[3], nc::Matrix4x4f::Identity[3], 0.0001f);
}

TEST_F(Matrix4x4OperationsTest, RotateYIdentity)
{
	const float deg = 90.0f;
	printf("The identity matrix:\n"); printMatrix(nc::Matrix4x4f::Identity);
	const nc::Matrix4x4f newMatrix = nc::Matrix4x4f::Identity.rotationY(deg);
	printf("Rotating the identity matrix by %.2f degrees around Y:\n", deg);
	printMatrix(newMatrix);

	assertVectorsAreNear(newMatrix[0], -nc::Matrix4x4f::Identity[2], 0.0001f);
	assertVectorsAreNear(newMatrix[1], nc::Matrix4x4f::Identity[1], 0.0001f);
	assertVectorsAreNear(newMatrix[2], nc::Matrix4x4f::Identity[0], 0.0001f);
	assertVectorsAreNear(newMatrix[3], nc::Matrix4x4f::Identity[3], 0.0001f);
}

TEST_F(Matrix4x4OperationsTest, RotateZIdentity)
{
	const float deg = 90.0f;
	printf("The identity matrix:\n"); printMatrix(nc::Matrix4x4f::Identity);
	const nc::Matrix4x4f newMatrix = nc::Matrix4x4f::Identity.rotationZ(deg);
	printf("Rotating the identity matrix by %.2f degrees around Z:\n", deg);
	printMatrix(newMatrix);

	assertVectorsAreNear(newMatrix[0], nc::Matrix4x4f::Identity[1], 0.0001f);
	assertVectorsAreNear(newMatrix[1], -nc::Matrix4x4f::Identity[0], 0.0001f);
	assertVectorsAreNear(newMatrix[2], nc::Matrix4x4f::Identity[2], 0.0001f);
	assertVectorsAreNear(newMatrix[3], nc::Matrix4x4f::Identity[3], 0.0001f);
}

TEST_F(Matrix4x4OperationsTest, InvertIdentity)
{
	printf("The identity matrix:\n"); printMatrix(nc::Matrix4x4f::Identity);
	const nc::Matrix4x4f newMatrix = nc::Matrix4x4f::Identity.inverse();
	printf("Inverting the identity matrix:\n");
	printMatrix(newMatrix);

	assertVectorsAreEqual(newMatrix[0], nc::Matrix4x4f::Identity[0]);
	assertVectorsAreEqual(newMatrix[1], nc::Matrix4x4f::Identity[1]);
	assertVectorsAreEqual(newMatrix[2], nc::Matrix4x4f::Identity[2]);
	assertVectorsAreEqual(newMatrix[3], nc::Matrix4x4f::Identity[3]);
}

TEST_F(Matrix4x4OperationsTest, InvertOrthogonal)
{
	printf("m1 is orthogonal:\n"); printMatrix(m1_);
	const nc::Matrix4x4f tr = m1_.transposed();
	printf("Creating a new matrix as the transpose of the first one:\n"); printMatrix(tr);
	const nc::Matrix4x4f inv = m1_.inverse();
	printf("Creating a new matrix as the inverse of the first one:\n"); printMatrix(inv);

	printf("The transpose and the inverse are equal: %d\n", tr == inv);

	ASSERT_TRUE(tr == inv);
	assertVectorsAreEqual(tr[0], inv[0]);
	assertVectorsAreEqual(tr[1], inv[1]);
	assertVectorsAreEqual(tr[2], inv[2]);
	assertVectorsAreEqual(tr[3], inv[3]);
}

TEST_F(Matrix4x4OperationsTest, TransformVector4)
{
	printf("m1:\n"); printMatrix(m1_);

	const nc::Vector4f v1 = nc::Vector4f(0.5f, 1.0f, 0.75f, 1.0f).normalize();
	printf("v1: <%.2f, %.2f, %.2f, %.2f> (length: %.2f)\n", v1.x, v1.y, v1.z, v1.w, v1.length());
	const nc::Vector4f tr = m1_ * v1;
	printf("Transforming the vector by the matrix: <%.2f, %.2f, %.2f, %.2f> (length: %.2f)\n", tr.x, tr.y, tr.z, tr.w, tr.length());

	ASSERT_FLOAT_EQ(v1.length(), 1.0f);
	ASSERT_FLOAT_EQ(tr.length(), 1.0f);
	ASSERT_FLOAT_EQ(tr.x, nc::dot(m1_[0], v1));
	ASSERT_FLOAT_EQ(tr.y, nc::dot(m1_[1], v1));
	ASSERT_FLOAT_EQ(tr.z, nc::dot(m1_[2], v1));
	ASSERT_FLOAT_EQ(tr.w, nc::dot(m1_[3], v1));
}

TEST_F(Matrix4x4OperationsTest, TransformVector3)
{
	printf("m1:\n"); printMatrix(m1_);

	const nc::Vector3f v1 = nc::Vector3f(0.5f, 1.0f, 0.75f).normalize();
	printf("v1: <%.2f, %.2f, %.2f> (length: %.2f)\n", v1.x, v1.y, v1.z, v1.length());
	const nc::Vector3f tr = m1_ * v1;
	printf("Transforming the vector by the matrix: <%.2f, %.2f, %.2f> (length: %.2f)\n", tr.x, tr.y, tr.z, tr.length());

	ASSERT_FLOAT_EQ(v1.length(), 1.0f);
	ASSERT_FLOAT_EQ(tr.length(), 1.0f);
	ASSERT_FLOAT_EQ(tr.x, nc::dot(m1_[0].toVector3(), v1));
	ASSERT_FLOAT_EQ(tr.y, nc::dot(m1_[1].toVector3(), v1));
	ASSERT_FLOAT_EQ(tr.z, nc::dot(m1_[2].toVector3(), v1));
}

TEST_F(Matrix4x4OperationsTest, Ortho)
{
	const nc::Matrix4x4f orthoMat = nc::Matrix4x4f::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f);
	printf("Build an orthographic matrix to be equal to identity:\n"); printMatrix(orthoMat);

	assertVectorsAreEqual(orthoMat[0], nc::Matrix4x4f::Identity[0]);
	assertVectorsAreEqual(orthoMat[1], nc::Matrix4x4f::Identity[1]);
	assertVectorsAreEqual(orthoMat[2], nc::Matrix4x4f::Identity[2]);
	assertVectorsAreEqual(orthoMat[3], nc::Matrix4x4f::Identity[3]);
}

TEST_F(Matrix4x4OperationsTest, Frustum)
{
	const nc::Matrix4x4f frustumMat = nc::Matrix4x4f::frustum(1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f);
	printf("Build a simple frustum matrix:\n"); printMatrix(frustumMat);

	assertVectorsAreEqual(frustumMat[0], nc::Matrix4x4f::Identity[0]);
	assertVectorsAreEqual(frustumMat[1], nc::Matrix4x4f::Identity[1]);
	assertVectorsAreEqual(frustumMat[2], -nc::Matrix4x4f::Identity[3]);
	assertVectorsAreEqual(frustumMat[3], nc::Matrix4x4f::Identity[2]);
}

TEST_F(Matrix4x4OperationsTest, Perspective)
{
	const nc::Matrix4x4f perspMat = nc::Matrix4x4f::perspective(90.0f, 1.0f, -1.0f, 1.0f);
	printf("Build a simple perspective matrix:\n"); printMatrix(perspMat);

	assertVectorsAreEqual(perspMat[0], nc::Matrix4x4f::Identity[0]);
	assertVectorsAreEqual(perspMat[1], nc::Matrix4x4f::Identity[1]);
	assertVectorsAreEqual(perspMat[2], -nc::Matrix4x4f::Identity[3]);
	assertVectorsAreEqual(perspMat[3], nc::Matrix4x4f::Identity[2]);
}

}
