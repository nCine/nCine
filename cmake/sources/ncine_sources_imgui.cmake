if(NCINE_WITH_IMGUI)
	target_compile_definitions(ncine PRIVATE "WITH_IMGUI")

	# For external projects compiling using an nCine build directory
	set(IMGUI_INCLUDE_ONLY_DIR ${IMGUI_SOURCE_DIR}/include_only)
	file(COPY ${IMGUI_SOURCE_DIR}/imgui.h DESTINATION ${IMGUI_INCLUDE_ONLY_DIR}/ncine)
	file(COPY ${IMGUI_SOURCE_DIR}/imgui_internal.h DESTINATION ${IMGUI_INCLUDE_ONLY_DIR}/ncine)
	file(COPY ${IMGUI_SOURCE_DIR}/imconfig.h DESTINATION ${IMGUI_INCLUDE_ONLY_DIR}/ncine)

	list(APPEND HEADERS
		${IMGUI_INCLUDE_ONLY_DIR}/ncine/imgui.h
		${IMGUI_INCLUDE_ONLY_DIR}/ncine/imgui_internal.h
		${IMGUI_INCLUDE_ONLY_DIR}/ncine/imconfig.h
	)

	list(APPEND PRIVATE_HEADERS
		${NCINE_ROOT}/src/include/ImGuiDrawing.h
		${NCINE_ROOT}/src/include/ImGuiJoyMappedInput.h
	)

	list(APPEND SOURCES
		${IMGUI_SOURCE_DIR}/imstb_rectpack.h
		${IMGUI_SOURCE_DIR}/imstb_textedit.h
		${IMGUI_SOURCE_DIR}/imstb_truetype.h
		${IMGUI_SOURCE_DIR}/imgui.cpp
		${IMGUI_SOURCE_DIR}/imgui_demo.cpp
		${IMGUI_SOURCE_DIR}/imgui_draw.cpp
		${IMGUI_SOURCE_DIR}/imgui_tables.cpp
		${IMGUI_SOURCE_DIR}/imgui_widgets.cpp
		${NCINE_ROOT}/src/graphics/ImGuiDrawing.cpp
		${NCINE_ROOT}/src/input/ImGuiJoyMappedInput.cpp
	)

	if(GLFW_FOUND AND NCINE_PREFERRED_BACKEND STREQUAL "GLFW")
		list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/ImGuiGlfwInput.h)
		list(APPEND SOURCES ${NCINE_ROOT}/src/input/ImGuiGlfwInput.cpp)
	elseif(SDL2_FOUND AND NCINE_PREFERRED_BACKEND STREQUAL "SDL2")
		list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/ImGuiSdlInput.h)
		list(APPEND SOURCES ${NCINE_ROOT}/src/input/ImGuiSdlInput.cpp)
	elseif(Qt5_FOUND AND NCINE_PREFERRED_BACKEND STREQUAL "QT5")
		list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/ImGuiQt5Input.h)
		list(APPEND SOURCES ${NCINE_ROOT}/src/input/ImGuiQt5Input.cpp)
	elseif(ANDROID)
		list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/ImGuiAndroidInput.h)
		list(APPEND SOURCES ${NCINE_ROOT}/src/android/ImGuiAndroidInput.cpp)
	endif()

	list(APPEND PRIVATE_HEADERS ${NCINE_ROOT}/src/include/ImGuiDebugOverlay.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/graphics/ImGuiDebugOverlay.cpp)

	if(MINGW)
		target_link_libraries(ncine PRIVATE imm32 dwmapi)
	endif()
endif()
