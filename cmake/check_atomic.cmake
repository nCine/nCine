# Based on https://stackoverflow.com/q/69281559

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	include(CheckCXXSourceCompiles)
	include(CMakePushCheckState)

	set(ATOMIC32_TEST_CODE "
		#include <atomic>
		#include <stdint.h>

		int main()
		{
			std::atomic<int32_t> x;
			x.store(1);
			x--;
			return x.load();
		}")

	set(ATOMIC64_TEST_CODE "
		#include <atomic>
		#include <stdint.h>

		int main()
		{
			std::atomic<int64_t> x;
			x.store(1);
			x--;
			return x.load();
		}")

	# Attempt to compile the two atomic tests
	check_cxx_source_compiles("${ATOMIC32_TEST_CODE}" atomic32_test)
	check_cxx_source_compiles("${ATOMIC64_TEST_CODE}" atomic64_test)

	# If a test does not compile, look for the atomic library, link with it and try again
	if(NOT atomic32_test OR NOT atomic64_test)
		find_library(ATOMIC_LIBRARY
			NAMES atomic atomic.so.1 libatomic.so.1
			HINTS
				${ATOMIC_ROOT}
				$ENV{HOME}/local/lib64
				$ENV{HOME}/local/lib
				/usr/local/lib64
				/usr/local/lib
				/opt/local/lib64
				/opt/local/lib
				/usr/lib64
				/usr/lib
				/lib64
				/lib
				/usr/lib/arm-linux-gnueabihf
		)

		if(ATOMIC_LIBRARY)
			message(STATUS "Atomic library found at: ${ATOMIC_LIBRARY}")

			get_filename_component(ATOMIC_DIRECTORY ${ATOMIC_LIBRARY} DIRECTORY)

			# Preserving current state before setting CMAKE_REQUIRED_LIBRARIES
			cmake_push_check_state()

			set(CMAKE_REQUIRED_LIBRARIES "-L${ATOMIC_DIRECTORY}" "-latomic")
			check_cxx_source_compiles("${ATOMIC32_TEST_CODE}" atomic32_test_with_library)
			check_cxx_source_compiles("${ATOMIC64_TEST_CODE}" atomic64_test_with_library)

			cmake_pop_check_state()

			if(NOT atomic32_test_with_library)
				message(FATAL_ERROR "Can't compile the 32-bit atomic test after linking with the atomic library")
			endif()

			if(NOT atomic64_test_with_library)
				message(FATAL_ERROR "Can't compile the 64-bit atomic test after linking with the atomic library")
			endif()

			set(ATOMIC_FOUND 1)
		else()
			message(FATAL_ERROR "Can't find the library required to compile atomic code")
		endif()
	endif()
endif()
