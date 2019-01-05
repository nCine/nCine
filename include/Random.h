#ifndef CLASS_NCINE_RANDOM
#define CLASS_NCINE_RANDOM

#include <cstdint>
#include "common_defines.h"

namespace ncine {

/// PCG32 random number generator
/*!
 * Based on the Apache License 2.0 code from pcg-random.org
 */
class DLL_PUBLIC Random
{
  public:
	/// Creates a new generator with default seeds
	Random();
	/// Creates a new generator with the specified seeds
	Random(uint64_t initState, uint64_t initSequence);

	/// Initializes the generator with the specified seeds
	void init(uint64_t initState, uint64_t initSequence);

	/// Generates a uniformly distributed `32-bit number
	uint32_t integer();
	/// Generates a uniformly distributed `32-bit number, r, where min <= r < max
	uint32_t integer(uint32_t min, uint32_t max);
	/// Generates a uniformly distributed float number, r, where 0 <= r < 1
	float real();
	/// Generates a uniformly distributed float number, r, where min <= r < max
	float real(float min, float max);

	/// Faster but less uniform version of `integer()`
	uint32_t fastInteger(uint32_t min, uint32_t max);
	/// Faster but less uniform version of `real()`
	float fastReal();
	/// Faster but less uniform version of `real()`
	float fastReal(float min, float max);

  private:
	uint64_t state_;
	uint64_t increment_;
};

// Meyers' Singleton
extern DLL_PUBLIC Random &random();

}

#endif
