if(NCINE_WITH_NUKLEAR)
	target_compile_definitions(ncine PRIVATE "WITH_NUKLEAR")

	# For external projects compiling using an nCine build directory
	set(NUKLEAR_INCLUDE_ONLY_DIR ${NUKLEAR_SOURCE_DIR}/include_only)
	file(COPY ${NUKLEAR_SOURCE_DIR}/nuklear.h DESTINATION ${NUKLEAR_INCLUDE_ONLY_DIR}/ncine)

	list(APPEND HEADERS
		${NUKLEAR_INCLUDE_ONLY_DIR}/ncine/nuklear.h
		${NCINE_ROOT}/include/ncine/NuklearContext.h
	)

	list(APPEND PRIVATE_HEADERS
		${NCINE_ROOT}/src/include/NuklearDrawing.h
	)

	list(APPEND SOURCES
		${NCINE_ROOT}/src/NuklearContext.cpp
		${NCINE_ROOT}/src/graphics/NuklearDrawing.cpp
	)

	if(GLFW_FOUND AND NCINE_PREFERRED_BACKEND STREQUAL "GLFW")
		list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/NuklearGlfwInput.h)
		list(APPEND SOURCES ${NCINE_ROOT}/src/input/NuklearGlfwInput.cpp)
	elseif(SDL2_FOUND AND NCINE_PREFERRED_BACKEND STREQUAL "SDL2")
		list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/NuklearSdlInput.h)
		list(APPEND SOURCES ${NCINE_ROOT}/src/input/NuklearSdlInput.cpp)
	elseif(Qt5_FOUND AND NCINE_PREFERRED_BACKEND STREQUAL "QT5")
		list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/NuklearQt5Input.h)
		list(APPEND SOURCES ${NCINE_ROOT}/src/input/NuklearQt5Input.cpp)
	elseif(ANDROID)
		list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/NuklearAndroidInput.h)
		list(APPEND SOURCES ${NCINE_ROOT}/src/android/NuklearAndroidInput.cpp)
	endif()
endif()
