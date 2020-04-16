# Has to be included after ncine_get_version.cmake

set(GENERATED_SOURCE_DIR "${CMAKE_BINARY_DIR}/generated")
set(GENERATED_INCLUDE_DIR "${GENERATED_SOURCE_DIR}/include/ncine")

if(NOT NCINE_DYNAMIC_LIBRARY)
	foreach(PRIVATE_HEADER ${PRIVATE_HEADERS})
		file(COPY ${PRIVATE_HEADER} DESTINATION ${GENERATED_INCLUDE_DIR})
	endforeach()
endif()

# Version strings
if(GIT_EXECUTABLE)
	message(STATUS "Exporting git version information to C strings")
endif()

set(VERSION_H_FILE "${GENERATED_INCLUDE_DIR}/version.h")
set(VERSION_CPP_FILE "${GENERATED_SOURCE_DIR}/version.cpp")
if(EXISTS ${VERSION_H_FILE})
	file(REMOVE ${VERSION_H_FILE})
endif()
if(EXISTS ${VERSION_CPP_FILE})
	file(REMOVE ${VERSION_CPP_FILE})
endif()

set(VERSION_STRUCT_NAME "VersionStrings")
set(VERSION_STRING_NAME "Version")
set(REVCOUNT_STRING_NAME "GitRevCount")
set(SHORTHASH_STRING_NAME "GitShortHash")
set(LASTCOMMITDATE_STRING_NAME "GitLastCommitDate")
set(BRANCH_STRING_NAME "GitBranch")
set(TAG_STRING_NAME "GitTag")
set(COMPILATION_DATE_STRING_NAME "CompilationDate")
set(COMPILATION_TIME_STRING_NAME "CompilationTime")

get_filename_component(VERSION_H_FILENAME ${VERSION_H_FILE} NAME)
file(APPEND ${VERSION_H_FILE} "#ifndef NCINE_VERSION\n")
file(APPEND ${VERSION_H_FILE} "#define NCINE_VERSION\n\n")
file(APPEND ${VERSION_H_FILE} "#include \"common_defines.h\"\n\n")
file(APPEND ${VERSION_H_FILE} "namespace ncine {\n\n")
file(APPEND ${VERSION_H_FILE} "struct DLL_PUBLIC ${VERSION_STRUCT_NAME}\n{\n")
file(APPEND ${VERSION_CPP_FILE} "#include \"${VERSION_H_FILENAME}\"\n\n")
file(APPEND ${VERSION_CPP_FILE} "namespace ncine {\n\n")

if(GIT_EXECUTABLE)
	target_compile_definitions(ncine PRIVATE "WITH_GIT_VERSION")
	list(APPEND ANDROID_GENERATED_FLAGS WITH_GIT_VERSION)

	file(APPEND ${VERSION_H_FILE} "\tstatic char const * const ${VERSION_STRING_NAME};\n")
	file(APPEND ${VERSION_CPP_FILE} "char const * const ${VERSION_STRUCT_NAME}::${VERSION_STRING_NAME} = \"${NCINE_VERSION}\";\n")
	file(APPEND ${VERSION_H_FILE} "\tstatic char const * const ${REVCOUNT_STRING_NAME};\n")
	file(APPEND ${VERSION_CPP_FILE} "char const * const ${VERSION_STRUCT_NAME}::${REVCOUNT_STRING_NAME} = \"${GIT_REV_COUNT}\";\n")
	file(APPEND ${VERSION_H_FILE} "\tstatic char const * const ${SHORTHASH_STRING_NAME};\n")
	file(APPEND ${VERSION_CPP_FILE} "char const * const ${VERSION_STRUCT_NAME}::${SHORTHASH_STRING_NAME} = \"${GIT_SHORT_HASH}\";\n")
	file(APPEND ${VERSION_H_FILE} "\tstatic char const * const ${LASTCOMMITDATE_STRING_NAME};\n")
	file(APPEND ${VERSION_CPP_FILE} "char const * const ${VERSION_STRUCT_NAME}::${LASTCOMMITDATE_STRING_NAME} = \"${GIT_LAST_COMMIT_DATE}\";\n")
	file(APPEND ${VERSION_H_FILE} "\tstatic char const * const ${BRANCH_STRING_NAME};\n")
	file(APPEND ${VERSION_CPP_FILE} "char const * const ${VERSION_STRUCT_NAME}::${BRANCH_STRING_NAME} = \"${GIT_BRANCH_NAME}\";\n")
	file(APPEND ${VERSION_H_FILE} "\tstatic char const * const ${TAG_STRING_NAME};\n")
	if(NOT GIT_NO_TAG)
		file(APPEND ${VERSION_CPP_FILE} "char const * const ${VERSION_STRUCT_NAME}::${TAG_STRING_NAME} = \"${GIT_TAG_NAME}\";\n")
	else()
		file(APPEND ${VERSION_CPP_FILE} "char const * const ${VERSION_STRUCT_NAME}::${TAG_STRING_NAME} = \"\";\n")
	endif()
