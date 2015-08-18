#include <cstdlib> // for EXIT_SUCCESS
#include <cstdio>
#include "Vector3.h"

namespace nc = ncine;

inline void print_vector3f(const nc::Vector3f& vec)
{
	printf("<%.2f, %.2f, %.2f>\n", vec.x, vec.y, vec.z);
}

int main()
{
	nc::Vector3f v1(2.5f, 2.0f, 0.5f);
	nc::Vector3f v2(1.0f, 1.5f, 1.0f);

	printf("First vector: ");
	print_vector3f(v1);
	printf("Second vector: ");
	print_vector3f(v2);

	printf("Vector addition: ");
	print_vector3f(v1 + v2);

	printf("Vector difference: ");
	print_vector3f(v1 - v2);

	printf("Vector multiplication: ");
	print_vector3f(v1 * v2);

	printf("Vector division: ");
	print_vector3f(v1 / v2);

	printf("Dot product: ");
	printf("%.2f\n", nc::dot(v1, v2));

	printf("Cross product: ");
	print_vector3f(nc::cross(v1, v2));

	printf("Add the second vector to the first: ");
	print_vector3f(v1 += v2);

	printf("Subtract the second vector from the first: ");
	print_vector3f(v1 -= v2);

	printf("Multiply the first vector by the second: ");
	print_vector3f(v1 *= v2);

	printf("Divide the first vector by the second: ");
	print_vector3f(v1 /= v2);

	printf("Normalize the first vector: ");
	print_vector3f(v1.normalize());

	printf("Print the length of the first vector (should be normalized): ");
	printf("%.2f\n", v1.length());

	printf("Print the squared length of the second vector: ");
	printf("%.2f\n", v2.sqrLength());

	printf("Add all the constant vectors together: ");
	nc::Vector3f constants = nc::Vector3f::Zero + nc::Vector3f::XAxis + nc::Vector3f::YAxis + nc::Vector3f::ZAxis;
	print_vector3f(constants);

	return EXIT_SUCCESS;
}
