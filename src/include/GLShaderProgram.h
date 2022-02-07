#ifndef CLASS_NCINE_GLSHADERPROGRAM
#define CLASS_NCINE_GLSHADERPROGRAM

#include <nctl/Array.h>
#include "GLUniform.h"
#include "GLUniformBlock.h"
#include "GLAttribute.h"

namespace ncine {

class GLShader;

/// A class to handle OpenGL shader programs
class GLShaderProgram
{
  public:
	enum class Introspection
	{
		ENABLED,
		NO_UNIFORMS_IN_BLOCKS,
		DISABLED
	};

	enum class Status
	{
		NOT_LINKED,
		LINKING_FAILED,
		LINKED,
		LINKED_WITH_DEFERRED_QUERIES,
		LINKED_WITH_INTROSPECTION
	};

	enum class QueryPhase
	{
		IMMEDIATE,
		DEFERRED
	};

	GLShaderProgram();
	explicit GLShaderProgram(QueryPhase queryPhase);
	GLShaderProgram(const char *vertexFile, const char *fragmentFile, Introspection introspection, QueryPhase queryPhase);
	GLShaderProgram(const char *vertexFile, const char *fragmentFile, Introspection introspection);
	GLShaderProgram(const char *vertexFile, const char *fragmentFile);
	~GLShaderProgram();

	inline GLuint glHandle() const { return glHandle_; }
	inline Status status() const { return status_; }
	inline Introspection introspection() const { return introspection_; }
	inline QueryPhase queryPhase() const { return queryPhase_; }

	/// Returns the total memory needed for all uniforms outside of blocks
	inline unsigned int uniformsSize() const { return uniformsSize_; }
	/// Returns the total memory needed for all uniforms inside of blocks
	inline unsigned int uniformBlocksSize() const { return uniformBlocksSize_; }

	bool attachShader(GLenum type, const char *filename);
	bool attachShaderFromString(GLenum type, const char *string);
	bool link(Introspection introspection);
	void use();

	void setObjectLabel(const char *label);

	/// Sets the fatal assert on errors flag
	/*! If the flag is true the application fatal asserts if the shader program cannot be compiled or linked. */
	inline void setFatalAssertOnErrors(bool shouldFatalAssertOnErrors) { shouldFatalAssertOnErrors_ = shouldFatalAssertOnErrors; }

  private:
	/// Max number of discoverable uniforms
	static const int MaxNumUniforms = 32;

	static GLuint boundProgram_;

	GLuint glHandle_;
	static const int AttachedShadersInitialSize = 4;
	nctl::Array<nctl::UniquePtr<GLShader>> attachedShaders_;
	Status status_;
	Introspection introspection_;
	QueryPhase queryPhase_;

	/// A flag indicating whether the class should fatal assert on compilation and linking errors
	/*! \note Useful for custom shaders creation. */
	bool shouldFatalAssertOnErrors_;

	unsigned int uniformsSize_;
	unsigned int uniformBlocksSize_;

	static const int UniformsInitialSize = 8;
	nctl::Array<GLUniform> uniforms_;
	static const int UniformBlocksInitialSize = 4;
	nctl::Array<GLUniformBlock> uniformBlocks_;
	static const int AttributesInitialSize = 4;
	nctl::Array<GLAttribute> attributes_;

	bool deferredQueries();
	bool checkLinking();
	void performIntrospection();

	void discoverUniforms();
	void discoverUniformBlocks(GLUniformBlock::DiscoverUniforms discover);
	void discoverAttributes();

	/// Deleted copy constructor
	GLShaderProgram(const GLShaderProgram &) = delete;
	/// Deleted assignment operator
	GLShaderProgram &operator=(const GLShaderProgram &) = delete;

	friend class GLShaderUniforms;
	friend class GLShaderUniformBlocks;
	friend class GLShaderAttributes;
};

}

#endif
