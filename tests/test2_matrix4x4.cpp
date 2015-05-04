#include <cstdlib> // for EXIT_SUCCESS
#include <cstdio>
#include "Matrix4x4.h"

namespace nc = ncine;

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
	nc::Matrix4x4f m1(nc::Matrix4x4f::rotationX(60));
	nc::Matrix4x4f m2(nc::Matrix4x4f::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 1.0f));

	printf("First matrix:\n");
	print_matrix4x4f(m1);

	printf("Second matrix:\n");
	print_matrix4x4f(m2);

	printf("Matrix multiplication:\n");
	print_matrix4x4f(m1 * m2);

	printf("Multiplication between the transposed matrices:\n");
	nc::Matrix4x4f m1m2(m1.transposed() * m2.transposed());
	print_matrix4x4f(m1m2);

	printf("Transposing the multiplication matrix in-place:\n");
	m1m2.transpose();
	print_matrix4x4f(m1m2);

	printf("Inverse of the first matrix:\n");
	nc::Matrix4x4f inv1(m1.inverse());
	print_matrix4x4f(inv1);

	return EXIT_SUCCESS;
}