endif()
file(APPEND ${VERSION_H_FILE} "\tstatic char const * const ${COMPILATION_DATE_STRING_NAME};\n")
file(APPEND ${VERSION_CPP_FILE} "char const * const ${VERSION_STRUCT_NAME}::${COMPILATION_DATE_STRING_NAME} = __DATE__;\n")
file(APPEND ${VERSION_H_FILE} "\tstatic char const * const ${COMPILATION_TIME_STRING_NAME};\n")
file(APPEND ${VERSION_CPP_FILE} "char const * const ${VERSION_STRUCT_NAME}::${COMPILATION_TIME_STRING_NAME} = __TIME__;\n")

file(APPEND ${VERSION_H_FILE} "};\n\n}\n\n")
file(APPEND ${VERSION_H_FILE} "#endif")
file(APPEND ${VERSION_CPP_FILE} "\n}\n")

list(APPEND HEADERS ${VERSION_H_FILE})
list(APPEND GENERATED_SOURCES ${VERSION_CPP_FILE})

# Copy the header required by `version.h` to the generated include directory
# for compiling external projects using an nCine build directory
file(COPY ${CMAKE_SOURCE_DIR}/include/ncine/common_defines.h DESTINATION ${GENERATED_INCLUDE_DIR})

# Shader strings
file(GLOB SHADER_FILES "src/shaders/*.glsl")
if(NCINE_EMBED_SHADERS)
	message(STATUS "Exporting shader files to C strings")

	set(SHADERS_H_FILE "${GENERATED_INCLUDE_DIR}/shader_strings.h")
	set(SHADERS_CPP_FILE "${GENERATED_SOURCE_DIR}/shader_strings.cpp")
	if(EXISTS ${SHADERS_H_FILE})
		file(REMOVE ${SHADERS_H_FILE})
	endif()
	if(EXISTS ${SHADERS_CPP_FILE})
		file(REMOVE ${SHADERS_CPP_FILE})
	endif()

	set(SHADER_STRUCT_NAME "ShaderStrings")
	get_filename_component(SHADERS_H_FILENAME ${SHADERS_H_FILE} NAME)
	file(APPEND ${SHADERS_H_FILE} "namespace ncine {\n\n")
	file(APPEND ${SHADERS_H_FILE} "struct ${SHADER_STRUCT_NAME}\n{\n")
	file(APPEND ${SHADERS_CPP_FILE} "#include \"${SHADERS_H_FILENAME}\"\n\n")
	file(APPEND ${SHADERS_CPP_FILE} "namespace ncine {\n\n")
	foreach(SHADER_FILE ${SHADER_FILES})
		get_filename_component(SHADER_CSTRING_NAME ${SHADER_FILE} NAME_WE)
		file(STRINGS ${SHADER_FILE} SHADER_LINES)
		file(APPEND ${SHADERS_H_FILE} "\tstatic char const * const ${SHADER_CSTRING_NAME};\n")
		file(APPEND ${SHADERS_CPP_FILE} "char const * const ${SHADER_STRUCT_NAME}::${SHADER_CSTRING_NAME} = ")
		foreach(SHADER_LINE ${SHADER_LINES})
			file(APPEND ${SHADERS_CPP_FILE} "\n\"${SHADER_LINE}\\n\"")
		endforeach()
		file(APPEND ${SHADERS_CPP_FILE} ";\n\n")
	endforeach()
	file(APPEND ${SHADERS_H_FILE} "};\n\n}\n")
	file(APPEND ${SHADERS_CPP_FILE} "}\n")

	list(APPEND GENERATED_SOURCES ${SHADERS_H_FILE})
	list(APPEND GENERATED_SOURCES ${SHADERS_CPP_FILE})
	target_compile_definitions(ncine PRIVATE "WITH_EMBEDDED_SHADERS")
	list(APPEND ANDROID_GENERATED_FLAGS WITH_EMBEDDED_SHADERS)

	# Don't need to add shader files to the library target if they are embedded
	set(SHADER_FILES "")
