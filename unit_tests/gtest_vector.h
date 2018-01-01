#ifndef GTEST_VECTOR_H
#define GTEST_VECTOR_H

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "gtest/gtest.h"

namespace nc = ncine;

namespace {

void printVector(const nc::Vector2f &vec)
{
	printf("<%.2f, %.2f>\n", vec.x, vec.y);
}

void printVector(const nc::Vector3f &vec)
{
	printf("<%.2f, %.2f, %.2f>\n", vec.x, vec.y, vec.z);
}

void printVector(const nc::Vector4f &vec)
{
	printf("<%.2f, %.2f, %.2f, %.2f>\n", vec.x, vec.y, vec.z, vec.w);
}

}

#endif
