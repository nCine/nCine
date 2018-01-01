#include "gtest_quaternion.h"
#include "gtest_matrix4x4.h"

namespace {

TEST(QuaternionOperationsTest, RotationXToMatrix)
{
	const float deg = 45.0f;
	const nc::Quaternionf newQuat = nc::Quaternionf::fromXAxisAngle(deg);
	printf("Creating a new quaternion from a %.2f degrees rotation around the X axis: ", deg);
	printQuaternion(newQuat);

	const nc::Matrix4x4f mat = nc::Matrix4x4f::rotationX(deg);
	printf("Creating a matrix from a %.2f degrees rotation around the X axis:\n", deg);
	printMatrix(mat);

	const nc::Matrix4x4f fromQuat = newQuat.toMatrix4x4();
	printf("Creating a matrix the quaternion:\n");
	printMatrix(fromQuat);

	assertVectorsAreEqual(mat[0], fromQuat[0]);
	assertVectorsAreEqual(mat[1], fromQuat[1]);
	assertVectorsAreEqual(mat[2], fromQuat[2]);
	assertVectorsAreEqual(mat[3], fromQuat[3]);
}

TEST(QuaternionOperationsTest, RotationYToMatrix)
{
	const float deg = 45.0f;
	const nc::Quaternionf newQuat = nc::Quaternionf::fromYAxisAngle(deg);
	printf("Creating a new quaternion from a %.2f degrees rotation around the Y axis: ", deg);
	printQuaternion(newQuat);

	const nc::Matrix4x4f mat = nc::Matrix4x4f::rotationY(deg);
	printf("Creating a matrix from a %.2f degrees rotation around the Y axis:\n", deg);
	printMatrix(mat);

	const nc::Matrix4x4f fromQuat = newQuat.toMatrix4x4();
	printf("Creating a matrix the quaternion:\n");
	printMatrix(fromQuat);

	assertVectorsAreEqual(mat[0], fromQuat[0]);
	assertVectorsAreEqual(mat[1], fromQuat[1]);
	assertVectorsAreEqual(mat[2], fromQuat[2]);
	assertVectorsAreEqual(mat[3], fromQuat[3]);
}

TEST(QuaternionOperationsTest, RotationZToMatrix)
{
	const float deg = 45.0f;
	const nc::Quaternionf newQuat = nc::Quaternionf::fromZAxisAngle(deg);
	printf("Creating a new quaternion from a %.2f degrees rotation around the Z axis: ", deg);
	printQuaternion(newQuat);

	const nc::Matrix4x4f mat = nc::Matrix4x4f::rotationZ(deg);
	printf("Creating a matrix from a %.2f degrees rotation around the Z axis:\n", deg);
	printMatrix(mat);

	const nc::Matrix4x4f fromQuat = newQuat.toMatrix4x4();
	printf("Creating a matrix the quaternion:\n");
	printMatrix(fromQuat);

	assertVectorsAreEqual(mat[0], fromQuat[0]);
	assertVectorsAreEqual(mat[1], fromQuat[1]);
	assertVectorsAreEqual(mat[2], fromQuat[2]);
	assertVectorsAreEqual(mat[3], fromQuat[3]);
}

TEST(QuaternionOperationsTest, RotationAxisXToMatrix)
{
	const float deg = 60.0f;
	const nc::Vector3f axis(1.0f, 0.0f, 0.0f);
	const nc::Quaternionf newQuat = nc::Quaternionf::fromAxisAngle(axis, deg);
	printf("Creating a new quaternion from a %.2f degrees rotation around the <%.2f, %.2f, %.2f> axis: ", deg, axis.x, axis.y, axis.y);
	printQuaternion(newQuat);

	const nc::Matrix4x4f mat = nc::Matrix4x4f::rotationX(deg);
	printf("Creating a matrix from a %.2f degrees rotation around the X axis:\n", deg);
	printMatrix(mat);

	const nc::Matrix4x4f fromQuat = newQuat.toMatrix4x4();
	printf("Creating a matrix the quaternion:\n");
	printMatrix(fromQuat);

	assertVectorsAreEqual(mat[0], fromQuat[0]);
	assertVectorsAreEqual(mat[1], fromQuat[1]);
	assertVectorsAreEqual(mat[2], fromQuat[2]);
	assertVectorsAreEqual(mat[3], fromQuat[3]);
}

TEST(QuaternionOperationsTest, RotationAxisYToMatrix)
{
	const float deg = 60.0f;
	const nc::Vector3f axis(0.0f, 1.0f, 0.0f);
	const nc::Quaternionf newQuat = nc::Quaternionf::fromAxisAngle(axis, deg);
	printf("Creating a new quaternion from a %.2f degrees rotation around the <%.2f, %.2f, %.2f> axis: ", deg, axis.x, axis.y, axis.y);
	printQuaternion(newQuat);

	const nc::Matrix4x4f mat = nc::Matrix4x4f::rotationY(deg);
	printf("Creating a matrix from a %.2f degrees rotation around the Y axis:\n", deg);
	printMatrix(mat);

	const nc::Matrix4x4f fromQuat = newQuat.toMatrix4x4();
	printf("Creating a matrix the quaternion:\n");
	printMatrix(fromQuat);

	assertVectorsAreEqual(mat[0], fromQuat[0]);
	assertVectorsAreEqual(mat[1], fromQuat[1]);
	assertVectorsAreEqual(mat[2], fromQuat[2]);
	assertVectorsAreEqual(mat[3], fromQuat[3]);
}

TEST(QuaternionOperationsTest, RotationAxisZToMatrix)
{
	const float deg = 60.0f;
	const nc::Vector3f axis(0.0f, 0.0f, 1.0f);
	const nc::Quaternionf newQuat = nc::Quaternionf::fromAxisAngle(axis, deg);
	printf("Creating a new quaternion from a %.2f degrees rotation around the <%.2f, %.2f, %.2f> axis: ", deg, axis.x, axis.y, axis.y);
	printQuaternion(newQuat);

	const nc::Matrix4x4f mat = nc::Matrix4x4f::rotationZ(deg);
	printf("Creating a matrix from a %.2f degrees rotation around the Z axis:\n", deg);
	printMatrix(mat);

	const nc::Matrix4x4f fromQuat = newQuat.toMatrix4x4();
	printf("Creating a matrix the quaternion:\n");
	printMatrix(fromQuat);

	assertVectorsAreEqual(mat[0], fromQuat[0]);
	assertVectorsAreEqual(mat[1], fromQuat[1]);
	assertVectorsAreEqual(mat[2], fromQuat[2]);
	assertVectorsAreEqual(mat[3], fromQuat[3]);
}

}
