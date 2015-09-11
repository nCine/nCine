#include <cstdlib> // for EXIT_SUCCESS
#include <cstdio>
#include "test_functions.h"

int main()
{
	nc::Matrix4x4f m1(nc::Matrix4x4f::rotationX(60));
	nc::Matrix4x4f m2(nc::Matrix4x4f::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 1.0f));

	printf("First matrix:\n");
	printMatrix4x4f(m1);

	printf("Second matrix:\n");
	printMatrix4x4f(m2);

	printf("Matrix multiplication:\n");
	printMatrix4x4f(m1 * m2);

	printf("Multiplication between the transposed matrices:\n");
	nc::Matrix4x4f m1m2(m1.transposed() * m2.transposed());
	printMatrix4x4f(m1m2);

	printf("Transposing the multiplication matrix in-place:\n");
	m1m2.transpose();
	printMatrix4x4f(m1m2);

	printf("Inverse of the first matrix:\n");
	nc::Matrix4x4f inv1(m1.inverse());
	printMatrix4x4f(inv1);

	return EXIT_SUCCESS;
}
