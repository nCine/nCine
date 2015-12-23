find_package(Doxygen)
if(NCINE_BUILD_DOCUMENTATION)
	if(NOT DOXYGEN_FOUND)
		message(FATAL_ERROR "Doxygen is needed to build the documentation")
	endif()

	set(DOXYGEN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/docs)
	set(DOXYFILE_IN ${CMAKE_SOURCE_DIR}/Doxyfile.in)
	set(DOXYFILE ${CMAKE_BINARY_DIR}/Doxyfile)

	configure_file(${DOXYFILE_IN} ${DOXYFILE} @ONLY)

	add_custom_target(documentation ALL DEPENDS ${DOXYGEN_OUTPUT_DIR})

	add_custom_command(OUTPUT ${DOXYGEN_OUTPUT_DIR}
		COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen"
	)

	if(WIN32)
		install(DIRECTORY ${DOXYGEN_OUTPUT_DIR}/html/ DESTINATION docs)
	else()
		install(DIRECTORY ${DOXYGEN_OUTPUT_DIR}/html/ DESTINATION share/doc/ncine)
	endif()
endif()