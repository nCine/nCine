#include <cstdlib> // for EXIT_SUCCESS
#include <cstdio>
#include "Quaternion.h"

namespace nc = ncine;

inline void print_quaternion(const nc::Quaternionf& quat)
{
	printf("<%.2f, %.2f, %.2f, %.2f>\n", quat.x, quat.y, quat.z, quat.w);
}

inline void print_matrix4x4f(const nc::Matrix4x4f& mat)
{
	printf("[");
	for (unsigned int i = 0; i < 4; i++)
	{
		if (i > 0)
		{
			printf(" ");
		}
		printf("%.2f,\t%.2f,\t%.2f,\t%.2f", mat[i].x, mat[i].y, mat[i].z, mat[i].w);
		if (i < 3)
		{
			printf(",\n");
		}
	}
	printf("]\n");
}

int main()
{
	nc::Quaternionf q1(nc::Quaternionf::fromAxisAngle(nc::Vector3f::XAxis, 60));
	nc::Quaternionf q2(nc::Quaternionf::fromAxisAngle(nc::Vector3f::YAxis, 45));

	nc::Matrix4x4f m1(nc::Matrix4x4f::rotationX(60));
	nc::Matrix4x4f m2(nc::Matrix4x4f::rotationY(45));

	printf("First matrix:\n");
	print_matrix4x4f(m1);

	printf("First quaternion as matrix:\n");
	print_matrix4x4f(q1.toMatrix4x4());

	printf("Second matrix:\n");
	print_matrix4x4f(m2);

	printf("Second quaternion as matrix:\n");
	print_matrix4x4f(q2.toMatrix4x4());

	printf("Transposed of the multiplication between the transposed matrices:\n");
	print_matrix4x4f((m1.transposed() * m2.transposed()).transposed());

	printf("Quaternion multiplication:\n");
	print_matrix4x4f((q1 * q2).toMatrix4x4());

	return EXIT_SUCCESS;
}
