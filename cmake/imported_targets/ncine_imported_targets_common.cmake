if(EMSCRIPTEN)
	return()
elseif(APPLE)
	set(CMAKE_FRAMEWORK_PATH "${PARENT_SOURCE_DIR}/nCine-external")
	set(CMAKE_MACOSX_RPATH ON)

	if(NOT IS_DIRECTORY ${CMAKE_FRAMEWORK_PATH})
		message(FATAL_ERROR "nCine external OS X frameworks directory not found at: ${CMAKE_FRAMEWORK_PATH}")
	else()
		message(STATUS "nCine external OS X frameworks directory: ${CMAKE_FRAMEWORK_PATH}")
	endif()
endif()

if(NCINE_WITH_CRASHPAD)
	find_package(Crashpad)
endif()
if(NCINE_WITH_THREADS)
	find_package(Threads)
endif()

if(NOT ANDROID AND NCINE_GRAPHICS_API STREQUAL "OPENGL")
	find_package(OpenGL REQUIRED)
elseif(NCINE_GRAPHICS_API STREQUAL "VULKAN")
	set(VULKAN_SDK_DIR "" CACHE PATH "The path to the Vulkan SDK directory")
	if(NOT IS_DIRECTORY ${VULKAN_SDK_DIR})
		set(VULKAN_SDK_DIR $ENV{VULKAN_SDK})
	endif()

	if(NOT IS_DIRECTORY ${VULKAN_SDK_DIR})
		message(FATAL_ERROR "Vulkan SDK directory not found")
	endif()
	message(STATUS "Vulkan SDK directory: ${VULKAN_SDK_DIR}")

	list(PREPEND CMAKE_PREFIX_PATH "${VULKAN_SDK_DIR}")
	find_package(Vulkan REQUIRED)

	if(Vulkan_GLSLANG_VALIDATOR_EXECUTABLE)
		set(GLSLANG_VALIDATOR_EXECUTABLE ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE})
		get_filename_component(VULKAN_BIN_DIR ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} DIRECTORY)

		message(STATUS "glslangValidator: ${GLSLANG_VALIDATOR_EXECUTABLE}")

		# Looking for Slang
		find_program(SLANGC_EXECUTABLE
			NAMES slangc
			HINTS ${VULKAN_BIN_DIR}
			NO_DEFAULT_PATH)

		if(SLANGC_EXECUTABLE)
			message(STATUS "slangc: ${SLANGC_EXECUTABLE}")
		endif()
	else()
		message(FATAL_ERROR "No shader compiler found (slangc or glslangValidator)")
	endif()

	set(VULKAN_FOUND 1)

	# Looking for the Volk implementation code
	if(IS_DIRECTORY ${Vulkan_INCLUDE_DIR})
		find_file(VOLK_C_SOURCE
			NAMES volk.c
			PATHS ${Vulkan_INCLUDE_DIR}/volk)
	endif()
endif()

if(NOT MSVC AND NOT ANDROID)
	if(NCINE_GRAPHICS_API STREQUAL "OPENGL")
		if(WIN32)
			find_package(GLEW REQUIRED)
		else()
			if(NCINE_WITH_GLEW)
				find_package(GLEW)
			endif()
		endif()
	endif()
	if(NCINE_ARM_PROCESSOR)
		include(check_atomic)
		find_package(OpenGLES2)
	endif()
	# Look for both GLFW and SDL2 to make the fallback logic work
	find_package(GLFW)
	find_package(SDL2)
	if(NCINE_WITH_PNG)
		find_package(PNG)
	endif()
	if(NCINE_WITH_WEBP)
		find_package(WebP)
	endif()
	if(NCINE_WITH_AUDIO)
		find_package(OpenAL)
		if(NCINE_WITH_VORBIS)
			find_package(Vorbis)
		endif()
	endif()
	if(NCINE_WITH_LUA)
		# Older CMake versions do not support Lua 5.4 or newer if not required explicitly
		find_package(Lua 5.5)
		if(NOT LUA_FOUND)
			find_package(Lua)
		endif()
	endif()
endif()

if(NOT ANDROID AND NCINE_PREFERRED_BACKEND STREQUAL "QT5")
	find_package(Qt5 COMPONENTS Widgets REQUIRED)
	find_package(Qt5Gamepad)
endif()
