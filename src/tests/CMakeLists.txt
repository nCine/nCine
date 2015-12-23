cmake_minimum_required(VERSION 2.8.12)
project(nCine-srctests)

if(UNIX)
	if(NOT NCINE_EMBED_SHADERS)
		add_custom_target(link_shaders_srctests ALL
			COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/src/shaders ${CMAKE_BINARY_DIR}/src/tests/shaders
			COMMENT "Symlinking shaders to src/tests..."
		)
		set_target_properties(link_shaders_srctests PROPERTIES FOLDER "CustomSymlinkTargets")
	endif()
	if(EXISTS ${NCINE_DATA_DIR} AND IS_DIRECTORY ${NCINE_DATA_DIR})
		add_custom_target(link_data_srctests ALL
			COMMAND ${CMAKE_COMMAND} -E create_symlink ${NCINE_DATA_DIR}/fonts ${CMAKE_BINARY_DIR}/src/tests/fonts
			COMMAND ${CMAKE_COMMAND} -E create_symlink ${NCINE_DATA_DIR}/sounds ${CMAKE_BINARY_DIR}/src/tests/sounds
			COMMAND ${CMAKE_COMMAND} -E create_symlink ${NCINE_DATA_DIR}/textures ${CMAKE_BINARY_DIR}/src/tests/textures
			COMMENT "Symlinking data to src/tests..."
		)
		set_target_properties(link_data_srctests PROPERTIES FOLDER "CustomSymlinkTargets")
	endif()
elseif(WIN32)
	if(NOT NCINE_EMBED_SHADERS)
		add_custom_target(copy_shaders_srctests ALL
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/shaders ${CMAKE_BINARY_DIR}/src/tests/shaders
			COMMENT "Copying shaders to src/tests..."
		)
		set_target_properties(copy_shaders_srctests PROPERTIES FOLDER "CustomCopyTargets")
	endif()
	add_custom_target(copy_dlls_srctests ALL
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${BINDIR} ${CMAKE_BINARY_DIR}/src/tests
		COMMENT "Copying DLLs to src/tests..."
	)
	set_target_properties(copy_dlls_srctests PROPERTIES FOLDER "CustomCopyTargets")
	if(EXISTS ${NCINE_DATA_DIR} AND IS_DIRECTORY ${NCINE_DATA_DIR})
		add_custom_target(copy_data_srctests ALL
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${NCINE_DATA_DIR}/ ${CMAKE_BINARY_DIR}/src/tests
			COMMENT "Copying data to src/tests..."
		)
		set_target_properties(copy_data_srctests PROPERTIES FOLDER "CustomCopyTargets")
	endif()
endif()

#add_executable(test_radixsort test_radixsort.cpp)
#target_link_libraries(test_radixsort ncine)
#target_include_directories(test_radixsort PRIVATE ../include)
#set_target_properties(test_radixsort PROPERTIES FOLDER "SrcTests")
#install(TARGETS test_radixsort RUNTIME DESTINATION bin)

if(Threads_FOUND)
	add_executable(apptest_threads ../../tests/main.cpp apptest_threads.cpp)
	target_link_libraries(apptest_threads ncine)
	target_include_directories(apptest_threads PRIVATE ../include)
	add_executable(apptest_threadpool ../../tests/main.cpp apptest_threadpool.cpp)
	target_link_libraries(apptest_threadpool ncine)
	target_include_directories(apptest_threadpool PRIVATE ../include)
	set_target_properties(apptest_threads apptest_threadpool PROPERTIES FOLDER "SrcTests")
	install(TARGETS apptest_threads apptest_threadpool RUNTIME DESTINATION bin)
endif()

add_executable(glapptest_fbo_cube ../../tests/main.cpp glapptest_fbo_cube.cpp)
target_link_libraries(glapptest_fbo_cube ncine)
target_include_directories(glapptest_fbo_cube PRIVATE ../include)
set_target_properties(glapptest_fbo_cube PROPERTIES FOLDER "SrcTests")
install(TARGETS glapptest_fbo_cube RUNTIME DESTINATION bin)
