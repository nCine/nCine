target_compile_features(ncine PUBLIC cxx_std_11)
set_target_properties(ncine PROPERTIES CXX_EXTENSIONS OFF)

target_compile_definitions(ncine PRIVATE "$<$<CONFIG:Debug>:NCINE_DEBUG>")

if(EMSCRIPTEN)
	set(EMSCRIPTEN_COMPILER_OPTIONS
		"SHELL:-s WASM=1"
		"SHELL:-s DISABLE_EXCEPTION_CATCHING=1"
		"SHELL:-s FORCE_FILESYSTEM=1"
		"SHELL:-s ALLOW_MEMORY_GROWTH=1"
		"SHELL:--bind")

	set(EMSCRIPTEN_COMPILER_OPTIONS_DEBUG
		"SHELL:-s ASSERTIONS=1"
		"SHELL:-s SAFE_HEAP=1"
		"SHELL:--profiling-funcs"
		"SHELL:-s DEMANGLE_SUPPORT=1")

	string(FIND ${CMAKE_CXX_COMPILER} "fastcomp" EMSCRIPTEN_FASTCOMP_POS)
	if(EMSCRIPTEN_FASTCOMP_POS GREATER -1)
		list(APPEND EMSCRIPTEN_COMPILER_OPTIONS "SHELL:-s BINARYEN_TRAP_MODE=clamp")
	else()
		list(APPEND EMSCRIPTEN_COMPILER_OPTIONS "SHELL:-mnontrapping-fptoint")
	endif()

	target_compile_options(ncine PUBLIC ${EMSCRIPTEN_COMPILER_OPTIONS})
	target_link_options(ncine PUBLIC ${EMSCRIPTEN_COMPILER_OPTIONS})
	target_compile_options(ncine PUBLIC "$<$<CONFIG:Debug>:${EMSCRIPTEN_COMPILER_OPTIONS_DEBUG}>")
	target_link_options(ncine PUBLIC "$<$<CONFIG:Debug>:${EMSCRIPTEN_COMPILER_OPTIONS_DEBUG}>")

	if(Threads_FOUND)
		target_link_libraries(ncine PUBLIC Threads::Threads)
	endif()

	if(OPENGL_FOUND)
		target_link_libraries(ncine PUBLIC OpenGL::GL)
	endif()

	if(GLFW_FOUND)
		target_link_libraries(ncine PUBLIC GLFW::GLFW)
	endif()

	if(SDL2_FOUND)
		target_link_libraries(ncine PUBLIC SDL2::SDL2)
	endif()

	if(PNG_FOUND)
		target_link_libraries(ncine PUBLIC PNG::PNG)
	endif()

	if(VORBIS_FOUND)
		target_link_libraries(ncine PUBLIC Vorbis::Vorbisfile)
	endif()
endif()

if(MSVC)
	# Build with Multiple Processes
	target_compile_options(ncine PRIVATE /MP)
	# Always use the non debug version of the runtime library
	target_compile_options(ncine PUBLIC /MD)
	# Extra optimizations in release
	target_compile_options(ncine PRIVATE $<$<CONFIG:Release>:/fp:fast /Ox /Qpar>)

	# Enabling Whole Program Optimization
	if(NCINE_LINKTIME_OPTIMIZATION)
		target_compile_options(ncine PRIVATE $<$<CONFIG:Release>:/GL>)
		target_link_options(ncine PRIVATE $<$<CONFIG:Release>:/LTCG>)
	endif()

	if(NCINE_AUTOVECTORIZATION_REPORT)
		target_compile_options(ncine PRIVATE $<$<CONFIG:Release>:/Qvec-report:2 /Qpar-report:2>)
	endif()

	target_compile_definitions(ncine PRIVATE "_CRT_SECURE_NO_DEPRECATE")
	# Suppress linker warning about templates
	target_compile_options(ncine PUBLIC "/wd4251")

	# Disabling incremental linking and manifest generation
	target_link_options(ncine PRIVATE $<$<CONFIG:Debug>:/MANIFEST:NO /INCREMENTAL:NO>)
	target_link_options(ncine PRIVATE $<$<CONFIG:RelWithDebInfo>:/MANIFEST:NO /INCREMENTAL:NO>)

	if(NCINE_WITH_TRACY)
		target_link_options(ncine PRIVATE $<$<CONFIG:Release>:/DEBUG>)
	endif()
