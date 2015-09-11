#include <cstdlib> // for EXIT_SUCCESS
#include <cstdio>
#include "Quaternion.h"

namespace nc = ncine;

namespace {

inline void printQuaternion(const nc::Quaternionf& quat)
{
	printf("<%.2f, %.2f, %.2f, %.2f>\n", quat.x, quat.y, quat.z, quat.w);
}

}

int main()
{
	nc::Quaternionf q1(nc::Quaternionf(2.5f, 2.0f, 0.5f, 1.0));
	nc::Quaternionf q2(nc::Quaternionf(1.0f, 1.5f, 1.0f, 2.0));

	printf("First quaternion:\n");
	printQuaternion(q1);
	printf("Second quaternion:\n");
	printQuaternion(q2);

	printf("Quaternion addition:\n");
	printQuaternion(q1 + q2);

	printf("Quaternion difference:\n");
	printQuaternion(q1 - q2);

	printf("Quaternion multiplication:\n");
	printQuaternion(q1 * q2);

	printf("Add the second quaternion to the first:\n");
	printQuaternion(q1 += q2);

	printf("Subtract the second quaternion from the first:\n");
	printQuaternion(q1 -= q2);

	printf("Multiply the first quaternion by the second:\n");
	printQuaternion(q1 *= q2);

	printf("Divide the first quaternion by a scalar:\n");
	printQuaternion(q1 /= 2);

	printf("Normalize the first quaternion:\n");
	printQuaternion(q1.normalize());

	printf("Conjugate the normalized first quaternion (thus inverting it):\n");
	printQuaternion(q1.conjugate());

	printf("Add all the constant quaternions together:\n");
	nc::Quaternionf constants = nc::Quaternionf::Zero + nc::Quaternionf::Identity;
	printQuaternion(constants);

	return EXIT_SUCCESS;
}
