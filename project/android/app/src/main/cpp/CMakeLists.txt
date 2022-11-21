cmake_minimum_required(VERSION 3.7)
project(${NCPROJECT_NAME}-Android-${ANDROID_ABI})

list(APPEND CMAKE_MODULE_PATH "${NCPROJECT_ROOT}/cmake")

include(${NCPROJECT_SOURCE_DIR}/CMakeLists.txt)
if(COMMAND callback_start)
	callback_start()
endif()

if(NCINE_DYNAMIC_LIBRARY)
	add_library(ncine SHARED IMPORTED)
	set_target_properties(ncine PROPERTIES
		IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/ncine/${ANDROID_ABI}/libncine.so"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/ncine/include")
else()
	add_library(ncine STATIC IMPORTED)
	set_target_properties(ncine PROPERTIES
		IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/ncine/${ANDROID_ABI}/libncine.a"
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/ncine/include")
	target_compile_definitions(ncine INTERFACE "NCINE_STATIC")
endif()

add_library(ncine_main STATIC IMPORTED)
set_target_properties(ncine_main PROPERTIES
	IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/ncine/${ANDROID_ABI}/libncine_main.a"
	INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/ncine/include")

if(COMMAND callback_before_target)
	callback_before_target()
endif()

foreach(SOURCE ${NCPROJECT_SOURCES})
	if(IS_ABSOLUTE ${SOURCE})
		list(APPEND ANDROID_PROJECT_SOURCES ${SOURCE})
	else()
		list(APPEND ANDROID_PROJECT_SOURCES ${NCPROJECT_SOURCE_DIR}/${SOURCE})
	endif()
endforeach()

foreach(INCLUDE_DIR ${NCPROJECT_INCLUDE_DIRS})
	if(IS_ABSOLUTE ${INCLUDE_DIR})
		list(APPEND ANDROID_PROJECT_INCLUDE_DIRS "${INCLUDE_DIR}")
	else()
		list(APPEND ANDROID_PROJECT_INCLUDE_DIRS "${NCPROJECT_SOURCE_DIR}/${INCLUDE_DIR}")
	endif()
endforeach()

add_library(jnicall_functions STATIC jnicall_functions.cpp)
add_library(native_app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
target_link_libraries(native_app_glue PRIVATE android log)
add_library(${NCPROJECT_EXE_NAME} SHARED ${ANDROID_PROJECT_SOURCES})
set_property(TARGET ${NCPROJECT_EXE_NAME} PROPERTY OUTPUT_NAME "game")
set(JNICALL_FUNCTIONS_LINKER_OPTIONS -Wl,--whole-archive jnicall_functions -Wl,--no-whole-archive)
target_link_libraries(${NCPROJECT_EXE_NAME} PRIVATE native_app_glue ${JNICALL_FUNCTIONS_LINKER_OPTIONS} ncine_main ncine)
target_include_directories(${NCPROJECT_EXE_NAME} PRIVATE ${ANDROID_NDK}/sources/android/native_app_glue)

target_compile_definitions(${NCPROJECT_EXE_NAME} PRIVATE "$<$<CONFIG:Debug>:${NCPROJECT_UPPER_NAME}_DEBUG>")
target_compile_definitions(${NCPROJECT_EXE_NAME} PRIVATE "$<$<CONFIG:Debug>:NCINE_ASSERT_BREAK>")

if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/ncine/include/tracy)
	target_include_directories(${NCPROJECT_EXE_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/ncine/include/tracy)
	target_compile_definitions(${NCPROJECT_EXE_NAME} PRIVATE "TRACY_ENABLE")
endif()

# Convert strings back to the original lists
string(REPLACE " " ";" GENERATED_SOURCES "${GENERATED_SOURCES}")
string(REPLACE " " ";" ANDROID_GENERATED_FLAGS "${ANDROID_GENERATED_FLAGS}")

target_include_directories(${NCPROJECT_EXE_NAME} PRIVATE ${ANDROID_PROJECT_INCLUDE_DIRS})
target_sources(${NCPROJECT_EXE_NAME} PRIVATE ${GENERATED_SOURCES})
if(IS_DIRECTORY ${GENERATED_INCLUDE_DIR})
	target_include_directories(${NCPROJECT_EXE_NAME} PRIVATE ${GENERATED_INCLUDE_DIR})
endif()
target_compile_definitions(${NCPROJECT_EXE_NAME} PRIVATE ${ANDROID_GENERATED_FLAGS})

if(COMMAND callback_after_target)
	callback_after_target()
endif()

if(NOT NCINE_DYNAMIC_LIBRARY)
	include(project_imported_targets)
	target_link_libraries(${NCPROJECT_EXE_NAME} PRIVATE
		${EGL_LIBRARY} ${GLES3_LIBRARY} ${OPENSLES_LIBRARY} ${ZLIB_LIBRARY})
	if(PNG_FOUND)
		target_link_libraries(${NCPROJECT_EXE_NAME} PRIVATE PNG::PNG)
	endif()
	if(WEBP_FOUND)
		target_link_libraries(${NCPROJECT_EXE_NAME} PRIVATE WebP::WebP)
	endif()
	if(OPENAL_FOUND)
		target_link_libraries(${NCPROJECT_EXE_NAME} PRIVATE OpenAL::AL)
	endif()
	if(VORBIS_FOUND)
		target_link_libraries(${NCPROJECT_EXE_NAME} PRIVATE Vorbis::Vorbisfile)
	endif()
	if(LUA_FOUND)
		target_link_libraries(${NCPROJECT_EXE_NAME} PRIVATE Lua::Lua)
	endif()
endif()

# Export ANativeActivity_onCreate() - https://github.com/android-ndk/ndk/issues/381
set_property(TARGET ${NCPROJECT_EXE_NAME} APPEND_STRING PROPERTY LINK_FLAGS "-u ANativeActivity_onCreate")

include(project_strip_binaries)
if(COMMAND callback_end)
	callback_end()
endif()
