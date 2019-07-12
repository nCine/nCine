cmake_minimum_required(VERSION 3.1)
project(nCine-srctests)

if(MSVC)
	add_custom_target(copy_dlls_srctests ALL
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${MSVC_BINDIR} ${CMAKE_BINARY_DIR}/src/tests
		COMMENT "Copying DLLs to src/tests..."
	)
	set_target_properties(copy_dlls_srctests PROPERTIES FOLDER "CustomCopyTargets")
endif()

list(APPEND SRCAPPTESTS glapptest_fbo_cube)
if(Threads_FOUND)
	list(APPEND SRCAPPTESTS apptest_threads apptest_threadpool)
endif()

foreach(SRCAPPTEST ${SRCAPPTESTS})
	add_executable(${SRCAPPTEST} ${SRCAPPTEST}.cpp)
	target_link_libraries(${SRCAPPTEST} PRIVATE ncine_main ncine)
	target_include_directories(${SRCAPPTEST} PRIVATE ${CMAKE_SOURCE_DIR}/include/ncine ${CMAKE_SOURCE_DIR}/src/include)

	set_target_properties(${SRCAPPTEST} PROPERTIES FOLDER "SrcTests")
	install(TARGETS ${SRCAPPTEST} RUNTIME DESTINATION ${RUNTIME_INSTALL_DESTINATION} COMPONENT tests)

	if(NCINE_EMBED_SHADERS)
		target_compile_definitions(${SRCAPPTEST} PRIVATE "WITH_EMBEDDED_SHADERS")
		target_include_directories(${SRCAPPTEST} PRIVATE ${GENERATED_INCLUDE_DIR})
	endif()
	if(WIN32)
		target_compile_definitions(${SRCAPPTEST} PRIVATE "WITH_GLEW")
		if(MSVC)
			target_include_directories(${SRCAPPTEST} PRIVATE "${EXTERNAL_MSVC_DIR}/include")
		endif()
	endif()
endforeach()

include(ncine_strip_binaries)
