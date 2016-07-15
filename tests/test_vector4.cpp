#include <cstdlib> // for EXIT_SUCCESS
#include <cstdio>
#include "Vector4.h"

namespace nc = ncine;

namespace {

inline void printVector4f(const nc::Vector4f &vec)
{
	printf("<%.2f, %.2f, %.2f, %.2f>\n", vec.x, vec.y, vec.z, vec.w);
}

}

int main()
{
	nc::Vector4f v1(2.5f, 2.0f, 0.5f, 1.0);
	nc::Vector4f v2(1.0f, 1.5f, 1.0f, 2.0);

	printf("First vector: ");
	printVector4f(v1);
	printf("Second vector: ");
	printVector4f(v2);

	printf("Vector addition: ");
	printVector4f(v1 + v2);

	printf("Vector difference: ");
	printVector4f(v1 - v2);

	printf("Vector multiplication: ");
	printVector4f(v1 * v2);

	printf("Vector division: ");
	printVector4f(v1 / v2);

	printf("Dot product: ");
	printf("%.2f\n", nc::dot(v1, v2));

	printf("Add the second vector to the first: ");
	printVector4f(v1 += v2);

	printf("Subtract the second vector from the first: ");
	printVector4f(v1 -= v2);

	printf("Multiply the first vector by the second: ");
	printVector4f(v1 *= v2);

	printf("Divide the first vector by the second: ");
	printVector4f(v1 /= v2);

	printf("Normalize the first vector: ");
	printVector4f(v1.normalize());

	printf("Print the length of the first vector (should be normalized): ");
	printf("%.2f\n", v1.length());

	printf("Print the squared length of the second vector: ");
	printf("%.2f\n", v2.sqrLength());

	printf("Add all the constant vectors together: ");
	nc::Vector4f constants = nc::Vector4f::Zero + nc::Vector4f::XAxis + nc::Vector4f::YAxis + nc::Vector4f::ZAxis + nc::Vector4f::WAxis;
	printVector4f(constants);

	return EXIT_SUCCESS;
}
