#include <cstdlib> // for EXIT_SUCCESS
#include <cstdio>
#include "test_functions.h"

int main()
{
	nc::Matrix4x4f m1(nc::Vector4f(2.5f, 2.0f, 0.5f, 1.0), nc::Vector4f(2.5f, 2.0f, 0.5f, 1.0), nc::Vector4f(2.5f, 2.0f, 0.5f, 1.0), nc::Vector4f(2.5f, 2.0f, 0.5f, 1.0));
	nc::Matrix4x4f m2(nc::Vector4f(1.0f, 1.5f, 1.0f, 2.0), nc::Vector4f(1.0f, 1.5f, 1.0f, 2.0), nc::Vector4f(1.0f, 1.5f, 1.0f, 2.0), nc::Vector4f(1.0f, 1.5f, 1.0f, 2.0));

	printf("First matrix:\n");
	printMatrix4x4f(m1);
	printf("Second matrix:\n");
	printMatrix4x4f(m2);

	printf("Matrix addition:\n");
	printMatrix4x4f(m1 + m2);

	printf("Matrix difference:\n");
	printMatrix4x4f(m1 - m2);

	printf("Matrix multiplication:\n");
	printMatrix4x4f(m1 * m2);

	printf("Transposed of the multiplication between the transposed matrices:\n");
	printMatrix4x4f((m1.transposed() * m2.transposed()).transposed());

	printf("Matrix division:\n");
	printMatrix4x4f(m1 / m2);

	printf("Add the second matrix to the first:\n");
	printMatrix4x4f(m1 += m2);

	printf("Subtract the second matrix from the first:\n");
	printMatrix4x4f(m1 -= m2);

	printf("Multiply the first matrix by the second:\n");
	printMatrix4x4f(m1 *= m2);

	printf("Divide the first matrix by the second:\n");
	printMatrix4x4f(m1 /= m2);

	printf("Add all the constant matrices together:\n");
	nc::Matrix4x4f constants = nc::Matrix4x4f::Zero + nc::Matrix4x4f::Identity;
	printMatrix4x4f(constants);

	return EXIT_SUCCESS;
}
