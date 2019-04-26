#ifndef GTEST_MATRIX4X4_H
#define GTEST_MATRIX4X4_H

#include <ncine/Matrix4x4.h>
#include "gtest/gtest.h"

namespace nc = ncine;

namespace {

void printMatrix(const nc::Matrix4x4f &mat)
{
	printf("(");
	for (unsigned int i = 0; i < 4; i++)
	{
		if (i > 0)
			printf(" ");
		printf("%.2f,\t%.2f,\t%.2f,\t%.2f", mat[i].x, mat[i].y, mat[i].z, mat[i].w);
		if (i < 3)
			printf(",\n");
	}
	printf(")\n");
}

void printMatrix(const char *message, const nc::Matrix4x4f &mat)
{
	printf("%s", message);
	printMatrix(mat);
}

void assertVectorsAreEqual(const nc::Vector4f &v1, const nc::Vector4f &v2)
{
	ASSERT_FLOAT_EQ(v1.x, v2.x);
	ASSERT_FLOAT_EQ(v1.y, v2.y);
	ASSERT_FLOAT_EQ(v1.z, v2.z);
	ASSERT_FLOAT_EQ(v1.w, v2.w);
}

void assertVectorsAreNear(const nc::Vector4f &v1, const nc::Vector4f &v2, float absError)
{
	ASSERT_NEAR(v1.x, v2.x, absError);
	ASSERT_NEAR(v1.y, v2.y, absError);
	ASSERT_NEAR(v1.z, v2.z, absError);
	ASSERT_NEAR(v1.w, v2.w, absError);
}

void assertVectorsAreEqual(const nc::Vector4f &v1, float x, float y, float z, float w)
{
	ASSERT_FLOAT_EQ(v1.x, x);
	ASSERT_FLOAT_EQ(v1.y, y);
	ASSERT_FLOAT_EQ(v1.z, z);
	ASSERT_FLOAT_EQ(v1.w, w);
}

}

#endif
