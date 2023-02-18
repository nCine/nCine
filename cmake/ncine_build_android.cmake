if(NCINE_BUILD_ANDROID)
	if(NOT IS_DIRECTORY ${EXTERNAL_ANDROID_DIR})
		message(FATAL_ERROR "nCine external Android libraries directory not found at: ${EXTERNAL_ANDROID_DIR}")
	else()
		message(STATUS "nCine external Android libraries directory: ${EXTERNAL_ANDROID_DIR}")
	endif()

	if(NOT IS_DIRECTORY ${NDK_DIR})
		unset(NDK_DIR CACHE)
		find_path(NDK_DIR
			NAMES ndk-build ndk-build.cmd ndk-gdb ndk-gdb.cmd ndk-stack ndk-stack.cmd ndk-which ndk-which.cmd
			PATHS $ENV{ANDROID_NDK_HOME} $ENV{ANDROID_NDK_ROOT} $ENV{ANDROID_NDK}
			DOC "Path to the Android NDK directory")

		if(NOT IS_DIRECTORY ${NDK_DIR})
			message(FATAL_ERROR "Cannot find the Android NDK directory")
		endif()
	endif()
	message(STATUS "Android NDK directory: ${NDK_DIR}")

	set(ANDROID_TOOLCHAIN clang)
	set(ANDROID_STL c++_shared)

	# Always add the OpenGL ES compile definition to Android builds
	list(APPEND ANDROID_GENERATED_FLAGS WITH_OPENGLES)

	# Reset compilation flags that external tools might have set in environment variables
	set(RESET_FLAGS_ARGS -DCMAKE_C_FLAGS="" -DCMAKE_CXX_FLAGS="" -DCMAKE_EXE_LINKER_FLAGS=""
		-DCMAKE_MODULE_LINKER_FLAGS="" -DCMAKE_SHARED_LINKER_FLAGS="" -DCMAKE_STATIC_LINKER_FLAGS="")
	set(ANDROID_PASSTHROUGH_ARGS -DNCINE_ROOT=${NCINE_ROOT} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
		-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH} -DNCINE_DYNAMIC_LIBRARY=${NCINE_DYNAMIC_LIBRARY}
		-DNCINE_STARTUP_TEST=${NCINE_STARTUP_TEST} -DEXTERNAL_ANDROID_DIR=${EXTERNAL_ANDROID_DIR}
		-DGENERATED_INCLUDE_DIR=${GENERATED_INCLUDE_DIR} -DNCINE_STRIP_BINARIES=${NCINE_STRIP_BINARIES}
		-DNCINE_WITH_PNG=${NCINE_WITH_PNG} -DNCINE_WITH_WEBP=${NCINE_WITH_WEBP}
		-DNCINE_WITH_AUDIO=${NCINE_WITH_AUDIO} -DNCINE_WITH_VORBIS=${NCINE_WITH_VORBIS}
		-DNCINE_WITH_THREADS=${NCINE_WITH_THREADS} -DNCINE_WITH_LUA=${NCINE_WITH_LUA}
		-DNCINE_WITH_SCRIPTING_API=${NCINE_WITH_SCRIPTING_API} -DNCINE_WITH_ALLOCATORS=${NCINE_WITH_ALLOCATORS}
		-DNCINE_WITH_IMGUI=${NCINE_WITH_IMGUI} -DIMGUI_SOURCE_DIR=${IMGUI_SOURCE_DIR}
		-DNCINE_WITH_NUKLEAR=${NCINE_WITH_NUKLEAR} -DNUKLEAR_SOURCE_DIR=${NUKLEAR_SOURCE_DIR}
		-DNCINE_WITH_TRACY=${NCINE_WITH_TRACY} -DTRACY_SOURCE_DIR=${TRACY_SOURCE_DIR})
	set(ANDROID_CMAKE_ARGS -DANDROID_TOOLCHAIN=${ANDROID_TOOLCHAIN} -DANDROID_STL=${ANDROID_STL})
	set(ANDROID_ARM_ARGS -DANDROID_ARM_MODE=thumb -DANDROID_ARM_NEON=ON)

	if(CMAKE_BUILD_TYPE MATCHES "Release")
		list(APPEND ANDROID_PASSTHROUGH_ARGS -DNCINE_LINKTIME_OPTIMIZATION=${NCINE_LINKTIME_OPTIMIZATION}
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

	# Added later to skip the string replace operation and keep them as lists in Gradle too
	list(APPEND ANDROID_PASSTHROUGH_ARGS -DGENERATED_SOURCES="${GENERATED_SOURCES}" -DANDROID_GENERATED_FLAGS="${ANDROID_GENERATED_FLAGS}")
	set(GRADLE_PASSTHROUGH_ARGS "${GRADLE_PASSTHROUGH_ARGS}', '-DGENERATED_SOURCES=${GENERATED_SOURCES}', '-DANDROID_GENERATED_FLAGS=${ANDROID_GENERATED_FLAGS}")
	# Not added to Gradle arguments as it is handled by substituting `GRADLE_NDK_DIR`
	list(APPEND ANDROID_CMAKE_ARGS -DCMAKE_ANDROID_NDK=${NDK_DIR})

	set(GRADLE_BUILDTOOLS_VERSION 33.0.0)
	set(GRADLE_COMPILESDK_VERSION 31)
	set(GRADLE_MINSDK_VERSION 21)
	set(GRADLE_TARGETSDK_VERSION 31)
	set(GRADLE_VERSIONCODE 1)
	set(GRADLE_VERSION ${NCINE_VERSION})

	set(GRADLE_LIBCPP_SHARED "false")
	if(ANDROID_STL STREQUAL "c++_shared")
		set(GRADLE_LIBCPP_SHARED "true")

		find_path(GRADLE_LIBCPP_DIR libc++_shared.so
			PATHS ${NDK_DIR}/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib/
			      ${NDK_DIR}/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/lib/
			      ${NDK_DIR}/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/lib/
			PATH_SUFFIXES aarch64-linux-android arm-linux-androideabi i686-linux-android x86_64-linux-android
			NO_CACHE)

		# Mapping from architecture name to toolchain tuple
		foreach(ARCHITECTURE ${NCINE_NDK_ARCHITECTURES})
			if("${ARCHITECTURE}" STREQUAL "arm64-v8a")
				list(APPEND GRADLE_LIBCPP_ARCHITECTURES "aarch64-linux-android")
			elseif("${ARCHITECTURE}" STREQUAL "armeabi-v7a")
				list(APPEND GRADLE_LIBCPP_ARCHITECTURES "arm-linux-androideabi")
			elseif("${ARCHITECTURE}" STREQUAL "x86")
				list(APPEND GRADLE_LIBCPP_ARCHITECTURES "i686-linux-android")
			elseif("${ARCHITECTURE}" STREQUAL "x86_64")
				list(APPEND GRADLE_LIBCPP_ARCHITECTURES "x86_64-linux-android")
			endif()
		endforeach()
		string(REPLACE ";" "', '" GRADLE_LIBCPP_ARCHITECTURES "${GRADLE_LIBCPP_ARCHITECTURES}")
		get_filename_component(GRADLE_LIBCPP_DIR ${GRADLE_LIBCPP_DIR} DIRECTORY)
		file(RELATIVE_PATH GRADLE_LIBCPP_DIR ${NDK_DIR} ${GRADLE_LIBCPP_DIR})
	endif()

	set(GRADLE_UNIVERSAL_APK "false")
	if(NCINE_UNIVERSAL_APK)
		set(GRADLE_UNIVERSAL_APK "true")
	endif()

	set(JAVA_SYSTEM_LOADLIBRARY
		"try {
			System.loadLibrary(\"JAVA_NATIVE_LIBRARY_NAME\")\;
		} catch (UnsatisfiedLinkError e) {
			System.err.println(\"Caught UnsatisfiedLinkError: \" + e.getMessage())\;
		}"
	)
	if(NCINE_WITH_AUDIO)
		string(REPLACE "JAVA_NATIVE_LIBRARY_NAME" "openal" JAVA_SYSTEM_LOADLIBRARY_OPENAL ${JAVA_SYSTEM_LOADLIBRARY})
	endif()
	if(NCINE_DYNAMIC_LIBRARY)
		string(REPLACE "JAVA_NATIVE_LIBRARY_NAME" "ncine" JAVA_SYSTEM_LOADLIBRARY_NCINE ${JAVA_SYSTEM_LOADLIBRARY})
	endif()

	set(LOAD_LIBRARIES_JAVA_IN ${CMAKE_SOURCE_DIR}/android/app/src/main/java/io/github/ncine/LoadLibraries.java.in)
	set(LOAD_LIBRARIES_JAVA ${CMAKE_BINARY_DIR}/android/app/src/main/java/io/github/ncine/LoadLibraries.java)
	configure_file(${LOAD_LIBRARIES_JAVA_IN} ${LOAD_LIBRARIES_JAVA} @ONLY)
	set(LOAD_LIBRARIES_TV_JAVA_IN ${CMAKE_SOURCE_DIR}/android/app/src/main/java/io/github/ncine/LoadLibrariesTV.java.in)
	set(LOAD_LIBRARIES_TV_JAVA ${CMAKE_BINARY_DIR}/android/app/src/main/java/io/github/ncine/LoadLibrariesTV.java)
	configure_file(${LOAD_LIBRARIES_TV_JAVA_IN} ${LOAD_LIBRARIES_TV_JAVA} @ONLY)

	file(COPY ${CMAKE_SOURCE_DIR}/android/build.gradle DESTINATION android)
	file(COPY ${CMAKE_SOURCE_DIR}/android/settings.gradle DESTINATION android)
	set(GRADLE_PROPERTIES_IN ${CMAKE_SOURCE_DIR}/android/gradle.properties.in)
	set(GRADLE_PROPERTIES ${CMAKE_BINARY_DIR}/android/gradle.properties)
	set(GRADLE_CMAKE_COMMAND ${CMAKE_COMMAND})
	set(GRADLE_NDK_DIR ${NDK_DIR})
	configure_file(${GRADLE_PROPERTIES_IN} ${GRADLE_PROPERTIES} @ONLY)
	set(APP_BUILD_GRADLE_IN ${CMAKE_SOURCE_DIR}/android/app/build.gradle.in)
	set(APP_BUILD_GRADLE ${CMAKE_BINARY_DIR}/android/app/build.gradle)
	if(NCINE_WITH_AUDIO)
		set(GRADLE_JNILIBS_DIRS "'${EXTERNAL_ANDROID_DIR}/openal'")
	endif()
	configure_file(${APP_BUILD_GRADLE_IN} ${APP_BUILD_GRADLE} @ONLY)

	set(GRADLE_JNILIBS_DIRS "'src/main/cpp/ncine'")
	if(NCINE_WITH_AUDIO)
		set(GRADLE_JNILIBS_DIRS "${GRADLE_JNILIBS_DIRS}, 'src/main/cpp/openal'")
	endif()

	set(MANIFEST_PERMISSIONS "<uses-permission android:name=\"android.permission.WRITE_EXTERNAL_STORAGE\" android:maxSdkVersion=\"28\" />")
	if(NCINE_WITH_TRACY)
		string(CONCAT MANIFEST_PERMISSIONS "${MANIFEST_PERMISSIONS}\n"
			"\t<uses-permission android:name=\"android.permission.INTERNET\" />\n"
			"\t<uses-permission android:name=\"android.permission.ACCESS_NETWORK_STATE\" />")
	endif()
	set(ANDROID_MANIFEST_XML_IN ${CMAKE_SOURCE_DIR}/android/app/src/main/AndroidManifest.xml.in)
	set(ANDROID_MANIFEST_XML ${CMAKE_BINARY_DIR}/android/app/src/main/AndroidManifest.xml)
	configure_file(${ANDROID_MANIFEST_XML_IN} ${ANDROID_MANIFEST_XML} @ONLY)

	# Copying data to assets directory to stop requiring the external storage permission
	file(GLOB FONT_TEXTURE_ASSETS_KTX "${NCINE_DATA_DIR}/android/fonts/*.ktx")
	file(GLOB FONT_TEXTURE_ASSETS_WEBP "${NCINE_DATA_DIR}/android/fonts/*.webp")
	file(GLOB FONT_FNT_ASSETS "${NCINE_DATA_DIR}/fonts/*.fnt")
	file(GLOB SCRIPT_ASSETS "${NCINE_DATA_DIR}/scripts/*.lua")
	file(GLOB SOUND_ASSETS "${NCINE_DATA_DIR}/sounds/*")
	file(GLOB_RECURSE TEXTURE_ASSETS_KTX "${NCINE_DATA_DIR}/android/textures/*.ktx")
	file(GLOB_RECURSE TEXTURE_ASSETS_WEBP "${NCINE_DATA_DIR}/android/textures/*.webp")
	set(ANDROID_ASSETS ${FONT_TEXTURE_ASSETS_KTX} ${FONT_TEXTURE_ASSETS_WEBP} ${FONT_FNT_ASSETS}
		${SCRIPT_ASSETS} ${SOUND_ASSETS} ${TEXTURE_ASSETS_KTX} ${TEXTURE_ASSETS_WEBP})
	foreach(ASSET ${ANDROID_ASSETS})
		# Preserving directory structure
		file(RELATIVE_PATH ASSET_RELPATH ${NCINE_DATA_DIR} ${ASSET})
		# Remove the specific Android directory from the path of some assets
		string(REGEX REPLACE "^android\/" "" ASSET_RELPATH ${ASSET_RELPATH})
		get_filename_component(ASSET_RELPATH ${ASSET_RELPATH} DIRECTORY)
		file(COPY ${ASSET} DESTINATION android/app/src/main/assets/${ASSET_RELPATH})
	endforeach()

	file(COPY android/app/src/main/cpp/main.cpp DESTINATION android/app/src/main/cpp)
	file(COPY android/app/src/main/cpp/jnicall_functions.cpp DESTINATION android/app/src/main/cpp)
	file(COPY android/app/src/main/cpp/CMakeLists.txt DESTINATION android/app/src/main/cpp)
	file(COPY android/app/src/main/res/values/strings.xml DESTINATION android/app/src/main/res/values)

	if(EXISTS ${NCINE_ICONS_DIR}/icon48.png)
		file(COPY ${NCINE_ICONS_DIR}/icon48.png DESTINATION android/app/src/main/res/mipmap-mdpi)
		file(RENAME ${CMAKE_BINARY_DIR}/android/app/src/main/res/mipmap-mdpi/icon48.png ${CMAKE_BINARY_DIR}/android/app/src/main/res/mipmap-mdpi/ic_launcher.png)
	endif()
	if(EXISTS ${NCINE_ICONS_DIR}/icon72.png)
		file(COPY ${NCINE_ICONS_DIR}/icon72.png DESTINATION android/app/src/main/res/mipmap-hdpi)
		file(RENAME ${CMAKE_BINARY_DIR}/android/app/src/main/res/mipmap-hdpi/icon72.png ${CMAKE_BINARY_DIR}/android/app/src/main/res/mipmap-hdpi/ic_launcher.png)
	endif()
	if(EXISTS ${NCINE_ICONS_DIR}/icon96.png)
		file(COPY ${NCINE_ICONS_DIR}/icon96.png DESTINATION android/app/src/main/res/mipmap-xhdpi)
		file(RENAME ${CMAKE_BINARY_DIR}/android/app/src/main/res/mipmap-xhdpi/icon96.png ${CMAKE_BINARY_DIR}/android/app/src/main/res/mipmap-xhdpi/ic_launcher.png)
	endif()
	if(EXISTS ${NCINE_ICONS_DIR}/icon144.png)
		file(COPY ${NCINE_ICONS_DIR}/icon144.png DESTINATION android/app/src/main/res/mipmap-xxhdpi)
		file(RENAME ${CMAKE_BINARY_DIR}/android/app/src/main/res/mipmap-xxhdpi/icon144.png ${CMAKE_BINARY_DIR}/android/app/src/main/res/mipmap-xxhdpi/ic_launcher.png)
	endif()
	if(EXISTS ${NCINE_ICONS_DIR}/icon192.png)
		file(COPY ${NCINE_ICONS_DIR}/icon192.png DESTINATION android/app/src/main/res/mipmap-xxxhdpi)
		file(RENAME ${CMAKE_BINARY_DIR}/android/app/src/main/res/mipmap-xxxhdpi/icon192.png ${CMAKE_BINARY_DIR}/android/app/src/main/res/mipmap-xxxhdpi/ic_launcher.png)
	endif()
	if(EXISTS ${NCINE_ICONS_DIR}/banner320x180.png)
		file(COPY ${NCINE_ICONS_DIR}/banner320x180.png DESTINATION android/app/src/main/res/drawable-xhdpi)
		file(RENAME ${CMAKE_BINARY_DIR}/android/app/src/main/res/drawable-xhdpi/banner320x180.png ${CMAKE_BINARY_DIR}/android/app/src/main/res/drawable-xhdpi/banner.png)
	endif()

	if(NCINE_DYNAMIC_LIBRARY)
		set(ANDROID_LIBNAME libncine.so)
	else()
		set(ANDROID_LIBNAME libncine.a)
		set(COPY_SRCINCLUDE_COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PRIVATE_HEADERS} ${CMAKE_BINARY_DIR}/android/app/src/main/cpp/ncine/include/ncine)
	endif()
	if(NCINE_WITH_TRACY)
		set(COPY_TRACYINCLUDE_COMMAND ${CMAKE_COMMAND} -E copy_directory ${TRACY_INCLUDE_ONLY_DIR}/tracy ${CMAKE_BINARY_DIR}/android/app/src/main/cpp/ncine/include/tracy)
	endif()

	add_custom_target(ncine_android ALL)
	set_target_properties(ncine_android PROPERTIES FOLDER "Android")
	add_dependencies(ncine_android ncine)

	foreach(ARCHITECTURE ${NCINE_NDK_ARCHITECTURES})
		set(ANDROID_BINARY_DIR ${CMAKE_BINARY_DIR}/android/app/build/ncine/${ARCHITECTURE})
		set(ANDROID_ARCH_ARGS -DANDROID_ABI=${ARCHITECTURE})
		if("${ARCHITECTURE}" STREQUAL "armeabi-v7a")
			list(APPEND ANDROID_ARCH_ARGS ${ANDROID_ARM_ARGS})
		endif()
		add_custom_command(OUTPUT ${ANDROID_BINARY_DIR}/${ANDROID_LIBNAME} ${ANDROID_BINARY_DIR}/libncine_main.a
			COMMAND ${CMAKE_COMMAND} -H${CMAKE_BINARY_DIR}/android/app/src/main/cpp/ -B${ANDROID_BINARY_DIR}
				-DCMAKE_TOOLCHAIN_FILE=${NDK_DIR}/build/cmake/android.toolchain.cmake
				-DANDROID_PLATFORM=${GRADLE_MINSDK_VERSION} -DANDROID_ABI=${ARCHITECTURE}
				${RESET_FLAGS_ARGS} ${ANDROID_PASSTHROUGH_ARGS} ${ANDROID_CMAKE_ARGS} ${ANDROID_ARCH_ARGS}
			COMMAND ${CMAKE_COMMAND} --build ${ANDROID_BINARY_DIR}
			COMMENT "Compiling the Android library for ${ARCHITECTURE}")
		add_custom_target(ncine_android_${ARCHITECTURE} DEPENDS ${ANDROID_BINARY_DIR}/${ANDROID_LIBNAME} ${ANDROID_BINARY_DIR}/libncine_main.a)
		set_target_properties(ncine_android_${ARCHITECTURE} PROPERTIES FOLDER "Android")
		add_dependencies(ncine_android ncine_android_${ARCHITECTURE})
	endforeach()

	if(NCINE_WITH_AUDIO)
		foreach(ARCHITECTURE ${NCINE_NDK_ARCHITECTURES})
			set(ANDROID_OPENAL_SRC_BINARY_DIR ${EXTERNAL_ANDROID_DIR}/openal/${ARCHITECTURE})
			set(ANDROID_OPENAL_DEST_BINARY_DIR ${CMAKE_BINARY_DIR}/android/app/src/main/cpp/openal/${ARCHITECTURE})
			add_custom_command(OUTPUT ${ANDROID_OPENAL_DEST_BINARY_DIR}/libopenal.so
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ANDROID_OPENAL_SRC_BINARY_DIR}/libopenal.so ${ANDROID_OPENAL_DEST_BINARY_DIR}/libopenal.so
				COMMENT "Copying OpenAL library for ${ARCHITECTURE}")
			add_custom_target(ncine_android_openal_${ARCHITECTURE} DEPENDS ${ANDROID_OPENAL_DEST_BINARY_DIR}/libopenal.so)
			set_target_properties(ncine_android_openal_${ARCHITECTURE} PROPERTIES FOLDER "Android")
			add_dependencies(ncine_android ncine_android_openal_${ARCHITECTURE})
		endforeach()
	endif()

	# Creating an ncine directory inside cpp to allow building external Android applications without installing the engine
	add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/android/app/src/main/cpp/ncine/include
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/android/app/src/main/cpp/ncine/include/ncine/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${HEADERS} ${CMAKE_BINARY_DIR}/android/app/src/main/cpp/ncine/include/ncine/
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/android/app/src/main/cpp/ncine/include/nctl/
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${NCTL_HEADERS} ${CMAKE_BINARY_DIR}/android/app/src/main/cpp/ncine/include/nctl/
		# Projects relying on a static and uninstalled version of the engine can use implementation headers
		COMMAND ${COPY_SRCINCLUDE_COMMAND}
		# If Tracy integration is enabled then projects relying on an uninstalled version of the engine can use its headers
		COMMAND ${COPY_TRACYINCLUDE_COMMAND}
		COMMENT "Copying Android header files")
	add_custom_target(ncine_ndkdir_include ALL DEPENDS ${CMAKE_BINARY_DIR}/android/app/src/main/cpp/ncine/include)
	set_target_properties(ncine_ndkdir_include PROPERTIES FOLDER "Android")
	add_dependencies(ncine_ndkdir_include ncine_android)

	if(NCINE_ASSEMBLE_APK)
		find_program(GRADLE_EXECUTABLE gradle $ENV{GRADLE_HOME}/bin)
		if(GRADLE_EXECUTABLE)
			message(STATUS "Gradle executable: ${GRADLE_EXECUTABLE}")

			if(CMAKE_BUILD_TYPE MATCHES "Release")
				set(GRADLE_TASK assembleRelease)
				set(APK_BUILDTYPE_DIR release)
				set(APK_FILE_SUFFIX release-unsigned)
			else()
				set(GRADLE_TASK assembleDebug)
				set(APK_BUILDTYPE_DIR debug)
				set(APK_FILE_SUFFIX debug)
			endif()

			foreach(ARCHITECTURE ${NCINE_NDK_ARCHITECTURES})
				list(APPEND APK_FILES ${CMAKE_BINARY_DIR}/android/app/build/outputs/apk/${APK_BUILDTYPE_DIR}/android-${ARCHITECTURE}-${APK_FILE_SUFFIX}.apk)
			endforeach()

			# Invoking Gradle to create the Android APK of the startup test
			add_custom_command(OUTPUT ${APK_FILES}
				COMMAND ${GRADLE_EXECUTABLE} -p android ${GRADLE_TASK})
			add_custom_target(ncine_gradle_apk ALL DEPENDS ${APK_FILES})
			set_target_properties(ncine_gradle_apk PROPERTIES FOLDER "Android")
			add_dependencies(ncine_gradle_apk ncine_android)
		else()
			message(WARNING "Gradle executable not found, Android APK will not be assembled")
		endif()
	endif()

	if(NCINE_INSTALL_DEV_SUPPORT)
		foreach(ARCHITECTURE ${NCINE_NDK_ARCHITECTURES})
			install(FILES ${CMAKE_BINARY_DIR}/android/app/build/ncine/${ARCHITECTURE}/${ANDROID_LIBNAME}
				DESTINATION ${ANDROID_INSTALL_DESTINATION}/app/src/main/cpp/ncine/${ARCHITECTURE}/ COMPONENT android)
			install(FILES ${CMAKE_BINARY_DIR}/android/app/build/ncine/${ARCHITECTURE}/libncine_main.a
				DESTINATION ${ANDROID_INSTALL_DESTINATION}/app/src/main/cpp/ncine/${ARCHITECTURE}/ COMPONENT android)
			if(NCINE_WITH_AUDIO)
				install(FILES ${EXTERNAL_ANDROID_DIR}/openal/${ARCHITECTURE}/libopenal.so
					DESTINATION ${ANDROID_INSTALL_DESTINATION}/app/src/main/cpp/openal/${ARCHITECTURE}/ COMPONENT android)
			endif()
		endforeach()

		install(FILES ${HEADERS} DESTINATION ${ANDROID_INSTALL_DESTINATION}/app/src/main/cpp/ncine/include/ncine COMPONENT android)
		install(FILES ${NCTL_HEADERS} DESTINATION ${ANDROID_INSTALL_DESTINATION}/app/src/main/cpp/ncine/include/nctl COMPONENT android)
		if(NOT NCINE_DYNAMIC_LIBRARY)
			install(FILES ${PRIVATE_HEADERS} DESTINATION ${ANDROID_INSTALL_DESTINATION}/app/src/main/cpp/ncine/include/ncine COMPONENT android)
		endif()
		if(NCINE_WITH_TRACY)
			install(DIRECTORY ${TRACY_INCLUDE_ONLY_DIR}/tracy DESTINATION ${ANDROID_INSTALL_DESTINATION}/app/src/main/cpp/ncine/include/ COMPONENT android)
		endif()

		install(DIRECTORY ${NCINE_DATA_DIR}/android/fonts DESTINATION ${DATA_INSTALL_DESTINATION}/android COMPONENT android)
		install(DIRECTORY ${NCINE_DATA_DIR}/android/textures DESTINATION ${DATA_INSTALL_DESTINATION}/android COMPONENT android)
	endif()
endif()
