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
	nc::Matrix4x4f m1(nc::Vector4f(2.5f, 2.0f, 0.5f, 1.0), nc::Vector4f(2.5f, 2.0f, 0.5f, 1.0), nc::Vector4f(2.5f, 2.0f, 0.5f, 1.0), nc::Vector4f(2.5f, 2.0f, 0.5f, 1.0));
	nc::Matrix4x4f m2(nc::Vector4f(1.0f, 1.5f, 1.0f, 2.0), nc::Vector4f(1.0f, 1.5f, 1.0f, 2.0), nc::Vector4f(1.0f, 1.5f, 1.0f, 2.0), nc::Vector4f(1.0f, 1.5f, 1.0f, 2.0));

	printf("First matrix:\n");
	print_matrix4x4f(m1);
	printf("Second matrix:\n");
	print_matrix4x4f(m2);

	printf("Matrix addition:\n");
	print_matrix4x4f(m1 + m2);

	printf("Matrix difference:\n");
	print_matrix4x4f(m1 - m2);

	printf("Matrix multiplication:\n");
	print_matrix4x4f(m1 * m2);

	printf("Transposed of the multiplication between the transposed matrices:\n");
	print_matrix4x4f((m1.transposed() * m2.transposed()).transposed());

	printf("Matrix division:\n");
	print_matrix4x4f(m1 / m2);

	printf("Add the second matrix to the first:\n");
	print_matrix4x4f(m1 += m2);

	printf("Subtract the second matrix from the first:\n");
	print_matrix4x4f(m1 -= m2);

	printf("Multiply the first matrix by the second:\n");
	print_matrix4x4f(m1 *= m2);

	printf("Divide the first matrix by the second:\n");
	print_matrix4x4f(m1 /= m2);

	printf("Add all the constant matrices together:\n");
	nc::Matrix4x4f constants = nc::Matrix4x4f::Zero + nc::Matrix4x4f::Identity;
	print_matrix4x4f(constants);

	return EXIT_SUCCESS;
}
