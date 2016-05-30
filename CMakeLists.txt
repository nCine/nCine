cmake_minimum_required(VERSION 2.8.12)
project(nCine)

option(NCINE_BUILD_TESTS "Build the engine test programs" ON)
option(NCINE_INSTALL_DEV_SUPPORT "Install files to support development" ON)
option(NCINE_EXTRA_OPTIMIZATION "Compile the engine with extra optimizations in release mode" ON)
option(NCINE_AUTOVECTORIZATION_REPORT "Enable report generation from compiler auto-vectorization" OFF)
option(NCINE_DYNAMIC_LIBRARY "Compile the engine as a dynamic library" ON)
option(NCINE_BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" OFF)
option(NCINE_IMPLEMENTATION_DOCUMENTATION "Include implementation classes in the documentation" OFF)
option(NCINE_EMBED_SHADERS "Export shader files to C strings to be included in engine sources" ON)
option(NCINE_BUILD_ANDROID "Build the Android version of the engine" OFF)
set(NDK_DIR $ENV{ANDROID_NDK_HOME} CACHE PATH "Set the path to the Android NDK")
set(NCINE_DEFAULT_DATA_DIR "" CACHE STRING "Set the default data directory for applications")
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	option(NCINE_ADDRESS_SANITIZER "Enable the AddressSanitizer memory error detector of GCC and Clang" OFF)
endif()
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
	option(NCINE_GCC_HARDENING "Enable memory corruption mitigation methods of GCC" OFF)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
get_filename_component(PARENT_SOURCE_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)

set(NCINE_DATA_DIR "${PARENT_SOURCE_DIR}/nCine-data" CACHE PATH "Set the path to the engine data directory")
if(NOT IS_DIRECTORY ${NCINE_DATA_DIR})
	message(WARNING "nCine data directory not found at: ${NCINE_DATA_DIR}")
else()
	message(STATUS "nCine data directory: ${NCINE_DATA_DIR}")
	if(MSVC)
		set(DATA_INSTALL_DESTINATION data)
	else()
		set(DATA_INSTALL_DESTINATION share/ncine/data)
	endif()
endif()

include(ncine_options_presets)

include(ncine_get_version)
set(CPACK_PACKAGE_NAME "nCine")
set(CPACK_PACKAGE_VENDOR "")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A multi-platform 2d game engine")
set(CPACK_PACKAGE_DESCRIPTION "nCine is a 2d game engine for Linux, Windows, OS X and Android")
set(CPACK_PACKAGE_CONTACT "encelo@gmail.com")
set(CPACK_PACKAGE_VERSION ${NCINE_VERSION})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "nCine")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_NSIS_COMPRESSOR "/SOLID lzma")
set(CPACK_NSIS_MODIFY_PATH ON)
include(CPack)

include(ProcessorCount)
ProcessorCount(NCINE_CORES)
if(NCINE_CORES EQUAL 0)
	set(NCINE_CORES 1)
endif()

find_package(Threads)
if(MSVC)
	set(EXTERNAL_MSVC_DIR "${PARENT_SOURCE_DIR}/nCine-external" CACHE PATH "Set the path to the MSVC libraries directory")
	if(NOT IS_DIRECTORY ${EXTERNAL_MSVC_DIR})
		message(FATAL_ERROR "nCine external MSVC libraries directory not found at: ${EXTERNAL_MSVC_DIR}")
	else()
		message(STATUS "nCine external MSVC libraries directory: ${EXTERNAL_MSVC_DIR}")
	endif()

	if(MSVC_C_ARCHITECTURE_ID MATCHES 64 OR MSVC_CXX_ARCHITECTURE_ID MATCHES 64)
		set(LIBDIR "${EXTERNAL_MSVC_DIR}/lib/x64")
		set(BINDIR "${EXTERNAL_MSVC_DIR}/bin/x64")
	else()
		set(LIBDIR "${EXTERNAL_MSVC_DIR}/lib/x86")
		set(BINDIR "${EXTERNAL_MSVC_DIR}/bin/x86")
	endif()
	
	set(GLEW_LIBRARY ${LIBDIR}/glew32.lib)
	set(GLEW_FOUND 1)
	set(OPENGL_gl_LIBRARY opengl32.lib)
	set(OPENGL_FOUND 1)
	set(GLFW_LIBRARY ${LIBDIR}/glfw3dll.lib)
	set(GLFW_FOUND 1)
	set(PNG_LIBRARY ${LIBDIR}/libpng16.lib)
	set(PNG_FOUND 1)
	set(OPENAL_LIBRARY ${LIBDIR}/OpenAL32.lib)
	set(OPENAL_FOUND 1)
	set(WEBP_LIBRARY ${LIBDIR}/libwebp_dll.lib)
	set(WEBP_FOUND 1)
	set(VORBISFILE_LIBRARY ${LIBDIR}/libogg.lib ${LIBDIR}/libvorbis.lib ${LIBDIR}/libvorbisfile.lib)
	set(VORBIS_FOUND 1)
