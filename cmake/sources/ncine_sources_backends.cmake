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
elseif(SDL3_FOUND AND NCINE_PREFERRED_BACKEND STREQUAL "SDL3")
	target_compile_definitions(ncine PRIVATE "WITH_SDL3")
	target_link_libraries(ncine PRIVATE SDL3::SDL3)

	list(APPEND PRIVATE_HEADERS
		${NCINE_ROOT}/src/include/SdlInputManager.h
		${NCINE_ROOT}/src/include/SdlGfxDevice.h
	)
	list(APPEND SOURCES
		${NCINE_ROOT}/src/input/Sdl3InputManager.cpp
		${NCINE_ROOT}/src/input/Sdl3Keys.cpp
		${NCINE_ROOT}/src/graphics/Sdl3GfxDevice.cpp
	)
elseif(SDL2_FOUND AND NCINE_PREFERRED_BACKEND STREQUAL "SDL2")
	target_compile_definitions(ncine PRIVATE "WITH_SDL2")
	target_link_libraries(ncine PRIVATE SDL2::SDL2)

	list(APPEND PRIVATE_HEADERS
		${NCINE_ROOT}/src/include/SdlInputManager.h
		${NCINE_ROOT}/src/include/SdlGfxDevice.h
	)
	list(APPEND SOURCES
		${NCINE_ROOT}/src/input/Sdl2InputManager.cpp
		${NCINE_ROOT}/src/input/Sdl2Keys.cpp
		${NCINE_ROOT}/src/graphics/Sdl2GfxDevice.cpp
	)
elseif((Qt6_FOUND AND NCINE_PREFERRED_BACKEND STREQUAL "QT6") OR
		(Qt5_FOUND AND NCINE_PREFERRED_BACKEND STREQUAL "QT5"))

	if (NCINE_PREFERRED_BACKEND STREQUAL "QT6")
		target_compile_definitions(ncine PRIVATE "WITH_QT6")
		target_compile_definitions(ncine_main PRIVATE "WITH_QT6")
		target_link_libraries(ncine PUBLIC Qt6::Widgets Qt6::OpenGLWidgets)
		qt6_wrap_cpp(MOC_SOURCES ${NCINE_ROOT}/include/ncine/QtWidget.h)
	elseif (NCINE_PREFERRED_BACKEND STREQUAL "QT5")
		target_compile_definitions(ncine PRIVATE "WITH_QT5")
		target_compile_definitions(ncine_main PRIVATE "WITH_QT5")
		target_link_libraries(ncine PUBLIC Qt5::Widgets)
		qt5_wrap_cpp(MOC_SOURCES ${NCINE_ROOT}/include/ncine/QtWidget.h)
	endif()
	if(Qt5Gamepad_FOUND)
		target_compile_definitions(ncine PRIVATE "WITH_QT5GAMEPAD")
		target_link_libraries(ncine PRIVATE Qt5::Gamepad)
	endif()

	list(APPEND HEADERS	${NCINE_ROOT}/include/ncine/QtWidget.h)

	list(APPEND PRIVATE_HEADERS
		${NCINE_ROOT}/src/include/QtInputManager.h
		${NCINE_ROOT}/src/include/QtGfxDevice.h
	)
	list(APPEND SOURCES
		${NCINE_ROOT}/src/QtWidget.cpp
		${NCINE_ROOT}/src/input/QtInputManager.cpp
		${NCINE_ROOT}/src/input/QtKeys.cpp
		${NCINE_ROOT}/src/graphics/QtGfxDevice.cpp
		${MOC_SOURCES}
	)

	list(REMOVE_ITEM SOURCES ${NCINE_ROOT}/src/input/JoyMapping.cpp)
	list(APPEND SOURCES ${NCINE_ROOT}/src/input/QtJoyMapping.cpp)
endif()
