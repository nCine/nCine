cmake_minimum_required(VERSION 3.7)
project(nCine-Android-${ANDROID_ABI})

include(ncine_imported_targets)

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
	# Internal sources can access a generated header with or without the parent directory
	get_filename_component(PARENT_GENERATED_INCLUDE_DIR ${GENERATED_INCLUDE_DIR} DIRECTORY)
	target_include_directories(ncine PRIVATE ${PARENT_GENERATED_INCLUDE_DIR})
	target_include_directories(ncine_main PRIVATE ${PARENT_GENERATED_INCLUDE_DIR})
endif()

target_include_directories(ncine PRIVATE ${CMAKE_ANDROID_NDK}/sources/android/native_app_glue)
target_include_directories(ncine_main PRIVATE ${CMAKE_ANDROID_NDK}/sources/android/native_app_glue)
if(NCINE_WITH_IMGUI)
	target_include_directories(ncine
		INTERFACE $<BUILD_INTERFACE:${IMGUI_INCLUDE_ONLY_DIR}>
		PRIVATE $<BUILD_INTERFACE:${IMGUI_INCLUDE_ONLY_DIR}/ncine>)
endif()
if(NCINE_WITH_NUKLEAR)
	target_include_directories(ncine
		INTERFACE $<BUILD_INTERFACE:${NUKLEAR_INCLUDE_ONLY_DIR}>
		PRIVATE $<BUILD_INTERFACE:${NUKLEAR_INCLUDE_ONLY_DIR}/ncine>)
endif()
if(NCINE_WITH_TRACY)
	target_include_directories(ncine
		PUBLIC $<BUILD_INTERFACE:${TRACY_INCLUDE_ONLY_DIR}/tracy>
		PUBLIC $<INSTALL_INTERFACE:include/tracy>)
endif()

target_compile_definitions(ncine PRIVATE ${ANDROID_GENERATED_FLAGS})
target_link_libraries(ncine PRIVATE ${PRIVATE_LINK_LIBRARIES})

add_library(native_app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
target_link_libraries(native_app_glue PRIVATE android log)

add_library(game SHARED ${NCINE_ROOT}/tests/${NCINE_STARTUP_TEST}.cpp)
target_link_libraries(game PRIVATE native_app_glue ncine_main ncine)
target_include_directories(game PRIVATE ${ANDROID_NDK}/sources/android/native_app_glue)
if(IS_DIRECTORY ${GENERATED_INCLUDE_DIR})
	get_filename_component(PARENT_GENERATED_INCLUDE_DIR ${GENERATED_INCLUDE_DIR} DIRECTORY)
	target_include_directories(game PRIVATE ${PARENT_GENERATED_INCLUDE_DIR}) # for ncine_config.h
endif()
if(NOT NCINE_DYNAMIC_LIBRARY)
	target_include_directories(game PRIVATE ${NCINE_ROOT}/include/ncine)
	target_compile_definitions(game PRIVATE ${ANDROID_GENERATED_FLAGS})
endif()

# Export ANativeActivity_onCreate() - https://github.com/android-ndk/ndk/issues/381
set_property(TARGET game APPEND_STRING PROPERTY LINK_FLAGS "-u ANativeActivity_onCreate")

include(ncine_strip_binaries)