else() # GCC and LLVM
	if(APPLE)
		set(CMAKE_FRAMEWORK_PATH "${PARENT_SOURCE_DIR}/nCine-external")
		set(CMAKE_MACOSX_RPATH ${CMAKE_FRAMEWORK_PATH})
		if(NOT IS_DIRECTORY ${CMAKE_FRAMEWORK_PATH})
			message(FATAL_ERROR "nCine external OS X frameworks directory not found at: ${CMAKE_FRAMEWORK_PATH}")
		else()
			message(STATUS "nCine external OS X frameworks directory: ${CMAKE_FRAMEWORK_PATH}")
		endif()
	endif()

	if(WIN32)
		find_package(GLEW REQUIRED)
	endif()
	find_package(OpenGL REQUIRED)
	find_package(SDL)
	find_package(SDL_image)
	find_package(PNG)
	find_package(OpenAL)
	find_package(GLFW)
	find_package(WebP)
	find_package(Vorbis)
endif()

include(ncine_compiler_options)

include(ncine_headers)
include(ncine_private_headers)
include(ncine_sources)

# Forcing the use of SDL
#set(GLFW_FOUND 0)
if(MSVC AND NOT GLFW_FOUND)
	set(SDL_LIBRARY ${LIBDIR}/SDLmain.lib ${LIBDIR}/SDL.lib)
	set(SDL_FOUND 1)
	set(SDLIMAGE_LIBRARY ${LIBDIR}/SDL_image.lib)
	set(SDLIMAGE_FOUND 1)
endif()

if(NOT SDL_FOUND AND NOT GLFW_FOUND)
	message(FATAL_ERROR "Neither SDL nor GLFW have been found")
endif()

if(WIN32 AND GLEW_FOUND)
	list(APPEND PRIVATE_LINK_LIBRARIES ${GLEW_LIBRARY})
	list(APPEND PRIVATE_COMPILE_DEFINITIONS "WITH_GLEW")
endif()

if(GLFW_FOUND)
	list(APPEND PRIVATE_COMPILE_DEFINITIONS "WITH_GLFW" "GLFW_NO_GLU")
	list(APPEND PRIVATE_LINK_LIBRARIES ${GLFW_LIBRARY})

	list(APPEND PRIVATE_HEADERS src/include/GlfwInputManager.h)
	list(APPEND PRIVATE_HEADERS src/include/GlfwGfxDevice.h)
	list(APPEND SOURCES src/GlfwInputManager.cpp)
	list(APPEND SOURCES src/GlfwKeys.cpp)
	list(APPEND SOURCES src/graphics/GlfwGfxDevice.cpp)
endif()

if(SDL_FOUND AND NOT GLFW_FOUND)
	list(APPEND PRIVATE_COMPILE_DEFINITIONS "WITH_SDL")
	if(${CMAKE_SYSTEM_NAME} MATCHES Darwin) # SDLmain on OS X
		list(APPEND PRIVATE_LINK_LIBRARIES ${SDLMAIN_LIBRARY})
	endif()
	list(APPEND PRIVATE_LINK_LIBRARIES ${SDL_LIBRARY})
	if(SDLIMAGE_FOUND)
		list(APPEND PRIVATE_COMPILE_DEFINITIONS "WITH_SDLIMAGE")
		list(APPEND PRIVATE_LINK_LIBRARIES ${SDLIMAGE_LIBRARY})
	endif()

	list(APPEND PRIVATE_HEADERS src/include/SdlInputManager.h)
	list(APPEND PRIVATE_HEADERS src/include/SdlGfxDevice.h)
	list(APPEND SOURCES src/SdlInputManager.cpp)
	list(APPEND SOURCES src/SdlKeys.cpp)
	list(APPEND SOURCES src/graphics/SdlGfxDevice.cpp)
	if(SDLIMAGE_FOUND)
		list(APPEND PRIVATE_HEADERS src/include/TextureLoaderSdl.h)
		list(APPEND SOURCES src/graphics/TextureLoaderSdl.cpp)
	endif()