endif()

if(WIN32 AND EXISTS ${NCINE_DATA_DIR}/icons/icon.ico)
	message(STATUS "Writing a resource file for executables icon")

	set(RESOURCE_RC_FILE "${GENERATED_SOURCE_DIR}/resource.rc")
	file(WRITE ${RESOURCE_RC_FILE} "IDI_ICON1 ICON DISCARDABLE \"ncine/icon.ico\"")
	file(COPY ${NCINE_DATA_DIR}/icons/icon.ico DESTINATION ${GENERATED_INCLUDE_DIR})
endif()

if(WIN32 AND NCINE_DYNAMIC_LIBRARY)
	message(STATUS "Writing a version info resource file")

	set(FILEVERSION_THIRD_WORD ${NCINE_VERSION_PATCH})
	if(DEFINED GIT_REV_COUNT)
		set(FILEVERSION_THIRD_WORD ${GIT_REV_COUNT})
	endif()
	get_target_property(NCINE_DEBUG_POSTFIX ncine DEBUG_POSTFIX)

	set(VERSION_RC_FILE "${GENERATED_SOURCE_DIR}/version.rc")
	file(WRITE ${VERSION_RC_FILE}
"#include \"winresrc.h\"\n\
\n\
LANGUAGE LANG_ENGLISH, SUBLANG_ENGLISH_US\n\
\n\
VS_VERSION_INFO VERSIONINFO\n\
 FILEVERSION ${NCINE_VERSION_MAJOR},${NCINE_VERSION_MINOR},${FILEVERSION_THIRD_WORD},0\n\
 PRODUCTVERSION ${NCINE_VERSION_MAJOR},${NCINE_VERSION_MINOR},${FILEVERSION_THIRD_WORD},0\n\
 FILEFLAGSMASK VS_FFI_FILEFLAGSMASK\n\
#ifdef NCINE_DEBUG\n\
 FILEFLAGS VS_FF_DEBUG\n\
#else\n\
 FILEFLAGS 0x0L\n\
#endif\n\
 FILEOS VOS_NT_WINDOWS32\n\
 FILETYPE VFT_DLL\n\
 FILESUBTYPE VFT2_UNKNOWN\n\
BEGIN\n\
    BLOCK \"StringFileInfo\"\n\
    BEGIN\n\
        BLOCK \"040904b0\"\n\
        BEGIN\n\
            VALUE \"CompanyName\", \"\\0\"\n\
            VALUE \"FileDescription\", \"nCine\\0\"\n\
            VALUE \"FileVersion\", \"${NCINE_VERSION_MAJOR},${NCINE_VERSION_MINOR},${FILEVERSION_THIRD_WORD},0\\0\"\n\
            VALUE \"InternalName\", \"nCine\\0\"\n\
            VALUE \"LegalCopyright\", \"Copyright ©2011-2020 Angelo Theodorou\\0\"\n\
#ifdef NCINE_DEBUG\n\
            VALUE \"OriginalFilename\", \"${CMAKE_SHARED_LIBRARY_PREFIX}ncine${NCINE_DEBUG_POSTFIX}${CMAKE_SHARED_LIBRARY_SUFFIX}\\0\"\n\
#else\n\
            VALUE \"OriginalFilename\", \"${CMAKE_SHARED_LIBRARY_PREFIX}ncine${CMAKE_SHARED_LIBRARY_SUFFIX}\\0\"\n\
#endif\n\
            VALUE \"ProductName\", \"nCine\\0\"\n\
            VALUE \"ProductVersion\", \"${NCINE_VERSION} (${GIT_BRANCH_NAME})\\0\"\n\
        END\n\
    END\n\
    BLOCK \"VarFileInfo\"\n\
    BEGIN\n\
        VALUE \"Translation\", 0x409, 1200\n\
    END\n\
END")
	list(APPEND GENERATED_SOURCES ${VERSION_RC_FILE})
