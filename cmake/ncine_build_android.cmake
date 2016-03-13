if(NCINE_BUILD_ANDROID)
	set(EXTERNAL_ANDROID_DIR "${PARENT_SOURCE_DIR}/nCine-android-external" CACHE PATH "Set the path to the Android libraries directory")
	if(NOT IS_DIRECTORY ${EXTERNAL_ANDROID_DIR})
		message(FATAL_ERROR "nCine external Android libraries directory not found at: ${EXTERNAL_ANDROID_DIR}")
	else()
		message(STATUS "nCine external Android libraries directory: ${EXTERNAL_ANDROID_DIR}")
	endif()

	if(WIN32)
		set(NDK_BUILD ndk-build.cmd)
		set(ANDROID_INSTALL_DESTINATION android)
	else()
		set(NDK_BUILD ndk-build)
		set(ANDROID_INSTALL_DESTINATION share/ncine/android)
	endif()

	if(NDK_DIR STREQUAL "")
		message(FATAL_ERROR "NDK_DIR is not set")
	endif()
	if(NOT EXISTS ${NDK_DIR}/${NDK_BUILD})
		message(FATAL_ERROR "${NDK_BUILD} command cannot be found in: ${NDK_DIR}")
	endif()

	set(ARCHITECTURES armeabi-v7a)
	set(NCINE_APP_MODULES ncine)
	foreach(ARCHITECTURE ${ARCHITECTURES})
		set(NCINE_APP_ABI "${NCINE_APP_ABI} ${ARCHITECTURE}")
	endforeach()
	set(NCINE_APP_PLATFORM android-19)
	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		set(NCINE_APP_OPTIM release)
	else()
		set(NCINE_APP_OPTIM debug)
	endif()
	set(APPLICATION_MK_IN ${CMAKE_SOURCE_DIR}/android/src/main/jni/Application.mk.in)
	set(APPLICATION_MK ${CMAKE_BINARY_DIR}/android/src/main/jni/Application.mk)
	configure_file(${APPLICATION_MK_IN} ${APPLICATION_MK} @ONLY)

	set(NCINE_APP_MODULES game)
	set(APPLICATION_DEVDIST_MK ${CMAKE_BINARY_DIR}/android/src/main/jni/Application-devdist.mk)
	configure_file(${APPLICATION_MK_IN} ${APPLICATION_DEVDIST_MK} @ONLY)

	set(ANDROID_MK_IN ${CMAKE_SOURCE_DIR}/android/src/main/jni/Android.mk.in)
	set(ANDROID_MK ${CMAKE_BINARY_DIR}/android/src/main/jni/Android.mk)
	configure_file(${ANDROID_MK_IN} ${ANDROID_MK} @ONLY)

	file(COPY android/build.gradle android/gradle.properties DESTINATION android)
	file(COPY android/src/main/AndroidManifest.xml DESTINATION android/src/main)
	file(COPY android/src/main/java/com/encelo/ncine/LoadLibraries.java DESTINATION android/src/main/java/com/encelo/ncine)
	file(COPY android/src/main/jni/main.cpp DESTINATION android/src/main/jni)
	file(COPY android/src/main/res/values/strings.xml DESTINATION android/src/main/res/values)
	file(COPY ${NCINE_DATA_DIR}/android/icon.png DESTINATION android/src/main/res/drawable)
	file(COPY ${EXTERNAL_ANDROID_DIR}/ DESTINATION android/src/main/jni/)

	foreach(ARCHITECTURE ${ARCHITECTURES})
		list(APPEND NCINE_ANDROID_LIBRARY_FILES android/src/main/libs/${ARCHITECTURE}/libncine.so)
	endforeach()
	add_custom_command(OUTPUT ${NCINE_ANDROID_LIBRARY_FILES}
		COMMAND ${NDK_DIR}/${NDK_BUILD} -j${NCINE_CORES}
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/android/src/main/jni
		COMMENT "Compiling Android library with ndk-build")
	add_custom_target(ncine_android ALL DEPENDS ${NCINE_ANDROID_LIBRARY_FILES})
	add_dependencies(ncine_android ncine)

	if(NCINE_INSTALL_DEV_SUPPORT)
		install(FILES ${CMAKE_BINARY_DIR}/android/build.gradle
			${CMAKE_BINARY_DIR}/android/gradle.properties
			DESTINATION ${ANDROID_INSTALL_DESTINATION})
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/AndroidManifest.xml
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/java/com/encelo/ncine/LoadLibraries.java
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/java/com/encelo/ncine)
		install(FILES ${CMAKE_SOURCE_DIR}/android/src/main/jni/Android-devdist.mk
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/jni RENAME Android.mk)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/jni/Application-devdist.mk
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/jni RENAME Application.mk)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/jni/main.cpp
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/jni)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/res/drawable/icon.png
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/res/drawable)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/res/values/strings.xml
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/res/values)

		foreach(ARCHITECTURE ${ARCHITECTURES})
			install(FILES ${CMAKE_BINARY_DIR}/android/src/main/libs/${ARCHITECTURE}/libncine.so
				DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/jni/ncine/${ARCHITECTURE}/)
			install(FILES ${CMAKE_BINARY_DIR}/android/src/main/libs/${ARCHITECTURE}/libopenal.so
				DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/jni/openal/${ARCHITECTURE}/)
		endforeach()
		install(DIRECTORY include/ DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/jni/ncine/include FILES_MATCHING PATTERN "*.h")
		file(GLOB APPTEST_SOURCES "tests/apptest*.h" "tests/apptest*.cpp")
		install(FILES ${APPTEST_SOURCES} DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/jni/tests)

		install(DIRECTORY ${NCINE_DATA_DIR}/android/fonts DESTINATION ${DATA_INSTALL_DESTINATION}/android)
		install(DIRECTORY ${NCINE_DATA_DIR}/android/textures DESTINATION ${DATA_INSTALL_DESTINATION}/android)
	endif()
endif()
