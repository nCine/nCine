if(NCINE_BUILD_ANDROID)
	set(EXTERNAL_ANDROID_DIR "${PARENT_SOURCE_DIR}/nCine-android-external" CACHE PATH "Set the path to the Android libraries directory")
	if(NOT IS_DIRECTORY ${EXTERNAL_ANDROID_DIR})
		message(FATAL_ERROR "nCine external Android libraries directory not found at: ${EXTERNAL_ANDROID_DIR}")
	else()
		message(STATUS "nCine external Android libraries directory: ${EXTERNAL_ANDROID_DIR}")
	endif()

	if(WIN32)
		set(NDK_BUILD ndk-build.cmd)
	else()
		set(NDK_BUILD ndk-build)
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
	if(NOT EXISTS ${NDK_DIR}/${NDK_BUILD})
		message(FATAL_ERROR "${NDK_BUILD} command cannot be found in: ${NDK_DIR}")
	endif()
	message(STATUS "Android NDK directory: ${NDK_DIR}")

	set(NCINE_APP_MODULES ncine)
	foreach(ARCHITECTURE ${NCINE_NDK_ARCHITECTURES})
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

	if(NCINE_DYNAMIC_LIBRARY)
		set(ANDROID_MK_IN ${CMAKE_SOURCE_DIR}/android/src/main/jni/Android.mk.in)
	else()
		set(ANDROID_MK_IN ${CMAKE_SOURCE_DIR}/android/src/main/jni/Android-static.mk.in)
	endif()
	set(ANDROID_MK ${CMAKE_BINARY_DIR}/android/src/main/jni/Android.mk)
	configure_file(${ANDROID_MK_IN} ${ANDROID_MK} @ONLY)

	set(NCINE_COMPILESDK_VERSION 23)
	set(NCINE_MINSDK_VERSION 13)
	set(NCINE_TARGETSDK_VERSION 23)
	set(BUILD_GRADLE_IN ${CMAKE_SOURCE_DIR}/android/build.gradle.in)
	set(BUILD_GRADLE ${CMAKE_BINARY_DIR}/android/build.gradle)
	configure_file(${BUILD_GRADLE_IN} ${BUILD_GRADLE} @ONLY)

	file(COPY android/gradle.properties DESTINATION android)
	file(COPY android/src/main/AndroidManifest.xml DESTINATION android/src/main)
	file(COPY android/src/main/java/io/github/ncine/LoadLibraries.java DESTINATION android/src/main/java/io/github/ncine)
	file(COPY android/src/main/java/io/github/ncine/LoadLibrariesTV.java DESTINATION android/src/main/java/io/github/ncine)
	file(COPY android/src/main/jni/main.cpp DESTINATION android/src/main/jni)
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
	file(COPY ${EXTERNAL_ANDROID_DIR}/ DESTINATION android/src/main/jni/)

	if(NCINE_DYNAMIC_LIBRARY)
		set(ANDROID_LIBPATH libs)
		set(ANDROID_LIBNAME libncine.so)
	else()
		set(ANDROID_LIBPATH obj/local)
		set(ANDROID_LIBNAME libncine.a)
		set(COPY_SRCINCLUDE_COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/include ${CMAKE_BINARY_DIR}/android/src/main/jni/ncine/include)
	endif()

	foreach(ARCHITECTURE ${NCINE_NDK_ARCHITECTURES})
		list(APPEND NCINE_ANDROID_LIBRARY_FILES android/src/main/${ANDROID_LIBPATH}/${ARCHITECTURE}/${ANDROID_LIBNAME})
	endforeach()
	add_custom_command(OUTPUT ${NCINE_ANDROID_LIBRARY_FILES}
		COMMAND ${NDK_DIR}/${NDK_BUILD} -j${NCINE_CORES}
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/android/src/main/jni
		COMMENT "Compiling Android library with ndk-build")
	add_custom_target(ncine_android ALL DEPENDS ${NCINE_ANDROID_LIBRARY_FILES})
	set_target_properties(ncine_android PROPERTIES FOLDER "Android")
	add_dependencies(ncine_android ncine)

	# Creating an ncine directory inside jni to allow building external Android applications without installing the engine
	add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/android/src/main/jni/ncine/include
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/android/src/main/jni/ncine/include
		# Projects relying on a static and uninstalled version of the engine can use implementation headers
		COMMAND ${COPY_SRCINCLUDE_COMMAND})
	add_custom_target(ncine_ndkdir_include ALL DEPENDS ${CMAKE_BINARY_DIR}/android/src/main/jni/ncine/include)
	set_target_properties(ncine_ndkdir_include PROPERTIES FOLDER "Android")
	add_dependencies(ncine_ndkdir_include ncine_android)

	foreach(ARCHITECTURE ${NCINE_NDK_ARCHITECTURES})
		add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/android/src/main/jni/ncine/${ARCHITECTURE}/${ANDROID_LIBNAME}
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/android/src/main/${ANDROID_LIBPATH}/${ARCHITECTURE}/${ANDROID_LIBNAME}
			${CMAKE_BINARY_DIR}/android/src/main/jni/ncine/${ARCHITECTURE}/${ANDROID_LIBNAME})
		add_custom_target(ncine_ndkdir_${ARCHITECTURE} ALL DEPENDS ${CMAKE_BINARY_DIR}/android/src/main/jni/ncine/${ARCHITECTURE}/${ANDROID_LIBNAME})
		set_target_properties(ncine_ndkdir_${ARCHITECTURE} PROPERTIES FOLDER "Android")
		add_dependencies(ncine_ndkdir_${ARCHITECTURE} ncine_android)
	endforeach()

	if(NCINE_INSTALL_DEV_SUPPORT)
		install(FILES ${CMAKE_BINARY_DIR}/android/build.gradle
			${CMAKE_BINARY_DIR}/android/gradle.properties
			DESTINATION ${ANDROID_INSTALL_DESTINATION} COMPONENT android)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/AndroidManifest.xml
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main COMPONENT android)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/java/io/github/ncine/LoadLibraries.java
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/java/io/github/ncine COMPONENT android)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/java/io/github/ncine/LoadLibrariesTV.java
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/java/io/github/ncine COMPONENT android)
		install(FILES ${CMAKE_SOURCE_DIR}/android/src/main/jni/Android-devdist.mk
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/jni RENAME Android.mk COMPONENT android)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/jni/Application-devdist.mk
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/jni RENAME Application.mk COMPONENT android)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/jni/main.cpp
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/jni COMPONENT android)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/res/values/strings.xml
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/res/values COMPONENT android)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/res/mipmap-mdpi/ic_launcher.png
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/res/mipmap-mdpi/ic_launcher.png COMPONENT android)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/res/mipmap-hdpi/ic_launcher.png
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/res/mipmap-hdpi/ic_launcher.png COMPONENT android)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/res/mipmap-xhdpi/ic_launcher.png
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/res/mipmap-xhdpi/ic_launcher.png COMPONENT android)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/res/mipmap-xxhdpi/ic_launcher.png
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/res/mipmap-xxhdpi/ic_launcher.png COMPONENT android)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/res/mipmap-xxxhdpi/ic_launcher.png
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/res/mipmap-xxxhdpi/ic_launcher.png COMPONENT android)
		install(FILES ${CMAKE_BINARY_DIR}/android/src/main/res/drawable-xhdpi/banner.png
			DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/res/drawable-xhdpi/banner.png COMPONENT android)

		foreach(ARCHITECTURE ${NCINE_NDK_ARCHITECTURES})
			# The static version of the library will be installed if available, but Android-devdist.mk does not support it
			install(FILES ${CMAKE_BINARY_DIR}/android/src/main/${ANDROID_LIBPATH}/${ARCHITECTURE}/${ANDROID_LIBNAME}
				DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/jni/ncine/${ARCHITECTURE}/ COMPONENT android)
			install(FILES ${CMAKE_BINARY_DIR}/android/src/main/libs/${ARCHITECTURE}/libopenal.so
				DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/jni/openal/${ARCHITECTURE}/ COMPONENT android)
		endforeach()
		install(DIRECTORY include/ DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/jni/ncine/include COMPONENT android FILES_MATCHING PATTERN "*.h")
		file(GLOB APPTEST_SOURCES "tests/apptest*.h" "tests/apptest*.cpp")
		install(FILES ${APPTEST_SOURCES} DESTINATION ${ANDROID_INSTALL_DESTINATION}/src/main/jni/tests COMPONENT android)

		install(DIRECTORY ${NCINE_DATA_DIR}/android/fonts DESTINATION ${DATA_INSTALL_DESTINATION}/android COMPONENT android)
		install(DIRECTORY ${NCINE_DATA_DIR}/android/textures DESTINATION ${DATA_INSTALL_DESTINATION}/android COMPONENT android)
	endif()
endif()