endif()

# Generate Nuklear implementation file
if(NCINE_WITH_NUKLEAR)
	set(NUKLEAR_CPP_FILE "${GENERATED_SOURCE_DIR}/nuklear.cpp")
	file(WRITE ${NUKLEAR_CPP_FILE} "#define NK_IMPLEMENTATION\n")
	file(APPEND ${NUKLEAR_CPP_FILE} "#include \"NuklearContext.h\"\n")
	file(APPEND ${NUKLEAR_CPP_FILE} "#include \"nuklear.h\"\n")

	list(APPEND GENERATED_SOURCES ${NUKLEAR_CPP_FILE})
endif()

# Generate custom allocators configuration file
if(NCINE_WITH_ALLOCATORS)
	set(CFGALLOC_H_FILE "${GENERATED_INCLUDE_DIR}/allocators_config.h")
	if(EXISTS ${CFGALLOC_H_FILE})
		file(REMOVE ${CFGALLOC_H_FILE})
	endif()
	file(WRITE ${CFGALLOC_H_FILE} "// Configuration file for the custom memory allocators\n")
	if(NCINE_RECORD_ALLOCATIONS)
		file(APPEND ${CFGALLOC_H_FILE} "#define RECORD_ALLOCATIONS\n")
	endif()
	if(NCINE_OVERRIDE_NEW)
		file(APPEND ${CFGALLOC_H_FILE} "#define OVERRIDE_NEW\n")
	endif()
	if(NCINE_USE_FREELIST)
		file(APPEND ${CFGALLOC_H_FILE} "#define USE_FREELIST\n")
		file(APPEND ${CFGALLOC_H_FILE} "#define FREELIST_BUFFER (${NCINE_FREELIST_BUFFER})\n")
	endif()
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/config.h.in)
	set(CONFIG_H_IN ${CMAKE_SOURCE_DIR}/config.h.in)
	set(CONFIG_H ${GENERATED_INCLUDE_DIR}/config.h)
	set(NCINE_WITH_THREADS ${Threads_FOUND})
	set(NCINE_WITH_ANGLE ${ANGLE_FOUND})
	set(NCINE_WITH_GLEW ${GLEW_FOUND})
	if(NCINE_PREFERRED_BACKEND STREQUAL "GLFW")
		set(NCINE_WITH_GLFW ${GLFW_FOUND})
	elseif(NCINE_PREFERRED_BACKEND STREQUAL "SDL2")
		set(NCINE_WITH_SDL ${SDL2_FOUND})
	elseif(NCINE_PREFERRED_BACKEND STREQUAL "QT5")
		set(NCINE_WITH_QT5 ${Qt5_FOUND})
	endif()
	set(NCINE_WITH_PNG ${PNG_FOUND})
	set(NCINE_WITH_WEBP ${WEBP_FOUND})
	set(NCINE_WITH_AUDIO ${OPENAL_FOUND})
	if(${NCINE_WITH_AUDIO})
		set(NCINE_WITH_VORBIS ${VORBIS_FOUND})
	endif()
	set(NCINE_WITH_LUA ${LUA_FOUND})
	configure_file(${CONFIG_H_IN} ${CONFIG_H} @ONLY)
	list(APPEND HEADERS ${CONFIG_H})
endif()
