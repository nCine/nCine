#ifndef NCINE_COMMON
#define NCINE_COMMON

#include <cstdlib> // for rand()

namespace ncine {

/// Swaps the content of two objects of the same type
template <class T>
inline void swap(T& a, T& b)
{
	T temp = a;
	a = b;
	b = temp;
}

/// Returns the minimum between two objects that support operator<
template <class T>
inline const T& min (const T& a, const T& b)
{
	return !(b < a) ? a : b;
}

/// Returns the maximum between two objects that support operator<
template<class T>
inline const T& max(const T& a, const T& b)
{
	return (a < b) ? b : a;
}

/// Returns a random float between x0 and x1
inline float randBetween(float x0, float x1)
{
	return x0 + (x1 - x0) * (rand() / float(RAND_MAX));
}

}

namespace nc = ncine;

#endif
