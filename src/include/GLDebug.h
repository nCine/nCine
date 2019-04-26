#ifndef CLASS_NCINE_GLDEBUG
#define CLASS_NCINE_GLDEBUG

#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"

#include "IGfxCapabilities.h"

namespace ncine {

/// A class to handle OpenGL debug functions
class GLDebug
{
  public:
	enum class LabelTypes
	{
#if !defined(__APPLE__)
		TRANSFORM_FEEDBACK = GL_TRANSFORM_FEEDBACK,
		TEXTURE = GL_TEXTURE,
		RENDERBUFFER = GL_RENDERBUFFER,
		FRAMEBUFFER = GL_FRAMEBUFFER,
	#if defined(__ANDROID__) && GL_ES_VERSION_3_0 && __ANDROID_API__ >= 21
		BUFFER = GL_BUFFER_KHR,
		SHADER = GL_SHADER_KHR,
		PROGRAM = GL_PROGRAM_KHR,
		VERTEX_ARRAY = GL_VERTEX_ARRAY_KHR,
		QUERY = GL_QUERY_KHR,
		PROGRAM_PIPELINE = GL_PROGRAM_PIPELINE_KHR,
		SAMPLER = GL_SAMPLER_KHR
	#else
		BUFFER = GL_BUFFER,
		SHADER = GL_SHADER,
		PROGRAM = GL_PROGRAM,
		VERTEX_ARRAY = GL_VERTEX_ARRAY,
		QUERY = GL_QUERY,
		PROGRAM_PIPELINE = GL_PROGRAM_PIPELINE,
		SAMPLER = GL_SAMPLER
	#endif
#else
		BUFFER,
		SHADER,
		PROGRAM,
		VERTEX_ARRAY,
		QUERY,
		PROGRAM_PIPELINE,
		TRANSFORM_FEEDBACK,
		SAMPLER,
		TEXTURE,
		RENDERBUFFER,
		FRAMEBUFFER
#endif
	};

	class ScopedGroup
	{
	  public:
		ScopedGroup(const char *message) { pushGroup(message); }
		~ScopedGroup() { popGroup(); }
	};

	static void init(const IGfxCapabilities &gfxCaps);
	static inline void reset() { debugGroupId_ = 0; }

	static void pushGroup(const char *message);
	static void popGroup();

	static void objectLabel(LabelTypes identifier, GLuint name, const char *label);
	static void objectLabel(LabelTypes identifier, GLuint name, GLsizei length, const char *label);
	static void getObjectLabel(LabelTypes identifier, GLuint name, GLsizei bufSize, GLsizei *length, char *label);

	static inline unsigned int maxLabelLength() { return maxLabelLength_; }

  private:
	static bool debugAvailable_;
	static GLuint debugGroupId_;
	static int maxLabelLength_;

	/// Enables OpenGL debug output and setup a callback function to log messages
	static void enableDebugOutput();
};

}

#endif
