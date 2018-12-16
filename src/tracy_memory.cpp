#include "Tracy.hpp"

void *operator new(std::size_t count)
{
	auto ptr = malloc(count);
	TracyAllocS(ptr , count, 5);
	return ptr;
}

void operator delete(void* ptr) noexcept
{
	TracyFreeS(ptr, 5);
	free(ptr);
}
