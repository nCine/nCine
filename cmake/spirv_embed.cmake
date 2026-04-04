# spirv_embed.cmake

# Expected inputs via -D:
#   MODE (FILE_H or FILE_CPP)
#   OUTPUT_BASENAME
#   OUT_DIR
#   FILES (semicolon-separated list)

if(NOT MODE)
	message(FATAL_ERROR "Missing MODE")
elseif(NOT MODE STREQUAL "FILE_H" AND NOT MODE STREQUAL "FILE_CPP")
	message(FATAL_ERROR "MODE should be either FILE_H or FILE_CPP")
endif()

if(NOT OUTPUT_BASENAME)
	message(FATAL_ERROR "Missing OUTPUT_BASENAME")
endif()

if(NOT OUT_DIR)
	message(FATAL_ERROR "Missing output directory")
endif()

if(NOT FILES)
	message(FATAL_ERROR "FILES list is empty")
endif()

separate_arguments(SPIRV_FILES UNIX_COMMAND "${FILES}")

if(MODE STREQUAL "FILE_H")
	set(H_FILE "${OUT_DIR}/${OUTPUT_BASENAME}.h")
	file(MAKE_DIRECTORY "${OUT_DIR}")

	# Header preamble
	set(H_CONTENT
"#ifndef GRAIL_SHADER_ARRAYS_H
#define GRAIL_SHADER_ARRAYS_H

#include <cstdint>
#include <cstddef>

namespace ncine {
namespace grail {
namespace ShaderArrays {

")

	foreach(FILE IN LISTS SPIRV_FILES)
		if(NOT EXISTS "${FILE}")
			message(FATAL_ERROR "SPIR-V file not found: ${FILE}")
		endif()

		get_filename_component(SYMBOL_WLE "${FILE}" NAME_WLE)
		string(REGEX REPLACE "[^a-zA-Z0-9_]" "_" SYMBOL "${SYMBOL_WLE}")

		string(APPEND H_CONTENT
"extern const uint32_t ${SYMBOL}[];
extern const size_t ${SYMBOL}_size;

")
	endforeach()

	string(APPEND H_CONTENT
"} // namespace ShaderArrays
} // namespace grail
} // namespace ncine

#endif // GRAIL_SHADER_ARRAYS_H
")

	# Avoid needless rebuilds: write only if changed
	if(EXISTS "${H_FILE}")
		file(READ "${H_FILE}" OLD_H_CONTENT)
	else()
		set(OLD_H_CONTENT "")
	endif()

	if(NOT OLD_H_CONTENT STREQUAL H_CONTENT)
		file(WRITE "${H_FILE}" "${H_CONTENT}")
	endif()
elseif(MODE STREQUAL "FILE_CPP")
	set(CPP_FILE "${OUT_DIR}/${OUTPUT_BASENAME}.cpp")
	file(MAKE_DIRECTORY "${OUT_DIR}")

	# Source preamble
	set(CPP_CONTENT
"#include <cstdint>
#include <cstddef>

namespace ncine {
namespace grail {
namespace ShaderArrays {

")

	foreach(FILE IN LISTS SPIRV_FILES)
		if(NOT EXISTS "${FILE}")
			message(FATAL_ERROR "SPIR-V file not found: ${FILE}")
		endif()

		get_filename_component(SYMBOL_WLE "${FILE}" NAME_WLE)
		string(REGEX REPLACE "[^a-zA-Z0-9_]" "_" SYMBOL "${SYMBOL_WLE}")

		file(READ "${FILE}" HEX_CONTENT HEX)
		string(LENGTH "${HEX_CONTENT}" HEX_LEN)

		if(HEX_LEN EQUAL 0)
			message(FATAL_ERROR "Empty or unreadable file: ${FILE}")
		endif()

		# Ensure valid SPIR-V size (multiple of 4 bytes = 8 hex chars)
		math(EXPR REMAINDER "${HEX_LEN} % 8")
		if(NOT REMAINDER EQUAL 0)
			message(FATAL_ERROR "Invalid SPIR-V size (not multiple of 4 bytes): ${FILE}")
		endif()

		math(EXPR WORD_COUNT "${HEX_LEN} / 8")
		math(EXPR LAST_INDEX "${WORD_COUNT} - 1")

		string(APPEND H_CONTENT
"extern const uint32_t ${SYMBOL}[];
extern const size_t ${SYMBOL}_size;

")

		string(APPEND CPP_CONTENT "extern const uint32_t ${SYMBOL}[] = {\n")

		foreach(i RANGE 0 ${LAST_INDEX})
			math(EXPR OFFSET "${i} * 8")
			string(SUBSTRING "${HEX_CONTENT}" ${OFFSET} 8 WORD_HEX)

			# Safety check
			string(LENGTH "${WORD_HEX}" WORD_HEX_LEN)
			if(NOT WORD_HEX_LEN EQUAL 8)
				message(FATAL_ERROR "Corrupted word at index ${i} in ${FILE}")
			endif()

			string(SUBSTRING "${WORD_HEX}" 0 2 B0)
			string(SUBSTRING "${WORD_HEX}" 2 2 B1)
			string(SUBSTRING "${WORD_HEX}" 4 2 B2)
			string(SUBSTRING "${WORD_HEX}" 6 2 B3)

			# reverse for little-endian word
			set(WORD_LE "${B3}${B2}${B1}${B0}")

			string(APPEND CPP_CONTENT "\t0x${WORD_LE},\n")
		endforeach()

		string(APPEND CPP_CONTENT "};\nextern const size_t ${SYMBOL}_size = ${WORD_COUNT};\n\n")
	endforeach()

	string(APPEND CPP_CONTENT
"} // namespace ShaderArrays
} // namespace grail
} // namespace ncine
")

	# Avoid needless rebuilds: write only if changed
	if(EXISTS "${CPP_FILE}")
		file(READ "${CPP_FILE}" OLD_CPP_CONTENT)
	else()
		set(OLD_CPP_CONTENT "")
	endif()

	if(NOT OLD_CPP_CONTENT STREQUAL CPP_CONTENT)
		file(WRITE "${CPP_FILE}" "${CPP_CONTENT}")
	endif()
endif()
