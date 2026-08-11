#include "tracy/Tracy.hpp"

#ifdef WITH_ALLOCATORS
	#include "allocators_config.h"
#endif

#ifndef OVERRIDE_NEW
constexpr int CallstackDepth = 5;

void *operator new(std::size_t count)
{
	auto ptr = malloc(count);
	TracyAllocS(ptr, count, CallstackDepth);
	return ptr;
}

void operator delete(void *ptr) noexcept
{
	TracyFreeS(ptr, CallstackDepth);
	free(ptr);
}

void operator delete(void *ptr, std::size_t size) noexcept
{
	TracyFreeS(ptr, CallstackDepth);
	free(ptr);
}

void *operator new[](std::size_t count)
{
	auto ptr = malloc(count);
	TracyAllocS(ptr, count, CallstackDepth);
	return ptr;
}

void operator delete[](void *ptr) noexcept
{
	TracyFreeS(ptr, CallstackDepth);
	free(ptr);
}

void operator delete[](void *ptr, std::size_t size) noexcept
{
	TracyFreeS(ptr, CallstackDepth);
	free(ptr);
}
#endif
