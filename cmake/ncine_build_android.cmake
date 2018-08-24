if(NCINE_BUILD_ANDROID)
	if(NOT IS_DIRECTORY ${EXTERNAL_ANDROID_DIR})
		message(FATAL_ERROR "nCine external Android libraries directory not found at: ${EXTERNAL_ANDROID_DIR}")
	else()
		message(STATUS "nCine external Android libraries directory: ${EXTERNAL_ANDROID_DIR}")
	endif()

	if(NDK_DIR STREQUAL "")
		if(DEFINED ENV{ANDROID_NDK_HOME})
			set(NDK_DIR $ENV{ANDROID_NDK_HOME})
		elseif(DEFINED ENV{ANDROID_NDK_ROOT})
			set(NDK_DIR $ENV{ANDROID_NDK_ROOT})
		elseif(DEFINED ENV{ANDROID_NDK})
			set(NDK_DIR $ENV{ANDROID_NDK})
		else()
			message(FATAL_ERROR "NDK_DIR is not set")
		endif()
	endif()
	message(STATUS "Android NDK directory: ${NDK_DIR}")

	set(ANDROID_API_LEVEL 21)
	set(ANDROID_TOOLCHAIN_VERSION clang)
	set(ANDROID_STL_TYPE c++_static)

	# Reset compilation flags that external tools might have set in environment variables
	set(RESET_FLAGS_ARGS -DCMAKE_C_FLAGS="" -DCMAKE_CXX_FLAGS="" -DCMAKE_EXE_LINKER_FLAGS=""
		-DCMAKE_MODULE_LINKER_FLAGS="" -DCMAKE_SHARED_LINKER_FLAGS="" -DCMAKE_STATIC_LINKER_FLAGS="")
	set(ANDROID_PASSTHROUGH_ARGS -DNCINE_ROOT=${NCINE_ROOT} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
		-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH} -DNCINE_DYNAMIC_LIBRARY=${NCINE_DYNAMIC_LIBRARY}
		-DNCINE_STARTUP_TEST=${NCINE_STARTUP_TEST} -DEXTERNAL_ANDROID_DIR=${EXTERNAL_ANDROID_DIR}
		-DGENERATED_INCLUDE_DIR=${GENERATED_INCLUDE_DIR} -DWITH_IMGUI=${WITH_IMGUI} -DIMGUI_SOURCE_DIR=${IMGUI_SOURCE_DIR})
	set(ANDROID_CMAKE_ARGS -DCMAKE_SYSTEM_NAME=Android -DCMAKE_SYSTEM_VERSION=${ANDROID_API_LEVEL}
		-DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION=${ANDROID_TOOLCHAIN_VERSION} -DCMAKE_ANDROID_STL_TYPE=${ANDROID_STL_TYPE})
	set(ANDROID_ARM_ARGS -DCMAKE_ANDROID_ARM_MODE=ON -DCMAKE_ANDROID_ARM_NEON=ON)
	set(ANDROID_DEVDIST_PASSTHROUGH_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DNCINE_STARTUP_TEST=${NCINE_STARTUP_TEST})

	if(CMAKE_BUILD_TYPE MATCHES "Release")
		list(APPEND ANDROID_PASSTHROUGH_ARGS -DNCINE_EXTRA_OPTIMIZATION=${NCINE_EXTRA_OPTIMIZATION}
			-DNCINE_AUTOVECTORIZATION_REPORT=${NCINE_AUTOVECTORIZATION_REPORT})
	endif()

	if(MSVC)
		list(APPEND ANDROID_CMAKE_ARGS "-GNMake Makefiles")
	else()
		list(APPEND ANDROID_CMAKE_ARGS "-G${CMAKE_GENERATOR}")
	endif()

	string(REPLACE ";" "', '" GRADLE_PASSTHROUGH_ARGS "${ANDROID_PASSTHROUGH_ARGS}")
	string(REPLACE ";" "', '" GRADLE_CMAKE_ARGS "${ANDROID_CMAKE_ARGS}")
	string(REPLACE ";" "', '" GRADLE_ARM_ARGS "${ANDROID_ARM_ARGS}")
	string(REPLACE ";" "', '" GRADLE_NDK_ARCHITECTURES "${NCINE_NDK_ARCHITECTURES}")
	string(REPLACE ";" "', '" GRADLE_DEVDIST_PASSTHROUGH_ARGS "${ANDROID_DEVDIST_PASSTHROUGH_ARGS}")

	# Added later to skip the string replace operation and keep them as lists in Gradle too
	list(APPEND ANDROID_PASSTHROUGH_ARGS -DGENERATED_SOURCES="${GENERATED_SOURCES}" -DANDROID_GENERATED_FLAGS="${ANDROID_GENERATED_FLAGS}")
	set(GRADLE_PASSTHROUGH_ARGS "${GRADLE_PASSTHROUGH_ARGS}', '-DGENERATED_SOURCES=${GENERATED_SOURCES}', '-DANDROID_GENERATED_FLAGS=${ANDROID_GENERATED_FLAGS}")
	# Not added to Gradle arguments as it is handled by substituting `GRADLE_NDK_DIR`
	list(APPEND ANDROID_CMAKE_ARGS -DCMAKE_ANDROID_NDK=${NDK_DIR})

	set(GRADLE_BUILDTOOLS_VERSION 26.0.1)
	set(GRADLE_COMPILESDK_VERSION 23)
	set(GRADLE_MINSDK_VERSION 18)
	set(GRADLE_TARGETSDK_VERSION 23)
	set(GRADLE_VERSIONCODE 1)
	set(GRADLE_VERSION ${NCINE_VERSION})

	set(BUILD_GRADLE_IN ${CMAKE_SOURCE_DIR}/android/build.gradle.in)
	set(BUILD_GRADLE ${CMAKE_BINARY_DIR}/android/build.gradle)
	set(GRADLE_JNILIBS_DIRS "'${EXTERNAL_ANDROID_DIR}/openal'")
	configure_file(${BUILD_GRADLE_IN} ${BUILD_GRADLE} @ONLY)
	set(GRADLE_PROPERTIES_IN ${CMAKE_SOURCE_DIR}/android/gradle.properties.in)
	set(GRADLE_PROPERTIES ${CMAKE_BINARY_DIR}/android/gradle.properties)
	set(GRADLE_CMAKE_COMMAND ${CMAKE_COMMAND})
	set(GRADLE_NDK_DIR ${NDK_DIR})
	configure_file(${GRADLE_PROPERTIES_IN} ${GRADLE_PROPERTIES} @ONLY)

	set(BUILD_DEVDIST_GRADLE ${CMAKE_BINARY_DIR}/android/build-devdist.gradle)
	set(GRADLE_PASSTHROUGH_ARGS ${GRADLE_DEVDIST_PASSTHROUGH_ARGS})
	set(GRADLE_JNILIBS_DIRS "'src/main/cpp/ncine', 'src/main/cpp/openal'")
	configure_file(${BUILD_GRADLE_IN} ${BUILD_DEVDIST_GRADLE} @ONLY)

	file(COPY android/gradle-devdist.properties DESTINATION android)
	file(COPY android/src/main/AndroidManifest.xml DESTINATION android/src/main)
	file(COPY android/src/main/java/io/github/ncine/LoadLibraries.java DESTINATION android/src/main/java/io/github/ncine)
	file(COPY android/src/main/java/io/github/ncine/LoadLibrariesTV.java DESTINATION android/src/main/java/io/github/ncine)
	file(COPY android/src/main/cpp/main.cpp DESTINATION android/src/main/cpp)
	file(COPY android/src/main/cpp/CMakeLists.txt DESTINATION android/src/main/cpp)
	file(COPY android/src/main/cpp/CMakeLists-devdist.txt DESTINATION android/src/main/cpp)
	file(COPY android/src/main/res/values/strings.xml DESTINATION android/src/main/res/values)
	file(COPY ${NCINE_DATA_DIR}/icons/icon48.png DESTINATION android/src/main/res/mipmap-mdpi)
	file(RENAME ${CMAKE_BINARY_DIR}/android/src/main/res/mipmap-mdpi/icon48.png ${CMAKE_BINARY_DIR}/android/src/main/res/mipmap-mdpi/ic_launcher.png)
	file(COPY ${NCINE_DATA_DIR}/icons/icon72.png DESTINATION android/src/main/res/mipmap-hdpi)
	file(RENAME ${CMAKE_BINARY_DIR}/android/src/main/res/mipmap-hdpi/icon72.png ${CMAKE_BINARY_DIR}/android/src/main/res/mipmap-hdpi/ic_launcher.png)
	file(COPY ${NCINE_DATA_DIR}/icons/icon96.png DESTINATION android/src/main/res/mipmap-xhdpi)
	file(RENAME ${CMAKE_BINARY_DIR}/android/src/main/res/mipmap-xhdpi/icon96.png ${CMAKE_BINARY_DIR}/android/src/main/res/mipmap-xhdpi/ic_launcher.png)
	file(COPY ${NCINE_DATA_DIR}/icons/icon144.png DESTINATION android/src/main/res/mipmap-xxhdpi)
	file(RENAME ${CMAKE_BINARY_DIR}/android/src/main/res/mipmap-xxhdpi/icon144.png ${CMAKE_BINARY_DIR}/android/src/main/res/mipmap-xxhdpi/ic_launcher.png)
	file(COPY ${NCINE_DATA_DIR}/icons/icon192.png DESTINATION android/src/main/res/mipmap-xxxhdpi)
	file(RENAME ${CMAKE_BINARY_DIR}/android/src/main/res/mipmap-xxxhdpi/icon192.png ${CMAKE_BINARY_DIR}/android/src/main/res/mipmap-xxxhdpi/ic_launcher.png)
	file(COPY ${NCINE_DATA_DIR}/icons/banner320x180.png DESTINATION android/src/main/res/drawable-xhdpi)
	file(RENAME ${CMAKE_BINARY_DIR}/android/src/main/res/drawable-xhdpi/banner320x180.png ${CMAKE_BINARY_DIR}/android/src/main/res/drawable-xhdpi/banner.png)

	if(NCINE_DYNAMIC_LIBRARY)
		set(ANDROID_LIBNAME libncine.so)
	else()
		set(ANDROID_LIBNAME libncine.a)
		set(COPY_SRCINCLUDE_COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/include ${CMAKE_BINARY_DIR}/android/src/main/cpp/ncine/include)
	endif()

	add_custom_target(ncine_android ALL)
	set_target_properties(ncine_android PROPERTIES FOLDER "Android")
	add_dependencies(ncine_android ncine)

	foreach(ARCHITECTURE ${NCINE_NDK_ARCHITECTURES})
		set(ANDROID_BINARY_DIR ${CMAKE_BINARY_DIR}/android/build/ncine/${ARCHITECTURE})
		set(ANDROID_ARCH_ARGS -DCMAKE_ANDROID_ARCH_ABI=${ARCHITECTURE})
		if(ARCHITECTURE STREQUAL "armeabi-v7a")
			list(APPEND ANDROID_ARCH_ARGS ${ANDROID_ARM_ARGS})
		endif()
		add_custom_command(OUTPUT ${ANDROID_BINARY_DIR}/${ANDROID_LIBNAME}
			COMMAND ${CMAKE_COMMAND} -H${CMAKE_BINARY_DIR}/android/src/main/cpp/ -B${ANDROID_BINARY_DIR}
				${RESET_FLAGS_ARGS} ${ANDROID_PASSTHROUGH_ARGS} ${ANDROID_CMAKE_ARGS} ${ANDROID_ARCH_ARGS}
			COMMAND ${CMAKE_COMMAND} --build ${ANDROID_BINARY_DIR}
			COMMENT "Compiling the Android library for ${ARCHITECTURE}")
		add_custom_target(ncine_android_${ARCHITECTURE} DEPENDS ${ANDROID_BINARY_DIR}/${ANDROID_LIBNAME})
		set_target_properties(ncine_android_${ARCHITECTURE} PROPERTIES FOLDER "Android")
		add_dependencies(ncine_android ncine_android_${ARCHITECTURE})
	endforeach()

	# Creating an ncine directory inside cpp to allow building external Android applications without installing the engine
	add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/android/src/main/cpp/ncine/include
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/android/src/main/cpp/ncine/include
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${NCINE_CONFIG_H} ${CMAKE_BINARY_DIR}/android/src/main/cpp/ncine/include
		# Projects relying on a static and uninstalled version of the engine can use implementation headers
		COMMAND ${COPY_SRCINCLUDE_COMMAND})
	add_custom_target(ncine_ndkdir_include ALL DEPENDS ${CMAKE_BINARY_DIR}/android/src/main/cpp/ncine/include)
	set_target_properties(ncine_ndkdir_include PROPERTIES FOLDER "Android")
	add_dependencies(ncine_ndkdir_include ncine_android)

	if (NCINE_ASSEMBLE_APK)
		if(CMAKE_BUILD_TYPE MATCHES "Release")
			set(GRADLE_TASK assembleRelease)
			set(APK_NAME android-release-unsigned.apk)
		else()
			set(GRADLE_TASK assembleDebug)
			set(APK_NAME android-debug.apk)
		endif()

		find_program(GRADLE_EXECUTABLE gradle $ENV{GRADLE_HOME}/bin)
		if (GRADLE_EXECUTABLE)
			message(STATUS "Gradle executable: ${GRADLE_EXECUTABLE}")
			# Invoking Gradle to create the Android APK of the startup test
			add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/android/build/output/apks/${APK_NAME}
				COMMAND ${GRADLE_EXECUTABLE} -p android ${GRADLE_TASK})
			add_custom_target(ncine_gradle_apk ALL DEPENDS ${CMAKE_BINARY_DIR}/android/build/output/apks/android-debug.apk)
			set_target_properties(ncine_gradle_apk PROPERTIES FOLDER "Android")
			add_dependencies(ncine_gradle_apk ncine_android)
		else()
			message(WARNING "Gradle executable not found, Android APK will not be assembled")
		endif()
	endif()

	if(NCINE_INSTALL_DEV_SUPPORT)
		install(FILES ${CMAKE_BINARY_DIR}/android/build-devdist.gradle
			DESTINATION ${ANDROID_INSTALL_DESTINATION} RENAME build.gradle COMPONENT android)
		install(FILES ${CMAKE_BINARY_DIR}/android/gradle-devdist.properties
			DESTINATION ${ANDROID_INSTALL_DESTINATION} RENAME gradle.properties COMPONENT android)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/AndroidManifest.xml
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main COMPONENT android)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/java/io/github/ncine/LoadLibraries.java
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/java/io/github/ncine COMPONENT android)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/java/io/github/ncine/LoadLibrariesTV.java
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/java/io/github/ncine COMPONENT android)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/cpp/CMakeLists-devdist.txt
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/cpp RENAME CMakeLists.txt COMPONENT android)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/cpp/main.cpp
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/cpp COMPONENT android)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/res/values/strings.xml
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/res/values COMPONENT android)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/res/mipmap-mdpi/ic_launcher.png
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/res/mipmap-mdpi COMPONENT android)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/res/mipmap-hdpi/ic_launcher.png
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/res/mipmap-hdpi COMPONENT android)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/res/mipmap-xhdpi/ic_launcher.png
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/res/mipmap-xhdpi COMPONENT android)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/res/mipmap-xxhdpi/ic_launcher.png
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/res/mipmap-xxhdpi COMPONENT android)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/res/mipmap-xxxhdpi/ic_launcher.png
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/res/mipmap-xxxhdpi COMPONENT android)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/res/drawable-xhdpi/banner.png
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/res/drawable-xhdpi COMPONENT android)

		foreach(ARCHITECTURE ${NCINE_NDK_ARCHITECTURES})
			install(FILES ${CMAKE_BINARY_DIR}/android/build/ncine/${ARCHITECTURE}/${ANDROID_LIBNAME}
				DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/cpp/ncine/${ARCHITECTURE}/ COMPONENT android)
			install(FILES ${EXTERNAL_ANDROID_DIR}/openal/${ARCHITECTURE}/libopenal.so
				DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/cpp/openal/${ARCHITECTURE}/ COMPONENT android)
		endforeach()
		install(DIRECTORY include/ DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/cpp/ncine/include COMPONENT android FILES_MATCHING PATTERN "*.h")
		file(GLOB APPTEST_SOURCES "tests/apptest*.h" "tests/apptest*.cpp")
		install(FILES ${APPTEST_SOURCES} DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/cpp/tests COMPONENT android)

		install(DIRECTORY ${NCINE_DATA_DIR}/android/fonts DESTINATION ${DATA_INSTALL_DESTINATION}/android COMPONENT android)
		install(DIRECTORY ${NCINE_DATA_DIR}/android/textures DESTINATION ${DATA_INSTALL_DESTINATION}/android COMPONENT android)
	endif()
endif()
