option(NCPROJECT_BUILD_ANDROID "Build the Android version of the project" OFF)
option(NCPROJECT_STRIP_BINARIES "Enable symbols stripping from libraries and executables when in release" OFF)

set(NCPROJECT_DATA_DIR "${PARENT_SOURCE_DIR}/${NCPROJECT_NAME}-data" CACHE PATH "Set the path to the project data directory")
set(NCPROJECT_ICONS_DIR "${PARENT_SOURCE_DIR}/${NCPROJECT_NAME}-data/icons" CACHE PATH "Set the path to the project icons directory")

if(MSVC OR APPLE)
	set(NCINE_EXTERNAL_DIR "${PARENT_SOURCE_DIR}/nCine-external" CACHE PATH "Set the path to the nCine MSVC libraries or macOS frameworks directory")
elseif(EMSCRIPTEN)
	set(NCINE_EXTERNAL_DIR "${PARENT_SOURCE_DIR}/nCine-external-emscripten" CACHE PATH "Set the path to the nCine Emscripten libraries")
endif()

if(MSVC)
	option(NCPROJECT_INSTALL_SYSLIBS "Install required MSVC system libraries with CMake" ON)
endif()

if(NCPROJECT_BUILD_ANDROID)
	set(NDK_DIR "" CACHE PATH "Set the path to the Android NDK")
	set(NCPROJECT_NDK_ARCHITECTURES arm64-v8a CACHE STRING "Set the NDK target architectures")
	option(NCPROJECT_ASSEMBLE_APK "Assemble the Android APK of the project with Gradle" OFF)
	option(NCPROJECT_UNIVERSAL_APK "Configure the Gradle build script to assemble an universal APK for all ABIs" OFF)
endif()

# Project options presets
set(NCPROJECT_OPTIONS_PRESETS "Default" CACHE STRING "Presets for CMake options")
set_property(CACHE NCPROJECT_OPTIONS_PRESETS PROPERTY STRINGS Default BinDist)

if(NCPROJECT_OPTIONS_PRESETS STREQUAL BinDist)
	message(STATUS "Options presets: ${NCPROJECT_OPTIONS_PRESETS}")

	set(CMAKE_BUILD_TYPE Release)
	set(CMAKE_CONFIGURATION_TYPES Release)
	set(NCPROJECT_DATA_DIR_DIST ON)
	set(NCPROJECT_BUILD_ANDROID OFF)
	set(NCPROJECT_STRIP_BINARIES ON)

	if((MINGW OR MSYS) AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		# Not stripping binaries on MinGW when using Clang to avoid the "unexpected associative section index" error
		# https://github.com/llvm/llvm-project/issues/53433
		set(NCPROJECT_STRIP_BINARIES OFF)
	endif()
	if(EMSCRIPTEN)
		set(NCPROJECT_BUILD_ANDROID OFF)
		set(NCPROJECT_STRIP_BINARIES OFF)
	endif()
endif()
