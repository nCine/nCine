#include <cstdlib> // for EXIT_SUCCESS
#include <cstdio>
#include "Quaternion.h"
#include "test_functions.h"

namespace nc = ncine;

int main()
{
	nc::Quaternionf q1(nc::Quaternionf::fromAxisAngle(nc::Vector3f::XAxis, 60));
	nc::Quaternionf q2(nc::Quaternionf::fromAxisAngle(nc::Vector3f::YAxis, 45));

	nc::Matrix4x4f m1(nc::Matrix4x4f::rotationX(60));
	nc::Matrix4x4f m2(nc::Matrix4x4f::rotationY(45));

	printf("First matrix:\n");
	printMatrix4x4f(m1);

	printf("First quaternion as matrix:\n");
	printMatrix4x4f(q1.toMatrix4x4());

	printf("Second matrix:\n");
	printMatrix4x4f(m2);

	printf("Second quaternion as matrix:\n");
	printMatrix4x4f(q2.toMatrix4x4());

	printf("Transposed of the multiplication between the transposed matrices:\n");
	printMatrix4x4f((m1.transposed() * m2.transposed()).transposed());

	printf("Quaternion multiplication:\n");
	printMatrix4x4f((q1 * q2).toMatrix4x4());

	return EXIT_SUCCESS;
}