endif()

if(OPENAL_FOUND)
	list(APPEND PRIVATE_COMPILE_DEFINITIONS "WITH_AUDIO")
	list(APPEND PRIVATE_LINK_LIBRARIES ${OPENAL_LIBRARY})
	if(VORBIS_FOUND)
		list(APPEND PRIVATE_COMPILE_DEFINITIONS "WITH_VORBIS")
		list(APPEND PRIVATE_LINK_LIBRARIES ${VORBISFILE_LIBRARY})
	endif()

	list(APPEND HEADERS
		include/IAudioDevice.h
		include/IAudioLoader.h
		include/AudioBuffer.h
		include/AudioStream.h
		include/IAudioPlayer.h
		include/AudioBufferPlayer.h
		include/AudioStreamPlayer.h
	)

	list(APPEND PRIVATE_HEADERS
		src/include/ALAudioDevice.h
		src/include/AudioLoaderWav.h
	)

	list(APPEND SOURCES
		src/audio/ALAudioDevice.cpp
		src/audio/IAudioLoader.cpp
		src/audio/AudioLoaderWav.cpp
		src/audio/AudioBuffer.cpp
		src/audio/AudioStream.cpp
		src/audio/IAudioPlayer.cpp
		src/audio/AudioBufferPlayer.cpp
		src/audio/AudioStreamPlayer.cpp
	)

	if(VORBIS_FOUND)
		list(APPEND PRIVATE_HEADERS src/include/AudioLoaderOgg.h)
		list(APPEND SOURCES src/audio/AudioLoaderOgg.cpp)
	endif()
endif()

if(PNG_FOUND AND GLFW_FOUND)
	list(APPEND PRIVATE_COMPILE_DEFINITIONS "WITH_PNG")
	list(APPEND PRIVATE_LINK_LIBRARIES ${PNG_LIBRARY})

	list(APPEND PRIVATE_HEADERS src/include/TextureLoaderPng.h)
	list(APPEND SOURCES src/graphics/TextureLoaderPng.cpp)
endif()
if(WEBP_FOUND)
	list(APPEND PRIVATE_COMPILE_DEFINITIONS "WITH_WEBP")
	list(APPEND PRIVATE_LINK_LIBRARIES ${WEBP_LIBRARY})

	list(APPEND PRIVATE_HEADERS src/include/TextureLoaderWebP.h)
	list(APPEND SOURCES src/graphics/TextureLoaderWebP.cpp)
endif()

