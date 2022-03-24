#include "common_macros.h"
#include "GLUniformBlock.h"
#include "GLShaderProgram.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GLUniformBlock::GLUniformBlock()
    : program_(0), index_(0), size_(0), bindingIndex_(-1)
{
	name_[0] = '\0';
}

GLUniformBlock::GLUniformBlock(GLuint program, GLuint index, DiscoverUniforms discover)
    : GLUniformBlock()
{
	GLint nameLength = 0;
	GLint uniformCount = 0;
	program_ = program;
	index_ = index;

	glGetActiveUniformBlockiv(program, index, GL_UNIFORM_BLOCK_DATA_SIZE, &size_);
	glGetActiveUniformBlockiv(program, index, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLength);
	ASSERT(nameLength <= MaxNameLength);
	glGetActiveUniformBlockName(program, index, MaxNameLength, &nameLength, name_);
	glGetActiveUniformBlockiv(program, index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &uniformCount);

	if (discover == DiscoverUniforms::ENABLED && uniformCount > 0)
	{
		ASSERT(uniformCount <= MaxNumBlockUniforms);
		GLuint uniformIndices[MaxNumBlockUniforms];
		GLint uniformTypes[MaxNumBlockUniforms];
		GLint uniformSizes[MaxNumBlockUniforms];
		GLint uniformOffsets[MaxNumBlockUniforms];
		GLint uniformNameLengths[MaxNumBlockUniforms];

		GLint uniformQueryIndices[MaxNumBlockUniforms];
		glGetActiveUniformBlockiv(program, index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, uniformQueryIndices);
		for (int i = 0; i < uniformCount; i++)
			uniformIndices[i] = static_cast<GLuint>(uniformQueryIndices[i]);

		glGetActiveUniformsiv(program, uniformCount, uniformIndices, GL_UNIFORM_TYPE, uniformTypes);
		glGetActiveUniformsiv(program, uniformCount, uniformIndices, GL_UNIFORM_SIZE, uniformSizes);
		glGetActiveUniformsiv(program, uniformCount, uniformIndices, GL_UNIFORM_OFFSET, uniformOffsets);
#ifndef __EMSCRIPTEN__
		glGetActiveUniformsiv(program, uniformCount, uniformIndices, GL_UNIFORM_NAME_LENGTH, uniformNameLengths);
#endif

		for (int i = 0; i < uniformCount; i++)
		{
			GLUniform blockUniform;
			blockUniform.index_ = uniformIndices[i];
			blockUniform.blockIndex_ = static_cast<GLint>(index);
			blockUniform.type_ = static_cast<GLenum>(uniformTypes[i]);
			blockUniform.size_ = uniformSizes[i];
			blockUniform.offset_ = uniformOffsets[i];

#ifndef __EMSCRIPTEN__
			ASSERT_MSG_X(uniformNameLengths[i] <= GLUniform::MaxNameLength, "Uniform %d name length is %d, which is more than %d", i, uniformNameLengths[i], GLUniform::MaxNameLength);
#endif

#if !defined(WITH_OPENGLES) && !defined(__EMSCRIPTEN__)
			glGetActiveUniformName(program, uniformIndices[i], MaxNameLength, &uniformNameLengths[i], blockUniform.name_);
#else
			// Some drivers do not accept a `nullptr` for size and type
			GLint unusedSize;
			GLenum unusedType;
			glGetActiveUniform(program, uniformIndices[i], MaxNameLength, &uniformNameLengths[i], &unusedSize, &unusedType, blockUniform.name_);
#endif
			blockUniforms_[blockUniform.name_] = blockUniform;
		}
	}

	// Align to 16 bytes as required by the `std140` memory layout
	const unsigned int alignAmount = (16 - size_ % 16) % 16;
	size_ += alignAmount;
}

GLUniformBlock::GLUniformBlock(GLuint program, GLuint index)
    : GLUniformBlock(program, index, DiscoverUniforms::ENABLED)
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void GLUniformBlock::setBlockBinding(GLuint blockBinding)
{
	ASSERT(program_ != 0);

	if (bindingIndex_ != static_cast<GLint>(blockBinding))
	{
		glUniformBlockBinding(program_, index_, blockBinding);
		bindingIndex_ = static_cast<GLint>(blockBinding);
	}
}

}
