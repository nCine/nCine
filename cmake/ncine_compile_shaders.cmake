# Has to be included after `ncine_sources_generated.cmake`

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/${NCINE_SRC_SHADERS_DIR})
	message(FATAL_ERROR "The shaders directory \"${CMAKE_SOURCE_DIR}/${NCINE_SRC_SHADERS_DIR}\" does not exist")
endif()

if(NCINE_WITH_VULKAN)
	function(compile_slang_shader SHADER ENTRY STAGE OUTPUT)
		add_custom_command(
			OUTPUT ${OUTPUT}
			COMMAND ${CMAKE_COMMAND} -E env
					PATH=${VULKAN_BIN_DIR}
					LD_LIBRARY_PATH=${VULKAN_SDK_DIR}/lib
					${SLANGC_EXECUTABLE}
					${SHADER}
					-target spirv
					-profile spirv_1_3
					-entry ${ENTRY} -stage ${STAGE}
					$<$<CONFIG:Debug>:-g> # `-g` alone crashes with AMD on Windows
					$<$<CONFIG:Debug>:-O0>
					-o ${OUTPUT}
			DEPENDS ${SHADER}
			COMMENT "Compiling Vulkan ${STAGE} Slang shader: ${SHADER} -> ${OUTPUT}"
		)
	endfunction()

	function(compile_glsl_shader SHADER OUTPUT)
		add_custom_command(
			OUTPUT ${OUTPUT}
			COMMAND ${CMAKE_COMMAND} -E env
					PATH=${VULKAN_BIN_DIR}
					LD_LIBRARY_PATH=${VULKAN_SDK_DIR}/lib
					${GLSLANG_VALIDATOR_EXECUTABLE}
					-V
					--target-env spirv1.3
					$<$<CONFIG:Debug>:-g>
					$<$<CONFIG:Debug>:-Od>
					-o ${OUTPUT}
					${SHADER}
			DEPENDS ${SHADER}
			COMMENT "Compiling Vulkan GLSL shader: ${SHADER} -> ${OUTPUT}"
		)
	endfunction()

	set(NCINE_SRC_VULKAN_SHADERS_DIR "${NCINE_SRC_SHADERS_DIR}/vulkan")
	set(NCINE_DEST_VULKAN_SHADERS_DIR "${CMAKE_BINARY_DIR}/shaders")
	file(MAKE_DIRECTORY ${NCINE_DEST_VULKAN_SHADERS_DIR})
	set(VULKAN_SHADER_OUTPUTS)

	file(GLOB VULKAN_SHADER_FILES_SLANG "${NCINE_SRC_VULKAN_SHADERS_DIR}/*.slang")
	foreach(SHADER IN LISTS VULKAN_SHADER_FILES_SLANG)
		get_filename_component(SHADER_NAME ${SHADER} NAME_WLE)
		set(OUTPUT_SPV ${NCINE_DEST_VULKAN_SHADERS_DIR}/${SHADER_NAME}.spv)

		get_filename_component(FILE_EXT ${SHADER} EXT)
		if(FILE_EXT STREQUAL ".vert.slang")
			compile_slang_shader(${SHADER} main vertex ${OUTPUT_SPV})
		elseif(FILE_EXT STREQUAL ".frag.slang")
			compile_slang_shader(${SHADER} main fragment ${OUTPUT_SPV})
		elseif(FILE_EXT STREQUAL ".comp.slang")
			compile_slang_shader(${SHADER} main compute ${OUTPUT_SPV})
		else()
			add_custom_command(
				OUTPUT ${OUTPUT_SPV}
				COMMAND ${CMAKE_COMMAND} -E env
						PATH=${VULKAN_BIN_DIR}
						LD_LIBRARY_PATH=${VULKAN_SDK_DIR}/lib
						${SLANGC_EXECUTABLE}
						${SHADER}
						-target spirv
						-profile spirv_1_3
						-entry VSMain -stage vertex
						-entry PSMain -stage fragment
						$<$<CONFIG:Debug>:-g> # `-g` alone crashes with AMD on Windows
						$<$<CONFIG:Debug>:-O0>
						-o ${OUTPUT_SPV}
				DEPENDS ${SHADER}
				COMMENT "Compiling Vulkan shader: ${SHADER} to ${OUTPUT_SPV}")
		endif()

		list(APPEND VULKAN_SHADER_OUTPUTS ${OUTPUT_SPV})
	endforeach()

	file(GLOB VULKAN_SHADER_FILES_GLSL "${NCINE_SRC_VULKAN_SHADERS_DIR}/*.glsl")
	foreach(SHADER IN LISTS VULKAN_SHADER_FILES_GLSL)
		get_filename_component(SHADER_NAME ${SHADER} NAME_WLE)
		set(OUTPUT_SPV ${NCINE_DEST_VULKAN_SHADERS_DIR}/${SHADER_NAME}.spv)

		compile_glsl_shader(${SHADER} ${OUTPUT_SPV})

		list(APPEND VULKAN_SHADER_OUTPUTS ${OUTPUT_SPV})
	endforeach()

	add_custom_target(COMPILE_VULKAN_SHADERS ALL DEPENDS ${VULKAN_SHADER_OUTPUTS})

