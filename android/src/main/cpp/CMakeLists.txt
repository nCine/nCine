cmake_minimum_required(VERSION 3.7)
project(nCine-Android-${ANDROID_ABI})

find_library(ANDROID_LIBRARY android)
find_library(EGL_LIBRARY EGL)
find_library(GLES3_LIBRARY GLESv3)
find_library(LOG_LIBRARY log)
find_library(OPENSLES_LIBRARY OpenSLES)
find_library(ZLIB_LIBRARY z)

find_package(Threads)

if(EXISTS ${EXTERNAL_ANDROID_DIR}/png/${ANDROID_ABI}/libpng16.a)
	add_library(PNG::PNG STATIC IMPORTED)
	set_target_properties(PNG::PNG PROPERTIES
		IMPORTED_LINK_INTERFACE_LANGUAGES "C"
		IMPORTED_LOCATION ${EXTERNAL_ANDROID_DIR}/png/${ANDROID_ABI}/libpng16.a
		INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_ANDROID_DIR}/png/include"
		INTERFACE_LINK_LIBRARIES ${ZLIB_LIBRARY})
	set(PNG_FOUND 1)
endif()

if(EXISTS ${EXTERNAL_ANDROID_DIR}/webp/${ANDROID_ABI}/libwebp.a)
	add_library(WebP::WebP STATIC IMPORTED)
	set_target_properties(WebP::WebP PROPERTIES
		IMPORTED_LOCATION ${EXTERNAL_ANDROID_DIR}/webp/${ANDROID_ABI}/libwebp.a
		INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_ANDROID_DIR}/webp/include")
	set(WEBP_FOUND 1)
endif()

if(EXISTS ${EXTERNAL_ANDROID_DIR}/openal/${ANDROID_ABI}/libopenal.so)
	add_library(OpenAL::AL SHARED IMPORTED)
	set_target_properties(OpenAL::AL PROPERTIES
		IMPORTED_LOCATION ${EXTERNAL_ANDROID_DIR}/openal/${ANDROID_ABI}/libopenal.so
		INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_ANDROID_DIR}/openal/include")
	set(OPENAL_FOUND 1)
endif()

if(EXISTS ${EXTERNAL_ANDROID_DIR}/vorbis/${ANDROID_ABI}/libvorbisfile.a AND
   EXISTS ${EXTERNAL_ANDROID_DIR}/vorbis/${ANDROID_ABI}/libvorbis.a AND
   EXISTS ${EXTERNAL_ANDROID_DIR}/ogg/${ANDROID_ABI}/libogg.a)
	add_library(Vorbis::Vorbisfile STATIC IMPORTED)
	set_target_properties(Vorbis::Vorbisfile PROPERTIES
		IMPORTED_LOCATION ${EXTERNAL_ANDROID_DIR}/vorbis/${ANDROID_ABI}/libvorbisfile.a
		INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_ANDROID_DIR}/vorbis/include;${EXTERNAL_ANDROID_DIR}/ogg/include"
		INTERFACE_LINK_LIBRARIES "${EXTERNAL_ANDROID_DIR}/vorbis/${ANDROID_ABI}/libvorbis.a;${EXTERNAL_ANDROID_DIR}/ogg/${ANDROID_ABI}/libogg.a")
	set(VORBIS_FOUND 1)
endif()

if(EXISTS ${EXTERNAL_ANDROID_DIR}/lua/${ANDROID_ABI}/liblua.a)
	add_library(Lua::Lua STATIC IMPORTED)
	set_target_properties(Lua::Lua PROPERTIES
		IMPORTED_LOCATION ${EXTERNAL_ANDROID_DIR}/lua/${ANDROID_ABI}/liblua.a
		INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_ANDROID_DIR}/lua/include")
	set(LUA_FOUND 1)
endif()

if(NCINE_DYNAMIC_LIBRARY)
	add_library(ncine SHARED)
else()
	add_library(ncine STATIC)
	target_compile_definitions(ncine PUBLIC "NCINE_STATIC")
endif()
add_library(ncine_main STATIC main.cpp)

target_link_libraries(ncine PRIVATE ${ANDROID_LIBRARY} ${EGL_LIBRARY} ${GLES3_LIBRARY} ${LOG_LIBRARY})
target_link_libraries(ncine_main PRIVATE ncine)

include(ncine_compiler_options)

include(ncine_headers)
include(ncine_private_headers)
include(ncine_sources)
include(ncine_extra_sources)

# Convert strings back to the original lists
string(REPLACE " " ";" GENERATED_SOURCES "${GENERATED_SOURCES}")
string(REPLACE " " ";" ANDROID_GENERATED_FLAGS "${ANDROID_GENERATED_FLAGS}")

target_sources(ncine PRIVATE ${SOURCES} ${PRIVATE_HEADERS} ${HEADERS} ${NCTL_HEADERS} ${SHADER_FILES} ${GENERATED_SOURCES})

target_include_directories(ncine PUBLIC ${NCINE_ROOT}/include PRIVATE ${NCINE_ROOT}/include/ncine PRIVATE ${NCINE_ROOT}/src/include)
target_include_directories(ncine_main PRIVATE ${NCINE_ROOT}/include)
if(IS_DIRECTORY ${GENERATED_INCLUDE_DIR})
	target_include_directories(ncine PRIVATE ${GENERATED_INCLUDE_DIR})
endif()

target_include_directories(ncine PRIVATE ${CMAKE_ANDROID_NDK}/sources/android/native_app_glue)
target_include_directories(ncine_main PRIVATE ${CMAKE_ANDROID_NDK}/sources/android/native_app_glue)
if(NCINE_WITH_IMGUI)
	target_include_directories(ncine PUBLIC ${IMGUI_SOURCE_DIR})
endif()
if(NCINE_WITH_TRACY)
	target_include_directories(ncine PUBLIC ${TRACY_SOURCE_DIR})
endif()

target_compile_definitions(ncine PRIVATE ${ANDROID_GENERATED_FLAGS})
target_link_libraries(ncine PRIVATE ${PRIVATE_LINK_LIBRARIES})

add_library(native_app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
target_link_libraries(native_app_glue PRIVATE android log)

add_library(game SHARED ${NCINE_ROOT}/tests/${NCINE_STARTUP_TEST}.cpp)
if(NCINE_WITH_TRACY AND NCINE_DYNAMIC_LIBRARY)
	target_sources(game PRIVATE ${TRACY_SOURCE_DIR}/TracyClientDLL.cpp)
endif()

target_link_libraries(game PRIVATE native_app_glue ncine_main ncine)
target_include_directories(game PRIVATE ${ANDROID_NDK}/sources/android/native_app_glue)
if(IS_DIRECTORY ${GENERATED_INCLUDE_DIR})
	target_include_directories(game PRIVATE ${GENERATED_INCLUDE_DIR}) # for ncine_config.h
endif()
if(NOT NCINE_DYNAMIC_LIBRARY)
	target_include_directories(game PRIVATE ${NCINE_ROOT}/src/include)
	target_compile_definitions(game PRIVATE ${ANDROID_GENERATED_FLAGS})
endif()
if(NCINE_WITH_TRACY)
	target_compile_definitions(game PRIVATE TRACY_ENABLE)
endif()

# Export ANativeActivity_onCreate() - https://github.com/android-ndk/ndk/issues/381
set_property(TARGET game APPEND_STRING PROPERTY LINK_FLAGS "-u ANativeActivity_onCreate")

include(ncine_strip_binaries)
