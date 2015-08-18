#include <cstdlib> // for EXIT_SUCCESS
#include <cstdio>
#include "Quaternion.h"

namespace nc = ncine;

inline void print_quaternion(const nc::Quaternionf& quat)
{
	printf("<%.2f, %.2f, %.2f, %.2f>\n", quat.x, quat.y, quat.z, quat.w);
}

int main()
{
	nc::Quaternionf q1(nc::Quaternionf(2.5f, 2.0f, 0.5f, 1.0));
	nc::Quaternionf q2(nc::Quaternionf(1.0f, 1.5f, 1.0f, 2.0));

	printf("First quaternion:\n");
	print_quaternion(q1);
	printf("Second quaternion:\n");
	print_quaternion(q2);

	printf("Quaternion addition:\n");
	print_quaternion(q1 + q2);

	printf("Quaternion difference:\n");
	print_quaternion(q1 - q2);

	printf("Quaternion multiplication:\n");
	print_quaternion(q1 * q2);

	printf("Add the second quaternion to the first:\n");
	print_quaternion(q1 += q2);

	printf("Subtract the second quaternion from the first:\n");
	print_quaternion(q1 -= q2);

	printf("Multiply the first quaternion by the second:\n");
	print_quaternion(q1 *= q2);

	printf("Divide the first quaternion by a scalar:\n");
	print_quaternion(q1 /= 2);

	printf("Normalize the first quaternion:\n");
	print_quaternion(q1.normalize());

	printf("Conjugate the normalized first quaternion (thus inverting it):\n");
	print_quaternion(q1.conjugate());

	printf("Add all the constant quaternions together:\n");
	nc::Quaternionf constants = nc::Quaternionf::Zero + nc::Quaternionf::Identity;
	print_quaternion(constants);

	return EXIT_SUCCESS;
}