# --------------------------------------------

	# Shader arrays
	if(NCINE_EMBED_SHADERS)
		set(OUT_DIR_H "${GENERATED_INCLUDE_DIR}/grail")
		set(OUT_DIR_CPP "${GENERATED_SOURCE_DIR}/grail")

		set(SHADER_ARRAYS_CPP_FILE_LIST)
		foreach(SHADER IN LISTS VULKAN_SHADER_OUTPUTS)
			get_filename_component(SHADER_NAME_WLE "${SHADER}" NAME_WLE)
			string(REGEX REPLACE "[^a-zA-Z0-9_]" "_" SHADER_BASENAME "${SHADER_NAME_WLE}")
			set(SHADER_ARRAYS_CPP_FILE "${OUT_DIR_CPP}/${SHADER_BASENAME}.cpp")

			add_custom_command(
				OUTPUT ${SHADER_ARRAYS_CPP_FILE}
				COMMAND ${CMAKE_COMMAND}
					-DMODE=FILE_CPP # Writing the shader cpp file
					-DOUTPUT_BASENAME=${SHADER_BASENAME}
					-DOUT_DIR=${OUT_DIR_CPP}
					-DFILES="${SHADER}"
					-P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/spirv_embed.cmake
				DEPENDS ${SHADER}
				COMMENT "Writing SPIR-V shader ${SHADER} to ${SHADER_ARRAYS_CPP_FILE}")

			list(APPEND SHADER_ARRAYS_CPP_FILE_LIST ${SHADER_ARRAYS_CPP_FILE})
			list(APPEND GENERATED_SOURCES ${SHADER_ARRAYS_CPP_FILE})
		endforeach()
		add_custom_target(WRITE_SHADER_ARRAYS_CPP DEPENDS ${SHADER_ARRAYS_CPP_FILE_LIST})
		add_dependencies(WRITE_SHADER_ARRAYS_CPP COMPILE_VULKAN_SHADERS)

		set(HEADER_BASENAME "shader_arrays")
		set(SHADER_ARRAYS_H_FILE "${OUT_DIR_H}/${HEADER_BASENAME}.h")

		add_custom_command(
			OUTPUT ${SHADER_ARRAYS_H_FILE}
			COMMAND ${CMAKE_COMMAND}
				-DMODE=FILE_H # Writing the common header file
				-DOUTPUT_BASENAME=${HEADER_BASENAME}
				-DOUT_DIR=${OUT_DIR_H}
				-DFILES="${VULKAN_SHADER_OUTPUTS}"
				-P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/spirv_embed.cmake
			DEPENDS ${SHADER_ARRAYS_CPP_FILE_LIST}
			COMMENT "Writing common SPIR-V exported shaders header"
		)
		add_custom_target(WRITE_SHADER_ARRAYS_HEADER DEPENDS ${SHADER_ARRAYS_H_FILE})
		add_dependencies(WRITE_SHADER_ARRAYS_HEADER COMPILE_VULKAN_SHADERS)
		list(APPEND GENERATED_SOURCES ${SHADER_ARRAYS_H_FILE})
	endif()
endif()
