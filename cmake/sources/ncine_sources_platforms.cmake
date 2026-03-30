if(WIN32)
	list(APPEND HEADERS ${NCINE_ROOT}/include/ncine/common_windefines.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/FileLoggerWindows.cpp)
	if(NOT MINGW)
		list(APPEND SOURCES ${NCINE_ROOT}/src/base/WindowsAtomic.cpp)
	else()
		list(APPEND SOURCES ${NCINE_ROOT}/src/base/GccAtomic.cpp)
	endif()
elseif(APPLE)
	list(APPEND SOURCES ${NCINE_ROOT}/src/base/StdAtomic.cpp)
else()
	if(ATOMIC_FOUND)
		target_link_libraries(ncine PRIVATE Atomic::Atomic)
	endif()
	list(APPEND SOURCES ${NCINE_ROOT}/src/base/GccAtomic.cpp)
endif()

if(EMSCRIPTEN)
	list(APPEND HEADERS ${NCINE_ROOT}/include/ncine/EmscriptenLocalFile.h)
	list(APPEND SOURCES ${NCINE_ROOT}/src/EmscriptenLocalFile.cpp)
endif()

if(NCINE_BUILD_ANDROID)
	list(APPEND HEADERS
		${NCINE_ROOT}/include/ncine/AndroidApplication.h
		${NCINE_ROOT}/include/ncine/AssetFile.h
	)
endif()

if(ANDROID)
	list(APPEND PRIVATE_HEADERS
		${NCINE_ROOT}/src/include/AndroidInputManager.h
		${NCINE_ROOT}/src/include/AndroidJniHelper.h
		${NCINE_ROOT}/src/include/EglGfxDevice.h
		${NCINE_ROOT}/src/include/TextureLoaderPkm.h
	)

	list(APPEND SOURCES
		${NCINE_ROOT}/src/android/AndroidApplication.cpp
		${NCINE_ROOT}/src/android/AndroidInputManager.cpp
		${NCINE_ROOT}/src/android/AndroidJniHelper.cpp
		${NCINE_ROOT}/src/android/AndroidKeys.cpp
		${NCINE_ROOT}/src/android/AssetFile.cpp
		${NCINE_ROOT}/src/android/EglGfxDevice.cpp
		${NCINE_ROOT}/src/graphics/TextureLoaderPkm.cpp
	)
else()
	list(APPEND SOURCES ${NCINE_ROOT}/src/PCApplication.cpp)
endif()
