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
	if(MSVC_IDE)
		add_custom_target(copy_shaders_srctests ALL
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/shaders ${CMAKE_BINARY_DIR}/src/tests/$<CONFIGURATION>/shaders
			COMMENT "Copying shaders to src/tests..."
		)
		add_custom_target(copy_dlls_srctests ALL
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${BINDIR} ${CMAKE_BINARY_DIR}/src/tests/$<CONFIGURATION>
			COMMENT "Copying DLLs to src/tests..."
		)
		if(EXISTS ${NCINE_DATA_DIR} AND IS_DIRECTORY ${NCINE_DATA_DIR})
			add_custom_target(copy_data_srctests ALL
				COMMAND ${CMAKE_COMMAND} -E copy_directory ${NCINE_DATA_DIR}/fonts ${CMAKE_BINARY_DIR}/src/tests/$<CONFIGURATION>/fonts
				COMMAND ${CMAKE_COMMAND} -E copy_directory ${NCINE_DATA_DIR}/sounds ${CMAKE_BINARY_DIR}/src/tests/$<CONFIGURATION>/sounds
				COMMAND ${CMAKE_COMMAND} -E copy_directory ${NCINE_DATA_DIR}/textures ${CMAKE_BINARY_DIR}/src/tests/$<CONFIGURATION>/textures
				COMMENT "Copying data to src/tests..."
			)
		endif()
	else()
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
				COMMAND ${CMAKE_COMMAND} -E copy_directory ${NCINE_DATA_DIR}/fonts ${CMAKE_BINARY_DIR}/src/tests/fonts
				COMMAND ${CMAKE_COMMAND} -E copy_directory ${NCINE_DATA_DIR}/sounds ${CMAKE_BINARY_DIR}/src/tests/sounds
				COMMAND ${CMAKE_COMMAND} -E copy_directory ${NCINE_DATA_DIR}/textures ${CMAKE_BINARY_DIR}/src/tests/textures
				COMMENT "Copying data to src/tests..."
			)
		endif()
	endif()
endif()

if(SDL_FOUND AND NOT GLFW_FOUND)
	if(OPENAL_FOUND)
		add_executable(sdltest_audio sdltest_audio.cpp)
		target_link_libraries(sdltest_audio ncine)
		target_include_directories(sdltest_audio PRIVATE ../include)
	endif()
	add_executable(sdltest_font sdltest_font.cpp)
	target_link_libraries(sdltest_font ncine)
	target_include_directories(sdltest_font PRIVATE ../include)
	add_executable(sdltest_particles sdltest_particles.cpp)
	target_link_libraries(sdltest_particles ncine)
	target_include_directories(sdltest_particles PRIVATE ../include)
	add_executable(sdltest_renderqueue sdltest_renderqueue.cpp)
	target_link_libraries(sdltest_renderqueue ncine)
	target_include_directories(sdltest_renderqueue PRIVATE ../include)
	add_executable(sdltest_scene sdltest_scene.cpp)
	target_link_libraries(sdltest_scene ncine)
	target_include_directories(sdltest_scene PRIVATE ../include)
endif()

#add_executable(test_radixsort test_radixsort.cpp)
#target_link_libraries(test_radixsort ncine)
#target_include_directories(test_radixsort PRIVATE ../include)

if(Threads_FOUND)
	add_executable(apptest_threads ../../tests/main.cpp apptest_threads.cpp)
	target_link_libraries(apptest_threads ncine)
	target_include_directories(apptest_threads PRIVATE ../include)
	add_executable(apptest_threadpool ../../tests/main.cpp apptest_threadpool.cpp)
	target_link_libraries(apptest_threadpool ncine)
	target_include_directories(apptest_threadpool PRIVATE ../include)
endif()

add_executable(glapptest_fbo_cube ../../tests/main.cpp glapptest_fbo_cube.cpp)
target_link_libraries(glapptest_fbo_cube ncine)
target_include_directories(glapptest_fbo_cube PRIVATE ../include)