else() # GCC and LLVM
	target_compile_options(ncine PRIVATE $<$<CONFIG:Release>:-ffast-math>)

	if(NCINE_DYNAMIC_LIBRARY)
		target_compile_options(ncine PRIVATE -fvisibility=hidden -fvisibility-inlines-hidden)
	endif()

	# Only in debug
	if(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND NOT (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8.0)) OR
	  (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang") AND NOT (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.0)) AND
	   NOT EMSCRIPTEN AND NCINE_ADDRESS_SANITIZER)
		# Add ASan options as public so that targets linking the library will also use them
		target_compile_options(ncine PUBLIC $<$<CONFIG:Debug>:-O1 -g -fsanitize=address -fsanitize-address-use-after-scope -fno-optimize-sibling-calls -fno-common -fno-omit-frame-pointer -rdynamic>)
		target_link_options(ncine PUBLIC $<$<CONFIG:Debug>:-fsanitize=address>)
	endif()

	# Only in debug
	if(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang") AND
	   NCINE_CODE_COVERAGE)
		# Add code coverage options as public so that targets linking the library will also use them
		target_compile_options(ncine PUBLIC $<$<CONFIG:Debug>:--coverage>)
		target_link_options(ncine PUBLIC $<$<CONFIG:Debug>:--coverage>)
	endif()

	if(NCINE_WITH_TRACY)
		target_compile_options(ncine PRIVATE $<$<CONFIG:Release>:-g>)
		if(MINGW OR MSYS)
			target_link_libraries(ncine PRIVATE ws2_32 dbghelp)
		elseif(NOT ANDROID AND NOT APPLE)
			target_link_libraries(ncine PRIVATE dl)
		endif()
	endif()

	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		target_compile_options(ncine PRIVATE -fdiagnostics-color=auto)
		target_compile_options(ncine PRIVATE -Wall -pedantic -Wextra -Wold-style-cast -Wno-long-long -Wno-unused-parameter -Wno-ignored-qualifiers -Wno-variadic-macros)

		if(NCINE_DYNAMIC_LIBRARY)
			target_link_options(ncine PRIVATE -Wl,--no-undefined)
		endif()

		target_compile_options(ncine PRIVATE $<$<CONFIG:Debug>:-fvar-tracking-assignments>)

		# Extra optimizations in release
		target_compile_options(ncine PRIVATE $<$<CONFIG:Release>:-Ofast -funsafe-loop-optimizations -ftree-loop-if-convert-stores>)

		if(NCINE_LINKTIME_OPTIMIZATION AND NOT (MINGW OR MSYS OR ANDROID))
			target_compile_options(ncine PRIVATE $<$<CONFIG:Release>:-flto=${NCINE_CORES}>)
			target_link_options(ncine PRIVATE $<$<CONFIG:Release>:-flto=${NCINE_CORES}>)
		endif()

		if(NCINE_AUTOVECTORIZATION_REPORT)
			target_compile_options(ncine PRIVATE $<$<CONFIG:Release>:-fopt-info-vec-optimized>)
		endif()

		# Enabling strong stack protector of GCC 4.9
		if(NCINE_GCC_HARDENING AND NOT (MINGW OR MSYS))
			target_compile_options(ncine PRIVATE $<$<CONFIG:Release>:-Wformat-security -fstack-protector-strong>)
			target_compile_definitions(ncine PRIVATE "$<$<CONFIG:Release>:_FORTIFY_SOURCE=2>")
			target_link_options(ncine PRIVATE $<$<CONFIG:Release>:-Wl,-z,relro -Wl,-z,now>)
		endif()
	elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
		target_compile_options(ncine PRIVATE -fcolor-diagnostics)
		target_compile_options(ncine PRIVATE -Wall -pedantic -Wextra -Wold-style-cast -Wno-gnu-zero-variadic-macro-arguments -Wno-unused-parameter -Wno-variadic-macros -Wno-c++11-long-long -Wno-missing-braces)

		if(NCINE_DYNAMIC_LIBRARY)
			target_link_options(ncine PRIVATE -Wl,-undefined,error)
		endif()

		# Extra optimizations in release
		if(NOT EMSCRIPTEN)
			target_compile_options(ncine PRIVATE $<$<CONFIG:Release>:-Ofast>)
		endif()

		# Enabling ThinLTO of Clang 4
		if(NCINE_LINKTIME_OPTIMIZATION)
			if(EMSCRIPTEN)
				target_compile_options(ncine PRIVATE $<$<CONFIG:Release>:--llvm-lto 1>)
				target_link_options(ncine PRIVATE $<$<CONFIG:Release>:--llvm-lto 1>)
			elseif(NOT (MINGW OR MSYS OR ANDROID))
				target_compile_options(ncine PRIVATE $<$<CONFIG:Release>:-flto=thin>)
				target_link_options(ncine PRIVATE $<$<CONFIG:Release>:-flto=thin>)
			endif()
		endif()

		if(NCINE_AUTOVECTORIZATION_REPORT)
			target_compile_options(ncine PRIVATE $<$<CONFIG:Release>:-Rpass=loop-vectorize -Rpass-analysis=loop-vectorize>)
		endif()
	endif()
endif()
