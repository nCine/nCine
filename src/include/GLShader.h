#ifndef CLASS_NCINE_GLSHADER
#define CLASS_NCINE_GLSHADER

#include <cstdint>
#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"

namespace ncine {

/// A class to handle OpenGL shader objects
class GLShader
{
  public:
	enum class Status
	{
		NOT_COMPILED,
		COMPILATION_FAILED,
		COMPILED,
		COMPILED_WITH_DEFERRED_CHECKS
	};

	enum class ErrorChecking
	{
		IMMEDIATE,
		DEFERRED
	};

	explicit GLShader(GLenum type);
	GLShader(GLenum type, const char *filename);
	~GLShader();

	inline GLenum type() const { return type_; }
	inline GLuint glHandle() const { return glHandle_; }
	inline Status status() const { return status_; }

	/// Loads a shader from the given string
	bool loadFromString(const char *string);
	/// Loads a shader from the given string and then append the specified file
	bool loadFromStringAndFile(const char *string, const char *filename);
	/// Loads a shader by concatenating the given strings in order
	bool loadFromStrings(const char **strings);
	/// Loads a shader by concatenating the given strings in order, then appending the specified file
	bool loadFromStringsAndFile(const char **strings, const char *filename);
	/// Loads a shader from the specified file
	bool loadFromFile(const char *filename);

	/// Used to force a `COMPILED` status when the shader program is loaded from a binary
	inline void setStatus(Status status) { status_ = status; }

	bool compile(ErrorChecking errorChecking, bool logOnErrors);
	bool checkCompilation(bool logOnErrors);

	inline uint64_t sourceHash() const { return sourceHash_; }
	void setObjectLabel(const char *label);

  private:
	static const unsigned int MaxShaderSourceLength = 32 * 1024;
	static const unsigned int MaxInfoLogLength = 512;
	static char infoLogString_[MaxInfoLogLength];

	GLenum type_;
	GLuint glHandle_;
	uint64_t sourceHash_;
	Status status_;

	/// Deleted copy constructor
	GLShader(const GLShader &) = delete;
	/// Deleted assignment operator
	GLShader &operator=(const GLShader &) = delete;
};

}

#endif
