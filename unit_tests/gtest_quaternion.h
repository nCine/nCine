#ifndef GTEST_QUATERNION_H
#define GTEST_QUATERNION_H

#include "Quaternion.h"
#include "gtest/gtest.h"

namespace nc = ncine;

namespace {

void printQuaternion(const nc::Quaternionf &quat)
{
	printf("<%.2f, %.2f, %.2f, %.2f>\n", quat.x, quat.y, quat.z, quat.w);
}

}

#endif
