cmake_minimum_required(VERSION 3.7)
project(nCine-Android-${CMAKE_ANDROID_ARCH_ABI})

find_library(ANDROID_LIBRARY android)
find_library(EGL_LIBRARY EGL)
find_library(GLES3_LIBRARY GLESv3)
find_library(LOG_LIBRARY log)
find_library(OPENSLES_LIBRARY OpenSLES)
find_library(ZLIB_LIBRARY z)

list(APPEND PRIVATE_LINK_LIBRARIES
	${ANDROID_LIBRARY} ${EGL_LIBRARY} ${GLES3_LIBRARY} ${LOG_LIBRARY} ${ZLIB_LIBRARY})

set(Threads_FOUND 1)

if(EXISTS ${EXTERNAL_ANDROID_DIR}/png/${CMAKE_ANDROID_ARCH_ABI}/libpng16.a)
	set(PNG_INCLUDE_DIR ${EXTERNAL_ANDROID_DIR}/png/include)
	set(PNG_LIBRARY ${EXTERNAL_ANDROID_DIR}/png/${CMAKE_ANDROID_ARCH_ABI}/libpng16.a)
	set(PNG_FOUND 1)
endif()
if(EXISTS ${EXTERNAL_ANDROID_DIR}/webp/${CMAKE_ANDROID_ARCH_ABI}/libwebp.a)
	set(WEBP_INCLUDE_DIR ${EXTERNAL_ANDROID_DIR}/webp/include)
	set(WEBP_LIBRARY ${EXTERNAL_ANDROID_DIR}/webp/${CMAKE_ANDROID_ARCH_ABI}/libwebp.a)
	set(WEBP_FOUND 1)
endif()
if(EXISTS ${EXTERNAL_ANDROID_DIR}/openal/${CMAKE_ANDROID_ARCH_ABI}/libopenal.so)
	set(OPENAL_INCLUDE_DIR ${EXTERNAL_ANDROID_DIR}/openal/include)
	set(OPENAL_LIBRARY ${EXTERNAL_ANDROID_DIR}/openal/${CMAKE_ANDROID_ARCH_ABI}/libopenal.so)
	set(OPENAL_FOUND 1)
endif()
if(EXISTS ${EXTERNAL_ANDROID_DIR}/vorbis/${CMAKE_ANDROID_ARCH_ABI}/libvorbisfile.a AND
   EXISTS ${EXTERNAL_ANDROID_DIR}/vorbis/${CMAKE_ANDROID_ARCH_ABI}/libvorbis.a AND
   EXISTS ${EXTERNAL_ANDROID_DIR}/ogg/${CMAKE_ANDROID_ARCH_ABI}/libogg.a)
	set(OGG_INCLUDE_DIR ${EXTERNAL_ANDROID_DIR}/ogg/include)
	set(VORBIS_INCLUDE_DIR ${EXTERNAL_ANDROID_DIR}/vorbis/include)
	set(VORBISFILE_LIBRARY
		${EXTERNAL_ANDROID_DIR}/vorbis/${CMAKE_ANDROID_ARCH_ABI}/libvorbisfile.a
		${EXTERNAL_ANDROID_DIR}/vorbis/${CMAKE_ANDROID_ARCH_ABI}/libvorbis.a
		${EXTERNAL_ANDROID_DIR}/ogg/${CMAKE_ANDROID_ARCH_ABI}/libogg.a)
	set(VORBIS_FOUND 1)
endif()
if(EXISTS ${EXTERNAL_ANDROID_DIR}/lua/${CMAKE_ANDROID_ARCH_ABI}/liblua.a)
	set(LUA_INCLUDE_DIR ${EXTERNAL_ANDROID_DIR}/lua/include)
	set(LUA_LIBRARY ${EXTERNAL_ANDROID_DIR}/lua/${CMAKE_ANDROID_ARCH_ABI}/liblua.a)
	set(LUA_FOUND 1)
endif()

include(ncine_compiler_options)

include(ncine_headers)
include(ncine_private_headers)
include(ncine_sources)
include(ncine_extra_sources)

# Convert strings back to the original lists
string(REPLACE " " ";" GENERATED_SOURCES "${GENERATED_SOURCES}")
string(REPLACE " " ";" ANDROID_GENERATED_FLAGS "${ANDROID_GENERATED_FLAGS}")

if(NCINE_DYNAMIC_LIBRARY)
	add_library(ncine SHARED ${SOURCES} ${PRIVATE_HEADERS} ${HEADERS} ${SHADER_FILES} ${GENERATED_SOURCES})
else()
	add_library(ncine STATIC ${SOURCES} ${PRIVATE_HEADERS} ${HEADERS} ${SHADER_FILES} ${GENERATED_SOURCES})
	target_compile_definitions(ncine PUBLIC "NCINE_STATIC")
endif()

target_include_directories(ncine PUBLIC ${NCINE_ROOT}/include PRIVATE ${NCINE_ROOT}/src/include)
if(IS_DIRECTORY ${GENERATED_INCLUDE_DIR})
	target_include_directories(ncine PRIVATE ${GENERATED_INCLUDE_DIR})
endif()

target_include_directories(ncine PRIVATE ${CMAKE_ANDROID_NDK}/sources/android/native_app_glue)
if(PNG_FOUND)
	target_include_directories(ncine PRIVATE ${PNG_INCLUDE_DIR})
endif()
if(WEBP_FOUND)
	target_include_directories(ncine PRIVATE ${WEBP_INCLUDE_DIR})
endif()
if(OPENAL_FOUND)
	target_include_directories(ncine PRIVATE ${OPENAL_INCLUDE_DIR})
endif()
if(VORBIS_FOUND)
	target_include_directories(ncine PRIVATE ${OGG_INCLUDE_DIR})
	target_include_directories(ncine PRIVATE ${VORBIS_INCLUDE_DIR})
endif()
if(LUA_FOUND)
	target_include_directories(ncine PRIVATE ${LUA_INCLUDE_DIR})
endif()

list(APPEND PRIVATE_COMPILE_DEFINITIONS ${ANDROID_GENERATED_FLAGS})
target_compile_definitions(ncine PRIVATE ${PRIVATE_COMPILE_DEFINITIONS})
target_compile_definitions(ncine PRIVATE __ANDROID_API__=${CMAKE_SYSTEM_VERSION})
target_link_libraries(ncine PRIVATE ${PRIVATE_LINK_LIBRARIES})

add_library(native_app_glue STATIC ${CMAKE_ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
add_library(game SHARED main.cpp ${NCINE_ROOT}/tests/${NCINE_STARTUP_TEST}.cpp)
target_link_libraries(game PRIVATE ncine native_app_glue)
target_include_directories(game PRIVATE ${CMAKE_ANDROID_NDK}/sources/android/native_app_glue)
if(IS_DIRECTORY ${GENERATED_INCLUDE_DIR})
	target_include_directories(game PRIVATE ${GENERATED_INCLUDE_DIR}) # for ncine_config.h
endif()
if(NOT NCINE_DYNAMIC_LIBRARY)
	target_include_directories(game PRIVATE ${NCINE_ROOT}/src/include)
	target_compile_definitions(game PRIVATE ${PRIVATE_COMPILE_DEFINITIONS})
endif()

# Export ANativeActivity_onCreate() - https://github.com/android-ndk/ndk/issues/381
set_property(TARGET game APPEND_STRING PROPERTY LINK_FLAGS " -u ANativeActivity_onCreate")