if(Threads_FOUND)
	list(APPEND PRIVATE_COMPILE_DEFINITIONS "WITH_THREADS")
	list(APPEND PRIVATE_LINK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
	# Experimental multi-threading scene update
	#list(APPEND PRIVATE_COMPILE_DEFINITIONS "WITH_MULTITHREADING")

	list(APPEND HEADERS include/IThreadPool.h)
	list(APPEND HEADERS include/IThreadCommand.h)
	list(APPEND PRIVATE_HEADERS src/include/Thread.h)
	list(APPEND PRIVATE_HEADERS src/include/ThreadSync.h)

	if(WIN32)
		list(APPEND SOURCES src/threading/WindowsThread.cpp)
		list(APPEND SOURCES src/threading/WindowsThreadSync.cpp)
	else()
		list(APPEND SOURCES src/threading/PosixThread.cpp)
		list(APPEND SOURCES src/threading/PosixThreadSync.cpp)
	endif()

	list(APPEND PRIVATE_HEADERS src/include/ThreadPool.h)
	list(APPEND SOURCES src/threading/ThreadPool.cpp)
	list(APPEND PRIVATE_HEADERS src/include/ThreadCommands.h)
endif()

include(ncine_generated_sources)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
foreach(SOURCE_FILE IN LISTS SOURCES HEADERS PRIVATE_HEADERS SHADER_FILES)
	get_filename_component(SOURCE_FILE_GROUP ${SOURCE_FILE} DIRECTORY)
	string(REPLACE / \\ SOURCE_FILE_GROUP ${SOURCE_FILE_GROUP})
	source_group("${SOURCE_FILE_GROUP}" FILES ${SOURCE_FILE})
endforeach()
foreach(SOURCE_FILE ${GENERATED_SOURCES})
	source_group("generated" FILES ${SOURCE_FILE})
endforeach()

if(NCINE_DYNAMIC_LIBRARY)
	add_library(ncine SHARED ${SOURCES} ${PRIVATE_HEADERS} ${HEADERS} ${SHADER_FILES} ${GENERATED_SOURCES})
else()
	add_library(ncine STATIC ${SOURCES} ${PRIVATE_HEADERS} ${HEADERS} ${SHADER_FILES} ${GENERATED_SOURCES})
	target_compile_definitions(ncine INTERFACE "NCINE_STATIC")
endif()

if(MSVC)
	target_include_directories(ncine PRIVATE "${EXTERNAL_MSVC_DIR}/include")
	# Suppress linker warning about templates
	target_compile_options(ncine PUBLIC "/wd4251")
else()
	if(PNG_FOUND AND GLFW_FOUND)
		target_include_directories(ncine PRIVATE ${PNG_INCLUDE_DIR})
	endif()
endif()
target_include_directories(ncine PUBLIC include PRIVATE src/include)
if(IS_DIRECTORY ${GENERATED_INCLUDE_DIR})
	target_include_directories(ncine PRIVATE ${GENERATED_INCLUDE_DIR})
endif()

if(NCINE_DEFAULT_DATA_DIR)
	string(LENGTH ${NCINE_DEFAULT_DATA_DIR} NCINE_DEFAULT_DATA_DIR_LENGTH)
	math(EXPR NCINE_DEFAULT_DATA_DIR_LENGTH "${NCINE_DEFAULT_DATA_DIR_LENGTH}-1")
	string(SUBSTRING ${NCINE_DEFAULT_DATA_DIR} ${NCINE_DEFAULT_DATA_DIR_LENGTH} 1 NCINE_DEFAULT_DATA_DIR_LAST_CHAR)
	if(NOT(NCINE_DEFAULT_DATA_DIR_LAST_CHAR STREQUAL "/"))
		string(CONCAT NCINE_DEFAULT_DATA_DIR ${NCINE_DEFAULT_DATA_DIR} "/")
	endif()

	message(STATUS "Default data directory: ${NCINE_DEFAULT_DATA_DIR}")
	list(APPEND PRIVATE_COMPILE_DEFINITIONS "NCINE_DEFAULT_DATA_DIR=\"${NCINE_DEFAULT_DATA_DIR}\"")
endif()

target_compile_definitions(ncine PRIVATE ${PRIVATE_COMPILE_DEFINITIONS})
target_link_libraries(ncine PRIVATE ${OPENGL_gl_LIBRARY} ${PRIVATE_LINK_LIBRARIES})

if(WIN32)
	if(NCINE_INSTALL_DEV_SUPPORT)
		install(TARGETS ncine RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
	else()
		install(TARGETS ncine RUNTIME DESTINATION bin)
	endif()
else()
	install(TARGETS ncine DESTINATION lib)
endif()

if(MSVC)
	set(MAIN_CPP_INSTALL_DESTINATION src)
	set(SHADERS_INSTALL_DESTINATION data/shaders)

	install(DIRECTORY ${BINDIR}/ DESTINATION bin)
else()
	set(MAIN_CPP_INSTALL_DESTINATION share/ncine)
	set(SHADERS_INSTALL_DESTINATION share/ncine/shaders)
endif()
if(NCINE_INSTALL_DEV_SUPPORT)
	install(DIRECTORY include/ DESTINATION include/ncine FILES_MATCHING PATTERN "*.h")
	install(FILES tests/main.cpp DESTINATION ${MAIN_CPP_INSTALL_DESTINATION})
endif()
if(NOT NCINE_EMBED_SHADERS)
	install(DIRECTORY src/shaders/ DESTINATION ${SHADERS_INSTALL_DESTINATION})
endif()

if(MSVC)
	set(NCINE_CONFIG_PATH "cmake")
else()
	set(NCINE_CONFIG_PATH "lib/cmake/nCine/")
endif()

include(CMakePackageConfigHelpers)
configure_package_config_file(
	"${CMAKE_SOURCE_DIR}/cmake/nCineConfig.cmake.in"
	"${CMAKE_BINARY_DIR}/nCineConfig.cmake"
	INSTALL_DESTINATION "${NCINE_CONFIG_PATH}"
	PATH_VARS CMAKE_INSTALL_PREFIX
	NO_CHECK_REQUIRED_COMPONENTS_MACRO)
if(NCINE_INSTALL_DEV_SUPPORT)
	install(FILES ${CMAKE_BINARY_DIR}/nCineConfig.cmake DESTINATION ${NCINE_CONFIG_PATH})
endif()

include(ncine_build_documentation)
include(ncine_build_tests)
include(ncine_build_android)
