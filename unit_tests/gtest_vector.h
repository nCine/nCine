#ifndef GTEST_VECTOR_H
#define GTEST_VECTOR_H

#include <ncine/Vector2.h>
#include <ncine/Vector3.h>
#include <ncine/Vector4.h>
#include "gtest/gtest.h"

namespace nc = ncine;

namespace {

void printVector(const nc::Vector2f &vec)
{
	printf("<%.2f, %.2f>\n", vec.x, vec.y);
}

void printVector(const char *message, const nc::Vector2f &vec)
{
	printf("%s", message);
	printVector(vec);
}

void printVector(const nc::Vector3f &vec)
{
	printf("<%.2f, %.2f, %.2f>\n", vec.x, vec.y, vec.z);
}

void printVector(const char *message, const nc::Vector3f &vec)
{
	printf("%s", message);
	printVector(vec);
}

void printVector(const nc::Vector4f &vec)
{
	printf("<%.2f, %.2f, %.2f, %.2f>\n", vec.x, vec.y, vec.z, vec.w);
}

void printVector(const char *message, const nc::Vector4f &vec)
{
	printf("%s", message);
	printVector(vec);
}

}

#endif
