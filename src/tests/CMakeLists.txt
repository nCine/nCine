cmake_minimum_required(VERSION 2.8.11)
project(nCine-private_tests)

# these tests can only be linked against the engine static library
add_definitions(-DNCINE_STATIC)

if(UNIX)
	add_custom_target(link_shaders_srctests ALL
		COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/src/shaders ${CMAKE_BINARY_DIR}/src/tests/shaders
		COMMENT "Symlinking shaders to src/tests..."
	)
	if(EXISTS ${NCINE_DATA_DIR} AND IS_DIRECTORY ${NCINE_DATA_DIR})
		add_custom_target(link_data_srctests ALL
			COMMAND ${CMAKE_COMMAND} -E create_symlink ${NCINE_DATA_DIR}/fonts ${CMAKE_BINARY_DIR}/src/tests/fonts
			COMMAND ${CMAKE_COMMAND} -E create_symlink ${NCINE_DATA_DIR}/sounds ${CMAKE_BINARY_DIR}/src/tests/sounds
			COMMAND ${CMAKE_COMMAND} -E create_symlink ${NCINE_DATA_DIR}/textures ${CMAKE_BINARY_DIR}/src/tests/textures
			COMMENT "Symlinking data to src/tests..."
		)
	endif()
elseif(WIN32)
	add_custom_target(copy_shaders_srctests ALL
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/shaders ${CMAKE_BINARY_DIR}/src/tests/shaders
		COMMENT "Copying shaders to src/tests..."
	)
	add_custom_target(copy_dlls_srctests ALL
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${BINDIR} ${CMAKE_BINARY_DIR}/src/tests
		COMMENT "Copying DLLs to src/tests..."
	)
	if(EXISTS ${NCINE_DATA_DIR} AND IS_DIRECTORY ${NCINE_DATA_DIR})
		add_custom_target(copy_data_srctests ALL
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${NCINE_DATA_DIR}/ ${CMAKE_BINARY_DIR}/src/tests
			COMMENT "Copying data to src/tests..."
		)
	endif()
endif()

#add_executable(test_radixsort test_radixsort.cpp)
#target_link_libraries(test_radixsort ncine)
#target_include_directories(test_radixsort PRIVATE ../include)
#install(TARGETS test_radixsort RUNTIME DESTINATION bin)

if(Threads_FOUND)
	add_executable(apptest_threads ../../tests/main.cpp apptest_threads.cpp)
	target_link_libraries(apptest_threads ncine)
	target_include_directories(apptest_threads PRIVATE ../include)
	add_executable(apptest_threadpool ../../tests/main.cpp apptest_threadpool.cpp)
	target_link_libraries(apptest_threadpool ncine)
	target_include_directories(apptest_threadpool PRIVATE ../include)
	install(TARGETS apptest_threads apptest_threadpool RUNTIME DESTINATION bin)
endif()

add_executable(glapptest_fbo_cube ../../tests/main.cpp glapptest_fbo_cube.cpp)
target_link_libraries(glapptest_fbo_cube ncine)
target_include_directories(glapptest_fbo_cube PRIVATE ../include)
install(TARGETS glapptest_fbo_cube RUNTIME DESTINATION bin)
