cmake_minimum_required(VERSION 2.8.11)
project(nCine-tests)

if(NOT NCINE_DYNAMIC_LIBRARY)
	add_definitions(-DNCINE_STATIC)
endif()

add_custom_target(copy_shaders_tests ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/shaders ${CMAKE_BINARY_DIR}/tests/shaders
	COMMENT "Copying shaders to tests..."
)
if(WIN32)
	add_custom_target(copy_dlls_tests ALL
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${BINDIR} ${CMAKE_BINARY_DIR}/tests
		COMMENT "Copying DLLs to tests..."
	)
endif()

if(EXISTS ${NCINE_DATADIR} AND IS_DIRECTORY ${NCINE_DATADIR})
	if(UNIX)
		add_custom_target(link_data_tests ALL
			COMMAND ${CMAKE_COMMAND} -E create_symlink ${NCINE_DATADIR}/fonts ${CMAKE_BINARY_DIR}/tests/fonts
			COMMAND ${CMAKE_COMMAND} -E create_symlink ${NCINE_DATADIR}/sounds ${CMAKE_BINARY_DIR}/tests/sounds
			COMMAND ${CMAKE_COMMAND} -E create_symlink ${NCINE_DATADIR}/textures ${CMAKE_BINARY_DIR}/tests/textures
			COMMENT "Symlinking data to tests..."
		)
	elseif(WIN32)
		add_custom_target(copy_data_tests ALL
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${NCINE_DATADIR}/fonts ${CMAKE_BINARY_DIR}/tests/fonts
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${NCINE_DATADIR}/sounds ${CMAKE_BINARY_DIR}/tests/sounds
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${NCINE_DATADIR}/textures ${CMAKE_BINARY_DIR}/tests/textures
			COMMENT "Copying data to tests..."
		)
	endif()
endif()

add_executable(apptest_mipmapping main.cpp apptest_mipmapping.cpp)
target_link_libraries(apptest_mipmapping ncine)
add_executable(apptest_joystick main.cpp apptest_joystick.cpp)
target_link_libraries(apptest_joystick ncine)
add_executable(apptest_overdraw main.cpp apptest_overdraw.cpp)
target_link_libraries(apptest_overdraw ncine)
add_executable(apptest_rotozoom main.cpp apptest_rotozoom.cpp)
target_link_libraries(apptest_rotozoom ncine)
if(OPENAL_FOUND)
	add_executable(apptest_animsprites main.cpp apptest_animsprites.cpp)
	target_link_libraries(apptest_animsprites ncine)
endif()
add_executable(apptest_particles main.cpp apptest_particles.cpp)
target_link_libraries(apptest_particles ncine)
add_executable(apptest_scene main.cpp apptest_scene.cpp)
target_link_libraries(apptest_scene ncine)

add_executable(test_array test_array.cpp)
target_link_libraries(test_array ncine)
add_executable(test_staticarray test_staticarray.cpp)
target_link_libraries(test_staticarray ncine)
add_executable(test_list test_list.cpp)
target_link_libraries(test_list ncine)
add_executable(test_string test_string.cpp)
target_link_libraries(test_string ncine)
add_executable(test_hashmap test_hashmap.cpp)
target_link_libraries(test_hashmap ncine)
add_executable(test2_hashmap test2_hashmap.cpp)
target_link_libraries(test2_hashmap ncine)

add_executable(test_vector2 test_vector2.cpp)
target_link_libraries(test_vector2 ncine)
add_executable(test_vector3 test_vector3.cpp)
target_link_libraries(test_vector3 ncine)
add_executable(test_vector4 test_vector4.cpp)
target_link_libraries(test_vector4 ncine)
add_executable(test_matrix4x4 test_matrix4x4.cpp)
target_link_libraries(test_matrix4x4 ncine)
add_executable(test2_matrix4x4 test2_matrix4x4.cpp)
target_link_libraries(test2_matrix4x4 ncine)
add_executable(test_quaternion test_quaternion.cpp)
target_link_libraries(test_quaternion ncine)
add_executable(test2_quaternion test2_quaternion.cpp)
target_link_libraries(test2_quaternion ncine)
