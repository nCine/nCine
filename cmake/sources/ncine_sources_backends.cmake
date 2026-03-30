if(GLFW_FOUND AND NCINE_PREFERRED_BACKEND STREQUAL "GLFW")
	target_compile_definitions(ncine PRIVATE "WITH_GLFW")
	target_link_libraries(ncine PRIVATE GLFW::GLFW)

	list(APPEND PRIVATE_HEADERS
		${NCINE_ROOT}/src/include/GlfwInputManager.h
		${NCINE_ROOT}/src/include/GlfwGfxDevice.h
	)
	list(APPEND SOURCES
		${NCINE_ROOT}/src/input/GlfwInputManager.cpp
		${NCINE_ROOT}/src/input/GlfwKeys.cpp
		${NCINE_ROOT}/src/graphics/GlfwGfxDevice.cpp
	)
elseif(SDL2_FOUND AND NCINE_PREFERRED_BACKEND STREQUAL "SDL2")
	target_compile_definitions(ncine PRIVATE "WITH_SDL")
	target_link_libraries(ncine PRIVATE SDL2::SDL2)

	list(APPEND PRIVATE_HEADERS
		${NCINE_ROOT}/src/include/SdlInputManager.h
		${NCINE_ROOT}/src/include/SdlGfxDevice.h
	)
	list(APPEND SOURCES
		${NCINE_ROOT}/src/input/SdlInputManager.cpp
		${NCINE_ROOT}/src/input/SdlKeys.cpp
		${NCINE_ROOT}/src/graphics/SdlGfxDevice.cpp
	)
elseif(Qt5_FOUND AND NCINE_PREFERRED_BACKEND STREQUAL "QT5")
	target_compile_definitions(ncine PRIVATE "WITH_QT5")
	target_compile_definitions(ncine_main PRIVATE "WITH_QT5")
	target_link_libraries(ncine PUBLIC Qt5::Widgets)
	if(Qt5Gamepad_FOUND)
		target_compile_definitions(ncine PRIVATE "WITH_QT5GAMEPAD")
		target_link_libraries(ncine PRIVATE Qt5::Gamepad)
	endif()

	list(APPEND HEADERS	${NCINE_ROOT}/include/ncine/Qt5Widget.h)
	qt5_wrap_cpp(MOC_SOURCES ${NCINE_ROOT}/include/ncine/Qt5Widget.h)

	list(APPEND PRIVATE_HEADERS
		${NCINE_ROOT}/src/include/Qt5InputManager.h
		${NCINE_ROOT}/src/include/Qt5GfxDevice.h
	)
	list(APPEND SOURCES
		${NCINE_ROOT}/src/Qt5Widget.cpp
		${NCINE_ROOT}/src/input/Qt5InputManager.cpp
		${NCINE_ROOT}/src/input/Qt5Keys.cpp
		${NCINE_ROOT}/src/graphics/Qt5GfxDevice.cpp
		${MOC_SOURCES}
	)

	list(REMOVE_ITEM SOURCES ${NCINE_ROOT}/src/input/JoyMapping.cpp)
	list(APPEND SOURCES ${NCINE_ROOT}/src/input/Qt5JoyMapping.cpp)
endif()
