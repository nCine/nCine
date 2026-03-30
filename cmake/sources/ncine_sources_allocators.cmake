if(NCINE_WITH_ALLOCATORS)
	target_compile_definitions(ncine PRIVATE "WITH_ALLOCATORS")

	list(APPEND HEADERS
		${NCINE_ROOT}/include/nctl/AllocManager.h
		${NCINE_ROOT}/include/nctl/IAllocator.h
		${NCINE_ROOT}/include/nctl/MallocAllocator.h
		${NCINE_ROOT}/include/nctl/LinearAllocator.h
		${NCINE_ROOT}/include/nctl/StackAllocator.h
		${NCINE_ROOT}/include/nctl/PoolAllocator.h
		${NCINE_ROOT}/include/nctl/FreeListAllocator.h
		${NCINE_ROOT}/include/nctl/ProxyAllocator.h
	)

	list(APPEND SOURCES
		${NCINE_ROOT}/src/base/AllocManager.cpp
		${NCINE_ROOT}/src/base/IAllocator.cpp
		${NCINE_ROOT}/src/base/MallocAllocator.cpp
		${NCINE_ROOT}/src/base/LinearAllocator.cpp
		${NCINE_ROOT}/src/base/StackAllocator.cpp
		${NCINE_ROOT}/src/base/PoolAllocator.cpp
		${NCINE_ROOT}/src/base/FreeListAllocator.cpp
		${NCINE_ROOT}/src/base/ProxyAllocator.cpp
	)
endif()
