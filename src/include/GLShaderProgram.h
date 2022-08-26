#ifndef CLASS_NCINE_GLSHADERPROGRAM
#define CLASS_NCINE_GLSHADERPROGRAM

#include <nctl/Array.h>
#include <nctl/StaticHashMap.h>
#include <nctl/String.h>

#include "GLUniform.h"
#include "GLUniformBlock.h"
#include "GLAttribute.h"
#include "GLVertexFormat.h"

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
		COMPILATION_FAILED,
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

	bool isLinked() const;

	/// Returns the length of the information log including the null termination character
	unsigned int retrieveInfoLogLength() const;
	/// Retrieves the information log and copies it in the provided string object
	void retrieveInfoLog(nctl::String &infoLog) const;

	/// Returns the total memory needed for all uniforms outside of blocks
	inline unsigned int uniformsSize() const { return uniformsSize_; }
	/// Returns the total memory needed for all uniforms inside of blocks
	inline unsigned int uniformBlocksSize() const { return uniformBlocksSize_; }

	bool attachShader(GLenum type, const char *filename);
	bool attachShaderFromString(GLenum type, const char *string);
	bool link(Introspection introspection);
	void use();
	bool validate();

	inline unsigned int numAttributes() const { return attributeLocations_.size(); }
	inline bool hasAttribute(const char *name) const { return (attributeLocations_.find(name) != nullptr); }
	GLVertexFormat::Attribute *attribute(const char *name);

	inline void defineVertexFormat(const GLBufferObject *vbo) { defineVertexFormat(vbo, nullptr, 0); }
	inline void defineVertexFormat(const GLBufferObject *vbo, const GLBufferObject *ibo) { defineVertexFormat(vbo, ibo, 0); }
	void defineVertexFormat(const GLBufferObject *vbo, const GLBufferObject *ibo, unsigned int vboOffset);

	/// Deletes the current OpenGL shader program so that new shaders can be attached
	void reset();

	void setObjectLabel(const char *label);

	/// Returns the automatic log on errors flag
	inline bool logOnErrors() const { return shouldLogOnErrors_; }
	/// Sets the automatic log on errors flag
	/*! If the flag is true the shader program will automatically log compilation and linking errors. */
	inline void setLogOnErrors(bool shouldLogOnErrors) { shouldLogOnErrors_ = shouldLogOnErrors; }

  private:
	/// Max number of discoverable uniforms
	static const unsigned int MaxNumUniforms = 32;

	static const unsigned int MaxInfoLogLength = 512;
	static char infoLogString_[MaxInfoLogLength];

	static GLuint boundProgram_;

	GLuint glHandle_;
	static const int AttachedShadersInitialSize = 4;
	nctl::Array<nctl::UniquePtr<GLShader>> attachedShaders_;
	Status status_;
	Introspection introspection_;
	QueryPhase queryPhase_;

	/// A flag indicating whether the shader program should automatically log errors (the information log)
	bool shouldLogOnErrors_;

	unsigned int uniformsSize_;
	unsigned int uniformBlocksSize_;

	static const int UniformsInitialSize = 8;
	nctl::Array<GLUniform> uniforms_;
	static const int UniformBlocksInitialSize = 4;
	nctl::Array<GLUniformBlock> uniformBlocks_;
	static const int AttributesInitialSize = 4;
	nctl::Array<GLAttribute> attributes_;

	nctl::StaticHashMap<nctl::String, int, GLVertexFormat::MaxAttributes> attributeLocations_;
	GLVertexFormat vertexFormat_;

	bool deferredQueries();
	bool checkLinking();
	void performIntrospection();

	void discoverUniforms();
	void discoverUniformBlocks(GLUniformBlock::DiscoverUniforms discover);
	void discoverAttributes();
	void initVertexFormat();

	/// Deleted copy constructor
	GLShaderProgram(const GLShaderProgram &) = delete;
	/// Deleted assignment operator
	GLShaderProgram &operator=(const GLShaderProgram &) = delete;

	friend class GLShaderUniforms;
	friend class GLShaderUniformBlocks;
};

}

#endif
